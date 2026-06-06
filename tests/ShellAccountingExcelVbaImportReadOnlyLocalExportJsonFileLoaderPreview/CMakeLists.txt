find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T262LocalExportJsonFileLoaderPreview
    excel_vba_import_readonly_local_export_json_file_loader_preview.cpp
)

target_link_libraries(T262LocalExportJsonFileLoaderPreview
    PRIVATE
        ETFDecisionTerminal::ShellServices
        Qt6::Core
)

target_compile_features(T262LocalExportJsonFileLoaderPreview PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_readonly_local_export_json_file_loader_preview
    COMMAND
        T262LocalExportJsonFileLoaderPreview
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_readonly_local_export_json_file_loader_preview PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
