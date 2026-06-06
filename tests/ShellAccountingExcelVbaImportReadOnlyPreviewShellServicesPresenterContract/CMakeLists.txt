find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(T260ShellServicesPreviewContract
    excel_vba_import_readonly_preview_shellservices_presenter_contract.cpp
)

target_link_libraries(T260ShellServicesPreviewContract
    PRIVATE
        ETFDecisionTerminal::ShellServices
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::DataAccess
        Qt6::Core
)

target_compile_features(T260ShellServicesPreviewContract PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract
    COMMAND
        T260ShellServicesPreviewContract
        --source-root
        ${PROJECT_SOURCE_DIR}
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
        --fixture
        ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/TASK260_valid_buy_presenter_preview_payload.json
        --fixture
        ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/TASK260_missing_required_header_presenter_preview_payload.json
)

set_tests_properties(shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
