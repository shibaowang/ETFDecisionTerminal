# TASK-248 ShellAccounting manual entry replay AccountingEngine bridge failure-mode hardening gate

## Purpose

TASK-248 records the failure-mode hardening boundary for the test-only ShellAccounting manual entry replay AccountingEngine bridge contract. The task hardens static and CLI probes around malformed inputs, fail-closed behavior, sanitized failure output, and exact changed-path policy.

## Relationship To TASK-246 And TASK-247

TASK-246 created the test-only AccountingEngine bridge contract shell. TASK-247 added the bridge regression matrix gate. TASK-248 builds on those tasks by hardening failure behavior while keeping the bridge test-only and non-executing.

## Failure-Mode Hardening Scope

The scope is limited to the test-only bridge script and a failure-mode-hardening gate. The bridge may classify missing files, malformed JSON, missing required fields, non-synthetic input, forbidden flags, invalid output paths, and forbidden payload tokens with stable sanitized issue codes.

## Bridge CLI Failure Contract

The bridge CLI keeps `--dry-run-summary-json`, `--implementation-summary-json`, `--summary-json`, and `--expect-no-accountingengine-call`. TASK-248 allows a test-only `--failure-json` destination for sanitized failure output written only to caller-provided temporary paths.

## Sanitized Failure Output

Failure output must not echo raw input summaries, raw payload fragments, SQL, credentials, broker payloads, endpoints, filesystem production paths, or exception stacks. Failures must include stable issue codes and safe reasons.

## Fail-Closed Matrix

The bridge must fail closed for:

- missing input summary
- malformed JSON
- missing required field
- `syntheticDataOnly=false`
- `accountingEngineCalled=true`
- `replayExecuted=true`
- `runtimeWrite=true` or equivalent runtime write flags
- invalid output path
- forbidden token placeholder in input or output

## No-AccountingEngine-Call Boundary

TASK-248 does not call AccountingEngine replay. The bridge must keep `accountingEngineCalled=false` on success and sanitized failure paths.

## No-Replay Boundary

TASK-248 does not call replay, `replayReadOnly`, a production replay bridge, or an AccountingEngine replay adapter. Any input claiming replay already ran must fail closed.

## No-Write Boundary

TASK-248 does not write runtime SQL, SQLite, audit, ledger, snapshot, trade, cash, or read-model rows. The only allowed writes are caller-specified temporary test output files.

## AccountingEngine Code Boundary

TASK-248 does not modify `libs/AccountingEngine/`, does not import it, and does not add a production bridge into AccountingEngine code.

## Production Integration Boundary

TASK-248 does not modify production code, QML, startup, Presenter, Controller, DataServiceActions, repositories, migrations, or production service wiring.

## Fixture Boundary

TASK-248 does not modify positive or negative replay fixture JSON files. Gate probes must verify fixture hashes remain unchanged.

## Broker And Network Boundary

TASK-248 does not add broker, network, credentials, endpoint, real order placement, or automatic trading capability.

## Go / No-Go Decision

Go only if the failure-mode gate, regression matrix, implementation gate, authorization gate, prior replay gates, full CTest, transport repeat, and `git diff --check` pass with no unauthorized changed paths.

## Formal Conclusion

TASK-248 hardens test-only AccountingEngine bridge failure modes.
TASK-248 does not call AccountingEngine replay.
TASK-248 does not modify AccountingEngine code.
TASK-248 does not implement production bridge.
TASK-248 does not modify fixture JSON.
TASK-248 does not authorize production integration.
TASK-248 does not authorize runtime SQL / SQLite read/write.
TASK-248 does not authorize audit / ledger / snapshot writes.
TASK-248 does not authorize read model refresh.
TASK-248 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-249 ShellAccounting manual entry replay AccountingEngine bridge CI closeout gate.

## Recommended Next Task

Recommended next task: TASK-249 ShellAccounting manual entry replay AccountingEngine bridge CI closeout gate.
