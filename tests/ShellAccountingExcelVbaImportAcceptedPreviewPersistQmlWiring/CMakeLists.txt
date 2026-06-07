find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T268AcceptedPreviewPersistQmlWiring
    excel_vba_import_accepted_preview_persist_qml_wiring.cpp
)

target_link_libraries(T268AcceptedPreviewPersistQmlWiring
    PRIVATE
        ETFDecisionTerminal::ShellServices
        Qt6::Core
)

target_compile_features(T268AcceptedPreviewPersistQmlWiring PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_accepted_preview_persist_qml_wiring
    COMMAND
        T268AcceptedPreviewPersistQmlWiring
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_accepted_preview_persist_qml_wiring PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
