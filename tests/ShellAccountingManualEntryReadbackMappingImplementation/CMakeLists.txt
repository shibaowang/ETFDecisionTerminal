add_executable(SAManualEntryReadbackMappingImplementationTests
    manual_entry_readback_mapping_implementation.cpp
)

target_link_libraries(SAManualEntryReadbackMappingImplementationTests
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::Protocol
)
target_compile_features(SAManualEntryReadbackMappingImplementationTests PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_manual_entry_readback_mapping_implementation
    COMMAND
        SAManualEntryReadbackMappingImplementationTests
        --source-root ${PROJECT_SOURCE_DIR}
)
