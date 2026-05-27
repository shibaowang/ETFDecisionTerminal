add_library(AccountingNoWriteTestUtils STATIC
    AccountingNoWriteTestUtils.cpp
    ForbiddenSqlScanner.cpp
)

target_include_directories(AccountingNoWriteTestUtils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(AccountingNoWriteTestUtils
    PUBLIC
        ETFDecisionTerminal::DataAccess
)

target_compile_features(AccountingNoWriteTestUtils PUBLIC cxx_std_17)

add_executable(AccountingForbiddenSqlScannerTests
    test_forbidden_sql_scanner.cpp
)

target_link_libraries(AccountingForbiddenSqlScannerTests
    PRIVATE
        AccountingNoWriteTestUtils
)

target_compile_features(AccountingForbiddenSqlScannerTests PRIVATE cxx_std_17)

add_executable(AccountingNoWriteHarnessTests
    test_accounting_no_write_harness.cpp
)

target_link_libraries(AccountingNoWriteHarnessTests
    PRIVATE
        AccountingNoWriteTestUtils
)

target_compile_features(AccountingNoWriteHarnessTests PRIVATE cxx_std_17)

add_test(
    NAME accounting_forbidden_sql_scanner
    COMMAND AccountingForbiddenSqlScannerTests
)

add_test(
    NAME accounting_no_write_harness
    COMMAND
        AccountingNoWriteHarnessTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)
