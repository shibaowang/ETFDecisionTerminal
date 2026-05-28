add_library(PositionListFirstStageScaffolding STATIC
    PositionListFirstStageExpectedContract.cpp
    PositionListFirstStageScenario.cpp
)

target_include_directories(PositionListFirstStageScaffolding
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(PositionListFirstStageScaffolding PUBLIC cxx_std_17)

add_executable(PositionListFirstStageScopeContractTests
    test_position_list_first_stage_scope_contract.cpp
)

target_link_libraries(PositionListFirstStageScopeContractTests
    PRIVATE
        PositionListFirstStageScaffolding
)

target_compile_features(PositionListFirstStageScopeContractTests PRIVATE cxx_std_17)

add_executable(PositionListFirstStageGuardTransitionContractTests
    test_position_list_first_stage_guard_transition_contract.cpp
)

target_link_libraries(PositionListFirstStageGuardTransitionContractTests
    PRIVATE
        PositionListFirstStageScaffolding
        AccountingNoWriteTestUtils
)

target_compile_features(PositionListFirstStageGuardTransitionContractTests PRIVATE cxx_std_17)

add_executable(PositionListFirstStageFailureMatrixContractTests
    test_position_list_first_stage_failure_matrix_contract.cpp
)

target_link_libraries(PositionListFirstStageFailureMatrixContractTests
    PRIVATE
        PositionListFirstStageScaffolding
)

target_compile_features(PositionListFirstStageFailureMatrixContractTests PRIVATE cxx_std_17)

add_test(
    NAME position_list_first_stage_scope_contract
    COMMAND PositionListFirstStageScopeContractTests
)

add_test(
    NAME position_list_first_stage_guard_transition_contract
    COMMAND PositionListFirstStageGuardTransitionContractTests
)

add_test(
    NAME position_list_first_stage_failure_matrix_contract
    COMMAND PositionListFirstStageFailureMatrixContractTests
)
