# TASK-246 ShellAccounting manual entry replay AccountingEngine bridge test-only implementation gate

## Purpose

TASK-246 implements a test-only AccountingEngine bridge contract shell. The shell converts existing manual entry replay test-only summaries into a sanitized request / response contract for future AccountingEngine adequacy comparison work.

TASK-246 is not a production bridge. It does not call AccountingEngine replay, does not implement replay, does not modify `libs/AccountingEngine/`, does not modify production code, does not modify fixture JSON, and does not authorize runtime integration.

## Relationship To TASK-245

TASK-245 authorized the bridge policy boundary and named the future test-only bridge directory candidate.

TASK-246 uses that authorization to create the test-only bridge directory and script under `tests/`, while preserving the no-replay, no-write, no-production, no-fixture-change, and no-broker boundaries.

## Implementation Scope

TASK-246 may create:

```text
tests/ShellAccountingManualEntryReplayAccountingEngineBridge/
tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/
```

The implementation scope is limited to a test-only bridge contract shell and a static CTest gate for that shell.

## Test-Only Bridge Directory

The test-only bridge directory is:

```text
tests/ShellAccountingManualEntryReplayAccountingEngineBridge/
```

The bridge script is:

```text
tests/ShellAccountingManualEntryReplayAccountingEngineBridge/manual_entry_replay_accountingengine_bridge.py
```

This directory remains test-only. It is not a production parser, loader, reader, runtime replay bridge, or read-model refresh path.

## Bridge Input Contract

Bridge input may come only from existing test-only manual entry replay summary JSON:

- TASK-231 dry-run harness summary.
- TASK-234 test-only implementation summary.

The bridge may invoke those test-only scripts to obtain summaries, or read explicitly provided test-only summary JSON files.

Bridge input must not come from production QML, DataServiceActions, DataAccess repositories, SQLite, broker payloads, network responses, credentials, endpoints, raw UI payloads, or fixture JSON mutation.

## Bridge Output Contract

The bridge output is a sanitized JSON contract with:

- `schemaVersion`.
- `task`.
- `mode`.
- `accountingEngineCalled=false`.
- `replayExecuted=false`.
- `runtimeWrite=false`.
- `bridgeRequest`.
- `bridgeResponse`.
- `issues`.

The bridge request marks `source=manual-entry-replay-test-only` and `syntheticDataOnly=true`.

The bridge response remains a placeholder with `status=not_executed`.

## Sanitized Failure Contract

Failures must fail closed and return sanitized output when a summary output path is supplied.

Failure output must not expose raw fixture payloads, raw SQL, internal exception stacks, credentials, endpoint values, broker payloads, real order ids, or production paths.

## AccountingEngine Call Boundary

TASK-246 does not call AccountingEngine replay.

The bridge script must set `accountingEngineCalled=false` and must not import `libs/AccountingEngine`, call `AccountingReplayEngine`, call `replayReadOnly`, or execute any AccountingEngine bridge.

## Replay Boundary

TASK-246 does not implement replay.

The bridge only prepares a future request / response contract and marks replay as not executed.

## Runtime Isolation Boundary

TASK-246 is isolated from runtime behavior. It does not modify production startup, QML, Presenter, Controller, ShellServices, DataServiceActions, DataServiceActionRegistrar, DataAccess repositories, migrations, AccountingEngine, StrategyEngine, or MarketEngine.

## No-Write Boundary

TASK-246 does not authorize runtime SQL / SQLite read/write.

TASK-246 does not authorize audit / ledger / snapshot writes.

TASK-246 does not authorize read model refresh.

The bridge and gate may write only temporary summary files under Python-created temporary directories or output JSON to stdout.

## AccountingEngine Code Boundary

TASK-246 does not modify AccountingEngine code.

`libs/AccountingEngine/` must remain unchanged. Any AccountingEngine code change or replay invocation requires a separate task.

## Production Integration Boundary

TASK-246 does not authorize production integration.

Future production parser, loader, reader, runtime replay, read model refresh, ShellServices binding, QML binding, DataService action integration, or AccountingEngine replay call must be separately authorized.

## Fixture Boundary

TASK-246 does not modify fixture JSON.

Positive and negative manual entry replay fixtures remain immutable.

## Broker And Network Boundary

TASK-246 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

The bridge must not introduce broker SDK references, network clients, credentials, endpoints, real broker order ids, order placement, strategy execution, or automatic trading.

## Go / No-Go Decision

Go means docs, indexes, CTest registration, test-only bridge CLI behavior, sanitized output, changed-path allowlists, prior gates, fixture immutability, no AccountingEngine code drift, no AccountingEngine replay call, no runtime write, no read model refresh, and no broker/network drift all pass.

No-Go means any production bridge, AccountingEngine replay call, replay implementation, AccountingEngine code drift, fixture JSON drift, production code drift, runtime write, read model refresh, broker/network drift, missing docs, missing CTest registration, unsanitized output, or unauthorized changed path.

## Formal Conclusion

TASK-246 implements a test-only AccountingEngine bridge contract shell.

TASK-246 does not call AccountingEngine replay.

TASK-246 does not modify AccountingEngine code.

TASK-246 does not implement production bridge.

TASK-246 does not modify fixture JSON.

TASK-246 does not authorize production integration.

TASK-246 does not authorize runtime SQL / SQLite read/write.

TASK-246 does not authorize audit / ledger / snapshot writes.

TASK-246 does not authorize read model refresh.

TASK-246 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-247 ShellAccounting manual entry replay AccountingEngine bridge regression matrix gate.

## Recommended Next Task

Recommended next task: TASK-247 ShellAccounting manual entry replay AccountingEngine bridge regression matrix gate.

TASK-247 must separately authorize regression coverage for the test-only bridge contract shell before any AccountingEngine replay call or production integration is introduced.
