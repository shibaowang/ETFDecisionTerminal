add_library(ShellAccountingQmlTypeRegistrationScaffoldScenarios STATIC
    ShellAccountingQmlTypeRegistrationExpectedType.cpp
    ShellAccountingQmlTypeRegistrationExpectedType.h
    ShellAccountingQmlTypeRegistrationScenario.cpp
    ShellAccountingQmlTypeRegistrationScenario.h
)

target_include_directories(ShellAccountingQmlTypeRegistrationScaffoldScenarios
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ShellAccountingQmlTypeRegistrationScaffoldScenarios PUBLIC cxx_std_17)

function(add_shell_accounting_qml_type_registration_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingQmlTypeRegistrationScaffoldScenarios
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_qml_type_registration_test(
    ShellAccountingQmlTypeRegistrationAllowlistScaffoldTests
    test_shell_accounting_qml_type_registration_allowlist_scaffold.cpp
    shell_accounting_qml_type_registration_allowlist_scaffold
)

add_shell_accounting_qml_type_registration_test(
    ShellAccountingQmlTypeRegistrationDenylistScaffoldTests
    test_shell_accounting_qml_type_registration_denylist_scaffold.cpp
    shell_accounting_qml_type_registration_denylist_scaffold
)

add_shell_accounting_qml_type_registration_test(
    ShellAccountingQmlTypeRegistrationMethodDenylistScaffoldTests
    test_shell_accounting_qml_type_registration_method_denylist_scaffold.cpp
    shell_accounting_qml_type_registration_method_denylist_scaffold
)

add_shell_accounting_qml_type_registration_test(
    ShellAccountingQmlTypeRegistrationModuleImportScaffoldTests
    test_shell_accounting_qml_type_registration_module_import_scaffold.cpp
    shell_accounting_qml_type_registration_module_import_scaffold
)

add_shell_accounting_qml_type_registration_test(
    ShellAccountingQmlTypeRegistrationGateDependencyTests
    test_shell_accounting_qml_type_registration_gate_dependency.cpp
    shell_accounting_qml_type_registration_gate_dependency
)

add_shell_accounting_qml_type_registration_test(
    ShellAccountingQmlTypeRegistrationNoProductionChangeTests
    test_shell_accounting_qml_type_registration_no_production_change.cpp
    shell_accounting_qml_type_registration_no_production_change
)
