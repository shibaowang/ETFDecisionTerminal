add_library(ShellAccountingQmlStartupRegistrationGateLib STATIC
    ShellAccountingQmlStartupRegistrationGate.cpp
    ShellAccountingQmlStartupRegistrationGate.h
)

target_include_directories(ShellAccountingQmlStartupRegistrationGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ShellAccountingQmlStartupRegistrationGateLib PUBLIC cxx_std_17)

function(add_shell_accounting_qml_startup_registration_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingQmlStartupRegistrationGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_qml_startup_registration_gate_test(
    ShellAccountingQmlStartupRegistrationGateTests
    test_shell_accounting_qml_startup_registration_gate.cpp
    shell_accounting_qml_startup_registration_gate
)

add_shell_accounting_qml_startup_registration_gate_test(
    ShellAccountingQmlStartupRegistrationLocationPolicyTests
    test_shell_accounting_qml_startup_registration_location_policy.cpp
    shell_accounting_qml_startup_registration_location_policy
)

add_shell_accounting_qml_startup_registration_gate_test(
    ShellAccountingQmlStartupRegistrationNoProductionWiringTests
    test_shell_accounting_qml_startup_registration_no_production_wiring.cpp
    shell_accounting_qml_startup_registration_no_production_wiring
)

add_shell_accounting_qml_startup_registration_gate_test(
    ShellAccountingQmlStartupRegistrationNoForbiddenExposureTests
    test_shell_accounting_qml_startup_registration_no_forbidden_exposure.cpp
    shell_accounting_qml_startup_registration_no_forbidden_exposure
)

add_shell_accounting_qml_startup_registration_gate_test(
    ShellAccountingQmlStartupRegistrationRollbackPolicyTests
    test_shell_accounting_qml_startup_registration_rollback_policy.cpp
    shell_accounting_qml_startup_registration_rollback_policy
)
