find_package(Qt6 COMPONENTS Core REQUIRED)

add_library(ETFShellServices STATIC
    include/ShellServices/ShellAccountingDataServiceClientPort.h
    include/ShellServices/ShellAccountingDataServiceClientPortAdapter.h
    include/ShellServices/ShellAccountingDataServiceAdapter.h
    include/ShellServices/ShellAccountingDisplayText.h
    include/ShellServices/ShellAccountingIssue.h
    include/ShellServices/ShellAccountingReadOnlyController.h
    include/ShellServices/ShellAccountingServiceAdapter.h
    include/ShellServices/ShellAccountingServiceTypes.h
    include/ShellServices/ShellAccountingState.h
    include/ShellServices/ShellDataConnectionObject.h
    include/ShellServices/ShellDataTypes.h
    include/ShellServices/ShellReadOnlyConnectionPresets.h
    include/ShellServices/ShellReadOnlyDataController.h
    include/ShellServices/ShellReadOnlyDataFacade.h
    include/ShellServices/ShellReadOnlyDataViewModel.h
    include/ShellServices/ShellReadOnlyListModels.h
    include/ShellServices/ShellReadOnlyTableProxyModel.h
    include/ShellServices/ShellServices.h
    src/ShellAccountingDataServiceAdapter.cpp
    src/ShellAccountingDataServiceClientPortAdapter.cpp
    src/ShellAccountingDisplayText.cpp
    src/ShellAccountingIssue.cpp
    src/ShellAccountingReadOnlyController.cpp
    src/ShellAccountingServiceTypes.cpp
    src/ShellAccountingState.cpp
    src/ShellDataConnectionObject.cpp
    src/ShellReadOnlyConnectionPresets.cpp
    src/ShellReadOnlyDataController.cpp
    src/ShellReadOnlyDataFacade.cpp
    src/ShellReadOnlyListModels.cpp
    src/ShellReadOnlyTableProxyModel.cpp
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
