#include "ShellCore/ShellDiagnosticQtAdapter.h"
#include "ShellCore/ShellDiagnosticServiceListModel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QtQml>
#include <QMetaObject>

#include <filesystem>
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

void expectEqual(int actual, int expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

void expectEqual(QString actual, QString expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected.toStdString()
                  << " actual " << actual.toStdString() << '\n';
    }
}

}  // namespace

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<etfdt::shell::ShellDiagnosticQtAdapter>(
        "ETFDecisionTerminal.Shell",
        1,
        0,
        "ShellDiagnosticQtAdapter");

    etfdt::shell::ShellDiagnosticQtAdapter adapter;
    expectTrue(adapter.loadMockMixed(), "adapter loadMockMixed succeeds");
    expectTrue(adapter.serviceModel()->rowCount() > 0, "adapter serviceModel has rows");
    expectTrue(adapter.selectService(0), "adapter selectService does not fail");
    expectTrue(adapter.summaryObject()->totalServices() > 0, "summaryObject has totalServices");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("diagnosticAdapter", &adapter);

    const std::filesystem::path mainQmlPath = std::filesystem::path(ETFDT_SHELL_QML_DIR) / "Main.qml";
    expectTrue(std::filesystem::exists(mainQmlPath), "Main.qml exists");

    engine.load(QUrl::fromLocalFile(QString::fromStdString(mainQmlPath.string())));
    if (engine.rootObjects().isEmpty()) {
        expectTrue(false, "QQmlApplicationEngine loaded Main.qml");
    }

    QObject* root = engine.rootObjects().isEmpty() ? nullptr : engine.rootObjects().front();
    expectTrue(root != nullptr, "Main.qml root object exists");
    QObject* appShell = root == nullptr ? nullptr : root->findChild<QObject*>("appShell");
    expectTrue(appShell != nullptr, "AppShell exists");
    expectTrue(
        appShell == nullptr ? false : appShell->findChild<QObject*>("sideNavigation") != nullptr,
        "SideNavigation exists");
    expectTrue(
        appShell == nullptr ? false : appShell->findChild<QObject*>("contentHost") != nullptr,
        "ContentHost exists");

    if (appShell != nullptr) {
        expectEqual(appShell->property("currentPageKey").toString(), "dashboard", "default page is dashboard");
        expectTrue(
            appShell->findChild<QObject*>("placeholderPage") != nullptr,
            "default placeholder page is loaded");

        bool invoked = QMetaObject::invokeMethod(
            appShell,
            "navigateTo",
            Qt::DirectConnection,
            Q_ARG(QVariant, QVariant(QStringLiteral("diagnostics"))));
        QCoreApplication::processEvents();
        expectTrue(invoked, "navigateTo diagnostics succeeds");
        expectEqual(appShell->property("currentPageKey").toString(), "diagnostics", "current page is diagnostics");
        expectTrue(
            appShell->findChild<QObject*>("diagnosticsMockPage") != nullptr,
            "DiagnosticsMockPage is loaded");
        expectTrue(adapter.serviceModel()->rowCount() > 0, "diagnostics mock serviceModel still has rows");

        invoked = QMetaObject::invokeMethod(
            appShell,
            "navigateTo",
            Qt::DirectConnection,
            Q_ARG(QVariant, QVariant(QStringLiteral("market"))));
        QCoreApplication::processEvents();
        expectTrue(invoked, "navigateTo market succeeds");
        expectEqual(appShell->property("currentPageKey").toString(), "market", "current page is market");
        expectTrue(
            appShell->findChild<QObject*>("placeholderPage") != nullptr,
            "PlaceholderPage is loaded after navigation");
    }

    return gFailures == 0 ? 0 : 1;
}
