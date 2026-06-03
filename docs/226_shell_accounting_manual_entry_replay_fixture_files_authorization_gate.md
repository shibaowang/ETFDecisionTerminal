# ShellAccounting Manual Entry Replay Fixture Files Authorization Gate

## Fixture Files Authorization Purpose

TASK-215 is gate-only / fixture-files-authorization-only.
TASK-215 authorizes replay fixture files contract only.
TASK-215 does not create replay fixture files.
TASK-215 does not create replay fixture JSON.
TASK-215 does not create seed DB.
TASK-215 does not create runtime fixture data.
TASK-215 does not implement replay.
TASK-215 does not call AccountingEngine replay.
TASK-215 does not modify production code.
TASK-215 does not modify DataServiceActions.
TASK-215 does not modify DataServiceActionRegistrar.
TASK-215 does not modify repositories.
TASK-215 does not modify migrations.
TASK-215 does not modify QML / startup / Presenter / Controller / ShellServices adapter / port.
TASK-215 does not add runtime SQL / SQLite read/write.
TASK-215 does not write audit / ledger.
TASK-215 does not write snapshot.
TASK-215 does not implement backup/export/restore.
TASK-215 does not connect broker / network / credentials / endpoint.
TASK-215 does not place real orders.
TASK-215 does not enable automatic trading.
Broker sandbox new capability remains paused.

TASK-215 inherits the TASK-214 fixture matrix and turns it into a future file
contract. The future replay fixture files may be used only as test input and
expected-output acceptance material for a separately authorized replay
implementation. They must not become a production runtime data source.

## Future Fixture Directory Policy

The future fixture directory candidate is:

```text
tests/fixtures/manual_entry_replay/
```

TASK-215 does not create this directory. Future creation of this directory must
be separately authorized. The directory may be used only for test fixtures and
must not be read by production code.

The future directory must not contain credentials, endpoint values, broker
payloads, real account data, real order data, SQLite DB files, WAL files, SHM
files, or runtime output.

## Future Fixture File Naming Policy

Future fixture file candidates are:

```text
MRF001_empty_manual_facts.json
MRF002_single_buy.json
MRF003_buy_deposit_baseline.json
MRF004_buy_sell_partial_reduction.json
MRF005_deposit_withdrawal_baseline.json
MRF006_daily_use_combined_baseline.json
```

TASK-215 does not create these files. Future fixture ids must be stable and
must not reuse meanings after a case is assigned. File names must use lowercase
snake_case after the stable `MRFnnn` prefix.

Future file names must not contain real account numbers, real names, broker
names, endpoint values, credentials, real order ids, or any production secret.
Fixture files must not depend on QML display order. Fixture files must not
depend on test execution order.

## Future Fixture Index Policy

The future fixture index candidate is:

```text
tests/fixtures/manual_entry_replay/fixtures_index.json
```

TASK-215 does not create this index file. A future index must list fixture id,
title, category, source file, expected output file or inline expected block,
privacy status, and blocking status.

The future index must validate unique fixture ids and stable file names. The
future index must not contain real account numbers, credentials, endpoint
values, broker payloads, or real order ids.

## Future Fixture JSON Schema Policy

The future fixture JSON schema contract must include these top-level fields:

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

The future `sourceFacts` candidate fields are:

```text
accounts
portfolios
instruments
tradeExecutionGroups
tradeLogRows
cashAdjustmentRows
requestTrace
```

The future `expectedOutputs` candidate fields are:

```text
positions
cashSummary
portfolioPnlSummary
unavailableFields
calculationMetadata
```

The future `expectedIssues` candidate fields are:

```text
code
severity
messageToken
blocking
sourceRef
```

The schema is a future contract only. TASK-215 does not create a schema file,
does not implement a parser, does not implement a validator, does not execute
replay, and does not write SQLite.

## Future Fixture Content Policy

Future source facts may simulate persisted manual facts candidates only.
Manual transaction facts may come from `trade_execution_group` plus
`trade_log`. Manual cash movement facts may come from `trade_log` plus
`cash_adjustment`.

Future fixture content must not include raw QML payload, DataService raw request
body, broker payload, real order id, credentials, endpoint, raw SQL, real user
data, or backup/export payload. Account, portfolio, instrument, currency, and
memo values must be synthetic and sanitized.

## Future Fixture Case Coverage Policy

Future fixture files must cover the TASK-214 fixture matrix:

- baseline success fixtures.
- ordering and idempotency fixtures.
- position safety fixtures.
- cash safety fixtures.
- PnL and market dependency fixtures.
- multi-scope fixtures.
- unsupported and failure fixtures.

The future file candidates `MRF001` through `MRF006` must be referenced by a
future fixture creation task, but TASK-215 must not create them.

## Privacy And Redaction Policy

Future fixture files and expected issues must not expose raw SQL, raw payload,
credentials, endpoint values, broker payload, internal exception stack, full
`trade_log` payload, full `cash_adjustment` payload, real account numbers, real
names, real orders, or real broker order ids.

Only synthetic account / portfolio / instrument identifiers, sanitized issue
code, sanitized memo, and safe metadata are allowed.

## Validation Tool Boundary

TASK-215 does not create a validator. TASK-215 does not create a parser.
TASK-215 does not read JSON fixtures. TASK-215 does not run replay.

TASK-215 CTest may perform only documentation token checks, changed-path
allowlist checks, untracked-file detection, and no-production-drift static
checks. Future validator work and future fixture file creation must be
separately authorized.

## Repository And Runtime Boundary

Future fixture files must not be read by production code, QML, Presenter,
Controller, or ShellServices. Future fixture files must not change
DataServiceActions runtime behavior, repositories, migrations, or AccountingEngine
replay behavior.

Future fixture files must not trigger runtime SQL / SQLite read/write, audit /
ledger writes, snapshot writes, backup/export/restore, broker integration,
network access, credentials use, endpoint access, real order placement, or
automatic trading.

## Out-of-Scope Boundaries

TASK-215 does not authorize:

- creation of replay fixture files.
- creation of replay fixture JSON.
- creation of seed DB files.
- creation of runtime fixture data.
- fixture parser implementation.
- fixture validator implementation.
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

TASK-215 authorizes replay fixture files contract only.
TASK-215 does not authorize creation of replay fixture files.
TASK-215 does not authorize replay implementation.
TASK-215 does not authorize AccountingEngine replay calls.
TASK-215 does not authorize runtime SQL / SQLite read/write.
TASK-215 does not authorize audit / ledger writes.
TASK-215 does not authorize snapshot writes.
TASK-215 does not authorize backup/export/restore.
TASK-215 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-216 manual entry replay fixture files scaffold authorization gate.
