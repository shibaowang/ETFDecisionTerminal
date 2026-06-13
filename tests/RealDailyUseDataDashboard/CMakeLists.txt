find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(RealDailyUseDataDashboard
    real_daily_use_data_dashboard.cpp
)

target_link_libraries(RealDailyUseDataDashboard
    PRIVATE
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::MarketEngine
        ETFDecisionTerminal::Protocol
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::Transport
        Qt6::Core
        Qt6::Network
)

target_compile_features(RealDailyUseDataDashboard PRIVATE cxx_std_17)

add_test(
    NAME real_daily_use_data_dashboard
    COMMAND
        RealDailyUseDataDashboard
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(real_daily_use_data_dashboard PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
