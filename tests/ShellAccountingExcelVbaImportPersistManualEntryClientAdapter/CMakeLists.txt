find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(ShellAccountingExcelVbaImportPersistManualEntryClientAdapter
    excel_vba_import_persist_manual_entry_client_adapter.cpp
)

target_link_libraries(ShellAccountingExcelVbaImportPersistManualEntryClientAdapter
    PRIVATE
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::DataAccess
        Qt6::Core
)

target_compile_features(ShellAccountingExcelVbaImportPersistManualEntryClientAdapter PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_persist_manual_entry_client_adapter
    COMMAND
        ShellAccountingExcelVbaImportPersistManualEntryClientAdapter
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_persist_manual_entry_client_adapter PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
