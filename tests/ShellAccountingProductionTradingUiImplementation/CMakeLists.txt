add_library(ShellAccountingProductionTradingUiImplementationLib STATIC
    ShellAccountingProductionTradingUiImplementation.cpp
    ShellAccountingProductionTradingUiImplementation.h
)

target_compile_features(ShellAccountingProductionTradingUiImplementationLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingProductionTradingUiImplementationLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_production_trading_ui_implementation_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingProductionTradingUiImplementationLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplImplementationTests
    test_shell_accounting_production_trading_ui_implementation.cpp
    shell_accounting_production_trading_ui_implementation
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplAuthorizedSectionTests
    test_shell_accounting_production_trading_ui_authorized_page_or_section.cpp
    shell_accounting_production_trading_ui_authorized_page_or_section
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplCreateBindingTests
    test_shell_accounting_production_trading_ui_create_draft_binding.cpp
    shell_accounting_production_trading_ui_create_draft_binding
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplConfirmBindingTests
    test_shell_accounting_production_trading_ui_confirm_draft_binding.cpp
    shell_accounting_production_trading_ui_confirm_draft_binding
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplUserConfirmTests
    test_shell_accounting_production_trading_ui_user_confirmation_required.cpp
    shell_accounting_production_trading_ui_user_confirmation_required
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplNoDirectClientTests
    test_shell_accounting_production_trading_ui_no_direct_dataserviceclient.cpp
    shell_accounting_production_trading_ui_no_direct_dataserviceclient
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplNoDirectDbTests
    test_shell_accounting_production_trading_ui_no_direct_db_write.cpp
    shell_accounting_production_trading_ui_no_direct_db_write
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplNoBrokerTests
    test_shell_accounting_production_trading_ui_no_broker_order.cpp
    shell_accounting_production_trading_ui_no_broker_order
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplNoStrategyTests
    test_shell_accounting_production_trading_ui_no_strategy_execution.cpp
    shell_accounting_production_trading_ui_no_strategy_execution
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplNoAutoTradingTests
    test_shell_accounting_production_trading_ui_no_auto_trading.cpp
    shell_accounting_production_trading_ui_no_auto_trading
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplErrorMappingTests
    test_shell_accounting_production_trading_ui_error_mapping.cpp
    shell_accounting_production_trading_ui_error_mapping
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplPrivacyTests
    test_shell_accounting_production_trading_ui_privacy.cpp
    shell_accounting_production_trading_ui_privacy
)

add_shell_accounting_production_trading_ui_implementation_test(
    SAPTUiImplRollbackTests
    test_shell_accounting_production_trading_ui_rollback_ready.cpp
    shell_accounting_production_trading_ui_rollback_ready
)
