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
    static_cast<void>(
        adapter.callPositionList(etfdt::shell_services::tests::makePortAdapterRequest("position.list")));
    static_cast<void>(
        adapter.callCashSummary(etfdt::shell_services::tests::makePortAdapterRequest("cash.summary")));
    static_cast<void>(adapter.callPortfolioPnlSummary(
        etfdt::shell_services::tests::makePortAdapterRequest("portfolio.pnl.summary")));
    static_cast<void>(adapter.callBasePositionSummary(
        etfdt::shell_services::tests::makePortAdapterRequest("base_position.summary")));
    static_cast<void>(adapter.callSniperPoolSummary(
        etfdt::shell_services::tests::makePortAdapterRequest("sniper_pool.summary")));

    expectTrue(server.waitForActionCount(5), "server observed all wrapper calls");
    const auto& actions = server.actions();
    expectTrue(actions.size() == 5, "exactly five wrapper calls");
    if (actions.size() == 5) {
        expectTrue(actions[0] == "position.list", "positionList maps to position.list");
        expectTrue(actions[1] == "cash.summary", "cashSummary maps to cash.summary");
        expectTrue(actions[2] == "portfolio.pnl.summary", "portfolioPnlSummary maps to portfolio.pnl.summary");
        expectTrue(actions[3] == "base_position.summary", "basePositionSummary maps to base_position.summary");
        expectTrue(actions[4] == "sniper_pool.summary", "sniperPoolSummary maps to sniper_pool.summary");
    }

    for (const auto& action : actions) {
        expectTrue(action != "data.audit.append", "data.audit.append not called");
        expectTrue(action.find("trade") == std::string::npos, "trade write action not called");
        expectTrue(action.find("draft") == std::string::npos, "draft action not called");
        expectTrue(action.find("snapshot") == std::string::npos, "snapshot write action not called");
        expectTrue(action.find("strategy") == std::string::npos, "strategy action not called");
        expectTrue(action.find("broker") == std::string::npos, "broker action not called");
    }

    return gFailures == 0 ? 0 : 1;
}
