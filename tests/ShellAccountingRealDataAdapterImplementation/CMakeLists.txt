add_library(ShellAccountingRealDataAdapterImplementationLib STATIC
    ShellAccountingRealDataAdapterImplementation.cpp
    ShellAccountingRealDataAdapterImplementation.h
)

target_compile_features(ShellAccountingRealDataAdapterImplementationLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingRealDataAdapterImplementationLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ShellAccountingRealDataAdapterImplementationLib
    PUBLIC
        ETFDecisionTerminal::ShellServices
)

function(add_shell_accounting_real_data_adapter_implementation_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingRealDataAdapterImplementationLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterImplementationTests
    test_shell_accounting_real_data_adapter_implementation.cpp
    shell_accounting_real_data_adapter_implementation
)

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterAuthorizedReadOnlyActionsTests
    test_shell_accounting_real_data_adapter_authorized_readonly_actions.cpp
    shell_accounting_real_data_adapter_authorized_readonly_actions
)

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterSuccessMappingTests
    test_shell_accounting_real_data_adapter_success_mapping.cpp
    shell_accounting_real_data_adapter_success_mapping
)

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterUnavailableMappingTests
    test_shell_accounting_real_data_adapter_unavailable_mapping.cpp
    shell_accounting_real_data_adapter_unavailable_mapping
)

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterTransportErrorMappingTests
    test_shell_accounting_real_data_adapter_transport_error_mapping.cpp
    shell_accounting_real_data_adapter_transport_error_mapping
)

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterProtocolErrorMappingTests
    test_shell_accounting_real_data_adapter_protocol_error_mapping.cpp
    shell_accounting_real_data_adapter_protocol_error_mapping
)

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterTimeoutMappingTests
    test_shell_accounting_real_data_adapter_timeout_mapping.cpp
    shell_accounting_real_data_adapter_timeout_mapping
)

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterPrivacyNoRawPayloadTests
    test_shell_accounting_real_data_adapter_privacy_no_raw_payload.cpp
    shell_accounting_real_data_adapter_privacy_no_raw_payload
)

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterNoWritePathAfterIntegrationTests
    test_shell_accounting_real_data_adapter_no_write_path_after_integration.cpp
    shell_accounting_real_data_adapter_no_write_path_after_integration
)

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterNoSqliteDirectAfterIntegrationTests
    test_shell_accounting_real_data_adapter_no_sqlite_direct_after_integration.cpp
    shell_accounting_real_data_adapter_no_sqlite_direct_after_integration
)

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterNoAccountingEngineDirectAfterIntegrationTests
    test_shell_accounting_real_data_adapter_no_accounting_engine_direct_after_integration.cpp
    shell_accounting_real_data_adapter_no_accounting_engine_direct_after_integration
)

add_shell_accounting_real_data_adapter_implementation_test(
    ShellAccountingRealDataAdapterRollbackReadyAfterIntegrationTests
    test_shell_accounting_real_data_adapter_rollback_ready_after_integration.cpp
    shell_accounting_real_data_adapter_rollback_ready_after_integration
)
