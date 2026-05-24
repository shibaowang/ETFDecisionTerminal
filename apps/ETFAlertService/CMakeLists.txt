add_executable(ETFAlertService
    src/main.cpp
)

target_link_libraries(ETFAlertService
    PRIVATE
        ETFDecisionTerminal::Protocol
)

target_compile_features(ETFAlertService PRIVATE cxx_std_17)
