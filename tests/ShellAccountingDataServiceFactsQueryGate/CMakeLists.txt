add_library(ShellAccountingDataServiceFactsQueryGateLib STATIC
    ShellAccountingDataServiceFactsQueryGate.cpp
    ShellAccountingDataServiceFactsQueryGate.h
)

target_compile_features(ShellAccountingDataServiceFactsQueryGateLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingDataServiceFactsQueryGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_dataservice_facts_query_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingDataServiceFactsQueryGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_dataservice_facts_query_gate_test(
    ShellAccountingDataServiceFactsQueryGateTests
    test_shell_accounting_dataservice_facts_query_gate.cpp
    shell_accounting_dataservice_facts_query_gate
)

add_shell_accounting_dataservice_facts_query_gate_test(
    ShellAccountingDataServiceFactsQueryNoQueryYetTests
    test_shell_accounting_dataservice_facts_query_no_query_yet.cpp
    shell_accounting_dataservice_facts_query_no_query_yet
)

add_shell_accounting_dataservice_facts_query_gate_test(
    ShellAccountingDataServiceFactsQueryAllowedSourcesPolicyTests
    test_shell_accounting_dataservice_facts_query_allowed_sources_policy.cpp
    shell_accounting_dataservice_facts_query_allowed_sources_policy
)

add_shell_accounting_dataservice_facts_query_gate_test(
    ShellAccountingDataServiceFactsQueryNoWriteSqlTests
    test_shell_accounting_dataservice_facts_query_no_write_sql.cpp
    shell_accounting_dataservice_facts_query_no_write_sql
)

add_shell_accounting_dataservice_facts_query_gate_test(
    ShellAccountingDataServiceFactsQueryNoSnapshotRebuildTests
    test_shell_accounting_dataservice_facts_query_no_snapshot_rebuild.cpp
    shell_accounting_dataservice_facts_query_no_snapshot_rebuild
)

add_shell_accounting_dataservice_facts_query_gate_test(
    ShellAccountingDataServiceFactsQueryNoAccountingEngineReplayTests
    test_shell_accounting_dataservice_facts_query_no_accounting_engine_replay.cpp
    shell_accounting_dataservice_facts_query_no_accounting_engine_replay
)

add_shell_accounting_dataservice_facts_query_gate_test(
    ShellAccountingDataServiceFactsQueryErrorMappingPolicyTests
    test_shell_accounting_dataservice_facts_query_error_mapping_policy.cpp
    shell_accounting_dataservice_facts_query_error_mapping_policy
)

add_shell_accounting_dataservice_facts_query_gate_test(
    ShellAccountingDataServiceFactsQueryPrivacyPolicyTests
    test_shell_accounting_dataservice_facts_query_privacy_policy.cpp
    shell_accounting_dataservice_facts_query_privacy_policy
)

add_shell_accounting_dataservice_facts_query_gate_test(
    ShellAccountingDataServiceFactsQueryRollbackPolicyTests
    test_shell_accounting_dataservice_facts_query_rollback_policy.cpp
    shell_accounting_dataservice_facts_query_rollback_policy
)
