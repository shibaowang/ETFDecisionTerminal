#pragma once

#include "DataServiceClient/DataServiceClient.h"
#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"
#include "Transport/FrameCodec.h"

#include <QCoreApplication>
#include <QLocalServer>
#include <QLocalSocket>

#include <memory>
#include <string>
#include <vector>

namespace etfdt::shell_services::tests {

class FakeAccountingWrapperServer final {
public:
    enum class ResponseMode {
        GuardPayload,
        ProtocolFailure,
        MalformedResponse,
        NoResponse,
    };

    explicit FakeAccountingWrapperServer(ResponseMode mode = ResponseMode::GuardPayload);
    ~FakeAccountingWrapperServer();

    [[nodiscard]] bool listen();
    [[nodiscard]] bool connectClient(etfdt::data_service_client::DataServiceClient& client);
    [[nodiscard]] const std::string& socketName() const noexcept;
    [[nodiscard]] const std::vector<std::string>& actions() const noexcept;
    [[nodiscard]] const std::vector<std::string>& payloads() const noexcept;
    [[nodiscard]] bool waitForActionCount(std::size_t expectedCount, int timeoutMs = 1000);

private:
    void acceptPendingConnections();
    void readFromClient();
    void handleMessage(const std::string& json);
    void sendResponse(const std::string& json);

    ResponseMode mode_;
    std::string socketName_;
    QLocalServer server_;
    std::unique_ptr<QLocalSocket> clientSocket_;
    etfdt::transport::FrameCodec codec_;
    std::vector<std::string> actions_;
    std::vector<std::string> payloads_;
};

[[nodiscard]] ShellAccountingDataServiceClientRequest makePortAdapterRequest(
    std::string actionName,
    std::string payloadJson = R"({"accountId":"ACC-PORT","includeIssues":true})");

[[nodiscard]] std::shared_ptr<etfdt::data_service_client::DataServiceClient> makeConnectedClient(
    FakeAccountingWrapperServer& server);

[[nodiscard]] bool containsIssueCode(
    const ShellAccountingDataServiceClientResponse& response,
    const std::string& code);

}  // namespace etfdt::shell_services::tests
