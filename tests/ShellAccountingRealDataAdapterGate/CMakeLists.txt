add_library(ShellAccountingRealDataAdapterGateLib STATIC
    ShellAccountingRealDataAdapterGate.cpp
    ShellAccountingRealDataAdapterGate.h
)

target_compile_features(ShellAccountingRealDataAdapterGateLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingRealDataAdapterGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_real_data_adapter_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingRealDataAdapterGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_real_data_adapter_gate_test(
    ShellAccountingRealDataAdapterGateTests
    test_shell_accounting_real_data_adapter_gate.cpp
    shell_accounting_real_data_adapter_gate
)

add_shell_accounting_real_data_adapter_gate_test(
    ShellAccountingRealDataAdapterNoRealActionYetTests
    test_shell_accounting_real_data_adapter_no_real_action_yet.cpp
    shell_accounting_real_data_adapter_no_real_action_yet
)

add_shell_accounting_real_data_adapter_gate_test(
    ShellAccountingRealDataAdapterReadOnlyActionPolicyTests
    test_shell_accounting_real_data_adapter_readonly_action_policy.cpp
    shell_accounting_real_data_adapter_readonly_action_policy
)

add_shell_accounting_real_data_adapter_gate_test(
    ShellAccountingRealDataAdapterNoWritePathTests
    test_shell_accounting_real_data_adapter_no_write_path.cpp
    shell_accounting_real_data_adapter_no_write_path
)

add_shell_accounting_real_data_adapter_gate_test(
    ShellAccountingRealDataAdapterNoSqliteDirectAccessTests
    test_shell_accounting_real_data_adapter_no_sqlite_direct_access.cpp
    shell_accounting_real_data_adapter_no_sqlite_direct_access
)

add_shell_accounting_real_data_adapter_gate_test(
    ShellAccountingRealDataAdapterNoAccountingEngineDirectAccessTests
    test_shell_accounting_real_data_adapter_no_accounting_engine_direct_access.cpp
    shell_accounting_real_data_adapter_no_accounting_engine_direct_access
)

add_shell_accounting_real_data_adapter_gate_test(
    ShellAccountingRealDataAdapterErrorMappingPolicyTests
    test_shell_accounting_real_data_adapter_error_mapping_policy.cpp
    shell_accounting_real_data_adapter_error_mapping_policy
)

add_shell_accounting_real_data_adapter_gate_test(
    ShellAccountingRealDataAdapterPrivacyPolicyTests
    test_shell_accounting_real_data_adapter_privacy_policy.cpp
    shell_accounting_real_data_adapter_privacy_policy
)

add_shell_accounting_real_data_adapter_gate_test(
    ShellAccountingRealDataAdapterRollbackPolicyTests
    test_shell_accounting_real_data_adapter_rollback_policy.cpp
    shell_accounting_real_data_adapter_rollback_policy
)
