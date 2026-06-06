find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T254FixtureParityMatrixSlice
    manual_entry_replay_fixture_parity_matrix_readonly_vertical_slice.cpp
)

target_link_libraries(T254FixtureParityMatrixSlice
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
        Qt6::Core
)

target_compile_features(T254FixtureParityMatrixSlice PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_manual_entry_replay_fixture_parity_matrix_readonly_vertical_slice
    COMMAND
        T254FixtureParityMatrixSlice
        tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_only.json
        tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_partial_sell.json
        tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_cash_adjustment.json
        tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_unsupported_or_issue.json
)

set_tests_properties(shell_accounting_manual_entry_replay_fixture_parity_matrix_readonly_vertical_slice PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
