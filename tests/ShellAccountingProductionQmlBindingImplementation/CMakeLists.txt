add_library(ShellAccountingProductionQmlBindingImplementationLib STATIC
    ShellAccountingProductionQmlBindingImplementation.cpp
    ShellAccountingProductionQmlBindingImplementation.h
)

target_compile_features(ShellAccountingProductionQmlBindingImplementationLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingProductionQmlBindingImplementationLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_production_qml_binding_implementation_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingProductionQmlBindingImplementationLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_production_qml_binding_implementation_test(
    ShellAccountingProductionQmlBindingImplementationTests
    test_shell_accounting_production_qml_binding_implementation.cpp
    shell_accounting_production_qml_binding_implementation
)

add_shell_accounting_production_qml_binding_implementation_test(
    ShellAccountingProductionQmlBindingAuthorizedImportTests
    test_shell_accounting_production_qml_binding_authorized_import.cpp
    shell_accounting_production_qml_binding_authorized_import
)

add_shell_accounting_production_qml_binding_implementation_test(
    ShellAccountingProductionQmlBindingAuthorizedPageTests
    test_shell_accounting_production_qml_binding_authorized_page.cpp
    shell_accounting_production_qml_binding_authorized_page
)

add_shell_accounting_production_qml_binding_implementation_test(
    ShellAccountingProductionQmlBindingUnavailableStateTests
    test_shell_accounting_production_qml_binding_unavailable_state.cpp
    shell_accounting_production_qml_binding_unavailable_state
)

add_shell_accounting_production_qml_binding_implementation_test(
    ShellAccountingProductionQmlBindingNoContextExposureAfterBindingTests
    test_shell_accounting_production_qml_binding_no_context_exposure_after_binding.cpp
    shell_accounting_production_qml_binding_no_context_exposure_after_binding
)

add_shell_accounting_production_qml_binding_implementation_test(
    ShellAcctQmlNoForbiddenRuntimeAfterBindingTests
    test_no_forbidden_runtime_after_binding.cpp
    shell_accounting_production_qml_binding_no_forbidden_runtime_access_after_binding
)

add_shell_accounting_production_qml_binding_implementation_test(
    ShellAccountingProductionQmlBindingNoTradeUiAfterBindingTests
    test_shell_accounting_production_qml_binding_no_trade_ui_after_binding.cpp
    shell_accounting_production_qml_binding_no_trade_ui_after_binding
)

add_shell_accounting_production_qml_binding_implementation_test(
    ShellAccountingProductionQmlBindingRollbackReadyAfterBindingTests
    test_shell_accounting_production_qml_binding_rollback_ready_after_binding.cpp
    shell_accounting_production_qml_binding_rollback_ready_after_binding
)
