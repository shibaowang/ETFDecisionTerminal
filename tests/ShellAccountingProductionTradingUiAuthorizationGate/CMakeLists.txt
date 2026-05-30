add_library(ShellAccountingProductionTradingUiAuthorizationGateLib STATIC
    ShellAccountingProductionTradingUiAuthorizationGate.cpp
    ShellAccountingProductionTradingUiAuthorizationGate.h
)

target_compile_features(ShellAccountingProductionTradingUiAuthorizationGateLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingProductionTradingUiAuthorizationGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_production_trading_ui_authorization_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingProductionTradingUiAuthorizationGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_production_trading_ui_authorization_gate_test(
    SAPTUiGateTests
    test_shell_accounting_production_trading_ui_authorization_gate.cpp
    shell_accounting_production_trading_ui_authorization_gate
)

add_shell_accounting_production_trading_ui_authorization_gate_test(
    SAPTUiNoUiTests
    test_shell_accounting_production_trading_ui_authorization_no_ui_yet.cpp
    shell_accounting_production_trading_ui_authorization_no_ui_yet
)

add_shell_accounting_production_trading_ui_authorization_gate_test(
    SAPTUiNoDraftCreateBindingTests
    test_shell_accounting_production_trading_ui_authorization_no_draft_create_binding.cpp
    shell_accounting_production_trading_ui_authorization_no_draft_create_binding
)

add_shell_accounting_production_trading_ui_authorization_gate_test(
    SAPTUiNoDraftConfirmBindingTests
    test_shell_accounting_production_trading_ui_authorization_no_draft_confirm_binding.cpp
    shell_accounting_production_trading_ui_authorization_no_draft_confirm_binding
)

add_shell_accounting_production_trading_ui_authorization_gate_test(
    SAPTUiDataServiceOnlyPolicyTests
    test_shell_accounting_production_trading_ui_authorization_dataservice_only_policy.cpp
    shell_accounting_production_trading_ui_authorization_dataservice_only_policy
)

add_shell_accounting_production_trading_ui_authorization_gate_test(
    SAPTUiUserConfirmationPolicyTests
    test_shell_accounting_production_trading_ui_authorization_user_confirmation_policy.cpp
    shell_accounting_production_trading_ui_authorization_user_confirmation_policy
)

add_shell_accounting_production_trading_ui_authorization_gate_test(
    SAPTUiNoDirectDbWriteTests
    test_shell_accounting_production_trading_ui_authorization_no_direct_db_write.cpp
    shell_accounting_production_trading_ui_authorization_no_direct_db_write
)

add_shell_accounting_production_trading_ui_authorization_gate_test(
    SAPTUiNoBrokerStrategyTests
    test_shell_accounting_production_trading_ui_authorization_no_broker_or_strategy.cpp
    shell_accounting_production_trading_ui_authorization_no_broker_or_strategy
)

add_shell_accounting_production_trading_ui_authorization_gate_test(
    SAPTUiErrorMappingTests
    test_shell_accounting_production_trading_ui_authorization_error_mapping_policy.cpp
    shell_accounting_production_trading_ui_authorization_error_mapping_policy
)

add_shell_accounting_production_trading_ui_authorization_gate_test(
    SAPTUiRollbackPolicyTests
    test_shell_accounting_production_trading_ui_authorization_rollback_policy.cpp
    shell_accounting_production_trading_ui_authorization_rollback_policy
)
