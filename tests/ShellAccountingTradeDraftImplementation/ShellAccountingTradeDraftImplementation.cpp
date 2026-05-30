#include "ShellAccountingTradeDraftImplementation.h"

#include "DataServiceApi/DataServiceActions.h"
#include "ServiceRuntime/ActionContext.h"

#include <QJsonDocument>

#include <iostream>

namespace etfdt::tests::shell_accounting_tradedraft {
namespace {

etfdt::protocol::MessageEnvelope envelope(std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-148-tradedraft";
    request.traceId = "task-148-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = etfdt::data_service_api::kActionAccountingTradeDraftCreate;
    request.timestampUtc = "2026-05-30T12:02:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

QJsonObject parsePayload(const std::string& payload)
{
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(payload));
    return document.isObject() ? document.object() : QJsonObject {};
}

std::string singleText(etfdt::data_access::SQLiteConnection& connection, const std::string& sql)
{
    auto rows = connection.queryRows(sql);
    if (!rows || rows.value().empty() || rows.value().front().empty()) {
        std::cerr << "query failed: " << sql << '\n';
        return {};
    }
    return rows.value().front().front().text;
}

}  // namespace

std::string authorizedTradeDraftPayload()
{
    return R"({"accountId":"1001","portfolioId":"1001","strategyId":"1001","strategyCode":"TASK139","instrumentId":"1001","instrumentCode":"159509","side":"BUY","quantity1e6":1000000,"reason":"rebalance draft","source":"snapshotRebuildPreview","sourceSnapshotId":"task-144-portfolio_summary-1001-1001-CNY","riskFlags":{"checked":true,"blocked":false},"authorization":"TASK-148_TRADEDRAFT_WRITE"})";
}

TradeDraftCall invokeTradeDraftCreate(
    etfdt::data_access::SQLiteConnection& connection,
    std::string payloadJson)
{
    const auto context = etfdt::service_runtime::makeActionContext(
        envelope(std::move(payloadJson)),
        etfdt::protocol::ServiceName::ETFDataService);
    auto response = etfdt::data_service_api::handleAccountingTradeDraftCreate(context, connection);
    return TradeDraftCall{response, parsePayload(response.payloadJson)};
}

bool expectSuccessfulTradeDraft(const TradeDraftCall& call)
{
    bool ok = expect(call.response.success, "TradeDraft protocol success");
    ok &= expect(call.payload.value("action").toString().toStdString() == "accounting.tradedraft.create",
                 "TradeDraft action name");
    ok &= expect(call.payload.value("status").toString().toStdString() == "OK", "TradeDraft status OK");
    ok &= expect(call.payload.value("draftOnly").toBool(false), "TradeDraft is draft only");
    ok &= expect(call.payload.value("draftWritten").toBool(false), "draftWritten true");
    ok &= expect(call.payload.value("auditWritten").toBool(false), "auditWritten true");
    ok &= expect(call.payload.value("databaseWritten").toBool(false), "databaseWritten true");
    ok &= expect(!call.payload.value("partialWrite").toBool(true), "partialWrite false");
    ok &= expect(call.payload.value("transactionCommitted").toBool(false), "transaction committed");
    ok &= expect(!call.payload.value("tradeLogModified").toBool(true), "trade_log not modified");
    ok &= expect(!call.payload.value("tradeExecutionGroupModified").toBool(true),
                 "trade_execution_group not modified");
    ok &= expect(!call.payload.value("strategyExecuted").toBool(true), "strategy not executed");
    ok &= expect(!call.payload.value("brokerOrderSubmitted").toBool(true), "broker order not submitted");
    return ok;
}

bool expectDuplicateTradeDraft(const TradeDraftCall& call)
{
    bool ok = expect(call.response.success, "duplicate TradeDraft protocol success");
    ok &= expect(call.payload.value("status").toString().toStdString() == "DUPLICATE", "duplicate status");
    ok &= expect(call.payload.value("duplicateDraft").toBool(false), "duplicateDraft true");
    ok &= expect(!call.payload.value("draftWritten").toBool(true), "draftWritten false on duplicate");
    ok &= expect(!call.payload.value("auditWritten").toBool(true), "auditWritten false on duplicate");
    return ok;
}

std::string latestTradeDraftUid(etfdt::data_access::SQLiteConnection& connection)
{
    return singleText(connection, "SELECT uid FROM trade_draft ORDER BY id DESC LIMIT 1;");
}

std::string latestTradeDraftAction(etfdt::data_access::SQLiteConnection& connection)
{
    return singleText(connection, "SELECT action_type FROM trade_draft ORDER BY id DESC LIMIT 1;");
}

std::string latestTradeDraftAuditPayload(etfdt::data_access::SQLiteConnection& connection)
{
    return singleText(connection, "SELECT COALESCE(new_value_json, '{}') FROM audit_log ORDER BY id DESC LIMIT 1;");
}

std::string docs100Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "100_shell_accounting_tradedraft_implementation.md");
}

std::string shellBoundaryText(const std::filesystem::path& root)
{
    auto files = filesUnder(root / "apps" / "ETFDecisionShell", {".cpp", ".h", ".hpp", ".qml", ".js"});
    const auto shellServices = filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"});
    const auto shellCore = filesUnder(root / "libs" / "ShellCore", {".cpp", ".h", ".hpp"});
    files.insert(files.end(), shellServices.begin(), shellServices.end());
    files.insert(files.end(), shellCore.begin(), shellCore.end());
    return joinFiles(files);
}

std::string dataServiceActionsText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
}

std::string tradeDraftRepositoryText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataAccess" / "src" / "ShellAccountingTradeDraftRepository.cpp");
}

std::vector<std::string> brokerTokens()
{
    return {"brokerOrder(", "placeOrder", "executeOrder", "BrokerAdapter"};
}

std::vector<std::string> strategyTokens()
{
    return {"StrategyEngine::execute", "executeStrategy(", "strategyExecute(", "autoTrading"};
}

std::vector<std::string> shellDraftTriggerTokens()
{
    return {
        "ShellAccountingTradeDraftRepository",
        "createTradeDraft",
        "submitDraft",
    };
}

std::vector<std::string> sensitivePayloadTokens()
{
    return {
        "SELECT ",
        "INSERT INTO trade_log",
        "INSERT INTO trade_execution_group",
        "stackTrace",
        "internal stack",
        "fullSnapshotPayloadExposed\":true",
        "rawTradeLogPayloadExposed\":true",
        "rawSqlExposed\":true",
    };
}

}  // namespace etfdt::tests::shell_accounting_tradedraft
