add_executable(ETFStrategyService
    src/main.cpp
)

target_link_libraries(ETFStrategyService
    PRIVATE
        ETFDecisionTerminal::StrategyEngine
        ETFDecisionTerminal::Protocol
)

target_compile_features(ETFStrategyService PRIVATE cxx_std_17)
