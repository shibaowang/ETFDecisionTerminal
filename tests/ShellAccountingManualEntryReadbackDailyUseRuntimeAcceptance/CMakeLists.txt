add_executable(SAManualEntryReadbackDailyUseRuntimeAcceptanceTests
    manual_entry_readback_daily_use_runtime_acceptance.cpp
)

target_link_libraries(SAManualEntryReadbackDailyUseRuntimeAcceptanceTests
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::Protocol
)
target_compile_features(SAManualEntryReadbackDailyUseRuntimeAcceptanceTests PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_manual_entry_readback_daily_use_runtime_acceptance
    COMMAND
        SAManualEntryReadbackDailyUseRuntimeAcceptanceTests
        --source-root ${PROJECT_SOURCE_DIR}
)
