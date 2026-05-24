add_executable(ServiceRuntimeTests
    test_service_runtime.cpp
)

target_link_libraries(ServiceRuntimeTests
    PRIVATE
        ETFDecisionTerminal::ServiceRuntime
)

target_compile_features(ServiceRuntimeTests PRIVATE cxx_std_17)

add_test(
    NAME service_runtime_dispatcher
    COMMAND ServiceRuntimeTests
)
