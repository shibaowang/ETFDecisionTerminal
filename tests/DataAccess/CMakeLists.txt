add_executable(DataAccessTests
    test_data_access.cpp
)

target_link_libraries(DataAccessTests
    PRIVATE
        ETFDecisionTerminal::DataAccess
)

target_compile_features(DataAccessTests PRIVATE cxx_std_17)

add_test(
    NAME dataaccess_database_init
    COMMAND
        DataAccessTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

add_test(
    NAME dataservice_cli_init
    COMMAND
        $<TARGET_FILE:ETFDataService>
        --init-db
        --db
        ${CMAKE_CURRENT_BINARY_DIR}/dataservice_cli.sqlite
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

add_test(
    NAME dataservice_cli_check
    COMMAND
        $<TARGET_FILE:ETFDataService>
        --check-db
        --db
        ${CMAKE_CURRENT_BINARY_DIR}/dataservice_cli.sqlite
)

set_tests_properties(dataservice_cli_check PROPERTIES
    DEPENDS dataservice_cli_init
)
