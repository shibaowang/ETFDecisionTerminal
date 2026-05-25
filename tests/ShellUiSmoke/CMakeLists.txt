find_package(Qt6 COMPONENTS Core Gui Qml Quick REQUIRED)

add_executable(ShellUiSmokeTests
    test_shell_ui_smoke.cpp
)

target_link_libraries(ShellUiSmokeTests
    PRIVATE
        ETFDecisionTerminal::ShellCore
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
)

target_compile_features(ShellUiSmokeTests PRIVATE cxx_std_17)

target_compile_definitions(ShellUiSmokeTests
    PRIVATE
        ETFDT_SHELL_QML_DIR="${PROJECT_SOURCE_DIR}/apps/ETFDecisionShell/qml"
)

add_test(
    NAME shell_qml_diagnostics_mock
    COMMAND ShellUiSmokeTests
)

set_tests_properties(shell_qml_diagnostics_mock PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>;QT_QPA_PLATFORM=set:offscreen"
)
