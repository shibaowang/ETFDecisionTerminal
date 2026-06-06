# TASK-247 ShellAccounting manual entry replay AccountingEngine bridge regression matrix gate

## Purpose

TASK-247 adds a test-only regression matrix gate for the TASK-246 manual entry replay AccountingEngine bridge contract shell.

The gate verifies the bridge CLI contract, sanitized output, failure behavior, and no-write / no-replay / no-production boundaries before any future AccountingEngine replay integration is authorized.

## Relationship To TASK-246

TASK-246 introduced the test-only bridge contract shell under `tests/ShellAccountingManualEntryReplayAccountingEngineBridge`.

TASK-247 does not change the TASK-246 successful bridge semantics. It adds a regression matrix around that contract and allows only test-only CLI failure support needed to prove the contract fails closed.

## Regression Matrix Scope

The regression matrix covers positive bridge output, malformed or unsafe input summaries, sanitized failure output, exact changed-path boundaries, and static no-production checks.

The matrix is test-only and does not execute AccountingEngine replay.

## Bridge CLI Contract

The bridge CLI must continue to support:

- `--dry-run-summary-json`
- `--implementation-summary-json`
- `--summary-json`
- `--expect-no-accountingengine-call`

The CLI may write a sanitized summary to stdout or to the requested summary file.

## Positive Matrix

The positive matrix requires the bridge to emit:

- `schemaVersion=1`
- `task=TASK-246`
- `mode=test-only-accountingengine-bridge-contract`
- `accountingEngineCalled=false`
- `replayExecuted=false`
- `runtimeWrite=false`
- `bridgeRequest.syntheticDataOnly=true`
- `bridgeResponse.status=not_executed`

The bridge request and response must be synthetic and sanitized.

## Failure Matrix

The failure matrix covers:

- missing input summary
- malformed JSON
- missing required fields
- non-synthetic flag
- `replayExecuted=true`
- `accountingEngineCalled=true`
- `runtimeWrite=true`

Each failure must fail closed and return sanitized output.

## Sanitized Output Matrix

Bridge output must not expose raw fixture payloads, raw manual entry payloads, raw SQL, SQLite details, exception stacks, credentials, endpoints, broker payloads, real order identifiers, `apps/`, `libs/`, or `migrations/` paths.

Failure output must not echo malformed payload content.

## No-AccountingEngine-Call Boundary

TASK-247 does not call AccountingEngine replay.

The bridge summary must keep `accountingEngineCalled=false`, and source scans must continue to reject AccountingEngine replay calls.

## No-Replay Boundary

TASK-247 does not implement replay, does not call `replayReadOnly`, and does not run `AccountingReplayEngine`.

The bridge response remains `not_executed`.

## No-Write Boundary

TASK-247 does not authorize runtime SQL / SQLite read/write, audit writes, ledger writes, snapshot writes, or read model refresh.

The gate must preserve fixture hashes and must not create database, WAL, SHM, or SQLite files.

## AccountingEngine Code Boundary

TASK-247 does not modify `libs/AccountingEngine/`.

Any future AccountingEngine code change must be separately authorized.

## Production Integration Boundary

TASK-247 does not create a production bridge and does not modify production code, QML, startup, DataServiceActions, repositories, or migrations.

## Fixture Boundary

TASK-247 does not modify positive or negative fixture JSON.

The regression matrix uses temporary synthetic summary files only.

## Broker And Network Boundary

TASK-247 does not authorize broker SDKs, network calls, credentials, endpoints, real order placement, or automatic trading.

## Go / No-Go Decision

Go only if the regression matrix proves:

- bridge CLI contract remains stable
- positive output is sanitized
- failure output is sanitized
- unsafe input fails closed
- no AccountingEngine replay is called
- no runtime writes occur
- no production paths change
- fixture JSON remains unchanged

No-go if any production integration, replay execution, runtime write, fixture mutation, or broker/network capability appears.

## Formal Conclusion

TASK-247 adds a test-only regression matrix gate for the AccountingEngine bridge contract.

TASK-247 does not call AccountingEngine replay.

TASK-247 does not modify AccountingEngine code.

TASK-247 does not implement production bridge.

TASK-247 does not modify fixture JSON.

TASK-247 does not authorize production integration.

TASK-247 does not authorize runtime SQL / SQLite read/write.

TASK-247 does not authorize audit / ledger / snapshot writes.

TASK-247 does not authorize read model refresh.

TASK-247 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-248 ShellAccounting manual entry replay AccountingEngine bridge failure-mode hardening gate.

## Recommended Next Task

TASK-248 ShellAccounting manual entry replay AccountingEngine bridge failure-mode hardening gate.
