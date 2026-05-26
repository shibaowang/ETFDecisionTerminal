find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(AccountingFixtureLoaderTests
    AccountingFixtureLoader.cpp
    test_accounting_fixture_loader.cpp
)

target_link_libraries(AccountingFixtureLoaderTests
    PRIVATE
        Qt6::Core
)

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

add_test(
    NAME accounting_replay_fixture_loader
    COMMAND
        AccountingFixtureLoaderTests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_fixture_loader PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
