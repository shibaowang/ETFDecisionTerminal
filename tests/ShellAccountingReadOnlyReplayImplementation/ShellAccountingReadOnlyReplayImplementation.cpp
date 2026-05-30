#include "ShellAccountingReadOnlyReplayImplementation.h"

#include "DataServiceApi/DataServiceActions.h"
#include "ServiceRuntime/ActionContext.h"

#include <QJsonArray>
#include <QJsonDocument>

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_readonly_replay {
namespace {

bool hasExtension(const std::filesystem::path& path, const std::vector<std::string>& extensions)
{
    const auto extension = path.extension().string();
    for (const auto& allowed : extensions) {
        if (extension == allowed) {
            return true;
        }
    }
    return false;
}

void requireDb(bool ok, const std::string& label)
{
    if (!ok) {
        std::cerr << "database setup failed: " << label << '\n';
        std::exit(1);
    }
}

void execute(etfdt::data_access::SQLiteConnection& connection, const std::string& sql)
{
    auto result = connection.executeSql(sql);
    if (!result.hasValue()) {
        std::cerr << result.error().message << '\n';
    }
    requireDb(result.hasValue(), sql);
}

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-139-" + action;
    request.traceId = "task-139-trace-" + action;
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-05-30T00:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

}  // namespace

std::filesystem::path sourceRoot(int argc, char** argv)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == "--source-root") {
            return argv[i + 1];
        }
    }
    return std::filesystem::current_path();
}

std::filesystem::path migrationPath(int argc, char** argv)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == "--migration") {
            return argv[i + 1];
        }
    }
    return {};
}

std::string readTextFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions)
{
    std::vector<std::filesystem::path> result;
    if (!std::filesystem::exists(root)) {
        return result;
    }
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file() && hasExtension(entry.path(), extensions)) {
            result.push_back(entry.path());
        }
    }
    return result;
}

bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (text.find(token) != std::string::npos) {
            std::cerr << "unexpected token `" << token << "`\n";
            return true;
        }
    }
    return false;
}

std::string joinFiles(const std::vector<std::filesystem::path>& files)
{
    std::string text;
    for (const auto& file : files) {
        text += "\n// FILE: " + file.generic_string() + "\n";
        text += readTextFile(file);
    }
    return text;
}

std::string shellBoundaryText(const std::filesystem::path& root)
{
    auto files = filesUnder(root / "apps" / "ETFDecisionShell", {".cpp", ".h", ".hpp", ".qml", ".js"});
    const auto shellServices = filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"});
    files.insert(files.end(), shellServices.begin(), shellServices.end());
    return joinFiles(files);
}

std::string dataServiceActionsText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
}

std::string readOnlyRegionText(const std::filesystem::path& root)
{
    const auto source = dataServiceActionsText(root);
    const auto begin = source.find("ShellAccountingReplayAttempt runShellAccountingReadOnlyReplay");
    const auto end = source.find("}  // namespace etfdt::data_service_api", begin);
    if (begin == std::string::npos || end == std::string::npos || end <= begin) {
        return {};
    }
    return source.substr(begin, end - begin);
}

std::string docs86Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "86_shell_accounting_readonly_replay_implementation.md");
}

std::unique_ptr<etfdt::data_access::SQLiteConnection> openMigratedDatabase(
    const std::filesystem::path& migration)
{
    const auto tempDir = std::filesystem::temp_directory_path()
        / ("etfdt_task_139_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(tempDir);

    auto connection = std::make_unique<etfdt::data_access::SQLiteConnection>();
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = tempDir / "task-139.sqlite";
    requireDb(connection->open(config).hasValue(), "open database");
    requireDb(connection->applyMigrationFile(migration).hasValue(), "apply migration");
    return connection;
}

void seedReplayFixture(etfdt::data_access::SQLiteConnection& connection)
{
    execute(connection, "INSERT INTO account(id,uid,name,account_type,is_active,initial_cash_cents,created_at_utc) "
                        "VALUES(1001,'task-139-account','Task 139 Account','SIMULATED',0,1000000,'2026-05-30T00:00:00Z');");
    execute(connection, "INSERT INTO portfolio(id,uid,name,is_active,created_at_utc) "
                        "VALUES(1001,'task-139-portfolio','Task 139 Portfolio',0,'2026-05-30T00:00:00Z');");
    execute(connection, "INSERT INTO instrument(id,uid,code,name,instrument_type,market_code,currency,enabled,created_at_utc) "
                        "VALUES(1001,'task-139-instrument','159509','Task ETF','ETF','CN_A','CNY',1,'2026-05-30T00:00:00Z');");
    execute(connection, "INSERT INTO strategy(id,uid,strategy_code,instrument_id,name,enabled,created_at_utc) "
                        "VALUES(1001,'task-139-strategy','TASK139',1001,'Task 139 Strategy',1,'2026-05-30T00:00:00Z');");
    execute(connection, "INSERT INTO grid_cycle(id,uid,account_id,portfolio_id,strategy_id,strategy_code,cycle_no,status,start_time_utc,created_at_utc) "
                        "VALUES(1001,'task-139-cycle',1001,1001,1001,'TASK139',1,'ACTIVE','2026-05-30T00:00:00Z','2026-05-30T00:00:00Z');");
    execute(connection, "INSERT INTO trade_execution_group(id,uid,account_id,portfolio_id,strategy_id,strategy_code,grid_cycle_id,action_type,trade_source,trade_intent_type,total_amount_cents,total_quantity_1e6,confirm_mode,manual_entry,created_at_utc) "
                        "VALUES(1001,'task-139-group',1001,1001,1001,'TASK139',1001,'BUY','MANUAL','BASE_INIT',120000,1000000,'MANUAL_ENTRY',1,'2026-05-30T00:01:00Z');");
    execute(connection, "INSERT INTO trade_log(id,uid,account_id,portfolio_id,trade_execution_group_id,grid_cycle_id,strategy_id,strategy_code,actual_instrument_id,actual_code,action_type,trade_source,trade_intent_type,price_1e6,quantity_1e6,amount_cents,fee_cents,net_cash_impact_cents,manual_entry,created_at_utc) "
                        "VALUES(1001,'task-139-log',1001,1001,1001,1001,1001,'TASK139',1001,'159509','BUY','MANUAL','BASE_INIT',1200000,1000000,120000,0,-120000,1,'2026-05-30T00:02:00Z');");
    execute(connection, "INSERT INTO base_position_allocation(uid,trade_execution_group_id,trade_log_id,account_id,portfolio_id,strategy_id,strategy_code,actual_instrument_id,actual_code,allocation_type,target_base_amount_cents,allocated_amount_cents,created_at_utc) "
                        "VALUES('task-139-base',1001,1001,1001,1001,1001,'TASK139',1001,'159509','INITIAL_BASE',200000,120000,'2026-05-30T00:03:00Z');");
    execute(connection, "INSERT INTO trade_tier_allocation(uid,trade_execution_group_id,account_id,portfolio_id,strategy_id,strategy_code,grid_cycle_id,tier_no,tier_name,allocated_amount_cents,created_at_utc) "
                        "VALUES('task-139-tier',1001,1001,1001,1001,'TASK139',1001,1,'T1',30000,'2026-05-30T00:04:00Z');");
}

void seedUsdReplayFixture(etfdt::data_access::SQLiteConnection& connection)
{
    seedReplayFixture(connection);
    execute(connection, "UPDATE instrument SET currency='USD' WHERE id=1001;");
    execute(connection, "INSERT INTO trade_log(id,uid,account_id,portfolio_id,trade_execution_group_id,grid_cycle_id,strategy_id,strategy_code,actual_instrument_id,actual_code,action_type,trade_source,trade_intent_type,price_1e6,quantity_1e6,amount_cents,fee_cents,net_cash_impact_cents,manual_entry,created_at_utc) "
                        "VALUES(1002,'task-139-usd-buy-2',1001,1001,1001,1001,1001,'TASK139',1001,'159509','BUY','MANUAL','BASE_INIT',1200000,1000000,120000,0,-120000,1,'2026-05-30T00:06:00Z');");
}

void seedSellExceedsFixture(etfdt::data_access::SQLiteConnection& connection)
{
    seedReplayFixture(connection);
    execute(connection, "INSERT INTO trade_log(uid,account_id,portfolio_id,trade_execution_group_id,grid_cycle_id,strategy_id,strategy_code,actual_instrument_id,actual_code,action_type,trade_source,trade_intent_type,price_1e6,quantity_1e6,amount_cents,fee_cents,net_cash_impact_cents,manual_entry,created_at_utc) "
                        "VALUES('task-139-sell',1001,1001,1001,1001,1001,'TASK139',1001,'159509','SELL','MANUAL','NORMAL_SELL',1200000,2000000,240000,0,240000,1,'2026-05-30T00:05:00Z');");
}

QJsonObject invokeReplayAction(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& action,
    std::string payloadJson)
{
    const auto context = etfdt::service_runtime::makeActionContext(
        envelope(action, std::move(payloadJson)),
        etfdt::protocol::ServiceName::ETFDataService);

    etfdt::protocol::ProtocolResponse response;
    if (action == "position.list") {
        response = etfdt::data_service_api::handlePositionList(context, connection);
    } else if (action == "cash.summary") {
        response = etfdt::data_service_api::handleCashSummary(context, connection);
    } else if (action == "portfolio.pnl.summary") {
        response = etfdt::data_service_api::handlePortfolioPnlSummary(context, connection);
    } else if (action == "base_position.summary") {
        response = etfdt::data_service_api::handleBasePositionSummary(context, connection);
    } else if (action == "sniper_pool.summary") {
        response = etfdt::data_service_api::handleSniperPoolSummary(context, connection);
    }

    if (!response.success) {
        std::cerr << "ProtocolResponse failed: " << response.errorMessage << '\n';
        return {};
    }
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(response.payloadJson));
    if (!document.isObject()) {
        std::cerr << "payload is not JSON object: " << response.payloadJson << '\n';
        return {};
    }
    return document.object();
}

bool expect(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return false;
    }
    return true;
}

bool expectReplayPayload(const QJsonObject& payload, const std::string& action, const std::string& label)
{
    bool ok = true;
    ok &= expect(payload.value("action").toString().toStdString() == action, label + " action");
    ok &= expect(payload.value("implemented").toBool(false), label + " implemented");
    ok &= expect(payload.value("readOnly").toBool(false), label + " readOnly");
    ok &= expect(!payload.value("writeEnabled").toBool(true), label + " write disabled");
    ok &= expect(payload.value("replayExecuted").toBool(false), label + " replay executed");
    ok &= expect(payload.value("accountingEngineCalled").toBool(false), label + " engine called");
    ok &= expect(!payload.value("snapshotRebuilt").toBool(true), label + " no snapshot rebuild");
    ok &= expect(!payload.value("tradeDraftGenerated").toBool(true), label + " no TradeDraft");
    const auto privacy = payload.value("privacy").toObject();
    ok &= expect(!privacy.value("rawSqlExposed").toBool(true), label + " no raw SQL");
    ok &= expect(!privacy.value("rawTradeLogPayloadExposed").toBool(true), label + " no raw trade_log");
    ok &= expect(!privacy.value("rawReplayPayloadExposed").toBool(true), label + " no raw replay payload");
    if (!ok) {
        std::cerr << QJsonDocument(payload).toJson(QJsonDocument::Compact).toStdString() << '\n';
    }
    return ok;
}

bool hasIssue(const QJsonObject& payload, const std::string& code)
{
    const auto issues = payload.value("issues").toArray();
    for (const auto& value : issues) {
        if (value.toObject().value("code").toString().toStdString() == code) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> snapshotWriteTokens()
{
    return {
        "INSERT INTO cash_snapshot",
        "INSERT INTO position_snapshot",
        "INSERT INTO portfolio_summary",
        "UPDATE cash_snapshot",
        "UPDATE position_snapshot",
        "UPDATE portfolio_summary",
        "DELETE FROM cash_snapshot",
        "DELETE FROM position_snapshot",
        "DELETE FROM portfolio_summary",
        "REPLACE INTO cash_snapshot",
        "REPLACE INTO position_snapshot",
        "REPLACE INTO portfolio_summary",
        "\"snapshotRebuilt\":true",
    };
}

std::vector<std::string> tradeOrStrategyTokens()
{
    return {
        "TradeDraft",
        "createTradeDraft",
        "brokerOrder(",
        "placeOrder",
        "strategyExecute",
        "executeStrategy",
        "confirmTrade",
    };
}

std::vector<std::string> uiDirectEngineTokens()
{
    return {
        "AccountingReplayEngine",
        "replayReadOnly",
        "replayFromFacts",
        "SQLiteConnection",
        "DataAccess/",
        "ShellAccountingReadOnlyFactsQuery",
    };
}

}  // namespace etfdt::tests::shell_accounting_readonly_replay
