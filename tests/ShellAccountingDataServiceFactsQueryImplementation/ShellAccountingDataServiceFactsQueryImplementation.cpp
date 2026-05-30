#include "ShellAccountingDataServiceFactsQueryImplementation.h"

#include "DataServiceApi/DataServiceActions.h"
#include "ServiceRuntime/ActionContext.h"

#include <QJsonArray>
#include <QJsonDocument>

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation {
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
    request.msgId = "task-137-" + action;
    request.traceId = "task-137-trace-" + action;
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-05-30T00:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

bool hasIssue(const QJsonObject& payload, const std::string& code, bool blocking)
{
    const auto issues = payload.value("issues").toArray();
    for (const auto& value : issues) {
        const auto issue = value.toObject();
        if (issue.value("code").toString().toStdString() == code
            && issue.value("blocking").toBool(!blocking) == blocking) {
            return true;
        }
    }
    return false;
}

bool expect(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        return false;
    }
    return true;
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

std::filesystem::path docs83Path(const std::filesystem::path& root)
{
    return root / "docs" / "83_shell_accounting_dataservice_readonly_facts_query_implementation.md";
}

std::filesystem::path docs81Path(const std::filesystem::path& root)
{
    return root / "docs" / "81_shell_accounting_dataservice_readonly_facts_query_gate.md";
}

std::filesystem::path docs82Path(const std::filesystem::path& root)
{
    return root / "docs" / "82_shell_accounting_dataservice_readonly_facts_query_test_plan.md";
}

std::filesystem::path dataServiceActionsPath(const std::filesystem::path& root)
{
    return root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp";
}

std::filesystem::path readOnlyFactsQuerySourcePath(const std::filesystem::path& root)
{
    return root / "libs" / "DataAccess" / "src" / "ShellAccountingReadOnlyFactsQuery.cpp";
}

std::vector<std::filesystem::path> shellAccountingProductionBoundaryFiles(
    const std::filesystem::path& root)
{
    auto result = filesUnder(root / "apps" / "ETFDecisionShell" / "src", {".cpp", ".h", ".hpp"});
    const auto qml = filesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"});
    const std::vector<std::filesystem::path> shellFiles{
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceAdapter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceAdapter.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceClientPortAdapter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceClientPortAdapter.cpp",
    };
    result.insert(result.end(), qml.begin(), qml.end());
    result.insert(result.end(), shellFiles.begin(), shellFiles.end());
    return result;
}

bool containsTokenInFiles(const std::vector<std::filesystem::path>& files, const std::string& token)
{
    for (const auto& file : files) {
        if (readTextFile(file).find(token) != std::string::npos) {
            std::cerr << file.generic_string() << ": unexpected token `" << token << "`\n";
            return true;
        }
    }
    return false;
}

int countTokenInText(const std::string& text, const std::string& token)
{
    int count = 0;
    std::size_t offset = 0;
    while ((offset = text.find(token, offset)) != std::string::npos) {
        ++count;
        offset += token.size();
    }
    return count;
}

std::vector<std::string> authorizedActions()
{
    return {
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
        "base_position.summary",
        "sniper_pool.summary",
    };
}

std::vector<std::string> forbiddenWriteTokens()
{
    return {
        "INSERT ",
        "UPDATE ",
        "DELETE ",
        "REPLACE ",
        "DROP ",
        "ALTER ",
        "CREATE TABLE",
        "data.audit.append",
        "createTradeDraft",
        "brokerOrder",
        "strategyExecute",
        "writeEnabled\":true",
    };
}

std::vector<std::string> forbiddenReplayTokens()
{
    return {
        "replayFromFacts",
        "snapshotRebuilt\":true",
        "rebuild summary",
        "snapshot rebuild",
    };
}

std::vector<std::string> privacyTokens()
{
    return {
        "rawSqlExposed\":false",
        "rawTradeLogPayloadExposed\":false",
        "full trade_log payload",
        "raw transport payload",
    };
}

std::unique_ptr<etfdt::data_access::SQLiteConnection> openMigratedDatabase(
    const std::filesystem::path& migration)
{
    const auto tempDir = std::filesystem::temp_directory_path()
        / ("etfdt_task_137_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::filesystem::create_directories(tempDir);

    auto connection = std::make_unique<etfdt::data_access::SQLiteConnection>();
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = tempDir / "task-137.sqlite";
    requireDb(connection->open(config).hasValue(), "open database");
    requireDb(connection->applyMigrationFile(migration).hasValue(), "apply migration");
    return connection;
}

void seedReadOnlyFactsFixture(etfdt::data_access::SQLiteConnection& connection)
{
    execute(connection, "INSERT INTO account(uid,name,account_type,is_active,initial_cash_cents,created_at_utc) "
                        "VALUES('task-137-account','Task 137 Account','SIMULATED',0,1000000,'2026-05-30T00:00:00Z');");
    execute(connection, "INSERT INTO portfolio(uid,name,is_active,created_at_utc) "
                        "VALUES('task-137-portfolio','Task 137 Portfolio',0,'2026-05-30T00:00:00Z');");
    execute(connection, "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,created_at_utc) "
                        "VALUES('task-137-instrument','159509','Task ETF','ETF','CN_A','CNY',1,'2026-05-30T00:00:00Z');");
    execute(connection, "INSERT INTO strategy(uid,strategy_code,instrument_id,name,enabled,created_at_utc) "
                        "VALUES('task-137-strategy','TASK137',1,'Task 137 Strategy',1,'2026-05-30T00:00:00Z');");
    execute(connection, "INSERT INTO grid_cycle(uid,account_id,portfolio_id,strategy_id,strategy_code,cycle_no,status,start_time_utc,created_at_utc) "
                        "VALUES('task-137-cycle',1,1,1,'TASK137',1,'ACTIVE','2026-05-30T00:00:00Z','2026-05-30T00:00:00Z');");
    execute(connection, "INSERT INTO trade_execution_group(uid,account_id,portfolio_id,strategy_id,strategy_code,grid_cycle_id,action_type,trade_source,trade_intent_type,total_amount_cents,total_quantity_1e6,confirm_mode,manual_entry,created_at_utc) "
                        "VALUES('task-137-group',1,1,1,'TASK137',1,'BUY','MANUAL','BASE_INIT',120000,1000000,'MANUAL_ENTRY',1,'2026-05-30T00:01:00Z');");
    execute(connection, "INSERT INTO trade_log(uid,account_id,portfolio_id,trade_execution_group_id,grid_cycle_id,strategy_id,strategy_code,actual_instrument_id,actual_code,action_type,trade_source,trade_intent_type,price_1e6,quantity_1e6,amount_cents,net_cash_impact_cents,manual_entry,created_at_utc) "
                        "VALUES('task-137-log',1,1,1,1,1,'TASK137',1,'159509','BUY','MANUAL','BASE_INIT',1200000,1000000,120000,-120000,1,'2026-05-30T00:02:00Z');");
    execute(connection, "INSERT INTO position_snapshot(uid,account_id,portfolio_id,strategy_id,strategy_code,actual_instrument_id,actual_code,trade_source,quantity_1e6,cost_amount_cents,market_value_cents,unrealized_pnl_cents,updated_at_utc) "
                        "VALUES('task-137-position',1,1,1,'TASK137',1,'159509','MANUAL',1000000,120000,125000,5000,'2026-05-30T00:03:00Z');");
    execute(connection, "INSERT INTO cash_snapshot(uid,account_id,portfolio_id,cash_balance_cents,principal_base_cents,updated_at_utc) "
                        "VALUES('task-137-cash',1,1,880000,1000000,'2026-05-30T00:04:00Z');");
    execute(connection, "INSERT INTO portfolio_summary(uid,account_id,portfolio_id,total_assets_cents,total_market_value_cents,cash_balance_cents,principal_base_cents,holding_cost_cents,total_pnl_cents,total_return_ppm,base_completion_ratio_ppm,sniper_pool_cents,active_grid_cycle_count,active_draft_count,base_violation_count,updated_at_utc) "
                        "VALUES('task-137-portfolio-summary',1,1,1005000,125000,880000,1000000,120000,5000,5000,200000,300000,1,0,0,'2026-05-30T00:05:00Z');");
    execute(connection, "INSERT INTO base_position_allocation(uid,trade_execution_group_id,trade_log_id,account_id,portfolio_id,strategy_id,strategy_code,actual_instrument_id,actual_code,allocation_type,target_base_amount_cents,allocated_amount_cents,created_at_utc) "
                        "VALUES('task-137-base',1,1,1,1,1,'TASK137',1,'159509','INITIAL_BASE',200000,120000,'2026-05-30T00:06:00Z');");
    execute(connection, "INSERT INTO trade_tier_allocation(uid,trade_execution_group_id,account_id,portfolio_id,strategy_id,strategy_code,grid_cycle_id,tier_no,tier_name,allocated_amount_cents,created_at_utc) "
                        "VALUES('task-137-tier',1,1,1,1,'TASK137',1,1,'T1',30000,'2026-05-30T00:07:00Z');");
}

void dropPositionSnapshotForQueryError(etfdt::data_access::SQLiteConnection& connection)
{
    execute(connection, "DROP TABLE position_snapshot;");
}

QJsonObject invokeReadOnlyAction(
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
    } else {
        std::cerr << "unknown action " << action << '\n';
        return {};
    }
    return payloadObject(response);
}

QJsonObject payloadObject(const etfdt::protocol::ProtocolResponse& response)
{
    if (!response.success) {
        std::cerr << "FAILED: ProtocolResponse success=false " << response.errorMessage << '\n';
        return {};
    }
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(response.payloadJson));
    if (!document.isObject()) {
        std::cerr << "FAILED: payload is not object: " << response.payloadJson << '\n';
        return {};
    }
    return document.object();
}

bool assertReadOnlySuccessPayload(
    const QJsonObject& payload,
    const std::string& action,
    const std::string& arrayField,
    const std::string& label)
{
    bool ok = true;
    ok &= expect(payload.value("action").toString().toStdString() == action, label + " action");
    ok &= expect(payload.value("implemented").toBool(false), label + " implemented=true");
    ok &= expect(payload.value("readOnly").toBool(false), label + " readOnly=true");
    ok &= expect(!payload.value("writeEnabled").toBool(true), label + " writeEnabled=false");
    ok &= expect(!payload.value("replayExecuted").toBool(true), label + " replayExecuted=false");
    ok &= expect(!payload.value("snapshotRebuilt").toBool(true), label + " snapshotRebuilt=false");
    ok &= expect(payload.value("sqliteAccessed").toBool(false), label + " sqliteAccessed=true");
    ok &= expect(!payload.value("accountingEngineCalled").toBool(true), label + " no AccountingEngine");
    ok &= expect(!payload.value("tradeDraftGenerated").toBool(true), label + " no TradeDraft");
    ok &= expect(payload.value("status").toString() == "OK", label + " status OK");
    ok &= expect(payload.value("hasRows").toBool(false), label + " hasRows=true");
    ok &= expect(!payload.value(QString::fromStdString(arrayField)).toArray().isEmpty(), label + " rows returned");
    const auto privacy = payload.value("privacy").toObject();
    ok &= expect(!privacy.value("rawSqlExposed").toBool(true), label + " no raw SQL");
    ok &= expect(!privacy.value("rawTradeLogPayloadExposed").toBool(true), label + " no raw trade_log");
    return ok;
}

bool assertEmptyPayload(
    const QJsonObject& payload,
    const std::string& action,
    const std::string& emptyIssueCode,
    const std::string& label)
{
    bool ok = true;
    ok &= expect(payload.value("action").toString().toStdString() == action, label + " action");
    ok &= expect(payload.value("status").toString() == "EMPTY", label + " status EMPTY");
    ok &= expect(payload.value("dataQualityStatus").toString() == "OK", label + " dataQuality OK");
    ok &= expect(!payload.value("hasRows").toBool(true), label + " hasRows=false");
    ok &= expect(hasIssue(payload, emptyIssueCode, false), label + " non-blocking empty issue");
    ok &= expect(!payload.value("writeEnabled").toBool(true), label + " writeEnabled=false");
    ok &= expect(!payload.value("snapshotRebuilt").toBool(true), label + " no snapshot rebuild");
    ok &= expect(!payload.value("accountingEngineCalled").toBool(true), label + " no AccountingEngine");
    return ok;
}

bool assertQueryErrorPayload(
    const QJsonObject& payload,
    const std::string& action,
    const std::string& errorIssueCode,
    const std::string& label)
{
    bool ok = true;
    ok &= expect(payload.value("action").toString().toStdString() == action, label + " action");
    ok &= expect(payload.value("status").toString() == "QUERY_ERROR", label + " status QUERY_ERROR");
    ok &= expect(payload.value("dataQualityStatus").toString() == "ERROR", label + " dataQuality ERROR");
    ok &= expect(hasIssue(payload, errorIssueCode, true), label + " blocking query error");
    ok &= expect(!payload.value("writeEnabled").toBool(true), label + " writeEnabled=false");
    ok &= expect(!payload.value("snapshotRebuilt").toBool(true), label + " no snapshot rebuild");
    ok &= expect(!payload.value("accountingEngineCalled").toBool(true), label + " no AccountingEngine");
    return ok;
}

}  // namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation
