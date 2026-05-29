add_library(ShellAccountingQmlBindingSmokeScenarios STATIC
    ShellAccountingQmlSmokeExpectedBinding.cpp
    ShellAccountingQmlSmokeExpectedBinding.h
    ShellAccountingQmlSmokeScenario.cpp
    ShellAccountingQmlSmokeScenario.h
)

target_include_directories(ShellAccountingQmlBindingSmokeScenarios
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ShellAccountingQmlBindingSmokeScenarios PUBLIC cxx_std_17)

function(add_shell_accounting_qml_binding_smoke_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingQmlBindingSmokeScenarios
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_qml_binding_smoke_test(
    ShellAccountingQmlBindingSmokeObjectContractTests
    test_shell_accounting_qml_binding_smoke_object_contract.cpp
    shell_accounting_qml_binding_smoke_object_contract
)

add_shell_accounting_qml_binding_smoke_test(
    ShellAccountingQmlBindingSmokeStateMatrixTests
    test_shell_accounting_qml_binding_smoke_state_matrix.cpp
    shell_accounting_qml_binding_smoke_state_matrix
)

add_shell_accounting_qml_binding_smoke_test(
    ShellAccountingQmlBindingSmokeGuardPayloadsTests
    test_shell_accounting_qml_binding_smoke_guard_payloads.cpp
    shell_accounting_qml_binding_smoke_guard_payloads
)

add_shell_accounting_qml_binding_smoke_test(
    ShellAccountingQmlBindingSmokeIssuePrivacyTests
    test_shell_accounting_qml_binding_smoke_issue_privacy.cpp
    shell_accounting_qml_binding_smoke_issue_privacy
)

add_shell_accounting_qml_binding_smoke_test(
    ShellAccountingQmlBindingSmokeNoTradeUiTests
    test_shell_accounting_qml_binding_smoke_no_trade_ui.cpp
    shell_accounting_qml_binding_smoke_no_trade_ui
)

add_shell_accounting_qml_binding_smoke_test(
    ShellAccountingQmlBindingSmokeStaticGateDependencyTests
    test_shell_accounting_qml_binding_smoke_static_gate_dependency.cpp
    shell_accounting_qml_binding_smoke_static_gate_dependency
)
