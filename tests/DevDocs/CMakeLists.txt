add_test(
    NAME readonly_demo_acceptance_assets
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/test_readonly_demo_acceptance.py
)

set_tests_properties(readonly_demo_acceptance_assets PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
