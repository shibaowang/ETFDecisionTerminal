add_library(ETFDataServiceClient STATIC
    src/DataServiceClient.cpp
    src/DataServiceClientJson.cpp
    src/DataServiceClientTypes.cpp
)

add_library(ETFDecisionTerminal::DataServiceClient ALIAS ETFDataServiceClient)

target_include_directories(ETFDataServiceClient
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFDataServiceClient
    PUBLIC
        ETFDecisionTerminal::CoreDomain
        ETFDecisionTerminal::Protocol
        ETFDecisionTerminal::Transport
)

target_compile_features(ETFDataServiceClient PUBLIC cxx_std_17)

set_target_properties(ETFDataServiceClient PROPERTIES
    OUTPUT_NAME ETFDataServiceClient
)
