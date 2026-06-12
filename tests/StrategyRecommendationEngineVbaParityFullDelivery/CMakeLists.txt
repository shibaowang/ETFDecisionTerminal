find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(E277StrategyRecommendationEngineVbaParityFullDelivery
    strategy_recommendation_engine_vba_parity_full_delivery.cpp
)

target_link_libraries(E277StrategyRecommendationEngineVbaParityFullDelivery
    PRIVATE
        ETFDecisionTerminal::StrategyEngine
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::ShellServices
        Qt6::Core
)

target_compile_features(E277StrategyRecommendationEngineVbaParityFullDelivery PRIVATE cxx_std_17)

add_test(
    NAME strategy_recommendation_engine_vba_parity_full_delivery
    COMMAND
        E277StrategyRecommendationEngineVbaParityFullDelivery
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(strategy_recommendation_engine_vba_parity_full_delivery PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
