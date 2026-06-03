# ShellAccounting Manual Entry Replay Fixture Files Scaffold

## Scaffold Purpose

TASK-217 creates test-only replay fixture scaffold files.
The files are only for future test design.
TASK-217 does not implement replay.
TASK-217 does not create parser / validator code.
TASK-217 does not call AccountingEngine replay.
TASK-217 does not modify production code.
TASK-217 does not modify DataServiceActions.
TASK-217 does not modify repositories.
TASK-217 does not modify migrations.
TASK-217 does not modify QML / startup / Presenter / Controller / ShellServices.
TASK-217 does not add runtime SQL / SQLite read/write.
TASK-217 does not write audit / ledger / snapshot rows.
TASK-217 does not connect broker / network / credentials / endpoint.
TASK-217 does not place real orders.
TASK-217 does not enable automatic trading.
Broker sandbox new capability remains paused.

TASK-217 follows TASK-216 by creating the documented scaffold files under the
test fixture directory only. The files are static JSON placeholders for future
manual entry replay fixture design. They are not runtime inputs and are not
loaded by any production path.

## Scaffold Files Created

TASK-217 creates these test-only scaffold files:

```text
tests/fixtures/manual_entry_replay/fixtures_index.json
tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json
tests/fixtures/manual_entry_replay/MRF002_single_buy.json
tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json
tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json
tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json
tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json
```

Each file is marked scaffold-only, runtimeUse=false, productionUse=false,
replayExecuted=false, and synthetic-only. The files are not seed databases,
runtime fixture data, replay output, backup/export/restore payloads, or
production configuration.

## Scaffold Index Contract

`fixtures_index.json` uses this static scaffold contract:

```text
schemaVersion = manual-entry-replay-fixtures.scaffold.v1
fixtureSet = manual_entry_replay_scaffold
generatedByTask = TASK-217
runtimeUse = false
productionUse = false
replayExecuted = false
containsSyntheticDataOnly = true
fixtures = MRF001 through MRF006
```

Each index entry records `fixtureId`, `title`, `category`, `file`,
`expectedStatus`, `blockingExpected`, and `privacyStatus`. The expected status
is `SCAFFOLD_ONLY`, blockingExpected is false, and privacyStatus is
`SYNTHETIC_ONLY`.

## Scaffold Fixture Contract

Each MRF fixture uses this static scaffold contract:

```text
schemaVersion
fixtureId
title
purpose
category
scaffoldOnly
runtimeUse
productionUse
replayExecuted
containsSyntheticDataOnly
sourceFacts
expectedOutputs
expectedIssues
blockingExpected
privacyExpectations
metadata
```

The fixture schemaVersion is `manual-entry-replay-fixture.scaffold.v1`.
`scaffoldOnly` and `containsSyntheticDataOnly` are true. `runtimeUse`,
`productionUse`, and `replayExecuted` are false. Expected values remain
placeholder-only until a later validator or replay implementation task
authorizes concrete synthetic expectations.

## Fixture Case Summary

| Fixture | Scenario | Scaffold purpose |
| --- | --- | --- |
| MRF001 | Empty manual facts | Empty or unavailable-safe output scaffold. |
| MRF002 | Single BUY | Position quantity increase and cash outflow candidate scaffold. |
| MRF003 | BUY + Deposit | Position and cash baseline scaffold. |
| MRF004 | BUY + SELL partial reduction | Reduced position candidate scaffold. |
| MRF005 | Deposit + Withdrawal | Cash movement baseline scaffold. |
| MRF006 | BUY + SELL + Deposit + Withdrawal | Combined daily-use baseline scaffold. |

The scaffold does not assert replay-calculated PnL, market value, realized
cash ledger rows, or AccountingEngine output. Unsupported values remain
`UNAVAILABLE_EXPECTED` or `TODO_PLACEHOLDER`.

## Privacy And Synthetic Data Guarantee

The fixture files contain only synthetic placeholders. Account, portfolio,
instrument, currency, memo, request id, transaction id, and movement id values
use `SYNTHETIC_*`, `SANITIZED_MEMO`, `TODO_PLACEHOLDER`, or
`UNAVAILABLE_EXPECTED`.

The scaffold must not contain raw SQL, raw UI payload, raw DataService payload,
full trade_log payload, full cash_adjustment payload, broker payload, endpoint
values, credentials, secret store values, internal exception stacks, real user
data, real account numbers, real broker order ids, or real order ids.

## Runtime Isolation

TASK-217 does not add parser, validator, loader, fixture reader, or replay
runner code. No production code reads the fixture directory. No DataService
action, repository, AccountingEngine replay path, QML path, Presenter,
Controller, ShellServices adapter, broker adapter, network code, credentials
path, endpoint path, backup/export/restore path, or automatic trading path is
connected to these files.

## Test-Only Boundary

The only runtime effect of TASK-217 is a static CTest gate that parses the
JSON scaffold files and checks their test-only flags, synthetic-only privacy
markers, stable index references, and changed-path boundaries. The gate does
not run replay, read SQLite, write SQLite, write audit, write ledger, write
snapshot, create backup/export/restore output, connect broker, call network,
read credentials, place real orders, or enable automatic trading.

## Out-of-Scope Boundaries

TASK-217 does not authorize:

- replay implementation.
- parser / validator / loader / fixture reader implementation.
- AccountingEngine replay calls.
- runtime SQL / SQLite read/write.
- production code changes.
- production QML / startup / Presenter / Controller / ShellServices changes.
- DataServiceActions or DataServiceActionRegistrar changes.
- repository changes.
- migration changes.
- audit / ledger writes.
- snapshot writes.
- backup/export/restore implementation.
- broker / network / credentials / endpoint integration.
- real order placement.
- automatic trading.

## Formal Conclusion And Next Task

TASK-217 creates test-only replay fixture scaffold files.
TASK-217 does not implement replay.
TASK-217 does not create parser or validator.
TASK-217 does not call AccountingEngine replay.
TASK-217 does not authorize runtime SQL / SQLite read/write.
TASK-217 does not modify production code.
TASK-217 does not authorize audit / ledger / snapshot writes.
TASK-217 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-218 manual entry replay fixture scaffold static validator authorization gate.
