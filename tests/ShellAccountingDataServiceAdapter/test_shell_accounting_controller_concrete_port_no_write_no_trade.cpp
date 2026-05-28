#include "ShellAccountingControllerConcretePortIntegrationTestUtils.h"

#include <QCoreApplication>

#include <array>
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

bool containsForbiddenActionText(const std::string& action)
{
    return action == "data.audit.append" ||
        action.find("trade") != std::string::npos ||
        action.find("draft") != std::string::npos ||
        action.find("snapshot") != std::string::npos ||
        action.find("strategy") != std::string::npos ||
        action.find("broker") != std::string::npos;
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    constexpr std::array<const char*, 5> kActions{
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
        "base_position.summary",
        "sniper_pool.summary",
    };

    for (const auto* action : kActions) {
        etfdt::shell_services::tests::FakeAccountingWrapperServer server;
        expectTrue(server.listen(), "fake guard endpoint listens");
        const auto result =
            etfdt::shell_services::tests::refreshControllerThroughConcretePort(server, action);

        expectTrue(result.refreshed, "controller refresh reaches guard wrapper");
        expectTrue(!containsForbiddenActionText(result.observedAction), "no forbidden action invoked");
        expectTrue(result.controllerReadOnly, "controller readOnly=true");
        expectTrue(!result.controllerWriteEnabled, "controller writeEnabled=false");
        expectTrue(!result.tradeDraftEnabled, "no TradeDraft generation");
        expectTrue(!result.tradeSuggestionEnabled, "no trade suggestion generation");
        expectTrue(!result.strategyExecutionEnabled, "no strategy execution");
        expectTrue(!result.brokerOrderEnabled, "no broker order");
    }

    return gFailures == 0 ? 0 : 1;
}
