add_executable(ETFMarketService
    src/main.cpp
)

target_link_libraries(ETFMarketService
    PRIVATE
        ETFDecisionTerminal::MarketEngine
        ETFDecisionTerminal::Protocol
)

target_compile_features(ETFMarketService PRIVATE cxx_std_17)
