find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_test(
    NAME shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/excel_vba_import_readonly_preview_qml_panel_wiring.py
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
