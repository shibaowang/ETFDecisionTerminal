find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_subdirectory(CoreDomain)
add_subdirectory(DataAccess)
add_subdirectory(DataService)
add_subdirectory(Protocol)
add_subdirectory(ServiceRuntime)
add_subdirectory(ServiceHost)
add_subdirectory(Transport)
add_subdirectory(DataServiceClient)

add_test(
    NAME database_initial_schema
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/database/validate_schema.py
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

set_tests_properties(database_initial_schema PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
