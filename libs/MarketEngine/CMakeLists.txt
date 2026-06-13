find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_library(ETFMarketEngine STATIC
    src/MarketEngine.cpp
    src/DisabledMarketDataProvider.cpp
    src/FixtureMarketDataProvider.cpp
    src/LivePublicMarketDataProvider.cpp
    src/MarketDataRefreshEngine.cpp
    src/MarketDataSafetyPolicy.cpp
)

add_library(ETFDecisionTerminal::MarketEngine ALIAS ETFMarketEngine)

target_include_directories(ETFMarketEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFMarketEngine
    PUBLIC
        ETFDecisionTerminal::CoreDomain
        Qt6::Core
        Qt6::Network
)

target_compile_features(ETFMarketEngine PUBLIC cxx_std_17)

set_target_properties(ETFMarketEngine PROPERTIES
    OUTPUT_NAME ETFMarketEngine
)
