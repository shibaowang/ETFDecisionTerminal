#pragma once

#include "DataAccess/DataAccess.h"
#include "Protocol/Protocol.h"

#include <QJsonObject>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::filesystem::path migrationPath(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::filesystem::path docs83Path(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path docs81Path(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path docs82Path(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path dataServiceActionsPath(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path readOnlyFactsQuerySourcePath(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> shellAccountingProductionBoundaryFiles(
    const std::filesystem::path& root);
[[nodiscard]] bool containsTokenInFiles(
    const std::vector<std::filesystem::path>& files,
    const std::string& token);
[[nodiscard]] int countTokenInText(const std::string& text, const std::string& token);
[[nodiscard]] std::vector<std::string> authorizedActions();
[[nodiscard]] std::vector<std::string> forbiddenWriteTokens();
[[nodiscard]] std::vector<std::string> forbiddenReplayTokens();
[[nodiscard]] std::vector<std::string> privacyTokens();

[[nodiscard]] std::unique_ptr<etfdt::data_access::SQLiteConnection> openMigratedDatabase(
    const std::filesystem::path& migrationPath);
void seedReadOnlyFactsFixture(etfdt::data_access::SQLiteConnection& connection);
void dropPositionSnapshotForQueryError(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] QJsonObject invokeReadOnlyAction(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& action,
    std::string payloadJson = "{}");
[[nodiscard]] QJsonObject payloadObject(const etfdt::protocol::ProtocolResponse& response);
bool assertReadOnlySuccessPayload(
    const QJsonObject& payload,
    const std::string& action,
    const std::string& arrayField,
    const std::string& label);
bool assertEmptyPayload(
    const QJsonObject& payload,
    const std::string& action,
    const std::string& emptyIssueCode,
    const std::string& label);
bool assertQueryErrorPayload(
    const QJsonObject& payload,
    const std::string& action,
    const std::string& errorIssueCode,
    const std::string& label);

}  // namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation
