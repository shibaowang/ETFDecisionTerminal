find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T272ExcelVbaImportMvpReleaseReadiness
    excel_vba_import_mvp_release_readiness.cpp
)

target_link_libraries(T272ExcelVbaImportMvpReleaseReadiness
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        Qt6::Core
)

target_compile_features(T272ExcelVbaImportMvpReleaseReadiness PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_mvp_release_readiness
    COMMAND
        T272ExcelVbaImportMvpReleaseReadiness
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_mvp_release_readiness PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
