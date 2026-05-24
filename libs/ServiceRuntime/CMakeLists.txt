add_library(ETFServiceRuntime STATIC
    src/ActionContext.cpp
    src/ActionDispatcher.cpp
    src/BuiltinActions.cpp
    src/ServiceRuntime.cpp
)

add_library(ETFDecisionTerminal::ServiceRuntime ALIAS ETFServiceRuntime)

target_include_directories(ETFServiceRuntime
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFServiceRuntime
    PUBLIC
        ETFDecisionTerminal::CoreDomain
        ETFDecisionTerminal::Protocol
)

target_compile_features(ETFServiceRuntime PUBLIC cxx_std_17)

set_target_properties(ETFServiceRuntime PROPERTIES
    OUTPUT_NAME ETFServiceRuntime
)
