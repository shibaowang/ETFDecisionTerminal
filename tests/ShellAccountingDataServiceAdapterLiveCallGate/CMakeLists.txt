add_library(ShellAccountingDataServiceAdapterLiveCallGate STATIC
    ShellAccountingDataServiceAdapterLiveCallGate.cpp
    ShellAccountingDataServiceAdapterLiveCallGate.h
)

target_include_directories(ShellAccountingDataServiceAdapterLiveCallGate
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ShellAccountingDataServiceAdapterLiveCallGate PUBLIC cxx_std_17)

set(SHELL_ACCOUNTING_DATASERVICE_ADAPTER_LIVE_CALL_GATE_TESTS
    allowlist_gate
    mapping_gate
    error_gate
    no_write_gate
    readiness_gate
)

# Registered CTest names:
# shell_accounting_dataservice_adapter_live_call_allowlist_gate
# shell_accounting_dataservice_adapter_live_call_mapping_gate
# shell_accounting_dataservice_adapter_live_call_error_gate
# shell_accounting_dataservice_adapter_live_call_no_write_gate
# shell_accounting_dataservice_adapter_live_call_readiness_gate

foreach(test_name IN LISTS SHELL_ACCOUNTING_DATASERVICE_ADAPTER_LIVE_CALL_GATE_TESTS)
    string(REPLACE "_" "" target_suffix "${test_name}")
    add_executable(ShellAccountingDataServiceAdapterLiveCall${target_suffix}Tests
        test_shell_accounting_dataservice_adapter_live_call_${test_name}.cpp
    )
    target_link_libraries(ShellAccountingDataServiceAdapterLiveCall${target_suffix}Tests
        PRIVATE
            ShellAccountingDataServiceAdapterLiveCallGate
    )
    target_compile_features(ShellAccountingDataServiceAdapterLiveCall${target_suffix}Tests PRIVATE cxx_std_17)

    if(test_name STREQUAL "no_write_gate")
        add_test(
            NAME shell_accounting_dataservice_adapter_live_call_${test_name}
            COMMAND
                ShellAccountingDataServiceAdapterLiveCall${target_suffix}Tests
                --source-root
                ${PROJECT_SOURCE_DIR}
        )
    else()
        add_test(
            NAME shell_accounting_dataservice_adapter_live_call_${test_name}
            COMMAND ShellAccountingDataServiceAdapterLiveCall${target_suffix}Tests
        )
    endif()
endforeach()
