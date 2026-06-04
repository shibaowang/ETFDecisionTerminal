find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_test(
    NAME shell_accounting_manual_entry_replay_test_only_dry_run_harness
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/manual_entry_replay_test_only_dry_run_harness.py
        --source-root
        ${PROJECT_SOURCE_DIR}
        --summary-json
)

set_tests_properties(shell_accounting_manual_entry_replay_test_only_dry_run_harness PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
