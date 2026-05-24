find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(WatchdogTests
    test_watchdog_service_manager.cpp
)

add_executable(WatchdogManifestTests
    test_watchdog_manifest.cpp
)

target_link_libraries(WatchdogTests
    PRIVATE
        ETFDecisionTerminal::Watchdog
        ETFDecisionTerminal::DataAccess
)

target_link_libraries(WatchdogManifestTests
    PRIVATE
        ETFDecisionTerminal::Watchdog
)

target_compile_features(WatchdogTests PRIVATE cxx_std_17)
target_compile_features(WatchdogManifestTests PRIVATE cxx_std_17)

add_test(
    NAME watchdog_service_manager
    COMMAND
        WatchdogTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
        --dataservice-exe
        $<TARGET_FILE:ETFDataService>
)

set_tests_properties(watchdog_service_manager PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)

add_test(
    NAME watchdog_service_manifest
    COMMAND
        WatchdogManifestTests
        --watchdog-exe
        $<TARGET_FILE:ETFWatchdog>
)

set_tests_properties(watchdog_service_manifest PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
