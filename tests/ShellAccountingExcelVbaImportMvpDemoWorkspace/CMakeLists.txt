find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T273ExcelVbaImportMvpDemoWorkspace
    excel_vba_import_mvp_demo_workspace.cpp
)

target_link_libraries(T273ExcelVbaImportMvpDemoWorkspace
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        Qt6::Core
)

target_compile_features(T273ExcelVbaImportMvpDemoWorkspace PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_mvp_demo_workspace
    COMMAND
        T273ExcelVbaImportMvpDemoWorkspace
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_mvp_demo_workspace PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
