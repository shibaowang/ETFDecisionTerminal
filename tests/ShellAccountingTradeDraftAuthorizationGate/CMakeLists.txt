add_library(ShellAccountingTradeDraftAuthorizationGateLib STATIC
    ShellAccountingTradeDraftAuthorizationGate.cpp
    ShellAccountingTradeDraftAuthorizationGate.h
)

target_compile_features(ShellAccountingTradeDraftAuthorizationGateLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingTradeDraftAuthorizationGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_tradedraft_authorization_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingTradeDraftAuthorizationGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_tradedraft_authorization_gate_test(
    ShellAccountingTradeDraftAuthorizationGateTests
    test_shell_accounting_tradedraft_authorization_gate.cpp
    shell_accounting_tradedraft_authorization_gate
)

add_shell_accounting_tradedraft_authorization_gate_test(
    ShellAccountingTradeDraftAuthorizationNoTradeDraftYetTests
    test_shell_accounting_tradedraft_authorization_no_tradedraft_yet.cpp
    shell_accounting_tradedraft_authorization_no_tradedraft_yet
)

add_shell_accounting_tradedraft_authorization_gate_test(
    ShellAccountingTradeDraftAuthorizationDataServiceOnlyPolicyTests
    test_shell_accounting_tradedraft_authorization_dataservice_only_policy.cpp
    shell_accounting_tradedraft_authorization_dataservice_only_policy
)

add_shell_accounting_tradedraft_authorization_gate_test(
    ShellAccountingTradeDraftAuthorizationInputPolicyTests
    test_shell_accounting_tradedraft_authorization_input_policy.cpp
    shell_accounting_tradedraft_authorization_input_policy
)

add_shell_accounting_tradedraft_authorization_gate_test(
    ShellAccountingTradeDraftAuthorizationPayloadPolicyTests
    test_shell_accounting_tradedraft_authorization_payload_policy.cpp
    shell_accounting_tradedraft_authorization_payload_policy
)

add_shell_accounting_tradedraft_authorization_gate_test(
    ShellAccountingTradeDraftAuthorizationAuditPolicyTests
    test_shell_accounting_tradedraft_authorization_audit_policy.cpp
    shell_accounting_tradedraft_authorization_audit_policy
)

add_shell_accounting_tradedraft_authorization_gate_test(
    ShellAccountingTradeDraftAuthorizationNoUiTriggeredDraftTests
    test_shell_accounting_tradedraft_authorization_no_ui_triggered_draft.cpp
    shell_accounting_tradedraft_authorization_no_ui_triggered_draft
)

add_shell_accounting_tradedraft_authorization_gate_test(
    ShellAccountingTradeDraftAuthorizationNoTradeExecutionTests
    test_shell_accounting_tradedraft_authorization_no_trade_execution.cpp
    shell_accounting_tradedraft_authorization_no_trade_execution
)

add_shell_accounting_tradedraft_authorization_gate_test(
    ShellAccountingTradeDraftAuthorizationNoStrategyOrBrokerTests
    test_shell_accounting_tradedraft_authorization_no_strategy_or_broker.cpp
    shell_accounting_tradedraft_authorization_no_strategy_or_broker
)

add_shell_accounting_tradedraft_authorization_gate_test(
    ShellAccountingTradeDraftAuthorizationErrorMappingPolicyTests
    test_shell_accounting_tradedraft_authorization_error_mapping_policy.cpp
    shell_accounting_tradedraft_authorization_error_mapping_policy
)

add_shell_accounting_tradedraft_authorization_gate_test(
    ShellAccountingTradeDraftAuthorizationRollbackPolicyTests
    test_shell_accounting_tradedraft_authorization_rollback_policy.cpp
    shell_accounting_tradedraft_authorization_rollback_policy
)
