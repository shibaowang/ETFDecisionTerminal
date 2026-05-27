add_executable(AccountingEngineBoundaryTests
    test_accounting_engine_boundary.cpp
)

target_link_libraries(AccountingEngineBoundaryTests
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
)

target_compile_features(AccountingEngineBoundaryTests PRIVATE cxx_std_17)

add_test(NAME accounting_engine_boundary COMMAND AccountingEngineBoundaryTests)

add_executable(AccountingReplayDtoParserBoundaryTests
    test_accounting_replay_dto_parser.cpp
)

target_link_libraries(AccountingReplayDtoParserBoundaryTests
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
)

target_compile_features(AccountingReplayDtoParserBoundaryTests PRIVATE cxx_std_17)

add_test(NAME accounting_replay_dto_parser_boundary COMMAND AccountingReplayDtoParserBoundaryTests)

add_executable(AccountingReplayEngineEmptyLedgerTests
    test_accounting_replay_engine_empty_ledger.cpp
)

target_link_libraries(AccountingReplayEngineEmptyLedgerTests
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
)

target_compile_features(AccountingReplayEngineEmptyLedgerTests PRIVATE cxx_std_17)

add_test(NAME accounting_replay_engine_empty_ledger COMMAND AccountingReplayEngineEmptyLedgerTests)

add_executable(AccountingReplayEngineSingleBuyTests
    test_accounting_replay_engine_single_buy.cpp
)

target_link_libraries(AccountingReplayEngineSingleBuyTests
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
)

target_compile_features(AccountingReplayEngineSingleBuyTests PRIVATE cxx_std_17)

add_test(NAME accounting_replay_engine_single_buy COMMAND AccountingReplayEngineSingleBuyTests)

add_executable(AccountingReplayEngineBuySellPartialTests
    test_accounting_replay_engine_buy_sell_partial.cpp
)

target_link_libraries(AccountingReplayEngineBuySellPartialTests
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
)

target_compile_features(AccountingReplayEngineBuySellPartialTests PRIVATE cxx_std_17)

add_test(NAME accounting_replay_engine_buy_sell_partial COMMAND AccountingReplayEngineBuySellPartialTests)

add_executable(AccountingReplayEngineSellExceedsPositionTests
    test_accounting_replay_engine_sell_exceeds_position.cpp
)

target_link_libraries(AccountingReplayEngineSellExceedsPositionTests
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
)

target_compile_features(AccountingReplayEngineSellExceedsPositionTests PRIVATE cxx_std_17)

add_test(NAME accounting_replay_engine_sell_exceeds_position COMMAND AccountingReplayEngineSellExceedsPositionTests)

add_executable(AccountingReplayEngineMissingFeeTests
    test_accounting_replay_engine_missing_fee.cpp
)

target_link_libraries(AccountingReplayEngineMissingFeeTests
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
)

target_compile_features(AccountingReplayEngineMissingFeeTests PRIVATE cxx_std_17)

add_test(NAME accounting_replay_engine_missing_fee COMMAND AccountingReplayEngineMissingFeeTests)

add_executable(AccountingReplayEngineNegativeCashTests
    test_accounting_replay_engine_negative_cash.cpp
)

target_link_libraries(AccountingReplayEngineNegativeCashTests
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
)

target_compile_features(AccountingReplayEngineNegativeCashTests PRIVATE cxx_std_17)

add_test(NAME accounting_replay_engine_negative_cash COMMAND AccountingReplayEngineNegativeCashTests)
