find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(ServiceHostTests
    test_service_host.cpp
)

target_link_libraries(ServiceHostTests
    PRIVATE
        ETFDecisionTerminal::ServiceHost
)

target_compile_features(ServiceHostTests PRIVATE cxx_std_17)

add_test(
    NAME service_host_demo_end_to_end
    COMMAND ServiceHostTests
)

set_tests_properties(service_host_demo_end_to_end PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
