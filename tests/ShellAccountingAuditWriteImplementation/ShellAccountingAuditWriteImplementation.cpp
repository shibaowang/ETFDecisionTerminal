#include "ShellAccountingAuditWriteImplementation.h"

#include "DataServiceApi/DataServiceActions.h"
#include "ServiceRuntime/ActionContext.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_audit_write {
namespace {

etfdt::protocol::MessageEnvelope envelope(std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-146-audit-write";
    request.traceId = "task-146-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = etfdt::data_service_api::kActionAccountingAuditWrite;
    request.timestampUtc = "2026-05-30T12:01:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

QJsonObject parsePayload(const std::string& payload)
{
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(payload));
    return document.isObject() ? document.object() : QJsonObject {};
}

int intField(const QJsonObject& object, const char* name)
{
    return object.value(name).toInt(0);
}

}  // namespace

std::string authorizedAuditWritePayload()
{
    return R"({"accountId":"1001","portfolioId":"1001","source":"snapshotWriteResult","sourceAction":"accounting.snapshot.write","resultStatus":"OK","snapshotWritten":true,"snapshotBatchId":"task-144-1001-1001","authorization":"TASK-146_AUDIT_WRITE","sanitizedIssueCode":"NONE","rowsWritten":{"cash_snapshot":1,"position_snapshot":2,"portfolio_summary":1}})";
}

std::string auditWritePayloadFromSnapshot(
    const etfdt::tests::shell_accounting_snapshot_write::SnapshotWriteCall& snapshotWrite)
{
    const auto rows = snapshotWrite.payload.value("rowsWritten").toObject();
    const auto status = snapshotWrite.payload.value("status").toString("UNKNOWN").toStdString();
    std::ostringstream stream;
    stream << "{"
           << "\"accountId\":\"1001\","
           << "\"portfolioId\":\"1001\","
           << "\"source\":\"snapshotWriteResult\","
           << "\"sourceAction\":\"accounting.snapshot.write\","
           << "\"resultStatus\":\"" << status << "\","
           << "\"snapshotWritten\":"
           << (snapshotWrite.payload.value("snapshotWritten").toBool(false) ? "true" : "false") << ','
           << "\"snapshotBatchId\":\"task-144-1001-1001\","
           << "\"authorization\":\"TASK-146_AUDIT_WRITE\","
           << "\"sanitizedIssueCode\":\"NONE\","
           << "\"rowsWritten\":{"
           << "\"cash_snapshot\":" << intField(rows, "cash_snapshot") << ','
           << "\"position_snapshot\":" << intField(rows, "position_snapshot") << ','
           << "\"portfolio_summary\":" << intField(rows, "portfolio_summary")
           << "}"
           << "}";
    return stream.str();
}

AuditWriteCall invokeAuditWrite(
    etfdt::data_access::SQLiteConnection& connection,
    std::string payloadJson)
{
    const auto context = etfdt::service_runtime::makeActionContext(
        envelope(std::move(payloadJson)),
        etfdt::protocol::ServiceName::ETFDataService);
    auto response = etfdt::data_service_api::handleAccountingAuditWrite(context, connection);
    return AuditWriteCall{response, parsePayload(response.payloadJson)};
}

bool expectSuccessfulAuditWrite(const AuditWriteCall& call)
{
    bool ok = expect(call.response.success, "audit write protocol success");
    ok &= expect(call.payload.value("action").toString().toStdString() == "accounting.audit.write",
                 "audit write action name");
    ok &= expect(call.payload.value("status").toString().toStdString() == "OK", "audit write status");
    ok &= expect(call.payload.value("auditWritten").toBool(false), "auditWritten true");
    ok &= expect(call.payload.value("databaseWritten").toBool(false), "databaseWritten true");
    ok &= expect(!call.payload.value("partialAuditWrite").toBool(true), "partialAuditWrite false");
    ok &= expect(call.payload.value("transactionCommitted").toBool(false), "transaction committed");
    ok &= expect(call.payload.value("idempotent").toBool(false), "idempotent true");
    ok &= expect(!call.payload.value("duplicateEvent").toBool(true), "duplicateEvent false");
    return ok;
}

bool expectDuplicateAuditWrite(const AuditWriteCall& call)
{
    bool ok = expect(call.response.success, "duplicate audit protocol success");
    ok &= expect(call.payload.value("status").toString().toStdString() == "DUPLICATE", "duplicate status");
    ok &= expect(call.payload.value("duplicateEvent").toBool(false), "duplicateEvent true");
    ok &= expect(!call.payload.value("auditWritten").toBool(true), "auditWritten false on duplicate");
    ok &= expect(!call.payload.value("databaseWritten").toBool(true), "databaseWritten false on duplicate");
    return ok;
}

std::string latestAuditNewValueJson(etfdt::data_access::SQLiteConnection& connection)
{
    auto rows = connection.queryRows("SELECT COALESCE(new_value_json, '{}') FROM audit_log ORDER BY id DESC LIMIT 1;");
    if (!rows || rows.value().empty() || rows.value().front().empty()) {
        std::cerr << "latest audit payload query failed\n";
        return {};
    }
    return rows.value().front().front().text;
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

std::string auditWriteRepositoryText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataAccess" / "src" / "ShellAccountingAuditWriteRepository.cpp");
}

std::string docs97Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "97_shell_accounting_audit_write_implementation.md");
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

}  // namespace etfdt::tests::shell_accounting_audit_write
