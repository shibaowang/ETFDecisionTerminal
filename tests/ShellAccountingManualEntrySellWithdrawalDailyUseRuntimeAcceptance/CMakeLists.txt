add_executable(SAManualEntrySellWithdrawalDailyUseRuntimeAcceptanceTests
    manual_entry_sell_withdrawal_daily_use_runtime_acceptance.cpp
)

target_link_libraries(SAManualEntrySellWithdrawalDailyUseRuntimeAcceptanceTests
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::Protocol
)
target_compile_features(SAManualEntrySellWithdrawalDailyUseRuntimeAcceptanceTests PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance
    COMMAND
        SAManualEntrySellWithdrawalDailyUseRuntimeAcceptanceTests
        --source-root
        ${PROJECT_SOURCE_DIR}
)
