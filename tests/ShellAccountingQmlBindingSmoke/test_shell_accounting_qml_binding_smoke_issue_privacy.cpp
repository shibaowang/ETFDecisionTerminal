#include "ShellAccountingQmlSmokeScenario.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_binding_smoke;

int main()
{
    const auto warning = warningIssueScenario();
    if (warning.issues.empty()) {
        std::cerr << "Warning issue scenario must expose an issue.\n";
        return 1;
    }
    const auto& issue = warning.issues.front();
    if (issue.code.empty() || issue.level.empty() || issue.message.empty()) {
        std::cerr << "Issue code, level, and message must be visible.\n";
        return 1;
    }
    if (!warning.expectedIssueVisible) {
        std::cerr << "Warning issue must not be hidden.\n";
        return 1;
    }

    const auto raw = privacyScenario(false);
    const auto masked = privacyScenario(true);
    if (raw.expectedDisplayTextPolicy != "displayText=rawText") {
        std::cerr << "privacyMode=false must display raw displayText policy.\n";
        return 1;
    }
    if (masked.expectedDisplayTextPolicy != "displayText=maskedText") {
        std::cerr << "privacyMode=true must display masked displayText policy.\n";
        return 1;
    }
    if (masked.notes.find("rawText") == std::string::npos) {
        std::cerr << "Privacy descriptor must preserve rawText ownership outside QML.\n";
        return 1;
    }

    for (const auto& guard : guardPayloadScenarios()) {
        if (guard.expectedDisplayTextPolicy != "unavailable-does-not-fake-data") {
            std::cerr << "Unavailable guard must not fake amount text.\n";
            return 1;
        }
    }
    return 0;
}
