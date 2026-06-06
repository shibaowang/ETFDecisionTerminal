find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_test(
    NAME shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_gate
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/excel_vba_import_preview_to_manual_entry_persistence_authorization_gate.py
        --source-root ${CMAKE_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_gate PROPERTIES
    LABELS "shell_accounting;excel_vba_import;authorization;persistence;task264"
)

