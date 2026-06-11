find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(T271ExcelVbaImportMvpLocalServiceE2eAcceptance
    excel_vba_import_mvp_local_service_e2e_acceptance.cpp
)

target_link_libraries(T271ExcelVbaImportMvpLocalServiceE2eAcceptance
    PRIVATE
        ETFDecisionTerminal::ShellServices
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::DataAccess
        Qt6::Core
)

target_compile_features(T271ExcelVbaImportMvpLocalServiceE2eAcceptance PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance
    COMMAND
        T271ExcelVbaImportMvpLocalServiceE2eAcceptance
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
