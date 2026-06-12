find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(E278TradeDraftManualRecommendationFlowFullDelivery
    tradedraft_manual_recommendation_flow_full_delivery.cpp
)

target_link_libraries(E278TradeDraftManualRecommendationFlowFullDelivery
    PRIVATE
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::ShellServices
        ETFDecisionTerminal::StrategyEngine
        Qt6::Core
)

target_compile_features(E278TradeDraftManualRecommendationFlowFullDelivery PRIVATE cxx_std_17)

add_test(
    NAME tradedraft_manual_recommendation_flow_full_delivery
    COMMAND
        E278TradeDraftManualRecommendationFlowFullDelivery
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(tradedraft_manual_recommendation_flow_full_delivery PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
