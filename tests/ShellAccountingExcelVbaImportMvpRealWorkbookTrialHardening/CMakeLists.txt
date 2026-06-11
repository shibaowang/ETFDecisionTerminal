find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(T275ExcelVbaImportMvpRealWorkbookTrialHardening
    excel_vba_import_mvp_real_workbook_trial_hardening.cpp
)

target_link_libraries(T275ExcelVbaImportMvpRealWorkbookTrialHardening
    PRIVATE
        ETFDecisionTerminal::ShellServices
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::DataAccess
        Qt6::Core
)

target_compile_features(T275ExcelVbaImportMvpRealWorkbookTrialHardening PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_mvp_real_workbook_trial_hardening
    COMMAND
        T275ExcelVbaImportMvpRealWorkbookTrialHardening
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_mvp_real_workbook_trial_hardening PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
