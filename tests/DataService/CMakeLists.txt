find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(DataServiceActionTests
    test_dataservice_readonly_actions.cpp
)

add_executable(DataServiceAuditWriteActionTests
    test_dataservice_audit_write_actions.cpp
)

target_link_libraries(DataServiceActionTests
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::Transport
)

target_link_libraries(DataServiceAuditWriteActionTests
    PRIVATE
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::Transport
)

target_compile_features(DataServiceActionTests PRIVATE cxx_std_17)
target_compile_features(DataServiceAuditWriteActionTests PRIVATE cxx_std_17)

add_test(
    NAME dataservice_readonly_socket_actions
    COMMAND
        DataServiceActionTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

add_test(
    NAME dataservice_position_list_guard
    COMMAND
        DataServiceActionTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

add_test(
    NAME dataservice_position_list_no_write
    COMMAND
        DataServiceActionTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

add_test(
    NAME dataservice_cash_summary_guard
    COMMAND
        DataServiceActionTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

add_test(
    NAME dataservice_cash_summary_no_write
    COMMAND
        DataServiceActionTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

set_tests_properties(dataservice_readonly_socket_actions PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)

set_tests_properties(dataservice_position_list_guard PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)

set_tests_properties(dataservice_position_list_no_write PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)

set_tests_properties(dataservice_cash_summary_guard PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)

set_tests_properties(dataservice_cash_summary_no_write PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)

add_test(
    NAME dataservice_audit_write_socket_actions
    COMMAND
        DataServiceAuditWriteActionTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

set_tests_properties(dataservice_audit_write_socket_actions PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
