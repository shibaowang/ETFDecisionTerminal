add_library(ETFShellCore STATIC
    include/ShellCore/ShellDiagnosticIssueListModel.h
    include/ShellCore/ShellDiagnosticQtAdapter.h
    include/ShellCore/ShellDiagnosticServiceListModel.h
    include/ShellCore/ShellDiagnosticSummaryObject.h
    include/ShellCore/ShellMockLogModel.h
    include/ShellCore/ShellNavigationController.h
    include/ShellCore/ShellNavigationModel.h
    include/ShellCore/ShellNavigationTypes.h
    include/ShellCore/ShellPageInfoObject.h
    include/ShellCore/ShellPageRegistry.h
    include/ShellCore/ShellPageStatusModel.h
    include/ShellCore/ShellPageStatusTypes.h
    include/ShellCore/ShellStatusController.h
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
    src/ShellMockLogModel.cpp
    src/ShellNavigationController.cpp
    src/ShellNavigationModel.cpp
    src/ShellPageInfoObject.cpp
    src/ShellPageRegistry.cpp
    src/ShellPageStatusModel.cpp
    src/ShellStatusController.cpp
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
