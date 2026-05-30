add_library(ShellAccountingSnapshotRebuildWriteGateLib STATIC
    ShellAccountingSnapshotRebuildWriteGate.cpp
    ShellAccountingSnapshotRebuildWriteGate.h
)

target_compile_features(ShellAccountingSnapshotRebuildWriteGateLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingSnapshotRebuildWriteGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_snapshot_rebuild_write_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingSnapshotRebuildWriteGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_snapshot_rebuild_write_gate_test(
    ShellAccountingSnapshotRebuildWriteGateTests
    test_shell_accounting_snapshot_rebuild_write_gate.cpp
    shell_accounting_snapshot_rebuild_write_gate
)

add_shell_accounting_snapshot_rebuild_write_gate_test(
    ShellAccountingSnapshotRebuildWriteNoRebuildYetTests
    test_shell_accounting_snapshot_rebuild_write_no_rebuild_yet.cpp
    shell_accounting_snapshot_rebuild_write_no_rebuild_yet
)

add_shell_accounting_snapshot_rebuild_write_gate_test(
    ShellAccountingSnapshotRebuildWriteNoSnapshotWriteYetTests
    test_shell_accounting_snapshot_rebuild_write_no_snapshot_write_yet.cpp
    shell_accounting_snapshot_rebuild_write_no_snapshot_write_yet
)

add_shell_accounting_snapshot_rebuild_write_gate_test(
    ShellAccountingSnapshotRebuildWriteDataServiceOnlyPolicyTests
    test_shell_accounting_snapshot_rebuild_write_dataservice_only_policy.cpp
    shell_accounting_snapshot_rebuild_write_dataservice_only_policy
)

add_shell_accounting_snapshot_rebuild_write_gate_test(
    ShellAccountingSnapshotRebuildWriteNoUiTriggeredWriteTests
    test_shell_accounting_snapshot_rebuild_write_no_ui_triggered_write.cpp
    shell_accounting_snapshot_rebuild_write_no_ui_triggered_write
)

add_shell_accounting_snapshot_rebuild_write_gate_test(
    ShellAccountingSnapshotRebuildWriteNoTradeOrStrategyTests
    test_shell_accounting_snapshot_rebuild_write_no_trade_or_strategy.cpp
    shell_accounting_snapshot_rebuild_write_no_trade_or_strategy
)

add_shell_accounting_snapshot_rebuild_write_gate_test(
    ShellAccountingSnapshotRebuildWriteErrorMappingPolicyTests
    test_shell_accounting_snapshot_rebuild_write_error_mapping_policy.cpp
    shell_accounting_snapshot_rebuild_write_error_mapping_policy
)

add_shell_accounting_snapshot_rebuild_write_gate_test(
    ShellAccountingSnapshotRebuildWriteRollbackPolicyTests
    test_shell_accounting_snapshot_rebuild_write_rollback_policy.cpp
    shell_accounting_snapshot_rebuild_write_rollback_policy
)
