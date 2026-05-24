find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(ShellCoreTests
    test_shell_diagnostics.cpp
)

target_link_libraries(ShellCoreTests
    PRIVATE
        ETFDecisionTerminal::ShellCore
        Qt6::Core
)

target_compile_features(ShellCoreTests PRIVATE cxx_std_17)

add_test(
    NAME shell_diagnostics_facade
    COMMAND ShellCoreTests
)

set_tests_properties(shell_diagnostics_facade PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
