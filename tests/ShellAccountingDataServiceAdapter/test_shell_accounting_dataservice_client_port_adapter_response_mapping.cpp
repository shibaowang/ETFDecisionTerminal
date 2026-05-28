#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"
#include "ShellAccountingDataServiceClientPortAdapterTestUtils.h"

#include <QCoreApplication>

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

void expectGuard(
    const etfdt::shell_services::ShellAccountingDataServiceClientResponse& response,
    const std::string& action,
    const std::string& status)
{
    expectTrue(response.protocolSuccess, "guard protocol success");
    expectTrue(response.actionName == action, "action maps from payload");
    expectTrue(!response.implemented, "implemented=false retained");
    expectTrue(response.readOnly, "readOnly=true retained");
    expectTrue(!response.writeEnabled, "writeEnabled=false retained");
    expectTrue(response.payloadStatus == status, "payload status retained");
    expectTrue(response.dataQualityStatus == "UNAVAILABLE", "dataQualityStatus retained");
    expectTrue(etfdt::shell_services::tests::containsIssueCode(response, status), "issue/warning/error code retained");
    expectTrue(response.rawPayload.find("unknownField") != std::string::npos, "rawPayload preserved");
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
    expectGuard(
        adapter.callPositionList(etfdt::shell_services::tests::makePortAdapterRequest("position.list")),
        "position.list",
        "POSITION_LIST_NOT_AVAILABLE");
    expectGuard(
        adapter.callCashSummary(etfdt::shell_services::tests::makePortAdapterRequest("cash.summary")),
        "cash.summary",
        "CASH_SUMMARY_NOT_AVAILABLE");
    expectGuard(
        adapter.callPortfolioPnlSummary(
            etfdt::shell_services::tests::makePortAdapterRequest("portfolio.pnl.summary")),
        "portfolio.pnl.summary",
        "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE");
    expectGuard(
        adapter.callBasePositionSummary(
            etfdt::shell_services::tests::makePortAdapterRequest("base_position.summary")),
        "base_position.summary",
        "BASE_POSITION_SUMMARY_NOT_AVAILABLE");
    expectGuard(
        adapter.callSniperPoolSummary(
            etfdt::shell_services::tests::makePortAdapterRequest("sniper_pool.summary")),
        "sniper_pool.summary",
        "SNIPER_POOL_SUMMARY_NOT_AVAILABLE");

    return gFailures == 0 ? 0 : 1;
}
