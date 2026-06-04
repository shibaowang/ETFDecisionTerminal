# ShellAccounting Manual Entry Replay Negative Fixture Scaffold Files Gate

## Purpose

TASK-222 creates test-only negative fixture scaffold JSON files for the manual
entry replay fixture static validator path. The files document future
fail-closed validator scenarios without becoming replay input, production seed
data, runtime data, database input, or broker/order input.

TASK-222 creates test-only negative fixture scaffold JSON files. TASK-222 does
not implement negative validator. TASK-222 does not implement parser, loader,
reader. TASK-222 does not implement replay. TASK-222 does not call
AccountingEngine replay.

## Relationship To TASK-220 And TASK-221

TASK-220 authorized negative fixture policy for the static validator. TASK-221
authorized the future negative fixture scaffold boundary without creating the
directory or files. TASK-222 is the narrow scaffold-file step after those gates:
it creates the negative fixture directory, one index, and the controlled
`NEG_MRF001` through `NEG_MRF010` JSON scaffold set.

TASK-222 does not modify the TASK-219 static validator. TASK-222 does not
modify positive fixture JSON. TASK-222 does not implement validation of these
negative fixtures. Future validation must be a separate authorization task.

## Created Test-Only Scaffold Files

TASK-222 creates this test-only directory:

```text
tests/fixtures/manual_entry_replay_negative/
```

The directory contains exactly these JSON files:

```text
negative_fixtures_index.json
NEG_MRF001_missing_required_field.json
NEG_MRF002_wrong_schema_version.json
NEG_MRF003_runtime_use_true.json
NEG_MRF004_production_use_true.json
NEG_MRF005_replay_executed_true.json
NEG_MRF006_non_synthetic_privacy.json
NEG_MRF007_extra_json_file.json
NEG_MRF008_forbidden_token.sql.json
NEG_MRF009_broker_payload_token.json
NEG_MRF010_real_order_id_token.json
```

No other JSON, database, WAL, SHM, SQLite, runtime output, parser, loader,
reader, replay output, broker output, or production seed file is authorized in
that directory by TASK-222.

## Negative Fixture Index Contract

`negative_fixtures_index.json` is test-only scaffold metadata. It must declare:

- `schemaVersion` as `manual-entry-replay-negative-fixtures-index/v1`.
- `scaffoldOnly=true`.
- `runtimeUse=false`.
- `productionUse=false`.
- `replayExecuted=false`.
- `containsSyntheticDataOnly=true`.
- Exactly ten referenced `NEG_MRF*.json` files.
- References only to files inside the TASK-222 negative fixture scaffold set.

The index must not reference docs, CMake files, Python gate scripts,
production paths, positive fixture files, database files, or unauthorized JSON.

## Negative Fixture JSON Contract

Each `NEG_MRF*.json` file is structured scaffold metadata. Each file must
include:

```text
schemaVersion
negativeFixtureId
title
purpose
sourcePositiveFixtureId
mutationCategory
targetValidatorRule
expectedIssueCode
expectedSeverity
expectedBlocking
scaffoldOnly
runtimeUse
productionUse
replayExecuted
containsSyntheticDataOnly
mutationDescription
privacyExpectations
sanitizationExpectations
metadata
```

Each file must set `schemaVersion` to
`manual-entry-replay-negative-fixture-scaffold/v1`, `scaffoldOnly=true`,
`runtimeUse=false`, `productionUse=false`, `replayExecuted=false`,
`containsSyntheticDataOnly=true`, `expectedSeverity=error`, and
`expectedBlocking=true`.

Each file must include `metadata.task=TASK-222`,
`metadata.notReplayInput=true`, `metadata.notProductionInput=true`, and
`metadata.notRuntimeDataSource=true`.

## Case Matrix

- `NEG_MRF001_missing_required_field.json` describes a future missing required
  field rejection.
- `NEG_MRF002_wrong_schema_version.json` describes a future schema version
  rejection.
- `NEG_MRF003_runtime_use_true.json` describes a future `runtimeUse=true`
  rejection while the scaffold file itself keeps `runtimeUse=false`.
- `NEG_MRF004_production_use_true.json` describes a future
  `productionUse=true` rejection while the scaffold file itself keeps
  `productionUse=false`.
- `NEG_MRF005_replay_executed_true.json` describes a future
  `replayExecuted=true` rejection while the scaffold file itself keeps
  `replayExecuted=false`.
- `NEG_MRF006_non_synthetic_privacy.json` describes a future synthetic flag
  rejection while the scaffold file itself keeps `containsSyntheticDataOnly=true`.
- `NEG_MRF007_extra_json_file.json` describes a future unauthorized extra JSON
  rejection while remaining one of the TASK-222 authorized scaffold files.
- `NEG_MRF008_forbidden_token.sql.json` describes a future sanitized SQL-token
  marker rejection without embedding executable SQL.
- `NEG_MRF009_broker_payload_token.json` describes a future sanitized broker
  payload marker rejection without embedding a broker payload.
- `NEG_MRF010_real_order_id_token.json` describes a future sanitized real order
  marker rejection without embedding a real order id.

## Expected Issue Code Mapping

```text
NEG_MRF001 -> NEG_FIXTURE_MISSING_REQUIRED_FIELD
NEG_MRF002 -> NEG_FIXTURE_SCHEMA_VERSION_INVALID
NEG_MRF003 -> NEG_FIXTURE_RUNTIME_USE_TRUE
NEG_MRF004 -> NEG_FIXTURE_PRODUCTION_USE_TRUE
NEG_MRF005 -> NEG_FIXTURE_REPLAY_EXECUTED_TRUE
NEG_MRF006 -> NEG_FIXTURE_SYNTHETIC_FLAG_FALSE
NEG_MRF007 -> NEG_FIXTURE_UNAUTHORIZED_FILE
NEG_MRF008 -> NEG_FIXTURE_RAW_SQL_TOKEN
NEG_MRF009 -> NEG_FIXTURE_BROKER_PAYLOAD_TOKEN
NEG_MRF010 -> NEG_FIXTURE_REAL_ORDER_TOKEN
```

Expected issue codes are sanitized identifiers only. Diagnostics must not echo
raw payloads or internal implementation details.

## Privacy And Sanitization Boundary

TASK-222 negative fixture scaffold JSON files contain synthetic metadata only.
They must not include real account identifiers, raw UI payloads, full manual
entry payloads, full positive fixture payloads, full trade facts, full cash
adjustments, executable SQL, credentials, endpoint values, access tokens, secret
keys, private keys, passwords, internal exception stacks, broker payloads, real
order ids, or production database paths.

Allowed marker text is limited to sanitized placeholders such as
`SANITIZED_FORBIDDEN_TOKEN_PLACEHOLDER`,
`SANITIZED_RAW_SQL_TOKEN_PLACEHOLDER`,
`SANITIZED_BROKER_PAYLOAD_PLACEHOLDER`, and
`SANITIZED_REAL_ORDER_ID_PLACEHOLDER`.

## No-Replay Boundary

TASK-222 does not implement replay. TASK-222 does not call AccountingEngine
replay. TASK-222 does not compute positions, cash, portfolio values,
unrealized PnL, daily-use readback, or readback refresh. TASK-222 does not
write snapshots.

The negative fixture scaffold files are not replay inputs. They are not runtime
fixtures. They are not production data. Future use by a negative static
validator must be separately authorized.

## No-Runtime-Dependency Boundary

TASK-222 does not modify production code. TASK-222 does not modify QML /
DataServiceActions / repositories / migrations. TASK-222 does not write runtime
SQL / SQLite / audit / ledger / snapshot. TASK-222 does not connect broker /
network / credentials / endpoint. TASK-222 does not place real orders or enable
automatic trading.

TASK-222 does not create services, sockets, DataService action calls, runtime
read models, backup files, export files, restore files, database files, or
broker artifacts.

## Out-of-Scope Boundaries

Out of scope for TASK-222:

- negative validator implementation
- parser / loader / reader implementation
- replay implementation
- AccountingEngine replay calls
- runtime SQL / SQLite read/write
- audit / ledger / snapshot writes
- changes to existing positive fixture JSON
- changes to TASK-219 static validator
- production code changes
- QML / DataServiceActions / repository / migration changes
- broker, network, credentials, endpoint, real order placement, or automatic
  trading

## Formal Conclusion And Next Task

TASK-222 creates test-only negative fixture scaffold JSON files.
TASK-222 does not authorize negative validator implementation.
TASK-222 does not authorize parser, loader, or reader implementation.
TASK-222 does not authorize replay implementation.
TASK-222 does not authorize AccountingEngine replay calls.
TASK-222 does not authorize runtime SQL / SQLite read/write.
TASK-222 does not authorize audit / ledger / snapshot writes.
TASK-222 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-223 manual entry replay negative fixture static validator authorization gate.
