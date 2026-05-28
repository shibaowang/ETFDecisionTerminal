#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"
#include "ShellAccountingDataServiceClientPortAdapterTestUtils.h"

#include <QCoreApplication>

#include <iostream>
#include <memory>
#include <string_view>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    {
        auto client = std::make_shared<etfdt::data_service_client::DataServiceClient>();
        etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter adapter(client);
        const auto response = adapter.callPositionList(
            etfdt::shell_services::tests::makePortAdapterRequest("position.list"));
        expectTrue(!response.protocolSuccess, "disconnected client maps to failure");
        expectTrue(response.transportError, "disconnected client maps to transportError");
        expectTrue(etfdt::shell_services::tests::containsIssueCode(response, "DATASERVICE_CLIENT_CALL_FAILED"), "transport issue visible");
    }
    {
        etfdt::shell_services::tests::FakeAccountingWrapperServer server(
            etfdt::shell_services::tests::FakeAccountingWrapperServer::ResponseMode::ProtocolFailure);
        expectTrue(server.listen(), "protocol failure server listens");
        auto client = etfdt::shell_services::tests::makeConnectedClient(server);
        etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter adapter(client);
        const auto response = adapter.callPositionList(
            etfdt::shell_services::tests::makePortAdapterRequest("position.list"));
        expectTrue(!response.protocolSuccess, "ProtocolResponse.success=false maps");
        expectTrue(response.protocolError, "protocol failure maps protocolError");
    }
    {
        etfdt::shell_services::tests::FakeAccountingWrapperServer server(
            etfdt::shell_services::tests::FakeAccountingWrapperServer::ResponseMode::MalformedResponse);
        expectTrue(server.listen(), "malformed response server listens");
        auto client = etfdt::shell_services::tests::makeConnectedClient(server);
        etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter adapter(client);
        const auto response = adapter.callPositionList(
            etfdt::shell_services::tests::makePortAdapterRequest("position.list"));
        expectTrue(!response.protocolSuccess, "malformed response maps to failure");
        expectTrue(response.protocolError, "malformed response maps protocolError");
    }
    {
        etfdt::shell_services::tests::FakeAccountingWrapperServer server(
            etfdt::shell_services::tests::FakeAccountingWrapperServer::ResponseMode::NoResponse);
        expectTrue(server.listen(), "timeout server listens");
        auto client = etfdt::shell_services::tests::makeConnectedClient(server);
        etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter adapter(client);
        auto request = etfdt::shell_services::tests::makePortAdapterRequest("position.list");
        request.timeoutMs = 30;
        const auto response = adapter.callPositionList(request);
        expectTrue(!response.protocolSuccess, "timeout maps to failure");
        expectTrue(response.timeout, "timeout flag retained");
    }

    return gFailures == 0 ? 0 : 1;
}
