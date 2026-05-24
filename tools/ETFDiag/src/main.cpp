#include "Diagnostics/WatchdogReportParser.h"

#include <QCoreApplication>

#include <iostream>
#include <string>
#include <vector>

namespace {

void printHelp()
{
    std::cout
        << "ETFDiag diagnostic utility\n"
        << "\n"
        << "Usage:\n"
        << "  ETFDiag --watchdog-report <path>\n"
        << "  ETFDiag --help\n"
        << "\n"
        << "Consumes a Watchdog manifest status JSON report and prints a diagnostic summary.\n";
}

bool hasOption(const std::vector<std::string>& args, const std::string& option)
{
    for (const auto& arg : args) {
        if (arg == option) {
            return true;
        }
    }
    return false;
}

std::string optionValue(const std::vector<std::string>& args, const std::string& option)
{
    for (std::size_t i = 0; i + 1 < args.size(); ++i) {
        if (args[i] == option) {
            return args[i + 1];
        }
    }
    return {};
}

int consumeWatchdogReport(const std::string& reportPath)
{
    if (reportPath.empty()) {
        std::cerr << "--watchdog-report is required\n";
        return 1;
    }

    const auto parsed = etfdt::diagnostics::WatchdogReportParser::parseFromFile(reportPath);
    if (!parsed.ok) {
        std::cerr << parsed.errorMessage << '\n';
        return 1;
    }

    const auto summary = etfdt::diagnostics::WatchdogReportParser::summarize(parsed.report);
    std::cout << summary.text;
    return summary.exitCode;
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    if (args.empty() || hasOption(args, "--help")) {
        printHelp();
        return 0;
    }

    if (hasOption(args, "--watchdog-report")) {
        return consumeWatchdogReport(optionValue(args, "--watchdog-report"));
    }

    printHelp();
    return 1;
}
