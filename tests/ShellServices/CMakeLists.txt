find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(ShellReadOnlyDataControllerTests
    test_shell_readonly_data_controller.cpp
)

target_link_libraries(ShellReadOnlyDataControllerTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::ServiceRuntime
        Qt6::Core
        Qt6::Network
)

target_compile_features(ShellReadOnlyDataControllerTests PRIVATE cxx_std_17)

add_test(
    NAME shell_readonly_data_controller
    COMMAND
        ShellReadOnlyDataControllerTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

set_tests_properties(shell_readonly_data_controller PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
