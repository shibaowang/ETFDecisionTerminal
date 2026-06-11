find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T270ExcelVbaImportPersistPostWriteReadbackRefresh
    excel_vba_import_persist_post_write_readback_refresh.cpp
)

target_link_libraries(T270ExcelVbaImportPersistPostWriteReadbackRefresh
    PRIVATE
        ETFDecisionTerminal::ShellServices
        Qt6::Core
)

target_compile_features(T270ExcelVbaImportPersistPostWriteReadbackRefresh PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_persist_post_write_readback_refresh
    COMMAND
        T270ExcelVbaImportPersistPostWriteReadbackRefresh
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_persist_post_write_readback_refresh PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
