add_library(ETFShellCore STATIC
    include/ShellCore/ShellDiagnosticIssueListModel.h
    include/ShellCore/ShellDiagnosticQtAdapter.h
    include/ShellCore/ShellDiagnosticServiceListModel.h
    include/ShellCore/ShellDiagnosticSummaryObject.h
    src/ShellCore.cpp
    src/ShellDiagnosticFacade.cpp
    src/ShellDiagnosticIssueListModel.cpp
    src/ShellDiagnosticMockReportBuilder.cpp
    src/ShellDiagnosticPresenter.cpp
    src/ShellDiagnosticQtAdapter.cpp
    src/ShellDiagnosticRows.cpp
    src/ShellDiagnosticServiceListModel.cpp
    src/ShellDiagnosticSummaryObject.cpp
    src/ShellDiagnosticViewModel.cpp
)

add_library(ETFDecisionTerminal::ShellCore ALIAS ETFShellCore)

find_package(Qt6 COMPONENTS Core REQUIRED)

set_target_properties(ETFShellCore PROPERTIES
    AUTOMOC ON
)

target_include_directories(ETFShellCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFShellCore
    PUBLIC
        ETFDecisionTerminal::Diagnostics
        Qt6::Core
)

target_compile_features(ETFShellCore PUBLIC cxx_std_17)
