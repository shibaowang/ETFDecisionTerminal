#include "ShellServices/ShellAccountingIssueListModel.h"
#include "ShellServices/ShellAccountingStatusObject.h"
#include "ShellServices/ShellPositionListModel.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

void expectTrue(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

std::filesystem::path sourceRoot(int argc, char** argv)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == "--source-root") {
            return argv[i + 1];
        }
    }
    return std::filesystem::current_path();
}

std::string readFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

}  // namespace

int main(int argc, char** argv)
{
    etfdt::shell_services::ShellAccountingStatusObject status;
    expectTrue(status.readOnly(), "status object is read-only");
    expectTrue(!status.writeEnabled(), "status object is not write-enabled");

    etfdt::shell_services::ShellPositionListRow row;
    row.costAmountText = {"1000.00 CNY", "***"};
    row.dataQualityStatus = "OK";
    etfdt::shell_services::ShellPositionListModel positions;
    positions.setRows({row});
    expectTrue(positions.rowCount() == 1, "position model can expose read-only display rows");

    etfdt::shell_services::ShellAccountingIssueListModel issues;
    issues.setIssues({{"SELLABLE_ABOVE_BASE_AMOUNT_TEXT", "WARNING", "Display text is not a sell suggestion.", false, "base_position.summary"}});
    expectTrue(issues.rowCount() == 1, "no-trade explanatory issue can remain visible");

    const auto root = sourceRoot(argc, argv);
    const std::vector<std::filesystem::path> files{
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingStatusObject.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingStatusObject.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingIssueListModel.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingIssueListModel.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellPositionListModel.h",
        root / "libs" / "ShellServices" / "src" / "ShellPositionListModel.cpp",
    };

    for (const auto& file : files) {
        const auto text = readFile(file);
        expectTrue(text.find("buyAction") == std::string::npos, "ViewModel does not expose buy action");
        expectTrue(text.find("sellAction") == std::string::npos, "ViewModel does not expose sell action");
        expectTrue(text.find("createTradeDraft") == std::string::npos, "ViewModel does not expose createTradeDraft");
        expectTrue(text.find("brokerOrder") == std::string::npos, "ViewModel does not expose brokerOrder");
        expectTrue(text.find("strategyExecute") == std::string::npos, "ViewModel does not expose strategyExecute");
        expectTrue(text.find("TradeDraft") == std::string::npos, "ViewModel does not generate TradeDraft");
        expectTrue(text.find("trade suggestion") == std::string::npos, "ViewModel does not generate trade suggestion");
    }

    return 0;
}
