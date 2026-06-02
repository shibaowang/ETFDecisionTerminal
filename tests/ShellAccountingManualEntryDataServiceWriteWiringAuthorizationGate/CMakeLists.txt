add_test(
    NAME shell_accounting_manual_entry_dataservice_write_wiring_authorization
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/manual_entry_dataservice_write_wiring_authorization_gate.py
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_manual_entry_dataservice_write_wiring_authorization PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
