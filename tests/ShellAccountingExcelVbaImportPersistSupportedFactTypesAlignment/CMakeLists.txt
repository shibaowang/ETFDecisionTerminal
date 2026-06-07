find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T269ExcelVbaImportPersistSupportedFactTypesAlignment
    excel_vba_import_persist_supported_fact_types_alignment.cpp
)

target_link_libraries(T269ExcelVbaImportPersistSupportedFactTypesAlignment
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        Qt6::Core
)

target_compile_features(T269ExcelVbaImportPersistSupportedFactTypesAlignment PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_excel_vba_import_persist_supported_fact_types_alignment
    COMMAND
        T269ExcelVbaImportPersistSupportedFactTypesAlignment
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_excel_vba_import_persist_supported_fact_types_alignment PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
