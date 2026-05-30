find_package(Qt6 COMPONENTS Core REQUIRED)

add_library(ShellAccountingDataServiceFactsQueryImplementationLib STATIC
    ShellAccountingDataServiceFactsQueryImplementation.cpp
    ShellAccountingDataServiceFactsQueryImplementation.h
)

target_compile_features(ShellAccountingDataServiceFactsQueryImplementationLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingDataServiceFactsQueryImplementationLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ShellAccountingDataServiceFactsQueryImplementationLib
    PUBLIC
        ETFDecisionTerminal::DataServiceApi
        Qt6::Core
)

function(add_shell_accounting_dataservice_facts_query_implementation_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingDataServiceFactsQueryImplementationLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(
        NAME ${ctest_name}
        COMMAND
            ${target_name}
            --source-root ${PROJECT_SOURCE_DIR}
            --migration ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
    )
    set_tests_properties(${ctest_name} PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
    )
endfunction()

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsImplTests
    test_shell_accounting_dataservice_facts_query_implementation.cpp
    shell_accounting_dataservice_facts_query_implementation
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsPositionSuccessTests
    test_shell_accounting_dataservice_facts_query_position_list_success.cpp
    shell_accounting_dataservice_facts_query_position_list_success
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsCashSuccessTests
    test_shell_accounting_dataservice_facts_query_cash_summary_success.cpp
    shell_accounting_dataservice_facts_query_cash_summary_success
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsPortfolioSuccessTests
    test_shell_accounting_dataservice_facts_query_portfolio_pnl_summary_success.cpp
    shell_accounting_dataservice_facts_query_portfolio_pnl_summary_success
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsBaseSuccessTests
    test_shell_accounting_dataservice_facts_query_base_position_summary_success.cpp
    shell_accounting_dataservice_facts_query_base_position_summary_success
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsSniperSuccessTests
    test_shell_accounting_dataservice_facts_query_sniper_pool_summary_success.cpp
    shell_accounting_dataservice_facts_query_sniper_pool_summary_success
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsEmptyMappingTests
    test_shell_accounting_dataservice_facts_query_empty_data_mapping.cpp
    shell_accounting_dataservice_facts_query_empty_data_mapping
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsMissingMappingTests
    test_shell_accounting_dataservice_facts_query_missing_data_mapping.cpp
    shell_accounting_dataservice_facts_query_missing_data_mapping
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsQueryErrorTests
    test_shell_accounting_dataservice_facts_query_query_error_mapping.cpp
    shell_accounting_dataservice_facts_query_query_error_mapping
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsNoWriteSqlTests
    test_shell_accounting_dataservice_facts_query_no_write_sql_after_implementation.cpp
    shell_accounting_dataservice_facts_query_no_write_sql_after_implementation
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsNoSnapshotRebuildTests
    test_shell_accounting_dataservice_facts_query_no_snapshot_rebuild_after_implementation.cpp
    shell_accounting_dataservice_facts_query_no_snapshot_rebuild_after_implementation
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsNoReplayTests
    test_shell_accounting_dataservice_facts_query_no_accounting_engine_replay_after_implementation.cpp
    shell_accounting_dataservice_facts_query_no_accounting_engine_replay_after_implementation
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsPrivacyTests
    test_shell_accounting_dataservice_facts_query_privacy_after_implementation.cpp
    shell_accounting_dataservice_facts_query_privacy_after_implementation
)

add_shell_accounting_dataservice_facts_query_implementation_test(
    SAcctFactsRollbackTests
    test_shell_accounting_dataservice_facts_query_rollback_ready_after_implementation.cpp
    shell_accounting_dataservice_facts_query_rollback_ready_after_implementation
)
