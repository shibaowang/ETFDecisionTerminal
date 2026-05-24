add_executable(ETFDecisionShell
    src/main.cpp
)

target_link_libraries(ETFDecisionShell
    PRIVATE
        ETFDecisionTerminal::Protocol
)

target_compile_features(ETFDecisionShell PRIVATE cxx_std_17)
