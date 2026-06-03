find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_test(
    NAME shell_accounting_manual_entry_readback_mapping_authorization
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/manual_entry_readback_mapping_authorization_gate.py
        --source-root ${CMAKE_SOURCE_DIR}
)
