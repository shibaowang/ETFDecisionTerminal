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
