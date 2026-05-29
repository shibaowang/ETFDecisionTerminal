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

    for (const QString& typeName : forbiddenTypes()) {
        const QString errors = componentErrors(instantiateTypeQml(typeName));
        expectTrue(errors.contains(typeName), "forbidden type attempt names the missing type");
        expectTrue(!errors.contains(QString::fromUtf8(kReadOnlyReason)), "forbidden type is not registered as read-only");
    }

    return 0;
}
