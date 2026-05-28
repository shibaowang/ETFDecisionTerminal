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
    etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter adapter(client);

    const auto response = adapter.callBasePositionSummary(
        etfdt::shell_services::tests::makePortAdapterRequest("base_position.summary"));
    expectTrue(response.readOnly, "response remains readOnly");
    expectTrue(!response.writeEnabled, "response remains write disabled");
    expectTrue(server.waitForActionCount(1), "server observed one call");
    expectTrue(server.actions().front() == "base_position.summary", "only read-only wrapper called");
    expectTrue(response.rawPayload.find("\"tradeDraftGenerated\":true") == std::string::npos, "no TradeDraft generated");
    expectTrue(response.rawPayload.find("\"tradeSuggestionGenerated\":true") == std::string::npos, "no trade suggestion generated");
    expectTrue(response.rawPayload.find("\"strategyExecuted\":true") == std::string::npos, "no strategy executed");
    expectTrue(response.rawPayload.find("broker") == std::string::npos, "no broker order submitted");

    return gFailures == 0 ? 0 : 1;
}
