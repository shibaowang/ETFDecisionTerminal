find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(E280MarketDataRefreshEngineFullDelivery
    market_data_historical_high_refresh_engine_full_delivery.cpp
)

target_link_libraries(E280MarketDataRefreshEngineFullDelivery
    PRIVATE
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::MarketEngine
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::ShellServices
        ETFDecisionTerminal::StrategyEngine
        Qt6::Core
)

target_compile_features(E280MarketDataRefreshEngineFullDelivery PRIVATE cxx_std_17)

add_test(
    NAME market_data_historical_high_refresh_engine_full_delivery
    COMMAND
        E280MarketDataRefreshEngineFullDelivery
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(market_data_historical_high_refresh_engine_full_delivery PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
