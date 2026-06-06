find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T255ExcelVbaExportSampleImportSlice
    manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice.cpp
)

target_link_libraries(T255ExcelVbaExportSampleImportSlice
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
        Qt6::Core
)

target_compile_features(T255ExcelVbaExportSampleImportSlice PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice
    COMMAND
        T255ExcelVbaExportSampleImportSlice
        tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportReadOnlyVerticalSlice/fixtures/TASK255_sanitized_excel_vba_export_buy_partial_sell.json
)

set_tests_properties(shell_accounting_manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
