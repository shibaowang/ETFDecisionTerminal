# ShellAccounting Manual Entry Replay Fixture Scaffold Static Validator

## Static Validator Purpose

TASK-219 implements a test-only static validator for the TASK-217 manual entry
replay fixture scaffold files. The validator checks fixture presence, index
metadata, fixture JSON scaffold flags, placeholder expected-output semantics,
synthetic-only privacy markers, and forbidden-token boundaries.

TASK-219 is static validation only. It does not implement replay, does not call
AccountingEngine replay, does not import production modules, and does not
modify fixture JSON files.

## Validator Input Scope

The validator may inspect only these test-only fixture scaffold files:

- `tests/fixtures/manual_entry_replay/fixtures_index.json`
- `tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json`
- `tests/fixtures/manual_entry_replay/MRF002_single_buy.json`
- `tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json`
- `tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json`
- `tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json`
- `tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json`

The validator must fail closed if the fixture directory is missing, an expected
file is missing, an extra JSON file exists, a DB / SQLite / WAL / SHM file
appears, or runtime parser / loader / reader / replay runner output appears in
the fixture directory.

## Validator Output Scope

The validator emits static CTest diagnostics only. Output is limited to pass /
fail status, sanitized issue code, fixture id, fixture file name, rule id, and
safe short reason.

The validator must not print raw fixture payloads, raw SQL, credentials,
endpoint values, broker payloads, raw trade_log payloads, raw cash_adjustment
payloads, full snapshot payloads, or internal stack traces.

## Index Validation

`fixtures_index.json` must remain a scaffold-only index with:

- `schemaVersion` = `manual-entry-replay-fixtures.scaffold.v1`
- `fixtureSet` = `manual_entry_replay_scaffold`
- `generatedByTask` = `TASK-217`
- `runtimeUse=false`
- `productionUse=false`
- `replayExecuted=false`
- `containsSyntheticDataOnly=true`
- fixture ids exactly `MRF001` through `MRF006`
- unique fixture ids
- exact fixture file references
- `expectedStatus` = `SCAFFOLD_ONLY`
- `privacyStatus` = `SYNTHETIC_ONLY`
- boolean `blockingExpected`

Index entries must not reference production paths, apps, libs, migrations,
runtime databases, broker payloads, credentials, endpoints, or replay outputs.

## Fixture JSON Validation

Each MRF fixture must parse as JSON and retain:

- `schemaVersion` = `manual-entry-replay-fixture.scaffold.v1`
- matching `fixtureId`
- non-empty `title`, `purpose`, and `category`
- `scaffoldOnly=true`
- `runtimeUse=false`
- `productionUse=false`
- `replayExecuted=false`
- `containsSyntheticDataOnly=true`
- `sourceFacts`
- `expectedOutputs`
- `expectedIssues`
- boolean `blockingExpected`
- `privacyExpectations`
- `metadata`

Expected outputs must remain scaffold placeholders such as `SCAFFOLD_ONLY`,
`UNAVAILABLE_EXPECTED`, or `TODO_PLACEHOLDER`. The validator does not require
or compute concrete replay-calculated PnL, market value, position quantity,
cash balance, realized PnL, unrealized PnL, or ledger output.

## Forbidden Token Validation

The validator scans the actual fixture JSON content for forbidden categories,
including raw SQL, `select *`, SQL write statements, credentials, endpoint
values, broker payloads, real order ids, real broker order ids, access tokens,
secret keys, private keys, passwords, SQLite runtime file markers, raw
trade_log payload markers, raw cash_adjustment payload markers, and internal
stack-trace markers.

Failures must report sanitized issue codes only. They must not echo the raw
fixture JSON payload.

## Test-Only Runtime Boundary

The validator is a Python CTest under
`tests/ShellAccountingManualEntryReplayFixtureStaticValidator/`. It may parse
JSON with the Python standard library and inspect the exact test-only fixture
directory. It must not import project production modules, DataServiceApi,
DataAccess, DataServiceClient, ShellServices, AccountingEngine, or SQLite
runtime helpers.

## No-Replay Boundary

TASK-219 does not implement replay. TASK-219 does not call AccountingEngine
replay. TASK-219 does not load runtime facts into replay, refresh read models,
write snapshots, or compute position / cash / PnL / market value.

## Out-of-Scope Boundaries

Out of scope for TASK-219:

- fixture JSON modification
- parser / loader / reader implementation
- replay implementation
- AccountingEngine replay calls
- runtime SQL / SQLite read/write
- audit / ledger / snapshot writes
- backup/export/restore implementation
- production code changes
- production QML / startup / Presenter / Controller / ShellServices changes
- DataServiceActions or DataServiceActionRegistrar changes
- DataAccess repository changes
- migration changes
- broker / network / credentials / endpoint integration
- real order placement
- automatic trading

## Formal Conclusion And Next Task

TASK-219 implements a test-only static validator for replay fixture scaffold files.
TASK-219 does not implement replay.
TASK-219 does not call AccountingEngine replay.
TASK-219 does not import production modules.
TASK-219 does not modify fixture JSON files.
TASK-219 does not authorize runtime SQL / SQLite read/write.
TASK-219 does not authorize audit / ledger / snapshot writes.
TASK-219 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-220 manual entry replay fixture static validator negative fixtures authorization gate.
