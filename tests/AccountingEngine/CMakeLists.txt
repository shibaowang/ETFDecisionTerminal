add_executable(AccountingEngineBoundaryTests
    test_accounting_engine_boundary.cpp
)

target_link_libraries(AccountingEngineBoundaryTests
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
)

target_compile_features(AccountingEngineBoundaryTests PRIVATE cxx_std_17)

add_test(NAME accounting_engine_boundary COMMAND AccountingEngineBoundaryTests)
