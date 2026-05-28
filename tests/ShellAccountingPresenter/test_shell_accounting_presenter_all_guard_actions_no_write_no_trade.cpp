#include "ShellAccountingPresenterConcretePortTestUtils.h"

#include <QCoreApplication>

#include <algorithm>
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
        etfdt::shell_services::tests::refreshPresenterAllReadOnlyThroughConcretePort(server);

    expectTrue(result.refreshed, "refreshAllReadOnly reaches guard wrappers");
    expectTrue(!result.tradeDraftEnabled, "TradeDraft generation remains disabled");
    expectTrue(!result.tradeSuggestionEnabled, "trade suggestion remains disabled");
    expectTrue(!result.strategyExecutionEnabled, "strategy execution remains disabled");
    expectTrue(!result.brokerSubmissionEnabled, "broker order remains disabled");
    expectTrue(
        std::none_of(
            result.observedActions.begin(),
            result.observedActions.end(),
            etfdt::shell_services::tests::containsPresenterForbiddenActionText),
        "no forbidden write/trade/draft/snapshot/strategy/broker action is called");
    expectTrue(
        result.observedActions.size() == 5,
        "only five read-only guard actions are called");
    expectTrue(result.positionRowCount == 0, "refreshAllReadOnly does not fabricate position rows");

    return gFailures == 0 ? 0 : 1;
}
