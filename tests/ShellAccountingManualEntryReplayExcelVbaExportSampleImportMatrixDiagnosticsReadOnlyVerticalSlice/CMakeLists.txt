find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T256ExcelVbaExportSampleImportMatrixDiagnosticsSlice
    t256_matrix_slice.cpp
)

target_link_libraries(T256ExcelVbaExportSampleImportMatrixDiagnosticsSlice
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
        Qt6::Core
)

target_compile_features(T256ExcelVbaExportSampleImportMatrixDiagnosticsSlice PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_readonly_vertical_slice
    COMMAND
        T256ExcelVbaExportSampleImportMatrixDiagnosticsSlice
        tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_buy_only_export_sample.json
        tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_buy_partial_sell_export_sample.json
        tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_cash_adjustment_export_sample.json
        tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_missing_required_header_sample.json
        tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_unsupported_or_issue_sample.json
)

set_tests_properties(shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_readonly_vertical_slice PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
