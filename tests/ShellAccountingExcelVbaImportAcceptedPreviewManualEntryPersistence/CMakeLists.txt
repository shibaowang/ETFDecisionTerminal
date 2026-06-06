find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T265ExcelVbaImportAcceptedPreviewManualEntryPersistence
    excel_vba_import_accepted_preview_manual_entry_persistence.cpp
)

target_link_libraries(T265ExcelVbaImportAcceptedPreviewManualEntryPersistence
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        Qt6::Core
)

target_compile_features(T265ExcelVbaImportAcceptedPreviewManualEntryPersistence PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence
    COMMAND
        T265ExcelVbaImportAcceptedPreviewManualEntryPersistence
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
