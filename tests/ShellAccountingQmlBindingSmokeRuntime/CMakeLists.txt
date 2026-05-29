find_package(Qt6 COMPONENTS Core Qml REQUIRED)

add_library(ShellAccountingQmlBindingSmokeRuntimeObjects STATIC
    ShellAccountingQmlSmokeTestObjects.cpp
    ShellAccountingQmlSmokeTestObjects.h
)

target_include_directories(ShellAccountingQmlBindingSmokeRuntimeObjects
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ShellAccountingQmlBindingSmokeRuntimeObjects
    PUBLIC
        Qt6::Core
        Qt6::Qml
)

target_compile_features(ShellAccountingQmlBindingSmokeRuntimeObjects PUBLIC cxx_std_17)

function(add_shell_accounting_qml_smoke_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingQmlBindingSmokeRuntimeObjects
            Qt6::Core
            Qt6::Qml
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
    set_tests_properties(${ctest_name} PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
    )
endfunction()

add_shell_accounting_qml_smoke_test(
    ShellAccountingQmlSmokeObjectAvailabilityTests
    test_shell_accounting_qml_smoke_object_availability.cpp
    shell_accounting_qml_smoke_object_availability
)

add_shell_accounting_qml_smoke_test(
    ShellAccountingQmlSmokeStateDisplayTests
    test_shell_accounting_qml_smoke_state_display.cpp
    shell_accounting_qml_smoke_state_display
)

add_shell_accounting_qml_smoke_test(
    ShellAccountingQmlSmokeGuardUnavailableTests
    test_shell_accounting_qml_smoke_guard_unavailable.cpp
    shell_accounting_qml_smoke_guard_unavailable
)

add_shell_accounting_qml_smoke_test(
    ShellAccountingQmlSmokeIssueVisibilityTests
    test_shell_accounting_qml_smoke_issue_visibility.cpp
    shell_accounting_qml_smoke_issue_visibility
)

add_shell_accounting_qml_smoke_test(
    ShellAccountingQmlSmokePrivacyDisplayTests
    test_shell_accounting_qml_smoke_privacy_display.cpp
    shell_accounting_qml_smoke_privacy_display
)

add_shell_accounting_qml_smoke_test(
    ShellAccountingQmlSmokeNoTradeUiTests
    test_shell_accounting_qml_smoke_no_trade_ui.cpp
    shell_accounting_qml_smoke_no_trade_ui
)

add_shell_accounting_qml_smoke_test(
    ShellAccountingQmlSmokeStaticGateRegressionTests
    test_shell_accounting_qml_smoke_static_gate_regression.cpp
    shell_accounting_qml_smoke_static_gate_regression
)
