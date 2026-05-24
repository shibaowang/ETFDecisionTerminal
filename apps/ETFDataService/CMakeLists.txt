add_executable(ETFDataService
    src/main.cpp
)

target_link_libraries(ETFDataService
    PRIVATE
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::CoreDomain
        ETFDecisionTerminal::Protocol
)

target_compile_features(ETFDataService PRIVATE cxx_std_17)
