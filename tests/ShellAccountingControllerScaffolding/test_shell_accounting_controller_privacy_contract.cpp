#include "ShellAccountingControllerScenario.h"
#include "ShellAccountingStateMappingTestUtils.h"

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

int main()
{
    constexpr std::string_view kRawAmount = "1000.00 CNY";
    constexpr std::string_view kMaskedAmount = "***";

    expectTrue(
        etfdt::tests::shell_accounting::displayTextForPrivacy(
            std::string(kRawAmount),
            std::string(kMaskedAmount),
            false,
            "Loaded") == kRawAmount,
        "privacyMode=false exposes raw display text");
    expectTrue(
        etfdt::tests::shell_accounting::displayTextForPrivacy(
            std::string(kRawAmount),
            std::string(kMaskedAmount),
            true,
            "Loaded") == kMaskedAmount,
        "privacyMode=true exposes masked display text");
    expectTrue(
        etfdt::tests::shell_accounting::displayTextForPrivacy(
            std::string(kRawAmount),
            std::string(kMaskedAmount),
            true,
            "Unavailable")
            .empty(),
        "Unavailable does not fabricate masked amount");
    expectTrue(
        etfdt::tests::shell_accounting::displayTextForPrivacy(
            std::string(kRawAmount),
            std::string(kMaskedAmount),
            true,
            "Empty")
            .empty(),
        "Empty does not fabricate amount");

    const auto issues = etfdt::tests::shell_accounting::issueContractScenarios();
    expectTrue(etfdt::tests::shell_accounting::hasIssueCode(issues, "MARKET_PRICE_MISSING"), "warning issue remains visible in privacy mode");
    expectTrue(etfdt::tests::shell_accounting::hasIssueCode(issues, "NEGATIVE_CASH"), "error issue remains visible in privacy mode");

    return gFailures == 0 ? 0 : 1;
}
