find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T258ExcelVbaImportDataServicePreviewAction
    excel_vba_import_readonly_dataservice_preview_action.cpp
)

target_link_libraries(T258ExcelVbaImportDataServicePreviewAction
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        Qt6::Core
)

target_compile_features(T258ExcelVbaImportDataServicePreviewAction PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_readonly_dataservice_preview_action
    COMMAND
        T258ExcelVbaImportDataServicePreviewAction
        migrations/001_initial_schema.sql
        tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/fixtures/TASK258_valid_buy_preview_payload.json
        tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/fixtures/TASK258_missing_required_header_preview_payload.json
)

set_tests_properties(shell_accounting_excel_vba_import_readonly_dataservice_preview_action PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
