add_library(ETFProtocol STATIC
    src/Protocol.cpp
)

add_library(ETFDecisionTerminal::Protocol ALIAS ETFProtocol)

target_include_directories(ETFProtocol
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(ETFProtocol PUBLIC cxx_std_17)

set_target_properties(ETFProtocol PROPERTIES
    OUTPUT_NAME ETFProtocol
)
