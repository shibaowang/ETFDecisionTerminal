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
