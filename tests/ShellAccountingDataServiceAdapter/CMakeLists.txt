add_executable(ShellAccountingDataServiceAdapterSkeletonInterfaceTests
    test_shell_accounting_dataservice_adapter_skeleton_interface.cpp
)

target_link_libraries(ShellAccountingDataServiceAdapterSkeletonInterfaceTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingDataServiceAdapterSkeletonInterfaceTests PRIVATE cxx_std_17)

add_executable(ShellAccountingDataServiceAdapterSkeletonNotConnectedTests
    test_shell_accounting_dataservice_adapter_skeleton_not_connected.cpp
)

target_link_libraries(ShellAccountingDataServiceAdapterSkeletonNotConnectedTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingDataServiceAdapterSkeletonNotConnectedTests PRIVATE cxx_std_17)

add_executable(ShellAccountingDataServiceAdapterSkeletonNoLiveCallsTests
    test_shell_accounting_dataservice_adapter_skeleton_no_live_calls.cpp
)

target_compile_features(ShellAccountingDataServiceAdapterSkeletonNoLiveCallsTests PRIVATE cxx_std_17)

add_executable(ShellAccountingDataServiceAdapterSkeletonNoWriteNoTradeTests
    test_shell_accounting_dataservice_adapter_skeleton_no_write_no_trade.cpp
)

target_link_libraries(ShellAccountingDataServiceAdapterSkeletonNoWriteNoTradeTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingDataServiceAdapterSkeletonNoWriteNoTradeTests PRIVATE cxx_std_17)

add_executable(ShellAccountingDataServiceAdapterSkeletonControllerIntegrationTests
    test_shell_accounting_dataservice_adapter_skeleton_controller_integration.cpp
)

target_link_libraries(ShellAccountingDataServiceAdapterSkeletonControllerIntegrationTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingDataServiceAdapterSkeletonControllerIntegrationTests PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_dataservice_adapter_skeleton_interface
    COMMAND ShellAccountingDataServiceAdapterSkeletonInterfaceTests
)

add_test(
    NAME shell_accounting_dataservice_adapter_skeleton_not_connected
    COMMAND ShellAccountingDataServiceAdapterSkeletonNotConnectedTests
)

add_test(
    NAME shell_accounting_dataservice_adapter_skeleton_no_live_calls
    COMMAND
        ShellAccountingDataServiceAdapterSkeletonNoLiveCallsTests
        --source-root
        ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME shell_accounting_dataservice_adapter_skeleton_no_write_no_trade
    COMMAND ShellAccountingDataServiceAdapterSkeletonNoWriteNoTradeTests
)

add_test(
    NAME shell_accounting_dataservice_adapter_skeleton_controller_integration
    COMMAND ShellAccountingDataServiceAdapterSkeletonControllerIntegrationTests
)
