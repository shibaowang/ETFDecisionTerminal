add_library(ETFQuantization STATIC
    src/Quantization.cpp
)

add_library(ETFDecisionTerminal::Quantization ALIAS ETFQuantization)

target_include_directories(ETFQuantization
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFQuantization
    PUBLIC
        ETFDecisionTerminal::CoreDomain
)

target_compile_features(ETFQuantization PUBLIC cxx_std_17)

set_target_properties(ETFQuantization PROPERTIES
    OUTPUT_NAME ETFQuantization
)
