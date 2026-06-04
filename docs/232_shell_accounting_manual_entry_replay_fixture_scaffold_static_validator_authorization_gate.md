# ShellAccounting Manual Entry Replay Fixture Scaffold Static Validator Authorization Gate

## Static Validator Authorization Purpose

TASK-218 is gate-only / static-validator-authorization-only. It authorizes the
policy for a future test-only static validator over the TASK-217 manual entry
replay fixture scaffold files.

TASK-218 authorizes future validator input boundary, output boundary, check
matrix, fixture index rules, fixture JSON rules, privacy rules, no-replay
boundary, no-runtime-dependency boundary, and Go / No-Go conditions.

TASK-218 does not create parser implementation. TASK-218 does not create
validator implementation. TASK-218 does not create loader implementation.
TASK-218 does not create reader implementation. TASK-218 does not implement
replay. TASK-218 does not call AccountingEngine replay. TASK-218 does not
modify production code. TASK-218 does not modify DataServiceActions. TASK-218
does not modify repositories. TASK-218 does not modify migrations. TASK-218
does not modify QML / startup / Presenter / Controller / ShellServices.
TASK-218 does not add runtime SQL / SQLite read/write. TASK-218 does not write
audit / ledger / snapshot rows. TASK-218 does not connect broker / network /
credentials / endpoint. TASK-218 does not place real orders. TASK-218 does not
enable automatic trading. Broker sandbox new capability remains paused.

## Future Validator Input Boundary

The future static validator may inspect only test-only scaffold files created
by TASK-217:

- `tests/fixtures/manual_entry_replay/fixtures_index.json`
- `tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json`
- `tests/fixtures/manual_entry_replay/MRF002_single_buy.json`
- `tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json`
- `tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json`
- `tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json`
- `tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json`

The future static validator must not read production databases, runtime SQLite
files, WAL files, SHM files, seed DB files, broker payloads, credentials,
endpoints, QML payloads, DataService runtime payloads, or internal exception
stacks.

The future static validator must not accept UI raw payload as fixture facts.
The future static validator must not read trade_log, cash_adjustment, audit,
ledger, snapshot, or broker order runtime rows.

## Future Validator Output Boundary

The future static validator output must be static test diagnostics only.
Allowed output is limited to:

- pass / fail status for each scaffold file
- sanitized issue code
- fixture id
- fixture file name
- rule id
- safe short reason
- no raw SQL
- no full trade_log payload
- no full cash_adjustment payload
- no full fixture payload
- no broker payload
- no credentials
- no endpoint values
- no internal stack trace

The future static validator must not write output to production stores, audit
tables, ledger tables, snapshots, backup/export/restore artifacts, broker
systems, or network endpoints.

## Future Validator Check Matrix

The future static validator must check at least:

- docs/230 scaffold contract remains present.
- docs/231 scaffold test plan remains present.
- `fixtures_index.json` exists.
- MRF001 through MRF006 exist.
- JSON parse succeeds for index and all fixtures.
- Index schemaVersion is `manual-entry-replay-fixtures.scaffold.v1`.
- Fixture schemaVersion is `manual-entry-replay-fixture.scaffold.v1`.
- fixtureSet is `manual_entry_replay_scaffold`.
- fixture ids are unique.
- fixture ids are exactly MRF001 through MRF006.
- index file references match exact fixture file names.
- every fixture has `scaffoldOnly=true`.
- every fixture has `runtimeUse=false`.
- every fixture has `productionUse=false`.
- every fixture has `replayExecuted=false`.
- every fixture has `containsSyntheticDataOnly=true`.
- every fixture contains `SYNTHETIC_*` placeholder values only.
- every fixture contains sanitized memo markers.
- every fixture contains placeholder expected-output markers.
- no DB, SQLite, WAL, SHM, or seed data file is present.
- no parser / validator / loader / reader implementation is present.
- no replay runner is present.
- no AccountingEngine replay call is introduced.
- no production code drift is introduced.
- no broker / network / credentials / endpoint token is introduced.
- no real order placement or automatic trading token is introduced.

## Fixture Index Validation Rules

The future static validator must treat `fixtures_index.json` as a scaffold
index only. It must require:

- `schemaVersion`
- `fixtureSet`
- `generatedByTask`
- `scaffoldOnly`
- `runtimeUse`
- `productionUse`
- `replayExecuted`
- `fixtures`

The index must declare TASK-217 as source task. The index must mark scaffold
state only. The index must not declare runtime fixture use. The index must not
declare replay execution. The index must not include broker order ids,
credentials, endpoints, raw SQL, raw UI payload, raw DataService payload,
production database paths, or sensitive payloads.

## Fixture JSON Validation Rules

The future static validator must require every MRF fixture JSON to contain the
TASK-217 scaffold fields:

- `schemaVersion`
- `fixtureId`
- `title`
- `purpose`
- `category`
- `scaffoldOnly`
- `runtimeUse`
- `productionUse`
- `replayExecuted`
- `containsSyntheticDataOnly`
- `sourceFacts`
- `expectedOutputs`
- `expectedIssues`
- `blockingExpected`
- `privacyExpectations`
- `metadata`

The future static validator must require `scaffoldOnly=true`,
`runtimeUse=false`, `productionUse=false`, `replayExecuted=false`, and
`containsSyntheticDataOnly=true` for every fixture. It must reject fixture files
that contain real account numbers, real portfolio identifiers, real instrument
positions, raw SQL, raw DataService payloads, raw UI payloads, broker payloads,
credentials, endpoints, real order ids, or real broker order ids.

## Privacy And Forbidden Token Rules

The future static validator must preserve synthetic-only privacy. Allowed
fixture values must use placeholder markers such as `SYNTHETIC_*`,
`SANITIZED_MEMO`, `TODO_PLACEHOLDER`, and `UNAVAILABLE_EXPECTED`.

Forbidden token categories include:

- raw SQL
- raw UI payload
- raw DataService payload
- full trade_log payload
- full cash_adjustment payload
- full snapshot payload
- broker payload
- credentials
- endpoint values
- access token
- secret key
- private key
- real account id
- real order id
- real broker order id
- internal stack trace
- production database path

The future static validator must return sanitized issue codes only. It must
not echo full fixture payloads.

## No-Replay Boundary

TASK-218 does not authorize replay implementation. TASK-218 does not authorize
AccountingEngine replay calls. TASK-218 does not authorize a replay runner.
TASK-218 does not authorize runtime fixture loading. TASK-218 does not
authorize manual entry facts to enter replay.

The future static validator may inspect JSON as text and parse JSON only for
static test validation. It must not execute any replay logic, call
AccountingEngine, produce replay output, write replay snapshots, or refresh
read models.

## No-Runtime-Dependency Boundary

TASK-218 does not authorize runtime SQL / SQLite read/write. TASK-218 does not
authorize DataServiceActions changes. TASK-218 does not authorize repository
changes. TASK-218 does not authorize migration changes. TASK-218 does not
authorize production code changes.

The future static validator must remain test-only. It must not depend on
DataServiceClient, DataServiceActions, DataAccess repositories, production
QML, startup, Presenter, Controller, ShellServices, AccountingEngine replay,
broker SDKs, network clients, credentials, endpoints, or environment secrets.

## Out-of-Scope Boundaries

Out of scope for TASK-218:

- parser implementation
- validator implementation
- loader implementation
- reader implementation
- replay implementation
- AccountingEngine replay calls
- runtime SQL / SQLite read/write
- fixture JSON content modification
- production code modification
- QML / startup / Presenter / Controller / ShellServices modification
- DataServiceActions modification
- DataServiceActionRegistrar modification
- repository modification
- migration modification
- audit / ledger writes
- snapshot writes
- read model refresh
- backup/export/restore implementation
- broker SDK integration
- network integration
- credentials integration
- endpoint integration
- real broker order id generation
- real order placement
- automatic trading

## Formal Authorization Conclusion And Next Task

TASK-218 authorizes replay fixture scaffold static validator policy only.
TASK-218 does not authorize parser implementation.
TASK-218 does not authorize validator implementation.
TASK-218 does not authorize loader or reader implementation.
TASK-218 does not authorize replay implementation.
TASK-218 does not authorize AccountingEngine replay calls.
TASK-218 does not authorize runtime SQL / SQLite read/write.
TASK-218 does not authorize audit / ledger writes.
TASK-218 does not authorize snapshot writes.
TASK-218 does not authorize backup/export/restore.
TASK-218 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-219 manual entry replay fixture scaffold static validator.
