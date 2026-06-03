add_executable(SAManualEntryMvpRuntimeE2eAcceptanceTests
    manual_entry_mvp_runtime_e2e_acceptance.cpp
)

target_link_libraries(SAManualEntryMvpRuntimeE2eAcceptanceTests
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::Protocol
)
target_compile_features(SAManualEntryMvpRuntimeE2eAcceptanceTests PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_manual_entry_mvp_runtime_e2e_acceptance
    COMMAND
        SAManualEntryMvpRuntimeE2eAcceptanceTests
        --source-root ${PROJECT_SOURCE_DIR}
)
