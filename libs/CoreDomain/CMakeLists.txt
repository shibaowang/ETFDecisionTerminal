add_library(ETFCoreDomain STATIC
    src/CoreDomain.cpp
    src/Enums.cpp
    src/FixedPoint.cpp
    src/Identifiers.cpp
)

add_library(ETFDecisionTerminal::CoreDomain ALIAS ETFCoreDomain)

target_include_directories(ETFCoreDomain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(ETFCoreDomain PUBLIC cxx_std_17)

set_target_properties(ETFCoreDomain PROPERTIES
    OUTPUT_NAME ETFCoreDomain
)
