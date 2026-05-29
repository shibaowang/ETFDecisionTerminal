#include "ShellAccountingQmlTypeRegistrationImplementationTestUtils.h"

#include "ShellServices/ShellAccountingQmlRegistration.h"

#include <QCoreApplication>
#include <QQmlComponent>
#include <QQmlEngine>

using namespace etfdt::tests::shell_accounting_qml_type_registration_implementation;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    (void)app;

    expectTrue(
        etfdt::shell_services::registerShellAccountingQmlTypes(),
        "ShellAccounting QML type registration succeeds");

    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.setData(importOnlyQml().toUtf8(), QUrl());
    expectTrue(component.status() == QQmlComponent::Ready, "ShellAccounting QML module imports");

    std::unique_ptr<QObject> object(component.create());
    expectTrue(object != nullptr, "import-only QML object can be created");
    expectTrue(object->property("imported").toBool(), "module import marker is readable");
    return 0;
}
