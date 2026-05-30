add_library(ShellAccountingAuditWriteAuthorizationGateLib STATIC
    ShellAccountingAuditWriteAuthorizationGate.cpp
    ShellAccountingAuditWriteAuthorizationGate.h
)

target_compile_features(ShellAccountingAuditWriteAuthorizationGateLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingAuditWriteAuthorizationGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_audit_write_authorization_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingAuditWriteAuthorizationGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_audit_write_authorization_gate_test(
    ShellAccountingAuditWriteAuthorizationGateTests
    test_shell_accounting_audit_write_authorization_gate.cpp
    shell_accounting_audit_write_authorization_gate
)

add_shell_accounting_audit_write_authorization_gate_test(
    ShellAccountingAuditWriteAuthorizationNoAuditWriteYetTests
    test_shell_accounting_audit_write_authorization_no_audit_write_yet.cpp
    shell_accounting_audit_write_authorization_no_audit_write_yet
)

add_shell_accounting_audit_write_authorization_gate_test(
    ShellAccountingAuditWriteAuthorizationDataServiceOnlyPolicyTests
    test_shell_accounting_audit_write_authorization_dataservice_only_policy.cpp
    shell_accounting_audit_write_authorization_dataservice_only_policy
)

add_shell_accounting_audit_write_authorization_gate_test(
    ShellAccountingAuditWriteAuthorizationSnapshotWriteInputPolicyTests
    test_shell_accounting_audit_write_authorization_snapshot_write_input_policy.cpp
    shell_accounting_audit_write_authorization_snapshot_write_input_policy
)

add_shell_accounting_audit_write_authorization_gate_test(
    ShellAccountingAuditWriteAuthorizationPayloadPolicyTests
    test_shell_accounting_audit_write_authorization_payload_policy.cpp
    shell_accounting_audit_write_authorization_payload_policy
)

add_shell_accounting_audit_write_authorization_gate_test(
    ShellAccountingAuditWriteAuthorizationForbiddenPayloadPolicyTests
    test_shell_accounting_audit_write_authorization_forbidden_payload_policy.cpp
    shell_accounting_audit_write_authorization_forbidden_payload_policy
)

add_shell_accounting_audit_write_authorization_gate_test(
    ShellAccountingAuditWriteAuthorizationNoUiTriggeredAuditTests
    test_shell_accounting_audit_write_authorization_no_ui_triggered_audit.cpp
    shell_accounting_audit_write_authorization_no_ui_triggered_audit
)

add_shell_accounting_audit_write_authorization_gate_test(
    ShellAccountingAuditWriteAuthorizationNoTradeOrStrategyTests
    test_shell_accounting_audit_write_authorization_no_trade_or_strategy.cpp
    shell_accounting_audit_write_authorization_no_trade_or_strategy
)

add_shell_accounting_audit_write_authorization_gate_test(
    ShellAccountingAuditWriteAuthorizationErrorMappingPolicyTests
    test_shell_accounting_audit_write_authorization_error_mapping_policy.cpp
    shell_accounting_audit_write_authorization_error_mapping_policy
)

add_shell_accounting_audit_write_authorization_gate_test(
    ShellAccountingAuditWriteAuthorizationRollbackPolicyTests
    test_shell_accounting_audit_write_authorization_rollback_policy.cpp
    shell_accounting_audit_write_authorization_rollback_policy
)
