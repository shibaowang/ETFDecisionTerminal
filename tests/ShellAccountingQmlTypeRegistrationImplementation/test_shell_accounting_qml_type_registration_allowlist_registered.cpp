#include "ShellAccountingQmlTypeRegistrationImplementationTestUtils.h"

#include "ShellServices/ShellAccountingQmlRegistration.h"

#include <QCoreApplication>

using namespace etfdt::tests::shell_accounting_qml_type_registration_implementation;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    (void)app;

    expectTrue(
        etfdt::shell_services::registerShellAccountingQmlTypes(),
        "ShellAccounting QML type registration succeeds");

    for (const QString& typeName : allowlistTypes()) {
        const QString errors = componentErrors(instantiateTypeQml(typeName));
        expectTrue(errors.contains(QString::fromUtf8(kReadOnlyReason)), "allowlist type is registered as uncreatable read-only");
        expectTrue(!errors.contains(QStringLiteral("is not a type")), "allowlist type is known to the module");
    }

    return 0;
}
