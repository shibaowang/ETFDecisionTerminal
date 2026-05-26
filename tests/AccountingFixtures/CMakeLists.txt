add_test(
    NAME accounting_replay_fixture_static_validation
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/validate_accounting_replay_fixtures.py
        --fixtures-dir
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_fixture_static_validation PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
