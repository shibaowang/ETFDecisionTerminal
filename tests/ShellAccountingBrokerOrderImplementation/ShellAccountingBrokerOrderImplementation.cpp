#include "ShellAccountingBrokerOrderImplementation.h"

#include "DataServiceApi/DataServiceActions.h"
#include "ServiceRuntime/ActionContext.h"

#include <QJsonDocument>

#include <iostream>

namespace etfdt::tests::shell_accounting_broker_order {
namespace {

etfdt::protocol::MessageEnvelope envelope(std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-154-broker-order";
    request.traceId = "task-154-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = etfdt::data_service_api::kActionAccountingBrokerOrderDryRun;
    request.timestampUtc = "2026-05-30T12:04:00Z";
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

std::string authorizedBrokerOrderDryRunPayload(std::int64_t draftId)
{
    return std::string(R"({"draftId":)") + std::to_string(draftId)
        + R"(,"accountId":"1001","portfolioId":"1001","source":"confirmedTradeDraft","userConfirmed":true,"authorization":"TASK-154_BROKER_ORDER_DRY_RUN"})";
}

BrokerOrderCall invokeBrokerOrderDryRun(
    etfdt::data_access::SQLiteConnection& connection,
    std::string payloadJson)
{
    const auto context = etfdt::service_runtime::makeActionContext(
        envelope(std::move(payloadJson)),
        etfdt::protocol::ServiceName::ETFDataService);
    auto response = etfdt::data_service_api::handleAccountingBrokerOrderDryRun(context, connection);
    return BrokerOrderCall{response, parsePayload(response.payloadJson)};
}

BrokerOrderCall createConfirmedTradeAndDryRun(etfdt::data_access::SQLiteConnection& connection)
{
    const auto confirmation = createDraftAndConfirm(connection);
    if (!confirmation.response.success) {
        std::cerr << "failed to create confirmed trade fixture: " << confirmation.response.errorMessage << '\n';
    }
    return invokeBrokerOrderDryRun(connection, authorizedBrokerOrderDryRunPayload(latestTradeDraftId(connection)));
}

bool expectSuccessfulBrokerOrderDryRun(const BrokerOrderCall& call)
{
    bool ok = expect(call.response.success, "broker dry-run protocol success");
    ok &= expect(call.payload.value("action").toString().toStdString() == "accounting.broker_order.dry_run",
                 "broker dry-run action name");
    ok &= expect(call.payload.value("status").toString().toStdString() == "DRY_RUN_ONLY",
                 "broker dry-run status");
    ok &= expect(call.payload.value("dryRun").toBool(false), "dryRun true");
    ok &= expect(call.payload.value("brokerDisabled").toBool(false), "brokerDisabled true");
    ok &= expect(call.payload.value("dryRunOnly").toBool(false), "dryRunOnly true");
    ok &= expect(!call.payload.value("realOrderPlacement").toBool(true), "no real order placement");
    ok &= expect(!call.payload.value("brokerSdkCalled").toBool(true), "broker SDK not called");
    ok &= expect(!call.payload.value("brokerOrderSubmitted").toBool(true), "broker order not submitted");
    ok &= expect(!call.payload.value("strategyExecuted").toBool(true), "strategy not executed");
    ok &= expect(!call.payload.value("automaticTrading").toBool(true), "automatic trading false");
    ok &= expect(call.payload.value("auditWritten").toBool(false), "audit written");
    ok &= expect(call.payload.value("databaseWritten").toBool(false), "databaseWritten true");
    ok &= expect(!call.payload.value("partialWrite").toBool(true), "partialWrite false");
    return ok;
}

bool expectDuplicateBrokerOrderDryRun(const BrokerOrderCall& call)
{
    bool ok = expect(call.response.success, "duplicate dry-run protocol success");
    ok &= expect(call.payload.value("status").toString().toStdString() == "DUPLICATE", "duplicate status");
    ok &= expect(call.payload.value("duplicateDryRun").toBool(false), "duplicateDryRun true");
    ok &= expect(!call.payload.value("auditWritten").toBool(true), "no duplicate audit write");
    ok &= expect(!call.payload.value("databaseWritten").toBool(true), "no duplicate database write");
    return ok;
}

std::string latestBrokerOrderAuditPayload(etfdt::data_access::SQLiteConnection& connection)
{
    return singleText(
        connection,
        "SELECT COALESCE(new_value_json, '{}') FROM audit_log "
        "WHERE entity_type = 'SHELL_ACCOUNTING_BROKER_ORDER_DRY_RUN' "
        "ORDER BY id DESC LIMIT 1;");
}

std::string docs109Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "109_shell_accounting_broker_order_dry_run_implementation.md");
}

std::string brokerOrderRepositoryText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataAccess" / "src" / "ShellAccountingBrokerOrderDryRunRepository.cpp");
}

std::vector<std::string> brokerSdkTokens()
{
    return {"BrokerAdapter", "BrokerSdk", "BrokerSDK", "brokerClient", "brokerApi"};
}

std::vector<std::string> realOrderPlacementTokens()
{
    return {"placeOrder(", "submitOrder(", "executeOrder(", "sendOrder(", "realOrderPlacement\":true"};
}

std::vector<std::string> strategyExecutionTokens()
{
    return {"StrategyEngine::execute", "strategyExecute(", "executeStrategy("};
}

std::vector<std::string> automaticTradingTokens()
{
    return {"automaticTradingEnabled", "AutomaticTrading", "auto_trade", "backgroundBrokerSubmission"};
}

std::vector<std::string> sensitivePayloadTokens()
{
    return {
        "rawSqlExposed\":true",
        "rawTradeLogPayloadExposed\":true",
        "fullTradeDraftPayloadExposed\":true",
        "fullBrokerPayloadExposed\":true",
        "internalStackExposed\":true",
        "stackTrace",
    };
}

}  // namespace etfdt::tests::shell_accounting_broker_order
