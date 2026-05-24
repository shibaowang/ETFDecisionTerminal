find_package(Qt6 COMPONENTS Core REQUIRED)

add_library(ETFWatchdogLib STATIC
    src/ServiceManifest.cpp
    src/ServiceManifestLoader.cpp
    src/ServiceManifestStatus.cpp
    src/ServiceProcessConfig.cpp
    src/ServiceProcessManager.cpp
    src/ServiceProcessStatus.cpp
    src/Watchdog.cpp
)

add_library(ETFDecisionTerminal::Watchdog ALIAS ETFWatchdogLib)

target_include_directories(ETFWatchdogLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFWatchdogLib
    PUBLIC
        ETFDecisionTerminal::CoreDomain
        ETFDecisionTerminal::Protocol
        ETFDecisionTerminal::DataServiceClient
        Qt6::Core
)

target_compile_features(ETFWatchdogLib PUBLIC cxx_std_17)

set_target_properties(ETFWatchdogLib PROPERTIES
    OUTPUT_NAME ETFWatchdogLib
)
