add_test(
    NAME shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py
        --source-root ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
