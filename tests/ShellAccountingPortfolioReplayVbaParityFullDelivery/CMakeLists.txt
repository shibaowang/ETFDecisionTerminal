find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(E276ShellAccountingPortfolioReplayVbaParityFullDelivery
    shell_accounting_portfolio_replay_vba_parity_full_delivery.cpp
)

target_link_libraries(E276ShellAccountingPortfolioReplayVbaParityFullDelivery
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::ShellServices
        Qt6::Core
)

target_compile_features(E276ShellAccountingPortfolioReplayVbaParityFullDelivery PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_portfolio_replay_vba_parity_full_delivery
    COMMAND
        E276ShellAccountingPortfolioReplayVbaParityFullDelivery
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_portfolio_replay_vba_parity_full_delivery PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
