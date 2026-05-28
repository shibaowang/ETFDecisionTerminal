find_package(Qt6 COMPONENTS Core REQUIRED)

function(add_shell_accounting_viewmodel_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ETFDecisionTerminal::ShellServices
            Qt6::Core
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_viewmodel_test(
    ShellAccountingStatusObjectReadOnlyBoundaryTests
    test_shell_accounting_status_object_readonly_boundary.cpp
    shell_accounting_status_object_readonly_boundary
)

add_shell_accounting_viewmodel_test(
    ShellAccountingIssueListModelReadOnlyBoundaryTests
    test_shell_accounting_issue_list_model_readonly_boundary.cpp
    shell_accounting_issue_list_model_readonly_boundary
)

add_shell_accounting_viewmodel_test(
    ShellPositionListModelReadOnlyBoundaryTests
    test_shell_position_list_model_readonly_boundary.cpp
    shell_position_list_model_readonly_boundary
)

add_shell_accounting_viewmodel_test(
    ShellAccountingViewModelPrivacyBoundaryTests
    test_shell_accounting_viewmodel_privacy_boundary.cpp
    shell_accounting_viewmodel_privacy_boundary
)

add_shell_accounting_viewmodel_test(
    ShellAccountingViewModelNoTradeActionBoundaryTests
    test_shell_accounting_viewmodel_no_trade_action_boundary.cpp
    shell_accounting_viewmodel_no_trade_action_boundary
)

add_shell_accounting_viewmodel_test(
    ShellAccountingViewModelNoServiceDependencyTests
    test_shell_accounting_viewmodel_no_service_dependency.cpp
    shell_accounting_viewmodel_no_service_dependency
)
