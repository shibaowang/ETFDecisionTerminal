add_library(ShellAccountingQmlStartupRegistrationWiringGateLib STATIC
    ShellAccountingQmlStartupRegistrationWiringGate.cpp
    ShellAccountingQmlStartupRegistrationWiringGate.h
)

target_include_directories(ShellAccountingQmlStartupRegistrationWiringGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ShellAccountingQmlStartupRegistrationWiringGateLib PUBLIC cxx_std_17)

function(add_shell_accounting_qml_startup_registration_wiring_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingQmlStartupRegistrationWiringGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_qml_startup_registration_wiring_test(
    ShellAccountingQmlStartupRegistrationWiringTests
    test_shell_accounting_qml_startup_registration_wiring.cpp
    shell_accounting_qml_startup_registration_wiring
)

add_shell_accounting_qml_startup_registration_wiring_test(
    ShellAccountingQmlStartupRegistrationWiringSingleCallTests
    test_shell_accounting_qml_startup_registration_wiring_single_call.cpp
    shell_accounting_qml_startup_registration_wiring_single_call
)

add_shell_accounting_qml_startup_registration_wiring_test(
    ShellAccountingQmlStartupRegistrationWiringLocationPolicyTests
    test_shell_accounting_qml_startup_registration_wiring_location_policy.cpp
    shell_accounting_qml_startup_registration_wiring_location_policy
)

add_shell_accounting_qml_startup_registration_wiring_test(
    ShellAccountingQmlStartupRegistrationWiringReadOnlyAllowlistTests
    test_shell_accounting_qml_startup_registration_wiring_preserves_readonly_allowlist.cpp
    shell_accounting_qml_startup_registration_wiring_preserves_readonly_allowlist
)

add_shell_accounting_qml_startup_registration_wiring_test(
    ShellAccountingQmlStartupRegistrationWiringNoQmlImportTests
    test_shell_accounting_qml_startup_registration_wiring_no_qml_import.cpp
    shell_accounting_qml_startup_registration_wiring_no_qml_import
)

add_shell_accounting_qml_startup_registration_wiring_test(
    ShellAccountingQmlStartupRegistrationWiringNoContextExposureTests
    test_shell_accounting_qml_startup_registration_wiring_no_context_exposure.cpp
    shell_accounting_qml_startup_registration_wiring_no_context_exposure
)

add_shell_accounting_qml_startup_registration_wiring_test(
    ShellAccountingQmlStartupRegistrationWiringNoForbiddenRuntimeAccessTests
    test_shell_accounting_qml_startup_registration_wiring_no_forbidden_runtime_access.cpp
    shell_accounting_qml_startup_registration_wiring_no_forbidden_runtime_access
)

add_shell_accounting_qml_startup_registration_wiring_test(
    ShellAccountingQmlStartupRegistrationWiringRollbackReadyTests
    test_shell_accounting_qml_startup_registration_wiring_rollback_ready.cpp
    shell_accounting_qml_startup_registration_wiring_rollback_ready
)
