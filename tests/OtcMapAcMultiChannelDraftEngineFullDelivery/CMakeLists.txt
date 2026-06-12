find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(E279OtcMapAcMultiChannelDraftEngineFullDelivery
    otcmap_ac_multichannel_draft_engine_full_delivery.cpp
)

target_link_libraries(E279OtcMapAcMultiChannelDraftEngineFullDelivery
    PRIVATE
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::DataServiceClient
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::ServiceRuntime
        ETFDecisionTerminal::ShellServices
        ETFDecisionTerminal::StrategyEngine
        Qt6::Core
)

target_compile_features(E279OtcMapAcMultiChannelDraftEngineFullDelivery PRIVATE cxx_std_17)

add_test(
    NAME otcmap_ac_multichannel_draft_engine_full_delivery
    COMMAND
        E279OtcMapAcMultiChannelDraftEngineFullDelivery
        --source-root
        ${PROJECT_SOURCE_DIR}
)

set_tests_properties(otcmap_ac_multichannel_draft_engine_full_delivery PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)

