add_library(ShellAccountingProductionQmlBindingGateLib STATIC
    ShellAccountingProductionQmlBindingGate.cpp
    ShellAccountingProductionQmlBindingGate.h
)

target_include_directories(ShellAccountingProductionQmlBindingGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ShellAccountingProductionQmlBindingGateLib PUBLIC cxx_std_17)

function(add_shell_accounting_production_qml_binding_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingProductionQmlBindingGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_production_qml_binding_gate_test(
    ShellAccountingProductionQmlBindingGateTests
    test_shell_accounting_production_qml_binding_gate.cpp
    shell_accounting_production_qml_binding_gate
)

add_shell_accounting_production_qml_binding_gate_test(
    ShellAccountingProductionQmlBindingNoImportYetTests
    test_shell_accounting_production_qml_binding_no_import_yet.cpp
    shell_accounting_production_qml_binding_no_import_yet
)

add_shell_accounting_production_qml_binding_gate_test(
    ShellAccountingProductionQmlBindingNoPageYetTests
    test_shell_accounting_production_qml_binding_no_page_yet.cpp
    shell_accounting_production_qml_binding_no_page_yet
)

add_shell_accounting_production_qml_binding_gate_test(
    ShellAccountingProductionQmlBindingNoContextExposureTests
    test_shell_accounting_production_qml_binding_no_context_exposure.cpp
    shell_accounting_production_qml_binding_no_context_exposure
)

add_shell_accounting_production_qml_binding_gate_test(
    ShellAccountingProductionQmlBindingNoForbiddenRuntimeAccessTests
    test_shell_accounting_production_qml_binding_no_forbidden_runtime_access.cpp
    shell_accounting_production_qml_binding_no_forbidden_runtime_access
)

add_shell_accounting_production_qml_binding_gate_test(
    ShellAccountingProductionQmlBindingUnavailablePolicyTests
    test_shell_accounting_production_qml_binding_unavailable_policy.cpp
    shell_accounting_production_qml_binding_unavailable_policy
)

add_shell_accounting_production_qml_binding_gate_test(
    ShellAccountingProductionQmlBindingRollbackPolicyTests
    test_shell_accounting_production_qml_binding_rollback_policy.cpp
    shell_accounting_production_qml_binding_rollback_policy
)
