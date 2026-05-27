find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(DataServiceClientTests
    test_dataservice_client.cpp
)

target_link_libraries(DataServiceClientTests
    PRIVATE
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::DataAccess
)

target_compile_features(DataServiceClientTests PRIVATE cxx_std_17)

add_test(
    NAME dataservice_client_end_to_end
    COMMAND
        DataServiceClientTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

add_test(
    NAME dataservice_client_position_list_guard
    COMMAND
        DataServiceClientTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

add_test(
    NAME dataservice_client_cash_summary_guard
    COMMAND
        DataServiceClientTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

set_tests_properties(dataservice_client_end_to_end PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)

set_tests_properties(dataservice_client_position_list_guard PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)

set_tests_properties(dataservice_client_cash_summary_guard PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
