#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"
#include "ShellAccountingDataServiceClientPortAdapterTestUtils.h"

#include <QCoreApplication>

#include <iostream>
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

    etfdt::shell_services::tests::FakeAccountingWrapperServer server;
    expectTrue(server.listen(), "fake wrapper server listens");
    auto client = etfdt::shell_services::tests::makeConnectedClient(server);
    expectTrue(static_cast<bool>(client), "real DataServiceClient connects to fake server");

    etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter adapter(client);
    const std::string payload =
        R"({"accountId":"ACC-REQ","portfolioId":"PF-REQ","includeIssues":true,"requestedOutputs":["positions"]})";
    auto request = etfdt::shell_services::tests::makePortAdapterRequest("position.list", payload);
    request.timeoutMs = 300;
    static_cast<void>(adapter.callPositionList(request));

    expectTrue(server.waitForActionCount(1), "server observed request");
    expectTrue(!server.payloads().empty(), "server captured payload");
    if (!server.payloads().empty()) {
        const auto& captured = server.payloads().front();
        expectTrue(captured.find("\"accountId\":\"ACC-REQ\"") != std::string::npos, "payloadJson accountId preserved");
        expectTrue(captured.find("\"portfolioId\":\"PF-REQ\"") != std::string::npos, "payloadJson portfolioId preserved");
        expectTrue(captured.find("TradeDraft") == std::string::npos, "payload does not add TradeDraft");
        expectTrue(captured.find("tradeSuggestion") == std::string::npos, "payload does not add trade suggestion");
        expectTrue(captured.find("strategy") == std::string::npos, "payload does not add strategy");
        expectTrue(captured.find("broker") == std::string::npos, "payload does not add broker");
        expectTrue(captured.find("\"writeEnabled\":true") == std::string::npos, "payload does not add writeEnabled=true");
    }

    return gFailures == 0 ? 0 : 1;
}
