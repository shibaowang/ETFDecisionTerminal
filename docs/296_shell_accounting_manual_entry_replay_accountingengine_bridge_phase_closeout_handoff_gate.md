# TASK-250 ShellAccounting manual entry replay AccountingEngine bridge phase closeout and handoff gate

## Purpose

TASK-250 closes out the test-only ShellAccounting manual entry replay
AccountingEngine bridge phase and records the handoff boundary for the next
runtime integration authorization step.

The task is phase-closeout and handoff-only. It verifies evidence from TASK-245
through TASK-249 while keeping the bridge isolated from production replay,
runtime writes, fixture mutation, broker paths, and AccountingEngine source.

## Relationship To TASK-245 Through TASK-249

TASK-245 authorized the test-only bridge boundary.

TASK-246 added the stdout-only bridge contract shell.

TASK-247 added bridge regression matrix coverage.

TASK-248 hardened bridge failure modes and sanitized output.

TASK-249 added CI closeout coverage for the bridge phase.

TASK-250 closes the phase and hands off to a future runtime integration
authorization gate. It does not replace or weaken the prior gates.

## Phase Closeout Scope

The phase closeout scope is limited to documentation, CTest registration,
changed-path allowlist checks, bridge contract evidence, failure-mode evidence,
and static source scans.

The gate may run the TASK-246 test-only bridge script as a stdout-only probe
with temporary input files outside the repository. It must not run full CTest
recursively, write repository files, access broker or network code, or invoke
AccountingEngine replay.

## Completed Bridge Gates

The completed bridge phase includes:

- `shell_accounting_manual_entry_replay_accountingengine_bridge_authorization`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_implementation`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout`.

## CI Evidence

TASK-250 requires evidence that TASK-245 through TASK-249 docs and CTests remain
indexed, registered, and present.

The required regression set also includes AccountingEngine adequacy review,
manual entry replay implementation, dry-run harness, and diagnostics gates so
the handoff does not hide a cross-phase regression.

## Bridge Contract Evidence

The TASK-246 bridge script must still support:

- `--dry-run-summary-json`.
- `--implementation-summary-json`.
- `--summary-json`.
- `--expect-no-accountingengine-call`.
- `--failure-json`.

The bridge stdout success contract must still report:

- `task=TASK-246`.
- `mode=test-only-accountingengine-bridge-contract`.
- `accountingEngineCalled=false`.
- `replayExecuted=false`.
- `runtimeWrite=false`.

## Failure-Mode Evidence

The bridge failure-mode issue codes from TASK-248 must remain present and
sanitized.

TASK-250 must preserve the boundary that failure evidence is test-only and does
not write audit, ledger, snapshot, database, fixture, or broker state.

## Handoff Preconditions

The next runtime integration task may proceed only after TASK-245 through
TASK-250 gates pass on the feature branch and clean main.

The future runtime integration authorization task must explicitly define the
DataService boundary, replay invocation boundary, read model refresh boundary,
runtime write boundary, privacy policy, rollback policy, and failure mapping.

## Handoff Non-Goals

TASK-250 does not authorize runtime integration.

TASK-250 does not authorize production bridge code.

TASK-250 does not authorize AccountingEngine replay calls.

TASK-250 does not authorize read model refresh, audit writes, ledger writes,
snapshot writes, broker access, network access, credentials, endpoint access,
real order placement, or automatic trading.

## No-AccountingEngine-Call Boundary

TASK-250 does not call AccountingEngine replay.

Static scans must continue to reject direct `libs/AccountingEngine/` imports,
`replayReadOnly`, production bridge paths, and runtime replay entry points.

## No-Replay Boundary

TASK-250 does not implement replay.

The bridge stdout contract must continue to report `replayExecuted=false`.

No production replay parser, loader, reader, bridge, runtime integration, or
AccountingEngine replay entry point is authorized by this task.

## No-Write Boundary

TASK-250 does not authorize runtime SQL / SQLite read/write.

TASK-250 does not authorize audit / ledger / snapshot writes.

TASK-250 does not authorize read model refresh.

The bridge stdout contract must continue to report `runtimeWrite=false`.

## AccountingEngine Code Boundary

TASK-250 does not modify AccountingEngine code.

The gate must fail if `libs/AccountingEngine/` appears in changed paths or if
bridge gate source attempts to import or call AccountingEngine replay APIs.

## Production Integration Boundary

TASK-250 does not implement production bridge.

TASK-250 does not implement runtime integration.

The gate must fail if `apps/`, `libs/`, or `migrations/` production paths are
changed as part of this task.

## Fixture Boundary

TASK-250 does not modify fixture JSON.

The gate must fail if positive or negative manual entry replay fixture JSON
paths are changed.

## Broker And Network Boundary

TASK-250 does not authorize broker, network, credentials, endpoint, real order
placement, or automatic trading.

Static scans must continue to reject broker and network tokens in bridge gate
source.

## Go / No-Go Decision

GO only if the TASK-245 through TASK-249 bridge gates, TASK-250 phase closeout
gate, AccountingEngine adequacy review gates, dry-run / replay implementation
gates, diagnostics facade, full CTest, transport 50-run repeat, and
`git diff --check` pass.

NO-GO if any production code, AccountingEngine code, fixture JSON, replay,
runtime write, read model refresh, broker, network, credentials, endpoint, real
order placement, or automatic trading boundary is crossed.

## Formal Conclusion

TASK-250 closes out the test-only AccountingEngine bridge phase.

TASK-250 does not call AccountingEngine replay.

TASK-250 does not modify AccountingEngine code.

TASK-250 does not implement production bridge.

TASK-250 does not implement runtime integration.

TASK-250 does not modify fixture JSON.

TASK-250 does not authorize production integration.

TASK-250 does not authorize runtime SQL / SQLite read/write.

TASK-250 does not authorize audit / ledger / snapshot writes.

TASK-250 does not authorize read model refresh.

TASK-250 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-251 ShellAccounting manual entry replay AccountingEngine runtime integration authorization gate.

## Recommended Next Task

TASK-251 ShellAccounting manual entry replay AccountingEngine runtime
integration authorization gate.
