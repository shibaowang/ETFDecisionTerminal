#include "ShellAccountingDataServiceAdapterLiveCallGate.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
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

std::filesystem::path optionValue(int argc, char* argv[], const std::string& option)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == option) {
            return argv[i + 1];
        }
    }
    return {};
}

std::string readFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

bool contains(const std::vector<std::string>& values, const std::string& value)
{
    return std::find(values.begin(), values.end(), value) != values.end();
}

bool containsInclude(const std::string& text, const std::string& name)
{
    return text.find("#include \"" + name) != std::string::npos ||
           text.find("#include <" + name) != std::string::npos;
}

}  // namespace

int main(int argc, char* argv[])
{
    using namespace etfdt::tests::shell_accounting_dataservice_adapter_live_call_gate;

    const auto root = optionValue(argc, argv, "--source-root");
    expectTrue(!root.empty(), "--source-root argument is required");

    const auto denylist = liveCallForbiddenDenylist();
    for (const auto& forbidden : {
             "data.audit.append",
             "trade.write",
             "snapshot.write",
             "trade_draft.create",
             "strategy.execute",
             "broker.order.submit",
             "SQLite direct access",
             "AccountingEngine direct call",
         }) {
        expectTrue(contains(denylist, forbidden), "no-write gate contains forbidden path");
    }

    const auto adapterHeader =
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceAdapter.h";
    const auto adapterSource =
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceAdapter.cpp";
    const auto adapterText = readFile(adapterHeader) + '\n' + readFile(adapterSource);

    for (const auto& forbidden : {"DataServiceClient", "DataServiceApi", "DataAccess", "AccountingEngine", "SQLite", "QtQuick", "QML"}) {
        expectTrue(!containsInclude(adapterText, forbidden), "current skeleton still avoids forbidden dependency include");
    }

    for (const auto& forbiddenCall : {
             "DataServiceClient::positionList",
             "DataServiceClient::cashSummary",
             "DataServiceClient::portfolioPnlSummary",
             "DataServiceClient::basePositionSummary",
             "DataServiceClient::sniperPoolSummary",
             "data.audit.append",
             "trade_draft",
             "snapshot.write",
             "strategy.execute",
             "broker.order",
         }) {
        expectTrue(adapterText.find(forbiddenCall) == std::string::npos, "current skeleton still avoids forbidden call");
    }

    return gFailures == 0 ? 0 : 1;
}
