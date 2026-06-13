#include "DataServiceClient/DataServiceClient.h"
#include "ShellCore/ShellDiagnosticQtAdapter.h"
#include "ShellCore/ShellNavigationController.h"
#include "ShellCore/ShellStatusController.h"
#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"
#include "ShellServices/ShellAccountingPresenter.h"
#include "ShellServices/ShellAccountingQmlRegistration.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"
#include "ShellServices/ShellReadOnlyDataController.h"
#include "Transport/LocalSocketName.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QtQml>
#include <iostream>
#include <memory>

namespace {

struct ShellStartupOptions final {
    bool dailyUse = false;
    QString socketName = QStringLiteral("ETFDataServiceDailyUse");
    QString databasePath = QStringLiteral(".local/daily_use/etfdt_daily_use.sqlite");
    QString defaultPage = QStringLiteral("dashboard");
};

QString argumentValue(
    const QStringList& arguments,
    const QString& name,
    const QString& fallback)
{
    const auto index = arguments.indexOf(name);
    if (index < 0 || index + 1 >= arguments.size()) {
        return fallback;
    }
    const auto value = arguments.at(index + 1).trimmed();
    return value.isEmpty() ? fallback : value;
}

QString pageKeyForDefaultPage(QString page)
{
    page = page.trimmed();
    if (page == QStringLiteral("shell-accounting-daily-use")
        || page == QStringLiteral("shell-accounting")
        || page == QStringLiteral("shell_accounting")) {
        return QStringLiteral("shell_accounting");
    }
    return page.isEmpty() ? QStringLiteral("dashboard") : page;
}

ShellStartupOptions parseStartupOptions(const QStringList& arguments)
{
    ShellStartupOptions options;
    options.dailyUse = arguments.contains(QStringLiteral("--daily-use"));
    options.socketName =
        argumentValue(arguments, QStringLiteral("--socket-name"), options.socketName);
    options.databasePath =
        argumentValue(arguments, QStringLiteral("--db"), options.databasePath);
    options.defaultPage = argumentValue(
        arguments,
        QStringLiteral("--default-page"),
        options.dailyUse ? QStringLiteral("shell-accounting-daily-use")
                         : options.defaultPage);
    return options;
}

QString dailyUseConnectionIssueFor(const std::string& rawMessage)
{
    const QString message = QString::fromStdString(rawMessage);
    if (message.contains(QStringLiteral("Invalid name"), Qt::CaseInsensitive)) {
        return QStringLiteral("daily-use socket name 非法");
    }
    if (message.contains(QStringLiteral("timeout"), Qt::CaseInsensitive)
        || message.contains(QStringLiteral("Timed out"), Qt::CaseInsensitive)) {
        return QStringLiteral("连接 daily-use DataService 超时");
    }
    return QStringLiteral("DataService 未启动或 socket 未就绪");
}

}  // namespace

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("ETFDecisionShell");
    const auto startupOptions = parseStartupOptions(app.arguments());

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
    qmlRegisterType<etfdt::shell_services::ShellReadOnlyDataController>(
        "ETFDecisionTerminal.Shell",
        1,
        0,
        "ShellReadOnlyDataController");
    (void)etfdt::shell_services::registerShellAccountingQmlTypes();

    etfdt::shell::ShellDiagnosticQtAdapter diagnosticAdapter;
    etfdt::shell::ShellNavigationController navigationController;
    etfdt::shell::ShellStatusController statusController;
    etfdt::shell_services::ShellReadOnlyDataController readOnlyDataController;
    auto shellAccountingController =
        std::make_shared<etfdt::shell_services::ShellAccountingReadOnlyController>();
    auto shellAccountingDataServiceClient =
        std::make_shared<etfdt::data_service_client::DataServiceClient>();
    auto shellAccountingClientPort =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter>(
            shellAccountingDataServiceClient);
    auto shellAccountingDataServiceAdapter =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceAdapter>(
            shellAccountingClientPort);
    shellAccountingController->setServiceAdapter(shellAccountingDataServiceAdapter);
    etfdt::shell_services::ShellAccountingPresenter shellAccountingPresenter;
    shellAccountingPresenter.setController(shellAccountingController);

    if (app.arguments().contains("--help")) {
        std::cout
            << "ETFDecisionShell [--daily-use] [--socket-name <name>] [--db <path>] "
               "[--default-page shell-accounting-daily-use]"
            << '\n';
        return 0;
    }
    if (!startupOptions.dailyUse) {
        (void)diagnosticAdapter.loadMockMixed();
    }

    QString dailyUseConnectionStatus = QStringLiteral("NOT_REQUESTED");
    QString dailyUseConnectionIssue;
    const auto normalizedSocketName = QString::fromStdString(
        etfdt::transport::normalizeLocalSocketName(startupOptions.socketName.toStdString()));
    if (startupOptions.dailyUse) {
        const auto connected = shellAccountingDataServiceClient->connect(
            normalizedSocketName.toStdString(),
            2000);
        if (connected.hasValue()) {
            dailyUseConnectionStatus = QStringLiteral("CONNECTED");
        } else {
            dailyUseConnectionStatus = QStringLiteral("CONNECTION_FAILED");
            dailyUseConnectionIssue = dailyUseConnectionIssueFor(connected.error().message);
        }
        (void)navigationController.selectPage(pageKeyForDefaultPage(startupOptions.defaultPage));
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("diagnosticAdapter", &diagnosticAdapter);
    engine.rootContext()->setContextProperty("shellNavigationController", &navigationController);
    engine.rootContext()->setContextProperty("shellStatusController", &statusController);
    engine.rootContext()->setContextProperty("readOnlyDataController", &readOnlyDataController);
    engine.rootContext()->setContextProperty("accountingPresenter", &shellAccountingPresenter);
    engine.rootContext()->setContextProperty("dailyUseMode", startupOptions.dailyUse);
    engine.rootContext()->setContextProperty("dailyUseServiceName", normalizedSocketName);
    engine.rootContext()->setContextProperty("dailyUseDatabasePath", startupOptions.databasePath);
    engine.rootContext()->setContextProperty(
        "dailyUseConnectionStatus",
        dailyUseConnectionStatus);
    engine.rootContext()->setContextProperty(
        "dailyUseConnectionIssue",
        dailyUseConnectionIssue);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("ETFDecisionTerminal.Shell", "Main");
    return app.exec();
}
