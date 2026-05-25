#include "ShellCore/ShellDiagnosticQtAdapter.h"
#include "ShellCore/ShellDiagnosticServiceListModel.h"
#include "ShellCore/ShellNavigationController.h"
#include "ShellCore/ShellStatusController.h"

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

void processQmlEvents()
{
    for (int i = 0; i < 10; ++i) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
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
    qmlRegisterType<etfdt::shell::ShellNavigationController>(
        "ETFDecisionTerminal.Shell",
        1,
        0,
        "ShellNavigationController");

    etfdt::shell::ShellDiagnosticQtAdapter adapter;
    etfdt::shell::ShellNavigationController navigationController;
    etfdt::shell::ShellStatusController statusController;
    expectTrue(adapter.loadMockMixed(), "adapter loadMockMixed succeeds");
    expectTrue(adapter.serviceModel()->rowCount() > 0, "adapter serviceModel has rows");
    expectTrue(adapter.selectService(0), "adapter selectService does not fail");
    expectTrue(adapter.summaryObject()->totalServices() > 0, "summaryObject has totalServices");
    expectEqual(navigationController.navigationModel()->rowCount(), 13, "navigation model has 13 rows");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("diagnosticAdapter", &adapter);
    engine.rootContext()->setContextProperty("shellNavigationController", &navigationController);
    engine.rootContext()->setContextProperty("shellStatusController", &statusController);

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
    expectTrue(
        appShell == nullptr ? false : appShell->findChild<QObject*>("topStatusBar") != nullptr,
        "TopStatusBar exists");
    expectTrue(
        appShell == nullptr ? false : appShell->findChild<QObject*>("rightInfoPanel") != nullptr,
        "RightInfoPanel exists");
    expectTrue(
        appShell == nullptr ? false : appShell->findChild<QObject*>("bottomLogPanel") != nullptr,
        "BottomLogPanel exists");

    if (appShell != nullptr) {
        expectEqual(navigationController.currentPageKey(), "dashboard", "default page is dashboard");
        expectEqual(statusController.pageInfo()->pageKey(), "dashboard", "default status page is dashboard");
        expectEqual(statusController.pageInfo()->moduleStatus(), "PLACEHOLDER", "default status is placeholder");
        expectTrue(statusController.logModel()->rowCount() > 0, "mock log model has rows");
        expectTrue(
            appShell->findChild<QObject*>("placeholderPage") != nullptr,
            "default placeholder page is loaded");

        bool invoked = navigationController.selectPage(QStringLiteral("diagnostics"));
        processQmlEvents();
        expectTrue(invoked, "navigateTo diagnostics succeeds");
        expectEqual(navigationController.currentPageKey(), "diagnostics", "current page is diagnostics");
        expectEqual(statusController.pageInfo()->pageKey(), "diagnostics", "status page is diagnostics");
        expectEqual(statusController.pageInfo()->moduleStatus(), "MOCK", "diagnostics status is MOCK");
        QObject* contentHost = appShell->findChild<QObject*>("contentHost");
        expectEqual(
            contentHost == nullptr ? QString() : contentHost->property("pageQmlComponent").toString(),
            "DiagnosticsMockPage",
            "ContentHost receives diagnostics qml component");
        expectTrue(
            appShell->findChild<QObject*>("diagnosticsMockPage") != nullptr,
            "DiagnosticsMockPage is loaded");
        expectTrue(adapter.serviceModel()->rowCount() > 0, "diagnostics mock serviceModel still has rows");

        invoked = navigationController.selectPage(QStringLiteral("market"));
        processQmlEvents();
        expectTrue(invoked, "navigateTo market succeeds");
        expectEqual(navigationController.currentPageKey(), "market", "current page is market");
        expectEqual(statusController.pageInfo()->pageKey(), "market", "status page is market");
        expectEqual(statusController.pageInfo()->moduleStatus(), "PLACEHOLDER", "market status is PLACEHOLDER");
        expectTrue(
            appShell->findChild<QObject*>("placeholderPage") != nullptr,
            "PlaceholderPage is loaded after navigation");
    }

    return gFailures == 0 ? 0 : 1;
}
