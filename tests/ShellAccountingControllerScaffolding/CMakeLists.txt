add_library(ShellAccountingControllerScaffolding STATIC
    ShellAccountingControllerScenario.cpp
    ShellAccountingStateMappingTestUtils.cpp
)

target_include_directories(ShellAccountingControllerScaffolding
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ShellAccountingControllerScaffolding PUBLIC cxx_std_17)

add_executable(ShellAccountingControllerStateContractTests
    test_shell_accounting_controller_state_contract.cpp
)

target_link_libraries(ShellAccountingControllerStateContractTests
    PRIVATE
        ShellAccountingControllerScaffolding
)

target_compile_features(ShellAccountingControllerStateContractTests PRIVATE cxx_std_17)

add_executable(ShellAccountingControllerIssueContractTests
    test_shell_accounting_controller_issue_contract.cpp
)

target_link_libraries(ShellAccountingControllerIssueContractTests
    PRIVATE
        ShellAccountingControllerScaffolding
)

target_compile_features(ShellAccountingControllerIssueContractTests PRIVATE cxx_std_17)

add_executable(ShellAccountingControllerPrivacyContractTests
    test_shell_accounting_controller_privacy_contract.cpp
)

target_link_libraries(ShellAccountingControllerPrivacyContractTests
    PRIVATE
        ShellAccountingControllerScaffolding
)

target_compile_features(ShellAccountingControllerPrivacyContractTests PRIVATE cxx_std_17)

add_executable(ShellAccountingControllerNoTradeActionContractTests
    test_shell_accounting_controller_no_trade_action_contract.cpp
)

target_link_libraries(ShellAccountingControllerNoTradeActionContractTests
    PRIVATE
        ShellAccountingControllerScaffolding
)

target_compile_features(ShellAccountingControllerNoTradeActionContractTests PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_controller_state_contract
    COMMAND ShellAccountingControllerStateContractTests
)

add_test(
    NAME shell_accounting_controller_issue_contract
    COMMAND ShellAccountingControllerIssueContractTests
)

add_test(
    NAME shell_accounting_controller_privacy_contract
    COMMAND ShellAccountingControllerPrivacyContractTests
)

add_test(
    NAME shell_accounting_controller_no_trade_action_contract
    COMMAND ShellAccountingControllerNoTradeActionContractTests
)
