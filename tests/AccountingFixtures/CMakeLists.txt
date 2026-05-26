find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(AccountingFixtureLoaderTests
    AccountingFixtureLoader.cpp
    test_accounting_fixture_loader.cpp
)

add_executable(AccountingReplayHarnessTests
    AccountingFixtureLoader.cpp
    AccountingReplayResult.cpp
    AccountingReplayResultMapper.cpp
    AccountingReplayMinimalEngine.cpp
    AccountingReplayStubEngine.cpp
    AccountingReplayTestHarness.cpp
    test_accounting_replay_harness.cpp
)

add_executable(AccountingReplayResultTests
    AccountingReplayResult.cpp
    AccountingReplayResultMapper.cpp
    AccountingReplayStubEngine.cpp
    test_accounting_replay_result.cpp
)

add_executable(AccountingReplayAssertionTests
    AccountingFixtureLoader.cpp
    AccountingReplayResult.cpp
    AccountingReplayResultMapper.cpp
    AccountingReplayMinimalEngine.cpp
    AccountingReplayStubEngine.cpp
    AccountingReplayTestHarness.cpp
    AccountingExpectedOutputInspector.cpp
    AccountingReplayAssertionSkeleton.cpp
    test_accounting_replay_assertions.cpp
)

add_executable(AccountingReplayMinimalFx001Tests
    AccountingFixtureLoader.cpp
    AccountingReplayResult.cpp
    AccountingReplayResultMapper.cpp
    AccountingReplayMinimalEngine.cpp
    AccountingReplayStubEngine.cpp
    AccountingReplayTestHarness.cpp
    AccountingExpectedOutputInspector.cpp
    AccountingReplayAssertionSkeleton.cpp
    test_accounting_replay_minimal_engine.cpp
)

target_link_libraries(AccountingFixtureLoaderTests
    PRIVATE
        Qt6::Core
)

target_link_libraries(AccountingReplayHarnessTests
    PRIVATE
        Qt6::Core
)

target_link_libraries(AccountingReplayResultTests
    PRIVATE
        Qt6::Core
)

target_link_libraries(AccountingReplayAssertionTests
    PRIVATE
        Qt6::Core
)

target_link_libraries(AccountingReplayMinimalFx001Tests
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

add_test(
    NAME accounting_replay_harness_skeleton
    COMMAND
        AccountingReplayHarnessTests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_harness_skeleton PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_result_skeleton
    COMMAND
        AccountingReplayResultTests
)

set_tests_properties(accounting_replay_result_skeleton PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_expected_output_assertions
    COMMAND
        AccountingReplayAssertionTests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_expected_output_assertions PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_minimal_fx001
    COMMAND
        AccountingReplayMinimalFx001Tests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_minimal_fx001 PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_minimal_fx002
    COMMAND
        AccountingReplayMinimalFx001Tests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_minimal_fx002 PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_minimal_fx003
    COMMAND
        AccountingReplayMinimalFx001Tests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_minimal_fx003 PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_minimal_fx004
    COMMAND
        AccountingReplayMinimalFx001Tests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_minimal_fx004 PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_minimal_fx005
    COMMAND
        AccountingReplayMinimalFx001Tests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_minimal_fx005 PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_minimal_fx006
    COMMAND
        AccountingReplayMinimalFx001Tests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_minimal_fx006 PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_minimal_fx007
    COMMAND
        AccountingReplayMinimalFx001Tests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_minimal_fx007 PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_minimal_fx008
    COMMAND
        AccountingReplayMinimalFx001Tests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_minimal_fx008 PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_minimal_fx009
    COMMAND
        AccountingReplayMinimalFx001Tests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_minimal_fx009 PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME accounting_replay_minimal_fx010
    COMMAND
        AccountingReplayMinimalFx001Tests
        ${PROJECT_SOURCE_DIR}/tests/fixtures/accounting_replay
)

set_tests_properties(accounting_replay_minimal_fx010 PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
