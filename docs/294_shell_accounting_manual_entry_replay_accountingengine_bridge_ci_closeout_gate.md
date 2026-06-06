# TASK-249 ShellAccounting manual entry replay AccountingEngine bridge CI closeout gate

## Purpose

TASK-249 adds a CI closeout gate for the test-only ShellAccounting manual entry
replay AccountingEngine bridge phase.

The gate closes the TASK-245 through TASK-248 bridge sequence by verifying that
authorization, test-only implementation, regression matrix, and failure-mode
hardening remain registered, documented, deterministic, and isolated from
production replay behavior.

## Relationship To TASK-245 Through TASK-248

TASK-245 authorized the bridge boundary only.

TASK-246 added the test-only bridge contract shell.

TASK-247 added the bridge regression matrix.

TASK-248 hardened bridge failure modes and sanitized failure output.

TASK-249 does not replace those gates. It verifies that the complete bridge
gate set remains present and that later work has not weakened the no-production,
no-replay, no-write, no-broker, and fixture immutability boundaries.

## CI Closeout Scope

The CI closeout scope is limited to docs, CTest registration, changed-path
allowlist checks, stdout-only bridge CLI probes, and static source scans.

The gate may verify bridge stdout output. It must not write files, run full
CTest recursively, call broker or network code, or invoke AccountingEngine
replay.

## Required Bridge Gate Set

The required bridge gate set is:

- `shell_accounting_manual_entry_replay_accountingengine_bridge_authorization`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_implementation`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout`.

## Required Regression Evidence

TASK-249 must require evidence that TASK-245 through TASK-248 docs and tests are
indexed, registered, and present.

TASK-249 must also require that dry-run harness, replay implementation, and
AccountingEngine adequacy review gates remain registered for cross-phase
regression coverage.

## Changed-Path Allowlist

The changed-path allowlist must use exact paths. It must not allow directory
wildcards such as `docs/`, `tests/`, `apps/`, `libs/`, or `migrations/`.

Clean main may have an empty changed-path set. Feature branches may have changed
paths, but every changed path must be exact-allowlisted and must stay out of
production code, fixture JSON, AccountingEngine code, migrations, QML, broker,
network, credentials, and endpoint paths.

## Clean-Main Invariants

The gate must pass on clean main with no changed paths.

The gate must not require current TASK files to appear in changed paths. File
existence, CTest registration, docs indexing, static boundaries, and bridge
stdout behavior are the clean-main evidence sources.

## No-AccountingEngine-Call Boundary

TASK-249 does not call AccountingEngine replay.

The bridge stdout contract must continue to report `accountingEngineCalled=false`.

Static scans must continue to reject AccountingEngine replay calls, direct
`libs/AccountingEngine/` imports, and production bridge paths.

## No-Replay Boundary

TASK-249 does not implement replay.

The bridge stdout contract must continue to report `replayExecuted=false`.

No production replay parser, loader, reader, bridge, runtime integration, or
AccountingEngine replay entry point is authorized by this task.

## No-Write Boundary

TASK-249 does not authorize runtime SQL / SQLite read/write.

TASK-249 does not authorize audit / ledger / snapshot writes.

The bridge stdout contract must continue to report `runtimeWrite=false`.

## AccountingEngine Code Boundary

TASK-249 does not modify AccountingEngine code.

The gate must fail if `libs/AccountingEngine/` appears in changed paths or if
bridge gate source attempts to import or call AccountingEngine replay APIs.

## Production Integration Boundary

TASK-249 does not implement production bridge or runtime integration.

The gate must fail if `apps/`, `libs/`, or `migrations/` production paths are
changed as part of this task.

## Fixture Boundary

TASK-249 does not modify fixture JSON.

The gate must fail if positive or negative manual entry replay fixture JSON
paths are changed.

## Broker And Network Boundary

TASK-249 does not authorize broker, network, credentials, endpoint, real order
placement, or automatic trading.

Static scans must continue to reject broker and network tokens in bridge gate
source.

## Go / No-Go Decision

GO only if the TASK-245 through TASK-248 bridge gates, TASK-249 CI closeout
gate, dry-run / replay implementation gates, diagnostics facade, full CTest,
transport 50-run repeat, and `git diff --check` pass.

NO-GO if any production code, AccountingEngine code, fixture JSON, replay,
runtime write, broker, network, credentials, endpoint, real order placement, or
automatic trading boundary is crossed.

## Formal Conclusion

TASK-249 adds a CI closeout gate for the test-only AccountingEngine bridge phase.

TASK-249 does not call AccountingEngine replay.

TASK-249 does not modify AccountingEngine code.

TASK-249 does not implement production bridge.

TASK-249 does not implement runtime integration.

TASK-249 does not modify fixture JSON.

TASK-249 does not authorize production integration.

TASK-249 does not authorize runtime SQL / SQLite read/write.

TASK-249 does not authorize audit / ledger / snapshot writes.

TASK-249 does not authorize read model refresh.

TASK-249 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-250 ShellAccounting manual entry replay AccountingEngine bridge phase closeout and handoff gate.

## Recommended Next Task

TASK-250 ShellAccounting manual entry replay AccountingEngine bridge phase
closeout and handoff gate.
