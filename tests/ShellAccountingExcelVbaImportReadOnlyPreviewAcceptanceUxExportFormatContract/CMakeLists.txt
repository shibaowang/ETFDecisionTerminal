find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_test(
    NAME shell_accounting_excel_vba_import_readonly_preview_acceptance_ux_export_format_contract
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/excel_vba_import_readonly_preview_acceptance_ux_export_format_contract.py
        --source-root ${CMAKE_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_readonly_preview_acceptance_ux_export_format_contract PROPERTIES
    LABELS "shell_accounting;excel_vba_import;readonly;preview;acceptance;task263"
)
