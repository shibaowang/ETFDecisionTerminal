add_library(ShellAccountingDataServiceAdapterScaffolding STATIC
    SpyAccountingDataServiceClient.cpp
    SpyAccountingDataServiceClient.h
    ShellAccountingDataServiceAdapterExpectedCall.cpp
    ShellAccountingDataServiceAdapterExpectedCall.h
)

target_include_directories(ShellAccountingDataServiceAdapterScaffolding
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ShellAccountingDataServiceAdapterScaffolding
    PUBLIC
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingDataServiceAdapterScaffolding PUBLIC cxx_std_17)

set(SHELL_ACCOUNTING_DATASERVICE_ADAPTER_SPY_TESTS
    allowlist
    method_mapping
    request_mapping
    response_mapping
    error_mapping
    no_write_no_trade
)

# Registered CTest names:
# shell_accounting_dataservice_adapter_spy_allowlist
# shell_accounting_dataservice_adapter_spy_method_mapping
# shell_accounting_dataservice_adapter_spy_request_mapping
# shell_accounting_dataservice_adapter_spy_response_mapping
# shell_accounting_dataservice_adapter_spy_error_mapping
# shell_accounting_dataservice_adapter_spy_no_write_no_trade
# shell_accounting_dataservice_adapter_spy_no_real_dependency

foreach(test_name IN LISTS SHELL_ACCOUNTING_DATASERVICE_ADAPTER_SPY_TESTS)
    string(REPLACE "_" "" target_suffix "${test_name}")
    add_executable(ShellAccountingDataServiceAdapterSpy${target_suffix}Tests
        test_shell_accounting_dataservice_adapter_spy_${test_name}.cpp
    )
    target_link_libraries(ShellAccountingDataServiceAdapterSpy${target_suffix}Tests
        PRIVATE
            ShellAccountingDataServiceAdapterScaffolding
    )
    target_compile_features(ShellAccountingDataServiceAdapterSpy${target_suffix}Tests PRIVATE cxx_std_17)
    add_test(
        NAME shell_accounting_dataservice_adapter_spy_${test_name}
        COMMAND ShellAccountingDataServiceAdapterSpy${target_suffix}Tests
    )
endforeach()

add_executable(ShellAccountingDataServiceAdapterSpyNoRealDependencyTests
    test_shell_accounting_dataservice_adapter_spy_no_real_dependency.cpp
)

target_compile_features(ShellAccountingDataServiceAdapterSpyNoRealDependencyTests PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_dataservice_adapter_spy_no_real_dependency
    COMMAND
        ShellAccountingDataServiceAdapterSpyNoRealDependencyTests
        --source-root
        ${PROJECT_SOURCE_DIR}
)
