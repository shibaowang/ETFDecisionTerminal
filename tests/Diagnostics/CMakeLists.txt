find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(DiagnosticsTests
    test_diagnostics.cpp
)

target_link_libraries(DiagnosticsTests
    PRIVATE
        ETFDecisionTerminal::Diagnostics
        Qt6::Core
)

target_compile_features(DiagnosticsTests PRIVATE cxx_std_17)

add_test(
    NAME diagnostics_report_model
    COMMAND DiagnosticsTests
)

set_tests_properties(diagnostics_report_model PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
