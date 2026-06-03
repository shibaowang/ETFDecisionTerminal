find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_test(
    NAME shell_accounting_manual_entry_post_write_readback_refresh_implementation
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/manual_entry_post_write_readback_refresh_implementation.py
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(shell_accounting_manual_entry_post_write_readback_refresh_implementation PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
