add_executable(SAManualEntryDataServiceWriteWiringImplementationTests
    manual_entry_dataservice_write_wiring_implementation.cpp
)

target_link_libraries(SAManualEntryDataServiceWriteWiringImplementationTests
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::Protocol
)
target_compile_features(SAManualEntryDataServiceWriteWiringImplementationTests PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_manual_entry_dataservice_write_wiring_implementation
    COMMAND
        SAManualEntryDataServiceWriteWiringImplementationTests
        --source-root ${PROJECT_SOURCE_DIR}
)
