#include "ShellAccountingQmlTypeRegistrationImplementationTestUtils.h"

#include <filesystem>

using namespace etfdt::tests::shell_accounting_qml_type_registration_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const std::vector<std::filesystem::path> files{
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingStatusObject.h",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingIssueListModel.h",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellPositionListModel.h",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingQmlRegistration.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingQmlRegistration.cpp",
    };

    for (const auto& file : files) {
        expectTrue(
            !textContainsAny(readTextFile(file), forbiddenMethodTokens()),
            "allowlist registration surface does not expose forbidden methods");
    }

    return 0;
}
