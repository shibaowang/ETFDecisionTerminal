add_library(ShellAccountingPresenterLifecycleGateLib STATIC
    ShellAccountingPresenterLifecycleGate.cpp
    ShellAccountingPresenterLifecycleGate.h
)

target_compile_features(ShellAccountingPresenterLifecycleGateLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingPresenterLifecycleGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_presenter_lifecycle_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingPresenterLifecycleGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_presenter_lifecycle_gate_test(
    ShellAccountingPresenterLifecycleGateTests
    test_shell_accounting_presenter_lifecycle_gate.cpp
    shell_accounting_presenter_lifecycle_gate
)

add_shell_accounting_presenter_lifecycle_gate_test(
    ShellAccountingPresenterLifecycleNoCreationYetTests
    test_shell_accounting_presenter_lifecycle_no_creation_yet.cpp
    shell_accounting_presenter_lifecycle_no_creation_yet
)

add_shell_accounting_presenter_lifecycle_gate_test(
    ShellAccountingPresenterLifecycleNoContextExposureYetTests
    test_shell_accounting_presenter_lifecycle_no_context_exposure_yet.cpp
    shell_accounting_presenter_lifecycle_no_context_exposure_yet
)

add_shell_accounting_presenter_lifecycle_gate_test(
    ShellAccountingPresenterLifecycleNullPresenterPolicyTests
    test_shell_accounting_presenter_lifecycle_null_presenter_policy.cpp
    shell_accounting_presenter_lifecycle_null_presenter_policy
)

add_shell_accounting_presenter_lifecycle_gate_test(
    ShellAccountingPresenterLifecycleNoForbiddenRuntimeAccessTests
    test_shell_accounting_presenter_lifecycle_no_forbidden_runtime_access.cpp
    shell_accounting_presenter_lifecycle_no_forbidden_runtime_access
)

add_shell_accounting_presenter_lifecycle_gate_test(
    ShellAccountingPresenterLifecycleNoTradeUiTests
    test_shell_accounting_presenter_lifecycle_no_trade_ui.cpp
    shell_accounting_presenter_lifecycle_no_trade_ui
)

add_shell_accounting_presenter_lifecycle_gate_test(
    ShellAccountingPresenterLifecycleRollbackPolicyTests
    test_shell_accounting_presenter_lifecycle_rollback_policy.cpp
    shell_accounting_presenter_lifecycle_rollback_policy
)
