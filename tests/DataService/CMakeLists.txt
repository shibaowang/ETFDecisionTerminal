find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(DataServiceActionTests
    test_dataservice_readonly_actions.cpp
)

target_link_libraries(DataServiceActionTests
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::Transport
)

target_compile_features(DataServiceActionTests PRIVATE cxx_std_17)

add_test(
    NAME dataservice_readonly_socket_actions
    COMMAND
        DataServiceActionTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

set_tests_properties(dataservice_readonly_socket_actions PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
