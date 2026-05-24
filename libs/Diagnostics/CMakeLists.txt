find_package(Qt6 COMPONENTS Core REQUIRED)

add_library(ETFDiagnostics STATIC
    src/Diagnostics.cpp
    src/DiagnosticIssue.cpp
    src/DiagnosticReport.cpp
    src/DiagnosticSummary.cpp
    src/WatchdogReportParser.cpp
)

add_library(ETFDecisionTerminal::Diagnostics ALIAS ETFDiagnostics)

target_include_directories(ETFDiagnostics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFDiagnostics
    PUBLIC
        Qt6::Core
)

target_compile_features(ETFDiagnostics PUBLIC cxx_std_17)
