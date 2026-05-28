#include "ShellServices/ShellAccountingDataServiceAdapter.h"
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

    etfdt::shell_services::tests::FakeAccountingWrapperServer server;
    expectTrue(server.listen(), "fake wrapper server listens");
    auto client = etfdt::shell_services::tests::makeConnectedClient(server);
    auto concretePort =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter>(client);
    etfdt::shell_services::ShellAccountingDataServiceAdapter adapter(concretePort);

    etfdt::shell_services::ShellAccountingServiceRequest request;
    request.actionName = "position.list";
    request.accountId = "ACC-ADAPTER";
    request.includeIssues = true;
    request.timeoutMs = 200;

    const auto result = adapter.fetchPositionList(request);
    expectTrue(server.waitForActionCount(1), "adapter reaches concrete port and real client wrapper");
    expectTrue(result.protocolSuccess, "guard payload protocol success");
    expectTrue(result.actionName == "position.list", "actionName maps");
    expectTrue(!result.implemented, "implemented=false retained");
    expectTrue(result.readOnly, "readOnly=true retained");
    expectTrue(!result.writeEnabled, "writeEnabled=false retained");
    expectTrue(result.payloadStatus == "POSITION_LIST_NOT_AVAILABLE", "guard status retained");
    expectTrue(!result.issues.empty(), "issue visible");
    expectTrue(!result.generatedTradeDraft, "no TradeDraft");
    expectTrue(!result.generatedTradeSuggestion, "no trade suggestion");

    return gFailures == 0 ? 0 : 1;
}
