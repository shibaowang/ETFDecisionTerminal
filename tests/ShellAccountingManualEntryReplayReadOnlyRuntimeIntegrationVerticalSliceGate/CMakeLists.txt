find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_executable(T252ReadOnlyRuntimeSlice
    manual_entry_replay_readonly_runtime_integration_vertical_slice.cpp
)

target_link_libraries(T252ReadOnlyRuntimeSlice
    PRIVATE
        ETFDecisionTerminal::AccountingEngine
)

target_compile_features(T252ReadOnlyRuntimeSlice PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.py
        --source-root
        ${PROJECT_SOURCE_DIR}
        --runtime-executable
        $<TARGET_FILE:T252ReadOnlyRuntimeSlice>
)

set_tests_properties(shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice PROPERTIES
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
