add_library(ShellAccountingBrokerOrderAuthorizationGateLib STATIC
    ShellAccountingBrokerOrderAuthorizationGate.cpp
    ShellAccountingBrokerOrderAuthorizationGate.h
)

target_compile_features(ShellAccountingBrokerOrderAuthorizationGateLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingBrokerOrderAuthorizationGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_broker_order_authorization_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingBrokerOrderAuthorizationGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_broker_order_authorization_gate_test(
    SABOAGateTests
    test_shell_accounting_broker_order_authorization_gate.cpp
    shell_accounting_broker_order_authorization_gate
)

add_shell_accounting_broker_order_authorization_gate_test(
    SABOANoBrokerOrderTests
    test_shell_accounting_broker_order_authorization_no_broker_order_yet.cpp
    shell_accounting_broker_order_authorization_no_broker_order_yet
)

add_shell_accounting_broker_order_authorization_gate_test(
    SABOADataServiceOnlyTests
    test_shell_accounting_broker_order_authorization_dataservice_only_policy.cpp
    shell_accounting_broker_order_authorization_dataservice_only_policy
)

add_shell_accounting_broker_order_authorization_gate_test(
    SABOAConfirmedTradeTests
    test_shell_accounting_broker_order_authorization_requires_confirmed_trade.cpp
    shell_accounting_broker_order_authorization_requires_confirmed_trade
)

add_shell_accounting_broker_order_authorization_gate_test(
    SABOAUserConfirmationTests
    test_shell_accounting_broker_order_authorization_user_confirmation_policy.cpp
    shell_accounting_broker_order_authorization_user_confirmation_policy
)

add_shell_accounting_broker_order_authorization_gate_test(
    SABOANoStrategyTests
    test_shell_accounting_broker_order_authorization_no_strategy_execution.cpp
    shell_accounting_broker_order_authorization_no_strategy_execution
)

add_shell_accounting_broker_order_authorization_gate_test(
    SABOANoAutoTradingTests
    test_shell_accounting_broker_order_authorization_no_auto_trading.cpp
    shell_accounting_broker_order_authorization_no_auto_trading
)

add_shell_accounting_broker_order_authorization_gate_test(
    SABOANoUiBrokerTests
    test_shell_accounting_broker_order_authorization_no_ui_direct_broker.cpp
    shell_accounting_broker_order_authorization_no_ui_direct_broker
)

add_shell_accounting_broker_order_authorization_gate_test(
    SABOAErrorMappingTests
    test_shell_accounting_broker_order_authorization_error_mapping_policy.cpp
    shell_accounting_broker_order_authorization_error_mapping_policy
)

add_shell_accounting_broker_order_authorization_gate_test(
    SABOARollbackTests
    test_shell_accounting_broker_order_authorization_rollback_policy.cpp
    shell_accounting_broker_order_authorization_rollback_policy
)
