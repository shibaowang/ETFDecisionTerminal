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
        qml/pages/DiagnosticsMockPage.qml
        qml/components/IssueListView.qml
        qml/components/ServiceListView.qml
        qml/components/SummaryCard.qml
)

target_link_libraries(ETFDecisionShell
    PRIVATE
        ETFDecisionTerminal::ShellCore
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
)

target_compile_features(ETFDecisionShell PRIVATE cxx_std_17)
