find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(ShellCoreTests
    test_shell_diagnostics.cpp
)

add_executable(ShellReadOnlyDataFacadeTests
    test_shell_readonly_data_facade.cpp
)

target_link_libraries(ShellCoreTests
    PRIVATE
        ETFDecisionTerminal::ShellCore
        Qt6::Core
)

target_link_libraries(ShellReadOnlyDataFacadeTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::ServiceRuntime
        Qt6::Core
        Qt6::Network
)

target_compile_features(ShellCoreTests PRIVATE cxx_std_17)
target_compile_features(ShellReadOnlyDataFacadeTests PRIVATE cxx_std_17)

add_test(
    NAME shell_diagnostics_facade
    COMMAND ShellCoreTests
)

set_tests_properties(shell_diagnostics_facade PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)

add_test(
    NAME shell_readonly_data_facade
    COMMAND
        ShellReadOnlyDataFacadeTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

set_tests_properties(shell_readonly_data_facade PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
