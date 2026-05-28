find_package(Qt6 COMPONENTS Core REQUIRED)

function(add_shell_accounting_presenter_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ETFDecisionTerminal::ShellServices
            Qt6::Core
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_presenter_test(
    ShellAccountingPresenterSkeletonConstructionTests
    test_shell_accounting_presenter_skeleton_construction.cpp
    shell_accounting_presenter_skeleton_construction
)

add_shell_accounting_presenter_test(
    ShellAccountingPresenterSkeletonStatusIssueModelTests
    test_shell_accounting_presenter_skeleton_status_issue_models.cpp
    shell_accounting_presenter_skeleton_status_issue_models
)

add_shell_accounting_presenter_test(
    ShellAccountingPresenterSkeletonRefreshBoundaryTests
    test_shell_accounting_presenter_skeleton_refresh_boundary.cpp
    shell_accounting_presenter_skeleton_refresh_boundary
)

add_shell_accounting_presenter_test(
    ShellAccountingPresenterSkeletonPrivacyBoundaryTests
    test_shell_accounting_presenter_skeleton_privacy_boundary.cpp
    shell_accounting_presenter_skeleton_privacy_boundary
)

add_shell_accounting_presenter_test(
    ShellAccountingPresenterSkeletonNoTradeActionTests
    test_shell_accounting_presenter_skeleton_no_trade_action.cpp
    shell_accounting_presenter_skeleton_no_trade_action
)

add_shell_accounting_presenter_test(
    ShellAccountingPresenterSkeletonNoServiceDependencyTests
    test_shell_accounting_presenter_skeleton_no_service_dependency.cpp
    shell_accounting_presenter_skeleton_no_service_dependency
)

add_library(ShellAccountingPresenterConcretePortTestUtils STATIC
    ShellAccountingPresenterConcretePortTestUtils.cpp
    ShellAccountingPresenterConcretePortTestUtils.h
)

target_link_libraries(ShellAccountingPresenterConcretePortTestUtils
    PUBLIC
        ShellAccountingControllerConcretePortIntegrationTestUtils
        Qt6::Core
)

target_include_directories(ShellAccountingPresenterConcretePortTestUtils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ShellAccountingPresenterConcretePortTestUtils PUBLIC cxx_std_17)

function(add_shell_accounting_presenter_concrete_port_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingPresenterConcretePortTestUtils
            Qt6::Core
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_presenter_concrete_port_test(
    ShellAccountingPresenterConcretePortPositionListGuardTests
    test_shell_accounting_presenter_concrete_port_position_list_guard.cpp
    shell_accounting_presenter_concrete_port_position_list_guard
)

add_shell_accounting_presenter_concrete_port_test(
    ShellAccountingPresenterConcretePortRefreshBoundaryTests
    test_shell_accounting_presenter_concrete_port_refresh_boundary.cpp
    shell_accounting_presenter_concrete_port_refresh_boundary
)

add_shell_accounting_presenter_concrete_port_test(
    ShellAccountingPresenterConcretePortNoWriteNoTradeTests
    test_shell_accounting_presenter_concrete_port_no_write_no_trade.cpp
    shell_accounting_presenter_concrete_port_no_write_no_trade
)

add_shell_accounting_presenter_concrete_port_test(
    ShellAccountingPresenterConcretePortNoQmlOrForbiddenDependencyTests
    test_shell_accounting_presenter_concrete_port_no_qml_or_forbidden_dependency.cpp
    shell_accounting_presenter_concrete_port_no_qml_or_forbidden_dependency
)
