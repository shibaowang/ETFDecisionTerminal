#include "ShellCore/ShellDiagnosticQtAdapter.h"
#include "ShellCore/ShellNavigationController.h"
#include "ShellCore/ShellStatusController.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QtQml>
#include <iostream>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("ETFDecisionShell");

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

    etfdt::shell::ShellDiagnosticQtAdapter diagnosticAdapter;
    etfdt::shell::ShellNavigationController navigationController;
    etfdt::shell::ShellStatusController statusController;
    if (app.arguments().contains("--help")) {
        std::cout << "ETFDecisionShell --diagnostics-mock" << '\n';
        return 0;
    }
    (void)diagnosticAdapter.loadMockMixed();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("diagnosticAdapter", &diagnosticAdapter);
    engine.rootContext()->setContextProperty("shellNavigationController", &navigationController);
    engine.rootContext()->setContextProperty("shellStatusController", &statusController);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("ETFDecisionTerminal.Shell", "Main");
    return app.exec();
}
