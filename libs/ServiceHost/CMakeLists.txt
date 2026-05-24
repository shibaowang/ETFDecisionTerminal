add_library(ETFServiceHost STATIC
    src/DemoServiceHost.cpp
    src/ServiceHost.cpp
)

add_library(ETFDecisionTerminal::ServiceHost ALIAS ETFServiceHost)

target_include_directories(ETFServiceHost
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFServiceHost
    PUBLIC
        ETFDecisionTerminal::CoreDomain
        ETFDecisionTerminal::Protocol
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::Transport
)

target_compile_features(ETFServiceHost PUBLIC cxx_std_17)

set_target_properties(ETFServiceHost PROPERTIES
    OUTPUT_NAME ETFServiceHost
)
