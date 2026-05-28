find_package(Qt6 COMPONENTS Core Network REQUIRED)

add_executable(ShellReadOnlyDataControllerTests
    test_shell_readonly_data_controller.cpp
)

target_link_libraries(ShellReadOnlyDataControllerTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
        ETFDecisionTerminal::DataAccess
        ETFDecisionTerminal::DataServiceApi
        ETFDecisionTerminal::ServiceHost
        ETFDecisionTerminal::ServiceRuntime
        Qt6::Core
        Qt6::Network
)

target_compile_features(ShellReadOnlyDataControllerTests PRIVATE cxx_std_17)

add_test(
    NAME shell_readonly_data_controller
    COMMAND
        ShellReadOnlyDataControllerTests
        --migration
        ${PROJECT_SOURCE_DIR}/migrations/001_initial_schema.sql
)

set_tests_properties(shell_readonly_data_controller PROPERTIES
    ENVIRONMENT_MODIFICATION
        "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>"
)

add_executable(ShellAccountingReadOnlyControllerSkeletonStateTests
    test_shell_accounting_readonly_controller_skeleton_state.cpp
)

target_link_libraries(ShellAccountingReadOnlyControllerSkeletonStateTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingReadOnlyControllerSkeletonStateTests PRIVATE cxx_std_17)

add_executable(ShellAccountingReadOnlyControllerSkeletonIssueTests
    test_shell_accounting_readonly_controller_skeleton_issue.cpp
)

target_link_libraries(ShellAccountingReadOnlyControllerSkeletonIssueTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingReadOnlyControllerSkeletonIssueTests PRIVATE cxx_std_17)

add_executable(ShellAccountingReadOnlyControllerSkeletonPrivacyTests
    test_shell_accounting_readonly_controller_skeleton_privacy.cpp
)

target_link_libraries(ShellAccountingReadOnlyControllerSkeletonPrivacyTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingReadOnlyControllerSkeletonPrivacyTests PRIVATE cxx_std_17)

add_executable(ShellAccountingReadOnlyControllerSkeletonNoTradeActionTests
    test_shell_accounting_readonly_controller_skeleton_no_trade_action.cpp
)

target_link_libraries(ShellAccountingReadOnlyControllerSkeletonNoTradeActionTests
    PRIVATE
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingReadOnlyControllerSkeletonNoTradeActionTests PRIVATE cxx_std_17)

add_executable(ShellAccountingReadOnlyControllerSkeletonNoServiceDependencyTests
    test_shell_accounting_readonly_controller_skeleton_no_service_dependency.cpp
)

target_compile_features(ShellAccountingReadOnlyControllerSkeletonNoServiceDependencyTests PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_readonly_controller_skeleton_state
    COMMAND ShellAccountingReadOnlyControllerSkeletonStateTests
)

add_test(
    NAME shell_accounting_readonly_controller_skeleton_issue
    COMMAND ShellAccountingReadOnlyControllerSkeletonIssueTests
)

add_test(
    NAME shell_accounting_readonly_controller_skeleton_privacy
    COMMAND ShellAccountingReadOnlyControllerSkeletonPrivacyTests
)

add_test(
    NAME shell_accounting_readonly_controller_skeleton_no_trade_action
    COMMAND ShellAccountingReadOnlyControllerSkeletonNoTradeActionTests
)

add_test(
    NAME shell_accounting_readonly_controller_skeleton_no_service_dependency
    COMMAND
        ShellAccountingReadOnlyControllerSkeletonNoServiceDependencyTests
        --source-root
        ${PROJECT_SOURCE_DIR}
)
