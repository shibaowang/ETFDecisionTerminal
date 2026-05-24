find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(ETFDiagTests
    test_etfdiag.cpp
)

target_link_libraries(ETFDiagTests
    PRIVATE
        Qt6::Core
)

target_compile_features(ETFDiagTests PRIVATE cxx_std_17)

add_test(
    NAME etfdiag_watchdog_report
    COMMAND
        ETFDiagTests
        --diag-exe
        $<TARGET_FILE:ETFDiag>
)

set_tests_properties(etfdiag_watchdog_report PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)
