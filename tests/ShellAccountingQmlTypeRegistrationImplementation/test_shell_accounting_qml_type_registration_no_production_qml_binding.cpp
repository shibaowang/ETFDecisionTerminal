#include "ShellAccountingQmlTypeRegistrationImplementationTestUtils.h"

using namespace etfdt::tests::shell_accounting_qml_type_registration_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    expectTrue(
        !productionQmlContainsAny(root, productionQmlBindingTokens()),
        "production QML has not imported or bound ShellAccounting module");
    return 0;
}
