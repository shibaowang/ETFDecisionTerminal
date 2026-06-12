add_library(ETFStrategyEngine STATIC
    src/StrategyRecommendationEngine.cpp
    src/StrategyEngine.cpp
)

add_library(ETFDecisionTerminal::StrategyEngine ALIAS ETFStrategyEngine)

target_include_directories(ETFStrategyEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFStrategyEngine
    PUBLIC
        ETFDecisionTerminal::CoreDomain
)

target_compile_features(ETFStrategyEngine PUBLIC cxx_std_17)

set_target_properties(ETFStrategyEngine PROPERTIES
    OUTPUT_NAME ETFStrategyEngine
)
