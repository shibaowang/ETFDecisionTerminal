# ShellAccounting Manual Entry Replay Fixture Files Scaffold Authorization Gate

## Scaffold Authorization Purpose

TASK-216 is gate-only / scaffold-authorization-only.
TASK-216 only authorizes future fixture files scaffold scope.
TASK-216 does not create replay fixture files.
TASK-216 does not create replay fixture JSON.
TASK-216 does not create fixture index.
TASK-216 does not create seed DB.
TASK-216 does not create runtime fixture data.
TASK-216 does not create parser / validator.
TASK-216 does not implement replay.
TASK-216 does not call AccountingEngine replay.
TASK-216 does not modify production code.
TASK-216 does not modify DataServiceActions.
TASK-216 does not modify DataServiceActionRegistrar.
TASK-216 does not modify repositories.
TASK-216 does not modify migrations.
TASK-216 does not modify QML / startup / Presenter / Controller / ShellServices adapter / port.
TASK-216 does not add runtime SQL / SQLite read/write.
TASK-216 does not write audit / ledger.
TASK-216 does not write snapshot.
TASK-216 does not implement backup/export/restore.
TASK-216 does not connect broker / network / credentials / endpoint.
TASK-216 does not place real orders.
TASK-216 does not enable automatic trading.
Broker sandbox new capability remains paused.

TASK-216 follows TASK-215 fixture file contract authorization. It narrows the
next step to a future scaffold policy only, so a later TASK may create stable
test-only fixture paths without adding runtime replay, parser, validator,
database, broker, network, or production UI behavior.

## Future Scaffold Directory Boundary

The future scaffold directory candidate is:

```text
tests/fixtures/manual_entry_replay/
```

TASK-216 does not create this directory. A future scaffold implementation must
create it only after separate authorization. The directory may contain only
test fixture scaffold files and must not become a production data source.

Production code, QML, Presenter, Controller, ShellServices, DataServiceActions,
DataServiceActionRegistrar, repositories, migrations, and AccountingEngine
replay must not read from this directory unless a later task explicitly
authorizes a test-only reader or replay path.

## Future Scaffold File Set Boundary

The future scaffold file candidates are:

```text
tests/fixtures/manual_entry_replay/fixtures_index.json
tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json
tests/fixtures/manual_entry_replay/MRF002_single_buy.json
tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json
tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json
tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json
tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json
```

TASK-216 does not create any of these files. Future scaffold file creation must
be a separate TASK. File names must remain stable, test-only, synthetic, and
detached from production runtime paths.

Future scaffold files must not include seed databases, SQLite files, WAL files,
SHM files, runtime output, real account data, real broker data, credentials,
endpoint values, real order ids, or backup/export/restore payloads.

## Future Fixture Index Scaffold Boundary

The future fixture index scaffold must be limited to the following minimum
top-level fields:

```text
schemaVersion
fixtureSet
fixtures
```

Future fixture index entries may use only these candidate fields:

```text
fixtureId
title
category
file
expectedStatus
blockingExpected
privacyStatus
```

TASK-216 does not create `fixtures_index.json`. A future index scaffold must be
static test data only. It must not run replay, read SQLite, write SQLite, call
AccountingEngine replay, or trigger DataServiceActions behavior.

## Future Placeholder Content Boundary

Future scaffold placeholders may use only explicit placeholder tokens until a
later task authorizes concrete synthetic fixture content:

```text
TODO_PLACEHOLDER
SYNTHETIC_ACCOUNT
SYNTHETIC_PORTFOLIO
SYNTHETIC_INSTRUMENT
SYNTHETIC_CURRENCY
SANITIZED_MEMO
UNAVAILABLE_EXPECTED
```

Future placeholders must not impersonate real users, real accounts, real
portfolios, real instruments, real orders, real broker order ids, credentials,
endpoints, secrets, production SQL, raw QML payload, or raw DataService payload.

## Future Fixture JSON Schema Boundary

Future scaffold JSON files may use only these top-level candidate fields:

```text
schemaVersion
fixtureId
title
purpose
category
sourceFacts
expectedOutputs
expectedIssues
blockingExpected
privacyExpectations
metadata
```

TASK-216 does not create replay fixture JSON. It does not create a JSON schema
file, parser, validator, loader, fixture reader, or replay runner. Future JSON
fields must stay synthetic and must remain test-only until a later TASK
authorizes validation or replay integration.

## Future Static Validation Boundary

Future static validation may check file presence, field presence, stable
fixture ids, placeholder-only content, privacy tokens, no seed DB files, no
runtime output files, and no production path dependencies.

TASK-216 does not implement static validation. It only records the future
validation boundary. Future parser or validator implementation must be a
separate TASK and must not imply replay implementation.

## Privacy And Synthetic Data Boundary

Future scaffold files must use synthetic account, portfolio, instrument,
currency, and memo placeholders. They must not include raw SQL, raw payload,
full trade_log payload, full cash_adjustment payload, broker payload, endpoint
values, credentials, secret store values, internal exception stacks, real user
data, real account numbers, real orders, or real broker order ids.

Privacy expectations must be explicit. Placeholder data must be visibly
synthetic. Unsupported expected values must be represented as safe not-ready or
unavailable placeholders, not fabricated PnL, fabricated market value, or fake
ledger output.

## No-Runtime-Dependency Boundary

Future scaffold files must not add runtime dependencies. They must not change
DataServiceActions runtime behavior, DataServiceActionRegistrar registration,
DataAccess repositories, migrations, AccountingEngine replay, production QML,
startup, Presenter, Controller, ShellServices adapter / port, StrategyEngine,
MarketEngine, broker adapter, network code, credentials handling, or endpoint
handling.

Future scaffold files must not perform runtime SQL / SQLite read/write, audit /
ledger writes, snapshot writes, backup/export/restore, broker order placement,
real order id generation, real broker order id generation, strategy execution,
or automatic trading.

## Out-of-Scope Boundaries

TASK-216 does not authorize:

- creation of replay fixture files.
- creation of replay fixture JSON.
- creation of fixture index files.
- creation of seed DB files.
- creation of runtime fixture data.
- fixture parser implementation.
- fixture validator implementation.
- fixture reader implementation.
- replay implementation.
- AccountingEngine replay calls.
- runtime SQL / SQLite read/write.
- production code changes.
- QML / startup / Presenter / Controller / ShellServices production changes.
- DataServiceActions or DataServiceActionRegistrar changes.
- repository changes.
- migration changes.
- audit / ledger writes.
- snapshot writes.
- backup/export/restore implementation.
- broker / network / credentials / endpoint integration.
- real broker order id generation.
- real order placement.
- automatic trading.

## Formal Authorization Conclusion And Next Task

TASK-216 authorizes replay fixture files scaffold policy only.
TASK-216 does not authorize creation of replay fixture files.
TASK-216 does not authorize creation of replay fixture JSON.
TASK-216 does not authorize creation of fixture index files.
TASK-216 does not authorize parser or validator implementation.
TASK-216 does not authorize replay implementation.
TASK-216 does not authorize AccountingEngine replay calls.
TASK-216 does not authorize runtime SQL / SQLite read/write.
TASK-216 does not authorize audit / ledger writes.
TASK-216 does not authorize snapshot writes.
TASK-216 does not authorize backup/export/restore.
TASK-216 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-217 manual entry replay fixture files scaffold.
