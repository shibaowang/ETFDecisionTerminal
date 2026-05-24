add_library(ETFShellCore STATIC
    src/ShellCore.cpp
    src/ShellDiagnosticFacade.cpp
    src/ShellDiagnosticMockReportBuilder.cpp
    src/ShellDiagnosticPresenter.cpp
    src/ShellDiagnosticRows.cpp
    src/ShellDiagnosticViewModel.cpp
)

add_library(ETFDecisionTerminal::ShellCore ALIAS ETFShellCore)

target_include_directories(ETFShellCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFShellCore
    PUBLIC
        ETFDecisionTerminal::Diagnostics
)

target_compile_features(ETFShellCore PUBLIC cxx_std_17)
