#include "ShellAccountingQmlTypeRegistrationImplementationTestUtils.h"

using namespace etfdt::tests::shell_accounting_qml_type_registration_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    expectTrue(
        !productionQmlContainsAny(root, productionQmlBindingTokens()),
        "production QML has no unapproved ShellAccounting binding or direct dependency tokens");

    const auto authorizedPage =
        root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml";
    const auto authorizedText = readTextFile(authorizedPage);
    expectTrue(
        authorizedText.find("import ETFDecisionTerminal.ShellAccounting 1.0") != std::string::npos,
        "production QML has one authorized ShellAccounting module import");
    expectTrue(
        authorizedText.find("property ShellAccountingPresenter accountingPresenter: null") != std::string::npos,
        "production QML has one nullable read-only presenter property");
    return 0;
}
