add_library(ShellAccountingPresenterLifecycleImplementationLib STATIC
    ShellAccountingPresenterLifecycleImplementation.cpp
    ShellAccountingPresenterLifecycleImplementation.h
)

target_compile_features(ShellAccountingPresenterLifecycleImplementationLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingPresenterLifecycleImplementationLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_presenter_lifecycle_implementation_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingPresenterLifecycleImplementationLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_presenter_lifecycle_implementation_test(
    ShellAccountingPresenterLifecycleImplementationTests
    test_shell_accounting_presenter_lifecycle_implementation.cpp
    shell_accounting_presenter_lifecycle_implementation
)

add_shell_accounting_presenter_lifecycle_implementation_test(
    ShellAccountingPresenterLifecycleAuthorizedCreationTests
    test_shell_accounting_presenter_lifecycle_authorized_creation.cpp
    shell_accounting_presenter_lifecycle_authorized_creation
)

add_shell_accounting_presenter_lifecycle_implementation_test(
    ShellAccountingPresenterLifecycleAuthorizedContextExposureTests
    test_shell_accounting_presenter_lifecycle_authorized_context_exposure.cpp
    shell_accounting_presenter_lifecycle_authorized_context_exposure
)

add_shell_accounting_presenter_lifecycle_implementation_test(
    ShellAccountingPresenterLifecycleOwnershipOrderTests
    test_shell_accounting_presenter_lifecycle_ownership_order.cpp
    shell_accounting_presenter_lifecycle_ownership_order
)

add_shell_accounting_presenter_lifecycle_implementation_test(
    ShellAccountingPresenterLifecycleUnavailableStateAfterExposureTests
    test_shell_accounting_presenter_lifecycle_unavailable_state_after_exposure.cpp
    shell_accounting_presenter_lifecycle_unavailable_state_after_exposure
)

add_shell_accounting_presenter_lifecycle_implementation_test(
    ShellAccountingPresenterLifecycleNoForbiddenRuntimeAccessAfterExposureTests
    test_shell_accounting_presenter_lifecycle_no_forbidden_runtime_access_after_exposure.cpp
    shell_accounting_presenter_lifecycle_no_forbidden_runtime_access_after_exposure
)

add_shell_accounting_presenter_lifecycle_implementation_test(
    ShellAccountingPresenterLifecycleNoTradeUiAfterExposureTests
    test_shell_accounting_presenter_lifecycle_no_trade_ui_after_exposure.cpp
    shell_accounting_presenter_lifecycle_no_trade_ui_after_exposure
)

add_shell_accounting_presenter_lifecycle_implementation_test(
    ShellAccountingPresenterLifecycleRollbackReadyAfterExposureTests
    test_shell_accounting_presenter_lifecycle_rollback_ready_after_exposure.cpp
    shell_accounting_presenter_lifecycle_rollback_ready_after_exposure
)
