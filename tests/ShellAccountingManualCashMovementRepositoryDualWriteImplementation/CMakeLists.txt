add_executable(SAManualCashMovementRepositoryDualWriteImplementationTests
    manual_cash_movement_repository_dual_write_implementation.cpp
)

target_link_libraries(SAManualCashMovementRepositoryDualWriteImplementationTests
    PRIVATE
        ETFDecisionTerminal::DataAccess
)
target_compile_features(SAManualCashMovementRepositoryDualWriteImplementationTests PRIVATE cxx_std_17)

function(add_shell_accounting_manual_cash_movement_repository_dual_write_implementation_test ctest_name case_name)
    add_test(
        NAME ${ctest_name}
        COMMAND
            SAManualCashMovementRepositoryDualWriteImplementationTests
            --source-root ${PROJECT_SOURCE_DIR}
            --case ${case_name}
    )
endfunction()

set(_task196_cases
    docs
    docs_index_prompt
    migrations_unmodified
    no_new_migration
    no_schema_file
    dataserviceactions_unmodified
    validation_code_unmodified
    task192_repository_unmodified
    repository_exists
    dataaccess_only
    not_in_dataserviceapi
    not_in_shell
    sqlite_apply_001_002
    deposit_success
    withdrawal_success
    deposit_withdrawal_trade_log
    deposit_withdrawal_cash_adjustment
    cash_adjustment_trade_log_id_link
    trade_log_cash_adjustment_uid_link
    cash_adjustment_trade_log_uid_link
    trade_log_request_id_written
    trade_log_idempotency_key_written
    trade_log_occurred_at_written
    trade_log_source_memo_sanitized
    cash_adjustment_request_id_written
    cash_adjustment_idempotency_key_written
    cash_adjustment_occurred_at_written
    cash_adjustment_source_memo_sanitized
    amount_sign_policy
    trade_log_net_cash_impact
    cash_adjustment_amount_type
    account_portfolio_fields
    duplicate_idempotency_no_extra_trade_log
    duplicate_idempotency_no_extra_cash_adjustment
    duplicate_idempotency_result
    empty_idempotency_stable
    invalid_command_no_trade_log
    invalid_command_no_cash_adjustment
    invalid_movement_type_fail_closed
    adjustment_unauthorized_fail_closed
    reference_failure_no_partial_rows
    cash_adjustment_insert_failure_rolls_back_trade_log
    rollback_failure_no_partial_facts
    no_audit_log_write
    no_ledger_write
    no_accounting_replay
    no_tradedraft_suggestion
    no_broker
    no_network_endpoint
    no_credentials
    no_real_order
    no_automatic_trading
    task195_contract_retained
    task194_gate_evolved
    task192_semantics_retained
    task190_migration_retained
    task182_validation_only
    task178_validation_retained
    broker_gates_retained
    result_flags
    sensitive_memo_rejected
)

foreach(_case IN LISTS _task196_cases)
    add_shell_accounting_manual_cash_movement_repository_dual_write_implementation_test(
        shell_accounting_manual_cash_movement_repository_dual_write_implementation_${_case}
        ${_case}
    )
endforeach()
