find_package(Qt6 COMPONENTS Core Gui Qml Quick QuickControls2 REQUIRED)

if(COMMAND qt_policy)
    qt_policy(SET QTP0001 NEW)
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_executable(ETFDecisionShell
    src/main.cpp
)

set_target_properties(ETFDecisionShell PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

qt_add_qml_module(ETFDecisionShell
    URI ETFDecisionTerminal.Shell
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/layout/AppShell.qml
        qml/layout/BottomLogPanel.qml
        qml/layout/ContentHost.qml
        qml/layout/RightInfoPanel.qml
        qml/layout/SideNavigation.qml
        qml/layout/TopStatusBar.qml
        qml/pages/AccountPortfolioReadOnlyPage.qml
        qml/pages/AccountPortfolioPlaceholderPage.qml
        qml/pages/AlertPlaceholderPage.qml
        qml/pages/CashPlanPlaceholderPage.qml
        qml/pages/DashboardPlaceholderPage.qml
        qml/pages/DiagnosticsMockPage.qml
        qml/pages/InstrumentStrategyReadOnlyPage.qml
        qml/pages/MarketPlaceholderPage.qml
        qml/pages/OtcChannelPlaceholderPage.qml
        qml/pages/PositionPlaceholderPage.qml
        qml/pages/ReadOnlyDataPage.qml
        qml/pages/SettingsPlaceholderPage.qml
        qml/pages/StrategyPlaceholderPage.qml
        qml/pages/TradeConfirmPlaceholderPage.qml
        qml/pages/TradeDraftPlaceholderPage.qml
        qml/pages/TradeLogPlaceholderPage.qml
        qml/components/ActionHintList.qml
        qml/components/AccountListView.qml
        qml/components/IssueListView.qml
        qml/components/InstrumentListView.qml
        qml/components/MetricCard.qml
        qml/components/OtcChannelListView.qml
        qml/components/PlaceholderPage.qml
        qml/components/PortfolioListView.qml
        qml/components/readonly/ReadOnlyEmptyState.qml
        qml/components/readonly/ReadOnlyFieldLabel.qml
        qml/components/readonly/ReadOnlyFilterBar.qml
        qml/components/readonly/ReadOnlySectionHeader.qml
        qml/components/readonly/ReadOnlyStatusBadge.qml
        qml/components/readonly/ReadOnlyTable.qml
        qml/components/ServiceListView.qml
        qml/components/StrategyListView.qml
        qml/components/SummaryCard.qml
)

target_link_libraries(ETFDecisionShell
    PRIVATE
        ETFDecisionTerminal::ShellCore
        ETFDecisionTerminal::ShellServices
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
)

target_compile_features(ETFDecisionShell PRIVATE cxx_std_17)
