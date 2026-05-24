add_executable(ETFWatchdog
    src/main.cpp
)

target_link_libraries(ETFWatchdog
    PRIVATE
        ETFDecisionTerminal::Protocol
)

target_compile_features(ETFWatchdog PRIVATE cxx_std_17)
