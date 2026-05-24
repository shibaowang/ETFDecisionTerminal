cmake_minimum_required(VERSION 3.20)

project(
    ETFDecisionTerminal
    VERSION 0.1.0
    DESCRIPTION "Cross-border ETF intelligent investment decision terminal"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ETFDT_BUILD_TESTS "Build ETFDecisionTerminal tests" ON)

add_subdirectory(libs/CoreDomain)
add_subdirectory(libs/Protocol)
add_subdirectory(libs/DataAccess)
add_subdirectory(libs/StrategyEngine)
add_subdirectory(libs/MarketEngine)
add_subdirectory(libs/Quantization)
add_subdirectory(libs/Security)

add_subdirectory(apps/ETFDecisionShell)
add_subdirectory(apps/ETFWatchdog)
add_subdirectory(apps/ETFDataService)
add_subdirectory(apps/ETFMarketService)
add_subdirectory(apps/ETFStrategyService)
add_subdirectory(apps/ETFAlertService)

include(CTest)
if(BUILD_TESTING AND ETFDT_BUILD_TESTS)
    set_property(
        DIRECTORY
        APPEND
        PROPERTY TEST_INCLUDE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/CTestDefaultConfig.cmake
    )
    add_subdirectory(tests)
endif()
