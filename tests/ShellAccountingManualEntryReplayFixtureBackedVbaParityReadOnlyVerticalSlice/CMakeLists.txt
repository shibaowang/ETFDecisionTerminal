find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(T253FixtureBackedVbaParitySlice
    manual_entry_replay_fixture_backed_vba_parity_readonly_vertical_slice.cpp
)

target_link_libraries(T253FixtureBackedVbaParitySlice
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
        Qt6::Core
)

target_compile_features(T253FixtureBackedVbaParitySlice PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_manual_entry_replay_fixture_backed_vba_parity_readonly_vertical_slice
    COMMAND
        T253FixtureBackedVbaParitySlice
        tests/ShellAccountingManualEntryReplayFixtureBackedVbaParityReadOnlyVerticalSlice/fixtures/TASK253_vba_parity_buy_partial_sell.json
)

set_tests_properties(shell_accounting_manual_entry_replay_fixture_backed_vba_parity_readonly_vertical_slice PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
