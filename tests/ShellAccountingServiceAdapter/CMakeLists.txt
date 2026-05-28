add_library(ShellAccountingServiceAdapterTestUtils STATIC
    FakeShellAccountingServiceAdapter.cpp
    FakeShellAccountingServiceAdapter.h
)

target_include_directories(ShellAccountingServiceAdapterTestUtils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ShellAccountingServiceAdapterTestUtils
    PUBLIC
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingServiceAdapterTestUtils PUBLIC cxx_std_17)

add_executable(ShellAccountingServiceAdapterInterfaceTests
    test_shell_accounting_service_adapter_interface.cpp
)

target_link_libraries(ShellAccountingServiceAdapterInterfaceTests
    PRIVATE
        ShellAccountingServiceAdapterTestUtils
)

target_compile_features(ShellAccountingServiceAdapterInterfaceTests PRIVATE cxx_std_17)

add_executable(ShellAccountingServiceResultContractTests
    test_shell_accounting_service_result_contract.cpp
)

target_link_libraries(ShellAccountingServiceResultContractTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingServiceResultContractTests PRIVATE cxx_std_17)

add_executable(ShellAccountingServiceAdapterNoRealDependencyTests
    test_shell_accounting_service_adapter_no_real_dependency.cpp
)

target_compile_features(ShellAccountingServiceAdapterNoRealDependencyTests PRIVATE cxx_std_17)

add_executable(ShellAccountingControllerAdapterInjectionTests
    test_shell_accounting_controller_adapter_injection.cpp
)

target_link_libraries(ShellAccountingControllerAdapterInjectionTests
    PRIVATE
        ShellAccountingServiceAdapterTestUtils
)

target_compile_features(ShellAccountingControllerAdapterInjectionTests PRIVATE cxx_std_17)

add_executable(ShellAccountingControllerFakeAdapterRefreshSuccessTests
    test_shell_accounting_controller_fake_adapter_refresh_success.cpp
)

target_link_libraries(ShellAccountingControllerFakeAdapterRefreshSuccessTests
    PRIVATE
        ShellAccountingServiceAdapterTestUtils
)

target_compile_features(ShellAccountingControllerFakeAdapterRefreshSuccessTests PRIVATE cxx_std_17)

add_executable(ShellAccountingControllerFakeAdapterUnavailableTests
    test_shell_accounting_controller_fake_adapter_unavailable.cpp
)

target_link_libraries(ShellAccountingControllerFakeAdapterUnavailableTests
    PRIVATE
        ShellAccountingServiceAdapterTestUtils
)

target_compile_features(ShellAccountingControllerFakeAdapterUnavailableTests PRIVATE cxx_std_17)

add_executable(ShellAccountingControllerFakeAdapterProtocolErrorTests
    test_shell_accounting_controller_fake_adapter_protocol_error.cpp
)

target_link_libraries(ShellAccountingControllerFakeAdapterProtocolErrorTests
    PRIVATE
        ShellAccountingServiceAdapterTestUtils
)

target_compile_features(ShellAccountingControllerFakeAdapterProtocolErrorTests PRIVATE cxx_std_17)

add_executable(ShellAccountingControllerFakeAdapterIssueMappingTests
    test_shell_accounting_controller_fake_adapter_issue_mapping.cpp
)

target_link_libraries(ShellAccountingControllerFakeAdapterIssueMappingTests
    PRIVATE
        ShellAccountingServiceAdapterTestUtils
)

target_compile_features(ShellAccountingControllerFakeAdapterIssueMappingTests PRIVATE cxx_std_17)

add_executable(ShellAccountingControllerFakeAdapterPrivacyNoTradeTests
    test_shell_accounting_controller_fake_adapter_privacy_no_trade.cpp
)

target_link_libraries(ShellAccountingControllerFakeAdapterPrivacyNoTradeTests
    PRIVATE
        ShellAccountingServiceAdapterTestUtils
)

target_compile_features(ShellAccountingControllerFakeAdapterPrivacyNoTradeTests PRIVATE cxx_std_17)

add_executable(ShellAccountingControllerFakeAdapterNoRealDependencyTests
    test_shell_accounting_controller_fake_adapter_no_real_dependency.cpp
)

target_compile_features(ShellAccountingControllerFakeAdapterNoRealDependencyTests PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_service_adapter_interface
    COMMAND ShellAccountingServiceAdapterInterfaceTests
)

add_test(
    NAME shell_accounting_service_result_contract
    COMMAND ShellAccountingServiceResultContractTests
)

add_test(
    NAME shell_accounting_service_adapter_no_real_dependency
    COMMAND
        ShellAccountingServiceAdapterNoRealDependencyTests
        --source-root
        ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME shell_accounting_controller_adapter_injection
    COMMAND ShellAccountingControllerAdapterInjectionTests
)

add_test(
    NAME shell_accounting_controller_fake_adapter_refresh_success
    COMMAND ShellAccountingControllerFakeAdapterRefreshSuccessTests
)

add_test(
    NAME shell_accounting_controller_fake_adapter_unavailable
    COMMAND ShellAccountingControllerFakeAdapterUnavailableTests
)

add_test(
    NAME shell_accounting_controller_fake_adapter_protocol_error
    COMMAND ShellAccountingControllerFakeAdapterProtocolErrorTests
)

add_test(
    NAME shell_accounting_controller_fake_adapter_issue_mapping
    COMMAND ShellAccountingControllerFakeAdapterIssueMappingTests
)

add_test(
    NAME shell_accounting_controller_fake_adapter_privacy_no_trade
    COMMAND ShellAccountingControllerFakeAdapterPrivacyNoTradeTests
)

add_test(
    NAME shell_accounting_controller_fake_adapter_no_real_dependency
    COMMAND
        ShellAccountingControllerFakeAdapterNoRealDependencyTests
        --source-root
        ${PROJECT_SOURCE_DIR}
)
