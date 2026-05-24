add_executable(DataAccessTests
    test_data_access.cpp
)

add_executable(ReadOnlyRepositoryTests
    test_readonly_repositories.cpp
)

target_link_libraries(DataAccessTests
    PRIVATE
        ETFDecisionTerminal::DataAccess
)

target_link_libraries(ReadOnlyRepositoryTests
    PRIVATE
        ETFDecisionTerminal::DataAccess
)

target_compile_features(DataAccessTests PRIVATE cxx_std_17)
target_compile_features(ReadOnlyRepositoryTests PRIVATE cxx_std_17)

add_test(
    NAME dataaccess_database_init
    COMMAND
        DataAccessTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

add_test(
    NAME dataaccess_readonly_repositories
    COMMAND
        ReadOnlyRepositoryTests
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

add_test(
    NAME dataservice_cli_summary
    COMMAND
        $<TARGET_FILE:ETFDataService>
        --db-summary
        --db
        ${CMAKE_CURRENT_BINARY_DIR}/dataservice_cli.sqlite
)

add_test(
    NAME dataservice_cli_list_accounts
    COMMAND
        $<TARGET_FILE:ETFDataService>
        --list-accounts
        --db
        ${CMAKE_CURRENT_BINARY_DIR}/dataservice_cli.sqlite
)

add_test(
    NAME dataservice_cli_list_portfolios
    COMMAND
        $<TARGET_FILE:ETFDataService>
        --list-portfolios
        --db
        ${CMAKE_CURRENT_BINARY_DIR}/dataservice_cli.sqlite
)

add_test(
    NAME dataservice_cli_list_instruments
    COMMAND
        $<TARGET_FILE:ETFDataService>
        --list-instruments
        --db
        ${CMAKE_CURRENT_BINARY_DIR}/dataservice_cli.sqlite
)

add_test(
    NAME dataservice_cli_list_strategies
    COMMAND
        $<TARGET_FILE:ETFDataService>
        --list-strategies
        --db
        ${CMAKE_CURRENT_BINARY_DIR}/dataservice_cli.sqlite
)

add_test(
    NAME dataservice_cli_list_otc_missing
    COMMAND
        $<TARGET_FILE:ETFDataService>
        --list-otc
        --db
        ${CMAKE_CURRENT_BINARY_DIR}/dataservice_cli.sqlite
        --strategy-code
        159509
)

set_tests_properties(
    dataservice_cli_summary
    dataservice_cli_list_accounts
    dataservice_cli_list_portfolios
    dataservice_cli_list_instruments
    dataservice_cli_list_strategies
    dataservice_cli_list_otc_missing
    PROPERTIES
        DEPENDS dataservice_cli_init
)

set_tests_properties(dataservice_cli_list_otc_missing PROPERTIES
    WILL_FAIL TRUE
)
