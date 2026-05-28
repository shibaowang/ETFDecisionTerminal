add_library(PositionListRealActionNoWriteSkeleton STATIC
    PositionListRealActionNoWriteSkeleton.cpp
)

target_include_directories(PositionListRealActionNoWriteSkeleton
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(PositionListRealActionNoWriteSkeleton PUBLIC cxx_std_17)

add_executable(PositionListRealActionNoWriteSkeletonTests
    test_position_list_real_action_no_write_skeleton.cpp
)

target_link_libraries(PositionListRealActionNoWriteSkeletonTests
    PRIVATE
        PositionListRealActionNoWriteSkeleton
        AccountingNoWriteTestUtils
)

target_compile_features(PositionListRealActionNoWriteSkeletonTests PRIVATE cxx_std_17)

add_executable(PositionListRealActionFailureNoWriteSkeletonTests
    test_position_list_real_action_failure_no_write_skeleton.cpp
)

target_link_libraries(PositionListRealActionFailureNoWriteSkeletonTests
    PRIVATE
        PositionListRealActionNoWriteSkeleton
        AccountingNoWriteTestUtils
)

target_compile_features(PositionListRealActionFailureNoWriteSkeletonTests PRIVATE cxx_std_17)

add_test(
    NAME position_list_real_action_no_write_skeleton
    COMMAND
        PositionListRealActionNoWriteSkeletonTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

add_test(
    NAME position_list_real_action_failure_no_write_skeleton
    COMMAND
        PositionListRealActionFailureNoWriteSkeletonTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)
