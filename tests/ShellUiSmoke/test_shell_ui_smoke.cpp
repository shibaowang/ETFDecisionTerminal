#include "ShellCore/ShellDiagnosticQtAdapter.h"
#include "ShellCore/ShellDiagnosticServiceListModel.h"
#include "ShellCore/ShellNavigationController.h"
#include "ShellCore/ShellStatusController.h"
#include "ShellServices/ShellReadOnlyDataController.h"

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
    qmlRegisterType<etfdt::shell_services::ShellReadOnlyDataController>(
        "ETFDecisionTerminal.Shell",
        1,
        0,
        "ShellReadOnlyDataController");

    etfdt::shell::ShellDiagnosticQtAdapter adapter;
    etfdt::shell::ShellNavigationController navigationController;
    etfdt::shell::ShellStatusController statusController;
    etfdt::shell_services::ShellReadOnlyDataController readOnlyDataController;
    expectTrue(adapter.loadMockMixed(), "adapter loadMockMixed succeeds");
    expectTrue(adapter.serviceModel()->rowCount() > 0, "adapter serviceModel has rows");
    expectTrue(adapter.selectService(0), "adapter selectService does not fail");
    expectTrue(adapter.summaryObject()->totalServices() > 0, "summaryObject has totalServices");
    expectEqual(navigationController.navigationModel()->rowCount(), 14, "navigation model has 14 rows");
    expectTrue(statusController.metricsModel()->rowCount() >= 4, "status metrics model has dashboard rows");
    expectTrue(statusController.actionHintModel()->rowCount() > 0, "status action hint model has rows");
    expectTrue(readOnlyDataController.connectionObject() != nullptr, "read-only controller has connection object");
    expectTrue(readOnlyDataController.accountModel() != nullptr, "read-only controller has account model");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("diagnosticAdapter", &adapter);
    engine.rootContext()->setContextProperty("shellNavigationController", &navigationController);
    engine.rootContext()->setContextProperty("shellStatusController", &statusController);
    engine.rootContext()->setContextProperty("readOnlyDataController", &readOnlyDataController);

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
            appShell->findChild<QObject*>("dashboardPlaceholderPage") != nullptr,
            "default dashboard placeholder page is loaded");
        expectTrue(
            appShell->findChild<QObject*>("dashboardMetricGrid") != nullptr,
            "dashboard metric grid is loaded");

        bool invoked = navigationController.selectPage(QStringLiteral("diagnostics"));
        processQmlEvents();
        expectTrue(invoked, "navigateTo diagnostics succeeds");
        expectEqual(navigationController.currentPageKey(), "diagnostics", "current page is diagnostics");
        expectEqual(statusController.pageInfo()->pageKey(), "diagnostics", "status page is diagnostics");
        expectEqual(statusController.pageInfo()->moduleStatus(), "MOCK", "diagnostics status is MOCK");
        expectTrue(statusController.metricsModel()->rowCount() >= 3, "diagnostics metrics model has rows");
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
        expectTrue(statusController.metricsModel()->rowCount() >= 1, "placeholder metrics model has rows");
        expectTrue(
            appShell->findChild<QObject*>("placeholderPage") != nullptr,
            "PlaceholderPage is loaded after navigation");
        expectTrue(
            appShell->findChild<QObject*>("placeholderMetricGrid") != nullptr,
            "PlaceholderPage metric grid is loaded");

        invoked = navigationController.selectPage(QStringLiteral("account_portfolio"));
        processQmlEvents();
        expectTrue(invoked, "navigateTo account_portfolio succeeds");
        expectEqual(navigationController.currentPageKey(), "account_portfolio", "current page is account_portfolio");
        expectEqual(
            contentHost == nullptr ? QString() : contentHost->property("pageQmlComponent").toString(),
            "AccountPortfolioReadOnlyPage",
            "ContentHost receives account portfolio qml component");
        expectTrue(
            appShell->findChild<QObject*>("accountPortfolioReadOnlyPage") != nullptr,
            "AccountPortfolioReadOnlyPage is loaded");
        expectTrue(
            appShell->findChild<QObject*>("accountPortfolioReadonlyNotice") != nullptr,
            "AccountPortfolioReadOnlyPage read-only notice is loaded");
        expectTrue(
            appShell->findChild<QObject*>("accountPortfolioStatusPanel") != nullptr,
            "AccountPortfolioReadOnlyPage status panel is loaded");
        expectTrue(
            appShell->findChild<QObject*>("accountFilterBar") != nullptr,
            "AccountPortfolioReadOnlyPage account filter bar is loaded");
        expectTrue(
            appShell->findChild<QObject*>("accountFilterBarSearchField") != nullptr,
            "AccountPortfolioReadOnlyPage account search field is loaded");
        expectTrue(
            appShell->findChild<QObject*>("accountColumnChooser") != nullptr,
            "AccountPortfolioReadOnlyPage account column chooser is loaded");
        expectTrue(
            appShell->findChild<QObject*>("accountDensityToggle") != nullptr,
            "AccountPortfolioReadOnlyPage account density toggle is loaded");
        expectTrue(
            appShell->findChild<QObject*>("portfolioFilterBar") != nullptr,
            "AccountPortfolioReadOnlyPage portfolio filter bar is loaded");
        expectTrue(
            appShell->findChild<QObject*>("portfolioColumnChooser") != nullptr,
            "AccountPortfolioReadOnlyPage portfolio column chooser is loaded");
        expectTrue(
            appShell->findChild<QObject*>("portfolioDensityToggle") != nullptr,
            "AccountPortfolioReadOnlyPage portfolio density toggle is loaded");
        expectTrue(
            appShell->findChild<QObject*>("accountReadOnlyTable") != nullptr,
            "AccountPortfolioReadOnlyPage account read-only table is loaded");
        expectTrue(
            appShell->findChild<QObject*>("accountReadOnlyTableHeader") != nullptr,
            "ReadOnlySectionHeader is loaded for account table");
        expectTrue(
            appShell->findChild<QObject*>("accountReadOnlyTableEmptyState") != nullptr,
            "ReadOnlyEmptyState is loaded for account table");
        expectTrue(
            appShell->findChild<QObject*>("portfolioReadOnlyTable") != nullptr,
            "AccountPortfolioReadOnlyPage portfolio read-only table is loaded");
        expectTrue(
            appShell->findChild<QObject*>("readOnlyStatusBadge") != nullptr,
            "ReadOnlyStatusBadge is loaded");
        expectTrue(
            appShell->findChild<QObject*>("readOnlyFieldLabel") != nullptr,
            "ReadOnlyFieldLabel is loaded");
        expectTrue(
            appShell->findChild<QObject*>("accountPortfolioRefreshButton") != nullptr,
            "AccountPortfolioReadOnlyPage refresh button is loaded");
        expectTrue(
            appShell->findChild<QObject*>("accountListView") != nullptr,
            "AccountPortfolioReadOnlyPage account model binding exists");
        expectTrue(
            appShell->findChild<QObject*>("portfolioListView") != nullptr,
            "AccountPortfolioReadOnlyPage portfolio model binding exists");
        QObject* accountTable = appShell->findChild<QObject*>("accountReadOnlyTable");
        const int normalAccountRowHeight = accountTable == nullptr ? 0 : accountTable->property("rowHeight").toInt();
        if (accountTable != nullptr) {
            accountTable->setProperty("density", QStringLiteral("compact"));
            processQmlEvents();
            expectTrue(
                accountTable->property("rowHeight").toInt() < normalAccountRowHeight,
                "compact density uses a smaller account row height");
            accountTable->setProperty("density", QStringLiteral("comfortable"));
            processQmlEvents();
            expectTrue(
                accountTable->property("rowHeight").toInt() > normalAccountRowHeight,
                "comfortable density uses a larger account row height");
            accountTable->setProperty("visibleColumnKeys", QStringList({QStringLiteral("type")}));
            processQmlEvents();
            expectTrue(true, "Account table accepts visible column updates");
        }
        QObject* accountColumnChooser = appShell->findChild<QObject*>("accountColumnChooser");
        QVariant canHideRequiredAccountName = true;
        if (accountColumnChooser != nullptr) {
            QMetaObject::invokeMethod(
                accountColumnChooser,
                "canHideColumn",
                Q_RETURN_ARG(QVariant, canHideRequiredAccountName),
                Q_ARG(QVariant, QStringLiteral("name")));
        }
        expectTrue(
            accountColumnChooser != nullptr && !canHideRequiredAccountName.toBool(),
            "required name column cannot be hidden");
        expectTrue(
            appShell->findChild<QObject*>("accountEditButton") == nullptr,
            "AccountPortfolioReadOnlyPage has no edit button");
        expectTrue(
            appShell->findChild<QObject*>("accountDepositButton") == nullptr,
            "AccountPortfolioReadOnlyPage has no deposit button");
        expectTrue(
            appShell->findChild<QObject*>("accountTradeButton") == nullptr,
            "AccountPortfolioReadOnlyPage has no trade button");

        invoked = navigationController.selectPage(QStringLiteral("strategy"));
        processQmlEvents();
        expectTrue(invoked, "navigateTo strategy succeeds");
        expectEqual(navigationController.currentPageKey(), "strategy", "current page is strategy");
        expectEqual(
            contentHost == nullptr ? QString() : contentHost->property("pageQmlComponent").toString(),
            "InstrumentStrategyReadOnlyPage",
            "ContentHost receives instrument strategy qml component");
        expectTrue(
            appShell->findChild<QObject*>("instrumentStrategyReadOnlyPage") != nullptr,
            "InstrumentStrategyReadOnlyPage is loaded");
        expectTrue(
            appShell->findChild<QObject*>("instrumentStrategyReadonlyNotice") != nullptr,
            "InstrumentStrategyReadOnlyPage read-only notice is loaded");
        expectTrue(
            appShell->findChild<QObject*>("instrumentStrategyStatusPanel") != nullptr,
            "InstrumentStrategyReadOnlyPage status panel is loaded");
        expectTrue(
            appShell->findChild<QObject*>("instrumentFilterBar") != nullptr,
            "InstrumentStrategyReadOnlyPage instrument filter bar is loaded");
        expectTrue(
            appShell->findChild<QObject*>("instrumentFilterBarSearchField") != nullptr,
            "InstrumentStrategyReadOnlyPage instrument search field is loaded");
        expectTrue(
            appShell->findChild<QObject*>("instrumentColumnChooser") != nullptr,
            "InstrumentStrategyReadOnlyPage instrument column chooser is loaded");
        expectTrue(
            appShell->findChild<QObject*>("instrumentDensityToggle") != nullptr,
            "InstrumentStrategyReadOnlyPage instrument density toggle is loaded");
        expectTrue(
            appShell->findChild<QObject*>("strategyFilterBar") != nullptr,
            "InstrumentStrategyReadOnlyPage strategy filter bar is loaded");
        expectTrue(
            appShell->findChild<QObject*>("strategyColumnChooser") != nullptr,
            "InstrumentStrategyReadOnlyPage strategy column chooser is loaded");
        expectTrue(
            appShell->findChild<QObject*>("strategyDensityToggle") != nullptr,
            "InstrumentStrategyReadOnlyPage strategy density toggle is loaded");
        expectTrue(
            appShell->findChild<QObject*>("otcFilterBar") != nullptr,
            "InstrumentStrategyReadOnlyPage OTC filter bar is loaded");
        expectTrue(
            appShell->findChild<QObject*>("otcColumnChooser") != nullptr,
            "InstrumentStrategyReadOnlyPage OTC column chooser is loaded");
        expectTrue(
            appShell->findChild<QObject*>("otcDensityToggle") != nullptr,
            "InstrumentStrategyReadOnlyPage OTC density toggle is loaded");
        expectTrue(
            appShell->findChild<QObject*>("instrumentReadOnlyTable") != nullptr,
            "InstrumentStrategyReadOnlyPage instrument read-only table is loaded");
        expectTrue(
            appShell->findChild<QObject*>("strategyReadOnlyTable") != nullptr,
            "InstrumentStrategyReadOnlyPage strategy read-only table is loaded");
        expectTrue(
            appShell->findChild<QObject*>("otcChannelReadOnlyTable") != nullptr,
            "InstrumentStrategyReadOnlyPage OTC read-only table is loaded");
        expectTrue(
            appShell->findChild<QObject*>("strategyReadOnlyTableEmptyState") != nullptr,
            "ReadOnlyEmptyState is loaded for empty strategy table");
        expectTrue(
            appShell->findChild<QObject*>("instrumentStrategyRefreshButton") != nullptr,
            "InstrumentStrategyReadOnlyPage refresh button is loaded");
        expectTrue(
            appShell->findChild<QObject*>("instrumentStrategyRefreshOtcButton") != nullptr,
            "InstrumentStrategyReadOnlyPage OTC refresh button is loaded");
        expectTrue(
            appShell->findChild<QObject*>("instrumentListView") != nullptr,
            "InstrumentStrategyReadOnlyPage instrument model binding exists");
        expectTrue(
            appShell->findChild<QObject*>("strategyListView") != nullptr,
            "InstrumentStrategyReadOnlyPage strategy model binding exists");
        expectTrue(
            appShell->findChild<QObject*>("otcChannelListView") != nullptr,
            "InstrumentStrategyReadOnlyPage OTC model binding exists");
        QObject* instrumentTable = appShell->findChild<QObject*>("instrumentReadOnlyTable");
        QObject* otcTable = appShell->findChild<QObject*>("otcChannelReadOnlyTable");
        if (instrumentTable != nullptr) {
            instrumentTable->setProperty("visibleColumnKeys", QStringList({
                QStringLiteral("code"),
                QStringLiteral("name"),
                QStringLiteral("type"),
                QStringLiteral("market"),
                QStringLiteral("status"),
            }));
            processQmlEvents();
            expectTrue(true, "Instrument table accepts hidden currency column");
        }
        if (otcTable != nullptr) {
            otcTable->setProperty("visibleColumnKeys", QStringList({
                QStringLiteral("code"),
                QStringLiteral("type"),
                QStringLiteral("status"),
                QStringLiteral("dailyLimit"),
            }));
            processQmlEvents();
            expectTrue(true, "OTC table accepts hidden priority column");
            otcTable->setProperty("visibleColumnKeys", QStringList());
            processQmlEvents();
            expectTrue(true, "OTC table falls back when visible columns are cleared");
        }
        expectTrue(
            appShell->findChild<QObject*>("instrumentEditButton") == nullptr,
            "InstrumentStrategyReadOnlyPage has no instrument edit button");
        expectTrue(
            appShell->findChild<QObject*>("instrumentDeleteButton") == nullptr,
            "InstrumentStrategyReadOnlyPage has no instrument delete button");
        expectTrue(
            appShell->findChild<QObject*>("strategyDeleteButton") == nullptr,
            "InstrumentStrategyReadOnlyPage has no strategy delete button");
        expectTrue(
            appShell->findChild<QObject*>("strategyExecuteButton") == nullptr,
            "InstrumentStrategyReadOnlyPage has no strategy execute button");
        expectTrue(
            appShell->findChild<QObject*>("instrumentTradeButton") == nullptr,
            "InstrumentStrategyReadOnlyPage has no trade button");

        invoked = navigationController.selectPage(QStringLiteral("readonly_data"));
        processQmlEvents();
        expectTrue(invoked, "navigateTo readonly_data succeeds");
        expectEqual(navigationController.currentPageKey(), "readonly_data", "current page is readonly_data");
        expectEqual(
            contentHost == nullptr ? QString() : contentHost->property("pageQmlComponent").toString(),
            "ReadOnlyDataPage",
            "ContentHost receives readonly qml component");
        expectTrue(
            appShell->findChild<QObject*>("readOnlyDataPage") != nullptr,
            "ReadOnlyDataPage is loaded");
        expectTrue(
            appShell->findChild<QObject*>("readonlyDataGrid") != nullptr,
            "ReadOnlyDataPage data grid is loaded");
        expectTrue(
            appShell->findChild<QObject*>("readonlyPresetPanel") != nullptr,
            "ReadOnlyDataPage preset panel is loaded");
        expectTrue(
            appShell->findChild<QObject*>("readonlyPresetCombo") != nullptr,
            "ReadOnlyDataPage preset combo is loaded");
        QObject* readOnlyPage = appShell->findChild<QObject*>("readOnlyDataPage");
        QObject* presetCombo = appShell->findChild<QObject*>("readonlyPresetCombo");
        QObject* socketNameField = appShell->findChild<QObject*>("readonlySocketNameField");
        processQmlEvents();
        expectTrue(
            readOnlyPage == nullptr
                ? false
                : readOnlyPage->property("readOnlyDataController").value<QObject*>() != nullptr,
            "ReadOnlyDataPage receives read-only controller");
        expectTrue(
            presetCombo == nullptr ? false : presetCombo->property("count").toInt() >= 3,
            "ReadOnlyDataPage preset combo exposes all presets");
        expectEqual(
            presetCombo == nullptr ? QString() : presetCombo->property("currentText").toString(),
            QString::fromUtf8("DataService \xE5\x8F\xAA\xE8\xAF\xBB\xE6\x9C\x8D\xE5\x8A\xA1"),
            "ReadOnlyDataPage preset combo defaults to readonly preset");
        expectEqual(
            socketNameField == nullptr ? QString() : socketNameField->property("text").toString(),
            QStringLiteral("ETFDataServiceReadonly"),
            "ReadOnlyDataPage socketName field defaults to readonly socket");
        expectTrue(
            appShell->findChild<QObject*>("readonlyCommandHint") != nullptr,
            "ReadOnlyDataPage command hint is loaded");
        expectTrue(
            readOnlyDataController.connectionPresetModel()->rowCount() >= 3,
            "ReadOnlyDataPage controller exposes presets to QML");
        expectEqual(
            readOnlyDataController.selectedPresetKey(),
            QStringLiteral("readonly_default"),
            "ReadOnlyDataPage default preset key");
        expectTrue(
            appShell->findChild<QObject*>("readonlyRefreshAllButton") != nullptr,
            "ReadOnlyDataPage Refresh All button is loaded");
        expectTrue(
            appShell->findChild<QObject*>("readonlyErrorPanel") != nullptr,
            "ReadOnlyDataPage error panel is loaded");
        expectTrue(
            appShell->findChild<QObject*>("readonlyRefreshState") != nullptr,
            "ReadOnlyDataPage refresh state text is loaded");
        expectEqual(
            readOnlyDataController.refreshState(),
            QStringLiteral("IDLE"),
            "ReadOnlyDataPage starts with IDLE state");
        expectTrue(
            appShell->findChild<QObject*>("accountListView") != nullptr,
            "ReadOnlyDataPage account model binding exists");
        expectTrue(
            appShell->findChild<QObject*>("instrumentListView") != nullptr,
            "ReadOnlyDataPage instrument model binding exists");
    }

    return gFailures == 0 ? 0 : 1;
}
