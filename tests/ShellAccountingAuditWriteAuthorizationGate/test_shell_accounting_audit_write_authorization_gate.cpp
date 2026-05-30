#include "ShellAccountingAuditWriteAuthorizationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_audit_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs95 = readTextFile(docs95Path(root));
    const auto docs96 = readTextFile(docs96Path(root));
    if (docs95.empty() || docs96.empty()) {
        std::cerr << "docs/95 and docs/96 must exist\n";
        return 1;
    }

    for (const auto& task : taskPrerequisiteTokens()) {
        if (docs95.find(task) == std::string::npos) {
            std::cerr << "docs/95 missing prerequisite " << task << '\n';
            return 1;
        }
    }

    for (const std::string token : {
             "TASK-145",
             "audit write implementation must be a separate TASK",
             "explicit user authorization",
             "audit write must go through the DataService boundary",
             "must not become a generic write escape hatch",
         }) {
        if (docs95.find(token) == std::string::npos) {
            std::cerr << "docs/95 missing `" << token << "`\n";
            return 1;
        }
    }

    if (docs96.find("Test Matrix") == std::string::npos) {
        std::cerr << "docs/96 missing Test Matrix\n";
        return 1;
    }
    return 0;
}
