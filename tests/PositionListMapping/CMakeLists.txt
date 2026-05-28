add_library(PositionListMappingContractTestUtils
    PositionListMappingContractTestUtils.cpp
)

target_include_directories(PositionListMappingContractTestUtils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(PositionListMappingContractTestUtils
    PUBLIC
        ETFDecisionTerminal::AccountingEngine
        ETFDecisionTerminal::DataAccess
)

target_compile_features(PositionListMappingContractTestUtils PUBLIC cxx_std_17)

add_executable(PositionListRequestMappingContractTests
    test_position_list_request_mapping.cpp
)

target_link_libraries(PositionListRequestMappingContractTests
    PRIVATE
        PositionListMappingContractTestUtils
)

target_compile_features(PositionListRequestMappingContractTests PRIVATE cxx_std_17)

add_test(
    NAME position_list_request_mapping_contract
    COMMAND PositionListRequestMappingContractTests
)

add_executable(PositionListTradeFactMappingContractTests
    test_position_list_trade_fact_mapping.cpp
)

target_link_libraries(PositionListTradeFactMappingContractTests
    PRIVATE
        PositionListMappingContractTestUtils
)

target_compile_features(PositionListTradeFactMappingContractTests PRIVATE cxx_std_17)

add_test(
    NAME position_list_trade_fact_mapping_contract
    COMMAND PositionListTradeFactMappingContractTests
)

add_executable(PositionListResponseMappingContractTests
    test_position_list_response_mapping.cpp
)

target_link_libraries(PositionListResponseMappingContractTests
    PRIVATE
        PositionListMappingContractTestUtils
)

target_compile_features(PositionListResponseMappingContractTests PRIVATE cxx_std_17)

add_test(
    NAME position_list_response_mapping_contract
    COMMAND PositionListResponseMappingContractTests
)
