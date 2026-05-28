add_library(ShellAccountingQmlStaticGateScanner STATIC
    QmlStaticGateScanner.cpp
    QmlStaticGateScanner.h
)

target_include_directories(ShellAccountingQmlStaticGateScanner
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ShellAccountingQmlStaticGateScanner PUBLIC cxx_std_17)

function(add_shell_accounting_qml_static_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingQmlStaticGateScanner
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_qml_static_gate_test(
    ShellAccountingQmlStaticGateNoDirectServiceTests
    test_shell_accounting_qml_static_gate_no_direct_service.cpp
    shell_accounting_qml_static_gate_no_direct_service
)

add_shell_accounting_qml_static_gate_test(
    ShellAccountingQmlStaticGateNoAccountingBindingYetTests
    test_shell_accounting_qml_static_gate_no_accounting_binding_yet.cpp
    shell_accounting_qml_static_gate_no_accounting_binding_yet
)

add_shell_accounting_qml_static_gate_test(
    ShellAccountingQmlStaticGateNoWriteTokensTests
    test_shell_accounting_qml_static_gate_no_write_tokens.cpp
    shell_accounting_qml_static_gate_no_write_tokens
)

add_shell_accounting_qml_static_gate_test(
    ShellAccountingQmlStaticGateAccountingScopeNoTradeTests
    test_shell_accounting_qml_static_gate_accounting_scope_no_trade.cpp
    shell_accounting_qml_static_gate_accounting_scope_no_trade
)

add_shell_accounting_qml_static_gate_test(
    ShellAccountingQmlStaticGateDocsSyncTests
    test_shell_accounting_qml_static_gate_docs_sync.cpp
    shell_accounting_qml_static_gate_docs_sync
)
