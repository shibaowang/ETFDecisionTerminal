add_executable(DataAccessTests
    test_data_access.cpp
)

if(ETFDT_QT6_ROOT)
    set(_etfdt_qt6_runtime_dir "${ETFDT_QT6_ROOT}/bin")
else()
    set(_etfdt_qt6_runtime_dir "D:/Qt/6.6.3/msvc2019_64/bin")
endif()

add_executable(ReadOnlyRepositoryTests
    test_readonly_repositories.cpp
)

add_executable(TransactionAuditTests
    test_transaction_audit.cpp
)

target_link_libraries(DataAccessTests
    PRIVATE
        ETFDecisionTerminal::DataAccess
)

target_link_libraries(ReadOnlyRepositoryTests
    PRIVATE
        ETFDecisionTerminal::DataAccess
)

target_link_libraries(TransactionAuditTests
    PRIVATE
        ETFDecisionTerminal::DataAccess
)

target_compile_features(DataAccessTests PRIVATE cxx_std_17)
target_compile_features(ReadOnlyRepositoryTests PRIVATE cxx_std_17)
target_compile_features(TransactionAuditTests PRIVATE cxx_std_17)

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
    NAME dataaccess_transaction_audit
    COMMAND
        TransactionAuditTests
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
    dataservice_cli_init
    dataservice_cli_check
    dataservice_cli_summary
    dataservice_cli_list_accounts
    dataservice_cli_list_portfolios
    dataservice_cli_list_instruments
    dataservice_cli_list_strategies
    dataservice_cli_list_otc_missing
    PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:${_etfdt_qt6_runtime_dir}"
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
