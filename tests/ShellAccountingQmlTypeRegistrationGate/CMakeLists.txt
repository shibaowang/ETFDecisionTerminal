add_library(ShellAccountingQmlTypeRegistrationGateLib STATIC
    ShellAccountingQmlTypeRegistrationGate.cpp
    ShellAccountingQmlTypeRegistrationGate.h
)

target_include_directories(ShellAccountingQmlTypeRegistrationGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ShellAccountingQmlTypeRegistrationGateLib PUBLIC cxx_std_17)

function(add_shell_accounting_qml_type_registration_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingQmlTypeRegistrationGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_qml_type_registration_gate_test(
    ShellAccountingQmlTypeRegistrationImplementationGateTests
    test_shell_accounting_qml_type_registration_implementation_gate.cpp
    shell_accounting_qml_type_registration_implementation_gate
)

add_shell_accounting_qml_type_registration_gate_test(
    ShellAccountingQmlTypeRegistrationLocationGateTests
    test_shell_accounting_qml_type_registration_location_gate.cpp
    shell_accounting_qml_type_registration_location_gate
)

add_shell_accounting_qml_type_registration_gate_test(
    ShellAccountingQmlTypeRegistrationAllowlistGateTests
    test_shell_accounting_qml_type_registration_allowlist_gate.cpp
    shell_accounting_qml_type_registration_allowlist_gate
)

add_shell_accounting_qml_type_registration_gate_test(
    ShellAccountingQmlTypeRegistrationForbiddenExposureGateTests
    test_shell_accounting_qml_type_registration_forbidden_exposure_gate.cpp
    shell_accounting_qml_type_registration_forbidden_exposure_gate
)

add_shell_accounting_qml_type_registration_gate_test(
    ShellAccountingQmlTypeRegistrationRollbackGateTests
    test_shell_accounting_qml_type_registration_rollback_gate.cpp
    shell_accounting_qml_type_registration_rollback_gate
)

add_shell_accounting_qml_type_registration_gate_test(
    ShellAccountingQmlTypeRegistrationNoProductionRegistrationTests
    test_shell_accounting_qml_type_registration_no_production_registration.cpp
    shell_accounting_qml_type_registration_no_production_registration
)
