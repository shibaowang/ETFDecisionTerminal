#include "ShellCore/ShellDiagnosticQtAdapter.h"
#include "ShellCore/ShellDiagnosticServiceListModel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QtQml>

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

    return gFailures == 0 ? 0 : 1;
}
