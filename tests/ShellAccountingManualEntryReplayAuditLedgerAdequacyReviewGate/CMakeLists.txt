find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_test(
    NAME shell_accounting_manual_entry_replay_audit_ledger_adequacy_review
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/manual_entry_replay_audit_ledger_adequacy_review_gate.py
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_manual_entry_replay_audit_ledger_adequacy_review PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

