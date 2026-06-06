find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter
    excel_vba_import_readonly_preview_client_adapter.cpp
)

target_link_libraries(ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter
    PRIVATE
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::DataAccess
        Qt6::Core
)

target_compile_features(ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_readonly_preview_client_adapter
    COMMAND
        ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
        --fixture
        ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/TASK259_valid_buy_preview_client_payload.json
        --fixture
        ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/TASK259_missing_required_header_preview_client_payload.json
)

set_tests_properties(shell_accounting_excel_vba_import_readonly_preview_client_adapter PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
