find_package(Qt6 COMPONENTS Core REQUIRED)

add_library(ETFShellServices STATIC
    include/ShellServices/ShellDataConnectionObject.h
    include/ShellServices/ShellDataTypes.h
    include/ShellServices/ShellReadOnlyDataController.h
    include/ShellServices/ShellReadOnlyDataFacade.h
    include/ShellServices/ShellReadOnlyDataViewModel.h
    include/ShellServices/ShellReadOnlyListModels.h
    include/ShellServices/ShellServices.h
    src/ShellDataConnectionObject.cpp
    src/ShellReadOnlyDataController.cpp
    src/ShellReadOnlyDataFacade.cpp
    src/ShellReadOnlyListModels.cpp
)

add_library(ETFDecisionTerminal::ShellServices ALIAS ETFShellServices)

target_include_directories(ETFShellServices
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFShellServices
    PUBLIC
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::CoreDomain
        ETFDecisionTerminal::Protocol
        Qt6::Core
)

target_compile_features(ETFShellServices PUBLIC cxx_std_17)

set_target_properties(ETFShellServices PROPERTIES
    AUTOMOC ON
)
