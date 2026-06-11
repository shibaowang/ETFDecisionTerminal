find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T274ExcelVbaImportMvpExportHelperCompatibility
    excel_vba_import_mvp_export_helper_compatibility.cpp
)

target_link_libraries(T274ExcelVbaImportMvpExportHelperCompatibility
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        Qt6::Core
)

target_compile_features(T274ExcelVbaImportMvpExportHelperCompatibility PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_mvp_export_helper_compatibility
    COMMAND
        T274ExcelVbaImportMvpExportHelperCompatibility
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_mvp_export_helper_compatibility PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
