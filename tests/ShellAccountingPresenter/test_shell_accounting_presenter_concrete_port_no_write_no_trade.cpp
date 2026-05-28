#include "ShellAccountingPresenterConcretePortTestUtils.h"

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
    expectTrue(server.listen(), "fake guard endpoint listens");
    const auto result =
        etfdt::shell_services::tests::refreshPresenterPositionListThroughConcretePort(server);

    expectTrue(result.refreshed, "presenter reaches guard wrapper");
    expectTrue(!etfdt::shell_services::tests::containsPresenterForbiddenActionText(result.observedAction), "no forbidden action invoked");
    expectTrue(result.observedAction == "position.list", "only read-only position.list action is invoked");
    expectTrue(result.presenterReadOnly, "presenter readOnly=true");
    expectTrue(!result.presenterWriteEnabled, "presenter writeEnabled=false");
    expectTrue(!result.tradeDraftEnabled, "generatedTradeDraft=false");
    expectTrue(!result.tradeSuggestionEnabled, "generatedTradeSuggestion=false");
    expectTrue(!result.strategyExecutionEnabled, "strategyExecutionEnabled=false");
    expectTrue(!result.brokerSubmissionEnabled, "brokerSubmissionEnabled=false");
    expectTrue(result.positionRowCount == 0, "no position rows are fabricated");

    return gFailures == 0 ? 0 : 1;
}
