# ShellAccounting Manual Entry Replay Test-Only Dry-Run Harness Implementation Gate

## Purpose

TASK-231 implements the manual entry replay test-only dry-run harness authorized
by TASK-230. The harness is a test-only Python tool that reads authorized manual
entry replay fixture metadata and JSON files, verifies fixture availability, and
prints a sanitized dry-run summary for CTest.

TASK-231 does not implement production parser / loader / reader behavior. It
does not implement replay, call AccountingEngine replay, write runtime SQL,
SQLite, audit, ledger, snapshot, cash, or trade rows, or compute real position,
cash, or PnL values.

## Relationship To TASK-230

TASK-230 fixed the future dry-run harness authorization boundary and kept the
harness absent. TASK-231 is the separate implementation gate for that exact
test-only boundary.

TASK-231 keeps the TASK-230 no-write, no-replay, no-AccountingEngine,
no-production, no-broker, no-network, no-credentials, no-endpoint, no-real-order,
and no-automatic-trading rules in force.

## Test-Only Harness Scope

The implemented harness lives only under:

```text
tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/
```

The CTest name is:

```text
shell_accounting_manual_entry_replay_test_only_dry_run_harness
```

The harness supports:

```text
--source-root
--summary-json
```

It uses Python `json` only for test fixture metadata and fixture JSON reads.
That JSON usage is not a production parser, loader, reader, or runtime data
source.

## Input Boundary

Allowed inputs are limited to:

```text
tests/fixtures/manual_entry_replay/fixtures_index.json
tests/fixtures/manual_entry_replay/*.json
tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json
tests/fixtures/manual_entry_replay_negative/NEG_MRF*.json
```

The harness must not accept production data, UI raw payload, DataService
payload, broker payload, credentials, endpoint configuration, command-line mode
sources, environment mode sources, or runtime database state as input.

## Output Contract

The harness prints sanitized JSON to stdout when `--summary-json` is used. The
summary schema is:

```json
{
  "schemaVersion": "manual-entry-replay-test-only-dry-run-summary/v1",
  "dryRunStatus": "ok",
  "positiveFixtureCount": 0,
  "negativeFixtureCount": 10,
  "entries": [
    {
      "fixtureId": "sanitized-id",
      "source": "positive_fixture_or_negative_scaffold",
      "dryRunStatus": "metadata_checked",
      "plannedStepCount": 0,
      "replayExecuted": false,
      "accountingEngineCalled": false,
      "runtimeWrites": false
    }
  ]
}
```

The output must not contain raw SQL, raw fixture payloads, full manual entry
payloads, ledger rows, audit rows, broker payloads, credentials, endpoints,
internal exception stacks, real broker order ids, or runtime database state.

## No-Write Boundary

TASK-231 is no-write. It does not write fixture JSON, runtime SQL, SQLite, audit
rows, ledger rows, snapshot rows, cash facts, trade facts, generated replay
results, broker status rows, or production state.

The harness verifies fixture hashes before and after the dry-run summary build
and fails if fixture JSON changes.

TASK-231 does not modify fixture JSON.

TASK-231 does not write runtime SQL / SQLite / audit / ledger / snapshot.

## No-Replay Boundary

TASK-231 does not implement replay and does not execute replay. The dry-run
summary uses `plannedStepCount: 0`, `replayExecuted: false`, and
`runtimeWrites: false` for every entry.

Replay execution, replay adapters, replay readers, replay loaders, replay
parsers, and runtime replay integration remain future work requiring separate
authorization.

## AccountingEngine Boundary

TASK-231 does not call AccountingEngine replay and does not use AccountingEngine
output as a hidden runtime substitute. The harness reports
`accountingEngineCalled: false` for each entry.

AccountingEngine replay invocation remains future work requiring separate
authorization.

## Runtime SQL And Ledger Boundary

TASK-231 does not read or write runtime SQL / SQLite. It does not write audit,
ledger, snapshot, manual transaction, manual cash movement, TradeDraft, broker,
cash fact, or trade fact rows.

Any runtime SQL, ledger, audit, snapshot, or replay persistence behavior must be
authorized by a separate future TASK before implementation.

## Broker And Network Boundary

TASK-231 does not connect broker SDKs, broker adapters, network clients,
credentials, endpoints, real broker order ids, real order placement, strategy
execution, or automatic trading.

The harness remains independent of broker and strategy execution.

## Fixture Immutability Boundary

TASK-231 does not modify positive fixture JSON, negative fixture JSON, fixture
indexes, or the TASK-219 positive fixture validator. It also does not change
TASK-224 negative validator validation behavior; any validator changes are
limited to exact-path changed-set self-consistency if required by this task.

## Formal Conclusion And Next Task

TASK-231 implements a test-only dry-run harness. It does not implement
production parser / loader / reader behavior, replay, AccountingEngine replay,
runtime SQL / SQLite writes, audit / ledger / snapshot writes, real position /
cash / PnL calculation, broker, network, credentials, endpoint, real order
placement, or automatic trading.

TASK-231 does not compute real position / cash / PnL.

Recommended next task: TASK-232 manual entry replay test-only dry-run harness
regression matrix gate.
