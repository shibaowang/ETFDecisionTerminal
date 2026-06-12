add_library(ETFDataServiceApi STATIC
    src/DataServiceActionRegistrar.cpp
    src/DataServiceActions.cpp
    src/DataServiceApi.cpp
    src/JsonBuilders.cpp
    src/ShellAccountingBrokerOrderPort.cpp
    src/ShellAccountingBrokerOrderPortModeSelector.cpp
    src/ShellAccountingBrokerOrderPortProvider.cpp
    src/ShellAccountingBrokerRuntimeModeSource.cpp
    src/ShellAccountingBrokerSandboxRuntimeEnablement.cpp
    src/ShellAccountingBrokerSandboxOrderPortScaffold.cpp
    src/ShellAccountingExcelVbaImportPersistManualEntryAction.cpp
    src/ShellAccountingExcelVbaImportReadOnlyParser.cpp
    src/ShellAccountingExcelVbaImportReadOnlyPreviewAction.cpp
    src/ShellAccountingOtcMapMultiChannelDraftAction.cpp
    src/ShellAccountingPortfolioReplayReadOnlySummaryAction.cpp
    src/ShellAccountingTradeDraftRecommendationAction.cpp
    src/ShellAccountingManualTransactionCashMovementValidation.cpp
    src/StrategyRecommendationReadOnlySummaryAction.cpp
    src/WriteActionPolicy.cpp
)

add_library(ETFDecisionTerminal::DataServiceApi ALIAS ETFDataServiceApi)

target_include_directories(ETFDataServiceApi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ETFDataServiceApi
    PUBLIC
        ETFDecisionTerminal::CoreDomain
        ETFDecisionTerminal::Protocol
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::AccountingEngine
        ETFDecisionTerminal::StrategyEngine
)

target_compile_features(ETFDataServiceApi PUBLIC cxx_std_17)

set_target_properties(ETFDataServiceApi PROPERTIES
    OUTPUT_NAME ETFDataServiceApi
)
