#include "ShellAccountingTradeDraftConfirmationImplementation.h"

#include "DataServiceApi/DataServiceActions.h"
#include "ServiceRuntime/ActionContext.h"

#include <QJsonDocument>

#include <iostream>

namespace etfdt::tests::shell_accounting_tradedraft_confirmation {
namespace {

etfdt::protocol::MessageEnvelope envelope(std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-150-tradedraft-confirm";
    request.traceId = "task-150-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = etfdt::data_service_api::kActionAccountingTradeDraftConfirm;
    request.timestampUtc = "2026-05-30T12:03:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

QJsonObject parsePayload(const std::string& payload)
{
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(payload));
    return document.isObject() ? document.object() : QJsonObject {};
}

std::int64_t singleInt64(etfdt::data_access::SQLiteConnection& connection, const std::string& sql)
{
    auto rows = connection.queryRows(sql);
    if (!rows || rows.value().empty() || rows.value().front().empty()) {
        std::cerr << "query failed: " << sql << '\n';
        return 0;
    }
    return rows.value().front().front().int64Value;
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

std::string authorizedConfirmationPayload(std::int64_t draftId)
{
    return std::string(R"({"draftId":)") + std::to_string(draftId)
        + R"(,"accountId":"1001","portfolioId":"1001","strategyId":"1001","strategyCode":"TASK139","instrumentId":"1001","instrumentCode":"159509","side":"BUY","quantity1e6":1000000,"source":"existingTradeDraft","sourceSnapshotId":"task-144-portfolio_summary-1001-1001-CNY","authorization":"TASK-150_TRADEDRAFT_CONFIRM"})";
}

ConfirmationCall invokeTradeDraftConfirm(
    etfdt::data_access::SQLiteConnection& connection,
    std::string payloadJson)
{
    const auto context = etfdt::service_runtime::makeActionContext(
        envelope(std::move(payloadJson)),
        etfdt::protocol::ServiceName::ETFDataService);
    auto response = etfdt::data_service_api::handleAccountingTradeDraftConfirm(context, connection);
    return ConfirmationCall{response, parsePayload(response.payloadJson)};
}

ConfirmationCall createDraftAndConfirm(etfdt::data_access::SQLiteConnection& connection)
{
    const auto draft = invokeTradeDraftCreate(connection);
    if (!draft.response.success) {
        std::cerr << "failed to create draft fixture: " << draft.response.errorMessage << '\n';
    }
    return invokeTradeDraftConfirm(connection, authorizedConfirmationPayload(latestTradeDraftId(connection)));
}

bool expectSuccessfulConfirmation(const ConfirmationCall& call)
{
    bool ok = expect(call.response.success, "confirmation protocol success");
    ok &= expect(call.payload.value("action").toString().toStdString() == "accounting.tradedraft.confirm",
                 "confirmation action name");
    ok &= expect(call.payload.value("status").toString().toStdString() == "OK", "confirmation status OK");
    ok &= expect(call.payload.value("confirmationOnly").toBool(false), "confirmation only");
    ok &= expect(call.payload.value("executionGroupWritten").toBool(false), "execution group written");
    ok &= expect(call.payload.value("tradeLogWritten").toBool(false), "trade_log written");
    ok &= expect(call.payload.value("draftStatusUpdated").toBool(false), "draft status updated");
    ok &= expect(call.payload.value("auditWritten").toBool(false), "audit written");
    ok &= expect(call.payload.value("databaseWritten").toBool(false), "databaseWritten true");
    ok &= expect(!call.payload.value("partialWrite").toBool(true), "partialWrite false");
    ok &= expect(call.payload.value("transactionCommitted").toBool(false), "transaction committed");
    ok &= expect(!call.payload.value("brokerOrderSubmitted").toBool(true), "broker not called");
    ok &= expect(!call.payload.value("strategyExecuted").toBool(true), "strategy not executed");
    return ok;
}

bool expectDuplicateConfirmation(const ConfirmationCall& call)
{
    bool ok = expect(call.response.success, "duplicate confirmation protocol success");
    ok &= expect(call.payload.value("status").toString().toStdString() == "DUPLICATE", "duplicate status");
    ok &= expect(call.payload.value("duplicateConfirmation").toBool(false), "duplicateConfirmation true");
    ok &= expect(!call.payload.value("executionGroupWritten").toBool(true), "no duplicate group write");
    ok &= expect(!call.payload.value("tradeLogWritten").toBool(true), "no duplicate trade_log write");
    return ok;
}

std::int64_t latestTradeDraftId(etfdt::data_access::SQLiteConnection& connection)
{
    return singleInt64(connection, "SELECT id FROM trade_draft ORDER BY id DESC LIMIT 1;");
}

std::string latestTradeDraftStatus(etfdt::data_access::SQLiteConnection& connection)
{
    return singleText(connection, "SELECT status FROM trade_draft ORDER BY id DESC LIMIT 1;");
}

std::int64_t latestTradeExecutionGroupId(etfdt::data_access::SQLiteConnection& connection)
{
    return singleInt64(connection, "SELECT id FROM trade_execution_group ORDER BY id DESC LIMIT 1;");
}

std::int64_t latestTradeLogExecutionGroupId(etfdt::data_access::SQLiteConnection& connection)
{
    return singleInt64(connection, "SELECT trade_execution_group_id FROM trade_log ORDER BY id DESC LIMIT 1;");
}

std::string docs103Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "103_shell_accounting_tradedraft_confirmation_implementation.md");
}

}  // namespace etfdt::tests::shell_accounting_tradedraft_confirmation
