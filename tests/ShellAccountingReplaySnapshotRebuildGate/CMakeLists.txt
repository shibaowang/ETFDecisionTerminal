add_library(ShellAccountingReplaySnapshotRebuildGateLib STATIC
    ShellAccountingReplaySnapshotRebuildGate.cpp
    ShellAccountingReplaySnapshotRebuildGate.h
)

target_compile_features(ShellAccountingReplaySnapshotRebuildGateLib PUBLIC cxx_std_17)

target_include_directories(ShellAccountingReplaySnapshotRebuildGateLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

function(add_shell_accounting_replay_snapshot_rebuild_gate_test target_name source_name ctest_name)
    add_executable(${target_name} ${source_name})
    target_link_libraries(${target_name}
        PRIVATE
            ShellAccountingReplaySnapshotRebuildGateLib
    )
    target_compile_features(${target_name} PRIVATE cxx_std_17)
    add_test(NAME ${ctest_name} COMMAND ${target_name} --source-root ${PROJECT_SOURCE_DIR})
endfunction()

add_shell_accounting_replay_snapshot_rebuild_gate_test(
    ShellAccountingReplaySnapshotRebuildGateTests
    test_shell_accounting_replay_snapshot_rebuild_gate.cpp
    shell_accounting_replay_snapshot_rebuild_gate
)

add_shell_accounting_replay_snapshot_rebuild_gate_test(
    ShellAccountingReplaySnapshotRebuildNoReplayYetTests
    test_shell_accounting_replay_snapshot_rebuild_no_replay_yet.cpp
    shell_accounting_replay_snapshot_rebuild_no_replay_yet
)

add_shell_accounting_replay_snapshot_rebuild_gate_test(
    ShellAccountingReplaySnapshotRebuildNoSnapshotWriteYetTests
    test_shell_accounting_replay_snapshot_rebuild_no_snapshot_write_yet.cpp
    shell_accounting_replay_snapshot_rebuild_no_snapshot_write_yet
)

add_shell_accounting_replay_snapshot_rebuild_gate_test(
    ShellAccountingReplaySnapshotRebuildDataServiceOnlyPolicyTests
    test_shell_accounting_replay_snapshot_rebuild_dataservice_only_policy.cpp
    shell_accounting_replay_snapshot_rebuild_dataservice_only_policy
)

add_shell_accounting_replay_snapshot_rebuild_gate_test(
    ShellAccountingReplaySnapshotRebuildNoUiDirectEngineAccessTests
    test_shell_accounting_replay_snapshot_rebuild_no_ui_direct_engine_access.cpp
    shell_accounting_replay_snapshot_rebuild_no_ui_direct_engine_access
)

add_shell_accounting_replay_snapshot_rebuild_gate_test(
    ShellAccountingReplaySnapshotRebuildNoWritePathTests
    test_shell_accounting_replay_snapshot_rebuild_no_write_path.cpp
    shell_accounting_replay_snapshot_rebuild_no_write_path
)

add_shell_accounting_replay_snapshot_rebuild_gate_test(
    ShellAccountingReplaySnapshotRebuildNoTradeOrStrategyTests
    test_shell_accounting_replay_snapshot_rebuild_no_trade_or_strategy.cpp
    shell_accounting_replay_snapshot_rebuild_no_trade_or_strategy
)

add_shell_accounting_replay_snapshot_rebuild_gate_test(
    ShellAccountingReplaySnapshotRebuildErrorMappingPolicyTests
    test_shell_accounting_replay_snapshot_rebuild_error_mapping_policy.cpp
    shell_accounting_replay_snapshot_rebuild_error_mapping_policy
)

add_shell_accounting_replay_snapshot_rebuild_gate_test(
    ShellAccountingReplaySnapshotRebuildRollbackPolicyTests
    test_shell_accounting_replay_snapshot_rebuild_rollback_policy.cpp
    shell_accounting_replay_snapshot_rebuild_rollback_policy
)
