find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T267ShellServicesPersistManualEntryContract
    excel_vba_import_persist_manual_entry_shellservices_presenter_contract.cpp
)

target_link_libraries(T267ShellServicesPersistManualEntryContract
    PRIVATE
        ETFDecisionTerminal::ShellServices
        Qt6::Core
)

target_compile_features(T267ShellServicesPersistManualEntryContract PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_persist_manual_entry_shellservices_presenter_contract
    COMMAND
        T267ShellServicesPersistManualEntryContract
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_persist_manual_entry_shellservices_presenter_contract PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
