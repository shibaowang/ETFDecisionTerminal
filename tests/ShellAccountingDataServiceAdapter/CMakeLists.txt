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

add_library(ShellAccountingDataServiceAdapterSpyPortTests STATIC
    SpyShellAccountingDataServiceClientPort.cpp
    SpyShellAccountingDataServiceClientPort.h
)

target_link_libraries(ShellAccountingDataServiceAdapterSpyPortTests
    PUBLIC
        ETFDecisionTerminal::ShellServices
)

target_include_directories(ShellAccountingDataServiceAdapterSpyPortTests
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ShellAccountingDataServiceAdapterSpyPortTests PUBLIC cxx_std_17)

add_executable(ShellAccountingDataServiceAdapterLiveCallSkeletonDefaultNotConnectedTests
    test_shell_accounting_dataservice_adapter_live_call_skeleton_default_not_connected.cpp
)

target_link_libraries(ShellAccountingDataServiceAdapterLiveCallSkeletonDefaultNotConnectedTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
)

target_compile_features(
    ShellAccountingDataServiceAdapterLiveCallSkeletonDefaultNotConnectedTests
    PRIVATE
        cxx_std_17
)

add_executable(ShellAccountingDataServiceAdapterLiveCallSkeletonMethodMappingTests
    test_shell_accounting_dataservice_adapter_live_call_skeleton_method_mapping.cpp
)

target_link_libraries(ShellAccountingDataServiceAdapterLiveCallSkeletonMethodMappingTests
    PRIVATE
        ShellAccountingDataServiceAdapterSpyPortTests
)

target_compile_features(ShellAccountingDataServiceAdapterLiveCallSkeletonMethodMappingTests PRIVATE cxx_std_17)

add_executable(ShellAccountingDataServiceAdapterLiveCallSkeletonRequestMappingTests
    test_shell_accounting_dataservice_adapter_live_call_skeleton_request_mapping.cpp
)

target_link_libraries(ShellAccountingDataServiceAdapterLiveCallSkeletonRequestMappingTests
    PRIVATE
        ShellAccountingDataServiceAdapterSpyPortTests
)

target_compile_features(ShellAccountingDataServiceAdapterLiveCallSkeletonRequestMappingTests PRIVATE cxx_std_17)

add_executable(ShellAccountingDataServiceAdapterLiveCallSkeletonResponseMappingTests
    test_shell_accounting_dataservice_adapter_live_call_skeleton_response_mapping.cpp
)

target_link_libraries(ShellAccountingDataServiceAdapterLiveCallSkeletonResponseMappingTests
    PRIVATE
        ShellAccountingDataServiceAdapterSpyPortTests
)

target_compile_features(ShellAccountingDataServiceAdapterLiveCallSkeletonResponseMappingTests PRIVATE cxx_std_17)

add_executable(ShellAccountingDataServiceAdapterLiveCallSkeletonErrorMappingTests
    test_shell_accounting_dataservice_adapter_live_call_skeleton_error_mapping.cpp
)

target_link_libraries(ShellAccountingDataServiceAdapterLiveCallSkeletonErrorMappingTests
    PRIVATE
        ShellAccountingDataServiceAdapterSpyPortTests
)

target_compile_features(ShellAccountingDataServiceAdapterLiveCallSkeletonErrorMappingTests PRIVATE cxx_std_17)

add_executable(ShellAccountingDataServiceAdapterLiveCallSkeletonNoWriteNoTradeTests
    test_shell_accounting_dataservice_adapter_live_call_skeleton_no_write_no_trade.cpp
)

target_link_libraries(ShellAccountingDataServiceAdapterLiveCallSkeletonNoWriteNoTradeTests
    PRIVATE
        ShellAccountingDataServiceAdapterSpyPortTests
)

target_compile_features(ShellAccountingDataServiceAdapterLiveCallSkeletonNoWriteNoTradeTests PRIVATE cxx_std_17)

add_executable(ShellAccountingDataServiceAdapterLiveCallSkeletonNoRealDependencyTests
    test_shell_accounting_dataservice_adapter_live_call_skeleton_no_real_dependency.cpp
)

target_compile_features(ShellAccountingDataServiceAdapterLiveCallSkeletonNoRealDependencyTests PRIVATE cxx_std_17)

add_executable(ShellAccountingControllerWithDataServiceAdapterSpyPortTests
    test_shell_accounting_controller_with_dataservice_adapter_spy_port.cpp
)

target_link_libraries(ShellAccountingControllerWithDataServiceAdapterSpyPortTests
    PRIVATE
        ShellAccountingDataServiceAdapterSpyPortTests
)

target_compile_features(ShellAccountingControllerWithDataServiceAdapterSpyPortTests PRIVATE cxx_std_17)

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

add_test(
    NAME shell_accounting_dataservice_adapter_live_call_skeleton_default_not_connected
    COMMAND ShellAccountingDataServiceAdapterLiveCallSkeletonDefaultNotConnectedTests
)

add_test(
    NAME shell_accounting_dataservice_adapter_live_call_skeleton_method_mapping
    COMMAND ShellAccountingDataServiceAdapterLiveCallSkeletonMethodMappingTests
)

add_test(
    NAME shell_accounting_dataservice_adapter_live_call_skeleton_request_mapping
    COMMAND ShellAccountingDataServiceAdapterLiveCallSkeletonRequestMappingTests
)

add_test(
    NAME shell_accounting_dataservice_adapter_live_call_skeleton_response_mapping
    COMMAND ShellAccountingDataServiceAdapterLiveCallSkeletonResponseMappingTests
)

add_test(
    NAME shell_accounting_dataservice_adapter_live_call_skeleton_error_mapping
    COMMAND ShellAccountingDataServiceAdapterLiveCallSkeletonErrorMappingTests
)

add_test(
    NAME shell_accounting_dataservice_adapter_live_call_skeleton_no_write_no_trade
    COMMAND ShellAccountingDataServiceAdapterLiveCallSkeletonNoWriteNoTradeTests
)

add_test(
    NAME shell_accounting_dataservice_adapter_live_call_skeleton_no_real_dependency
    COMMAND
        ShellAccountingDataServiceAdapterLiveCallSkeletonNoRealDependencyTests
        --source-root
        ${PROJECT_SOURCE_DIR}
)

add_test(
    NAME shell_accounting_controller_with_dataservice_adapter_spy_port
    COMMAND ShellAccountingControllerWithDataServiceAdapterSpyPortTests
)
