add_library(ShellAccountingFakeClientBoundary STATIC
    FakeAccountingClientPayload.cpp
    FakeAccountingClientScenario.cpp
)

target_include_directories(ShellAccountingFakeClientBoundary
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ShellAccountingFakeClientBoundary
    PUBLIC
        ETFDecisionTerminal::ShellServices
)

target_compile_features(ShellAccountingFakeClientBoundary PUBLIC cxx_std_17)

add_executable(ShellAccountingFakeClientGuardPayloadTests
    test_shell_accounting_fake_client_guard_payloads.cpp
)

target_link_libraries(ShellAccountingFakeClientGuardPayloadTests
    PRIVATE
        ShellAccountingFakeClientBoundary
)

target_compile_features(ShellAccountingFakeClientGuardPayloadTests PRIVATE cxx_std_17)

add_executable(ShellAccountingFakeClientStateTransitionTests
    test_shell_accounting_fake_client_state_transitions.cpp
)

target_link_libraries(ShellAccountingFakeClientStateTransitionTests
    PRIVATE
        ShellAccountingFakeClientBoundary
)

target_compile_features(ShellAccountingFakeClientStateTransitionTests PRIVATE cxx_std_17)

add_executable(ShellAccountingFakeClientIssueMappingTests
    test_shell_accounting_fake_client_issue_mapping.cpp
)

target_link_libraries(ShellAccountingFakeClientIssueMappingTests
    PRIVATE
        ShellAccountingFakeClientBoundary
)

target_compile_features(ShellAccountingFakeClientIssueMappingTests PRIVATE cxx_std_17)

add_executable(ShellAccountingFakeClientPrivacyAndNoTradeTests
    test_shell_accounting_fake_client_privacy_and_no_trade.cpp
)

target_link_libraries(ShellAccountingFakeClientPrivacyAndNoTradeTests
    PRIVATE
        ShellAccountingFakeClientBoundary
)

target_compile_features(ShellAccountingFakeClientPrivacyAndNoTradeTests PRIVATE cxx_std_17)

add_executable(ShellAccountingFakeClientNoRealDependencyTests
    test_shell_accounting_fake_client_no_real_dependency.cpp
)

target_compile_features(ShellAccountingFakeClientNoRealDependencyTests PRIVATE cxx_std_17)

add_test(
    NAME shell_accounting_fake_client_guard_payloads
    COMMAND ShellAccountingFakeClientGuardPayloadTests
)

add_test(
    NAME shell_accounting_fake_client_state_transitions
    COMMAND ShellAccountingFakeClientStateTransitionTests
)

add_test(
    NAME shell_accounting_fake_client_issue_mapping
    COMMAND ShellAccountingFakeClientIssueMappingTests
)

add_test(
    NAME shell_accounting_fake_client_privacy_and_no_trade
    COMMAND ShellAccountingFakeClientPrivacyAndNoTradeTests
)

add_test(
    NAME shell_accounting_fake_client_no_real_dependency
    COMMAND
        ShellAccountingFakeClientNoRealDependencyTests
        --source-root
        ${PROJECT_SOURCE_DIR}
)
