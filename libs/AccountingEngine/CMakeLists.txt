add_library(ETFAccountingEngine STATIC
    src/AccountingEngineInfo.cpp
    src/AccountingEngineCapabilities.cpp
    src/AccountingEngineBoundary.cpp
    src/AccountingIssue.cpp
    src/AccountingReplayDtos.cpp
    src/AccountingReplayEngine.cpp
    src/AccountingReplayParser.cpp
    src/AccountingReplayResult.cpp
    src/AccountingReplayValidation.cpp
    src/ShellAccountingPortfolioReplayEngine.cpp
)

add_library(ETFDecisionTerminal::AccountingEngine ALIAS ETFAccountingEngine)

target_include_directories(ETFAccountingEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(ETFAccountingEngine PUBLIC cxx_std_17)

set_target_properties(ETFAccountingEngine PROPERTIES
    OUTPUT_NAME ETFAccountingEngine
)
