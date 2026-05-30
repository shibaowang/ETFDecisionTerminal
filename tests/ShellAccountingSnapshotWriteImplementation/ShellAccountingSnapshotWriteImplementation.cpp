#include "ShellAccountingSnapshotWriteImplementation.h"

#include "DataServiceApi/DataServiceActions.h"
#include "ServiceRuntime/ActionContext.h"

#include <QJsonDocument>

#include <iostream>

namespace etfdt::tests::shell_accounting_snapshot_write {
namespace {

etfdt::protocol::MessageEnvelope envelope(std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-144-snapshot-write";
    request.traceId = "task-144-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = etfdt::data_service_api::kActionAccountingSnapshotWrite;
    request.timestampUtc = "2026-05-30T12:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

QJsonObject parsePayload(const std::string& payload)
{
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(payload));
    return document.isObject() ? document.object() : QJsonObject {};
}

}  // namespace

std::string authorizedSnapshotWritePayload()
{
    return R"({"accountId":"1001","portfolioId":"1001","calculationMode":"readonlyReplay","source":"snapshotRebuildPreview","authorization":"TASK-144_SNAPSHOT_WRITE"})";
}

SnapshotWriteCall invokeSnapshotWrite(
    etfdt::data_access::SQLiteConnection& connection,
    std::string payloadJson)
{
    const auto context = etfdt::service_runtime::makeActionContext(
        envelope(std::move(payloadJson)),
        etfdt::protocol::ServiceName::ETFDataService);
    auto response = etfdt::data_service_api::handleAccountingSnapshotWrite(context, connection);
    return SnapshotWriteCall{response, parsePayload(response.payloadJson)};
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& tableName)
{
    auto count = connection.querySingleInt("SELECT COUNT(*) FROM " + tableName + ";");
    if (!count) {
        std::cerr << "count failed for " << tableName << ": " << count.error().message << '\n';
        return -1;
    }
    return count.value();
}

std::int64_t querySingleInt64(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& sql)
{
    auto rows = connection.queryRows(sql);
    if (!rows || rows.value().empty() || rows.value().front().empty()) {
        std::cerr << "query failed: " << sql << '\n';
        return -1;
    }
    return rows.value().front().front().int64Value;
}

bool expect(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
    }
    return condition;
}

bool expectSuccessfulSnapshotWrite(const SnapshotWriteCall& call)
{
    bool ok = expect(call.response.success, "snapshot write protocol success");
    ok &= expect(call.payload.value("action").toString().toStdString() == "accounting.snapshot.write",
                 "snapshot write action name");
    ok &= expect(call.payload.value("status").toString().toStdString() == "OK", "snapshot write status");
    ok &= expect(call.payload.value("databaseWritten").toBool(false), "databaseWritten true");
    ok &= expect(call.payload.value("snapshotWritten").toBool(false), "snapshotWritten true");
    ok &= expect(!call.payload.value("partialWrite").toBool(true), "partialWrite false");
    ok &= expect(call.payload.value("transactionCommitted").toBool(false), "transaction committed");
    ok &= expect(call.payload.value("idempotent").toBool(false), "idempotent upsert");
    return ok;
}

std::string docs94Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "94_shell_accounting_snapshot_write_implementation.md");
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

std::string dataAccessSnapshotWriteText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataAccess" / "src" / "ShellAccountingSnapshotWriteRepository.cpp");
}

}  // namespace etfdt::tests::shell_accounting_snapshot_write
