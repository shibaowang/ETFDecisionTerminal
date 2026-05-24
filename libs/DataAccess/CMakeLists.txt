add_library(ETFDataAccess STATIC
    src/DataAccess.cpp
)

add_library(ETFDecisionTerminal::DataAccess ALIAS ETFDataAccess)

target_include_directories(ETFDataAccess
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFDataAccess
    PUBLIC
        ETFDecisionTerminal::CoreDomain
)

target_compile_features(ETFDataAccess PUBLIC cxx_std_17)

set_target_properties(ETFDataAccess PROPERTIES
    OUTPUT_NAME ETFDataAccess
)
