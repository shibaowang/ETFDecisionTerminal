#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_broker_adapter_interface {
namespace {

bool hasExtension(const std::filesystem::path& path, const std::vector<std::string>& extensions)
{
    const auto extension = path.extension().string();
    for (const auto& allowed : extensions) {
        if (extension == allowed) {
            return true;
        }
    }
    return false;
}

}  // namespace

etfdt::dataservice::ShellAccountingBrokerOrderResponse
TestOnlyFakeShellAccountingBrokerOrderPort::submitOrderEnvelope(
    const etfdt::dataservice::ShellAccountingBrokerOrderRequest& request)
{
    ++callCount;
    auto response = nextResponse;
    response.sanitizedCorrelationId.clear();
    if (request.idempotencyKey.empty()) {
        response.status = etfdt::dataservice::ShellAccountingBrokerOrderStatus::Rejected;
        response.success = false;
        response.errorCode = "IDEMPOTENCY_KEY_MISSING";
        response.sanitizedMessage = "Idempotency key is required.";
    }
    return response;
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

std::string readTextFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions)
{
    std::vector<std::filesystem::path> result;
    if (!std::filesystem::exists(root)) {
        return result;
    }
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file() && hasExtension(entry.path(), extensions)) {
            result.push_back(entry.path());
        }
    }
    return result;
}

std::string joinFiles(const std::vector<std::filesystem::path>& files)
{
    std::string result;
    for (const auto& file : files) {
        result += "\n// FILE: " + file.generic_string() + "\n";
        result += readTextFile(file);
    }
    return result;
}

std::string brokerInterfaceText(const std::filesystem::path& root)
{
    return readTextFile(root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
                        "ShellAccountingBrokerOrderPort.h") +
        readTextFile(root / "libs" / "DataServiceApi" / "src" / "ShellAccountingBrokerOrderPort.cpp");
}

std::string qmlText(const std::filesystem::path& root)
{
    return joinFiles(filesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"}));
}

std::string shellServicesText(const std::filesystem::path& root)
{
    auto files = filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"});
    const auto shellCore = filesUnder(root / "libs" / "ShellCore", {".cpp", ".h", ".hpp"});
    files.insert(files.end(), shellCore.begin(), shellCore.end());
    return joinFiles(files);
}

std::string docs114Text(const std::filesystem::path& root)
{
    return readTextFile(root / "docs" / "114_shell_accounting_broker_adapter_interface_scaffold.md");
}

bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (text.find(token) != std::string::npos) {
            std::cerr << "unexpected token `" << token << "`\n";
            return true;
        }
    }
    return false;
}

bool containsAllTokens(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (text.find(token) == std::string::npos) {
            std::cerr << "missing token `" << token << "`\n";
            return false;
        }
    }
    return true;
}

bool expect(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << message << '\n';
        return false;
    }
    return true;
}

etfdt::dataservice::ShellAccountingBrokerOrderRequest validRequest()
{
    etfdt::dataservice::ShellAccountingBrokerOrderRequest request;
    request.accountId = "ACC-DEMO-001";
    request.portfolioId = "PF-DEMO-001";
    request.instrumentId = "ETF-001";
    request.symbol = "510300";
    request.side = "BUY";
    request.quantity = 100.0;
    request.reason = "test-only envelope";
    request.sourceTradeDraftId = "DRAFT-001";
    request.sourceLedgerId = "LEDGER-001";
    request.sourceDryRunId = "DRYRUN-001";
    request.userConfirmationId = "CONFIRM-001";
    request.idempotencyKey = "IDEMPOTENT-001";
    request.finalUserConfirmed = true;
    request.riskFlags = {"TEST_ONLY"};
    return request;
}

}  // namespace etfdt::tests::shell_accounting_broker_adapter_interface
