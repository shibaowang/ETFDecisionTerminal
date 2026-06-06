find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T257ExcelVbaImportParserBoundarySlice
    t257_parser_boundary_slice.cpp
)

target_link_libraries(T257ExcelVbaImportParserBoundarySlice
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::AccountingEngine
        Qt6::Core
)

target_compile_features(T257ExcelVbaImportParserBoundarySlice PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary
    COMMAND
        T257ExcelVbaImportParserBoundarySlice
        tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_buy_only_import_payload.json
        tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_chinese_header_buy_partial_sell_import_payload.json
        tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_cash_adjustment_import_payload.json
        tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_missing_required_header_import_payload.json
        tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_invalid_action_amount_cash_import_payload.json
)

set_tests_properties(shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
