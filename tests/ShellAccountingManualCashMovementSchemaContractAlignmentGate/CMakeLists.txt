add_test(
    NAME shell_accounting_manual_cash_movement_schema_contract_alignment_gate
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/manual_cash_movement_schema_contract_alignment_gate.py
        --source-root ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_manual_cash_movement_schema_contract_alignment_gate PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
