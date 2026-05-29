find_package(Qt6 COMPONENTS Core Qml REQUIRED)

add_library(ShellAccountingQmlTypeRegistrationImplementationTestUtils STATIC
    ShellAccountingQmlTypeRegistrationImplementationTestUtils.cpp
    ShellAccountingQmlTypeRegistrationImplementationTestUtils.h
)

target_include_directories(ShellAccountingQmlTypeRegistrationImplementationTestUtils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ShellAccountingQmlTypeRegistrationImplementationTestUtils
    PUBLIC
        Qt6::Core
        Qt6::Qml
)

target_compile_features(ShellAccountingQmlTypeRegistrationImplementationTestUtils PUBLIC cxx_std_17)

function(add_shell_accounting_qml_type_registration_implementation_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingQmlTypeRegistrationImplementationTestUtils
            ETFDecisionTerminal::ShellServices
            Qt6::Core
            Qt6::Qml
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_qml_type_registration_implementation_test(
    ShellAccountingQmlTypeRegistrationModuleImportTests
    test_shell_accounting_qml_type_registration_module_import.cpp
    shell_accounting_qml_type_registration_module_import
)

add_shell_accounting_qml_type_registration_implementation_test(
    ShellAccountingQmlTypeRegistrationAllowlistRegisteredTests
    test_shell_accounting_qml_type_registration_allowlist_registered.cpp
    shell_accounting_qml_type_registration_allowlist_registered
)

add_shell_accounting_qml_type_registration_implementation_test(
    ShellAccountingQmlTypeRegistrationForbiddenTypesAbsentTests
    test_shell_accounting_qml_type_registration_forbidden_types_absent.cpp
    shell_accounting_qml_type_registration_forbidden_types_absent
)

add_shell_accounting_qml_type_registration_implementation_test(
    ShellAccountingQmlTypeRegistrationForbiddenMethodsAbsentTests
    test_shell_accounting_qml_type_registration_forbidden_methods_absent.cpp
    shell_accounting_qml_type_registration_forbidden_methods_absent
)

add_shell_accounting_qml_type_registration_implementation_test(
    ShellAccountingQmlTypeRegistrationUncreatableReadonlyTests
    test_shell_accounting_qml_type_registration_uncreatable_readonly.cpp
    shell_accounting_qml_type_registration_uncreatable_readonly
)

add_shell_accounting_qml_type_registration_implementation_test(
    ShellAccountingQmlTypeRegistrationNoProductionQmlBindingTests
    test_shell_accounting_qml_type_registration_no_production_qml_binding.cpp
    shell_accounting_qml_type_registration_no_production_qml_binding
)

add_shell_accounting_qml_type_registration_implementation_test(
    ShellAccountingQmlTypeRegistrationGateRegressionTests
    test_shell_accounting_qml_type_registration_gate_regression.cpp
    shell_accounting_qml_type_registration_gate_regression
)
