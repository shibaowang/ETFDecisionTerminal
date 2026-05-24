add_library(ETFSecurity STATIC
    src/Security.cpp
)

add_library(ETFDecisionTerminal::Security ALIAS ETFSecurity)

target_include_directories(ETFSecurity
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(ETFSecurity PUBLIC cxx_std_17)

set_target_properties(ETFSecurity PROPERTIES
    OUTPUT_NAME ETFSecurity
)
