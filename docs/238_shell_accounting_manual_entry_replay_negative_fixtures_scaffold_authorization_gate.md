# ShellAccounting Manual Entry Replay Negative Fixtures Scaffold Authorization Gate

## Authorization Purpose

TASK-221 is authorization-gate-only. It records the future scaffold policy for
manual entry replay negative fixtures before any negative fixture files,
negative fixture directory, negative validator, parser, loader, reader, or
replay implementation is created.

TASK-221 only authorizes future negative fixture scaffold policy. TASK-221
does not create negative fixture files. TASK-221 does not create negative
fixture directory. TASK-221 does not implement negative validator. TASK-221
does not implement parser, loader, reader. TASK-221 does not implement replay.
TASK-221 does not call AccountingEngine replay.

## Relationship To TASK-220

TASK-220 established the static validator negative fixture authorization
boundary. TASK-221 narrows the next step to future scaffold policy only: names,
directory contract, index contract, metadata contract, mutation categories,
expected issue codes, privacy, no-replay, and no-runtime-dependency rules.

TASK-221 does not modify TASK-220 behavior. TASK-221 does not modify the
TASK-219 static validator. TASK-221 does not create the future negative
fixture scaffold.

## Future Negative Fixture Scaffold Boundary

Future negative fixture scaffold work must be a separate task. It may only
create test-only negative fixture scaffold files after explicit authorization.
It must not be treated as runtime accounting data, production seed data,
database input, replay facts, or broker/order input.

TASK-221 does not modify production code. TASK-221 does not modify QML /
Presenter / Controller / ShellServices. TASK-221 does not modify
DataServiceActions. TASK-221 does not modify repositories. TASK-221 does not
modify migrations. TASK-221 does not modify existing fixture JSON.
TASK-221 does not connect broker / network / credentials / endpoint.
TASK-221 does not place real orders or enable automatic trading.

## Future Directory And Index Contract

The future directory candidate is documented as contract text only and must not
be created by TASK-221:

```text
tests/fixtures/manual_entry_replay_negative/
```

The future index candidate is documented as contract text only and must not be
created by TASK-221:

```text
tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json
```

Future implementation must prove the directory and index are test-only,
synthetic-only, not production seed data, not runtime input, and not replay
output.

## Future Negative Fixture File Set

Future negative fixture names are candidates only and must not be created by
TASK-221:

```text
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

Future scaffold implementation must require exact file names, unique negative
fixture ids, a single target validation rule per file unless separately
authorized, and fail-closed handling for extra JSON files.

## Future Negative Fixture Metadata Contract

Future negative fixture metadata must be structured. Candidate required fields
are:

```text
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
privacyExpectations
sanitizationExpectations
metadata
```

Future metadata must be synthetic-only and must not embed raw fixture payloads,
raw SQL, credentials, endpoint values, broker payloads, real order ids,
database paths, or internal stack traces.

## Future Mutation Categories

Future negative fixture scaffold categories must be static validation inputs
only. Candidate mutation categories include:

- missing required field
- wrong schema version
- fixture id mismatch
- duplicate fixture id
- unauthorized negative fixture file
- unauthorized negative fixture path
- `runtimeUse=true`
- `productionUse=true`
- `replayExecuted=true`
- `containsSyntheticDataOnly=false`
- forbidden token
- raw SQL token
- credential token
- endpoint token
- broker payload token
- real order token
- DB artifact present
- production path reference

These mutations must not trigger replay, runtime loading, SQLite read/write,
service startup, broker calls, network calls, credential lookup, or order
placement.

## Future Expected Issue Code Mapping

Future issue codes must follow the TASK-220 sanitized code set:

```text
NEG_FIXTURE_MISSING_REQUIRED_FIELD
NEG_FIXTURE_SCHEMA_VERSION_INVALID
NEG_FIXTURE_ID_MISMATCH
NEG_FIXTURE_DUPLICATE_ID
NEG_FIXTURE_UNAUTHORIZED_FILE
NEG_FIXTURE_UNAUTHORIZED_PATH
NEG_FIXTURE_RUNTIME_USE_TRUE
NEG_FIXTURE_PRODUCTION_USE_TRUE
NEG_FIXTURE_REPLAY_EXECUTED_TRUE
NEG_FIXTURE_SYNTHETIC_FLAG_FALSE
NEG_FIXTURE_FORBIDDEN_TOKEN
NEG_FIXTURE_RAW_SQL_TOKEN
NEG_FIXTURE_CREDENTIAL_TOKEN
NEG_FIXTURE_ENDPOINT_TOKEN
NEG_FIXTURE_BROKER_PAYLOAD_TOKEN
NEG_FIXTURE_REAL_ORDER_TOKEN
NEG_FIXTURE_DB_ARTIFACT_PRESENT
NEG_FIXTURE_PRODUCTION_PATH_REFERENCE
```

Future diagnostics may include negative fixture id, target rule, sanitized
issue code, severity, blocking flag, and a safe short reason. Diagnostics must
not echo raw payloads.

## Privacy And Sanitization Boundary

Future negative fixtures must contain synthetic data only. They must reject raw
SQL, raw UI payload, raw DataService payload, full trade_log payload, full
cash_adjustment payload, full snapshot payload, broker payload, credentials,
endpoint values, access tokens, secret keys, private keys, passwords, real
account ids, real order ids, real broker order ids, internal stack traces, and
production database paths.

Allowed placeholder markers are limited to safe synthetic markers such as
`SYNTHETIC_*`, `SANITIZED_MEMO`, `TODO_PLACEHOLDER`, and
`UNAVAILABLE_EXPECTED`.

## No-Replay Boundary

TASK-221 does not implement replay and does not call AccountingEngine replay.
Future negative fixture scaffold work must not compute positions, cash,
portfolio PnL, market value, daily-use acceptance, or readback refresh. It must
not run replay preview and must not write snapshots.

## No-Runtime-Dependency Boundary

TASK-221 does not write runtime SQL / SQLite / audit / ledger / snapshot. It
does not start services, does not connect sockets, does not call DataService actions, read
SQLite, write SQLite, write audit rows, write ledger rows, write snapshot rows,
create backup/export/restore artifacts, or depend on broker, network,
credentials, endpoint, real order placement, or automatic trading.

## Out-of-Scope Boundaries

Out of scope for TASK-221:

- creating `tests/fixtures/manual_entry_replay_negative/`
- creating `negative_fixtures_index.json`
- creating `NEG_MRF*.json`
- modifying `tests/fixtures/manual_entry_replay/*.json`
- modifying TASK-219 static validator
- implementing negative validator
- implementing parser / loader / reader
- implementing replay
- calling AccountingEngine replay
- modifying production code
- modifying QML / Presenter / Controller / ShellServices
- modifying DataServiceActions
- modifying repositories
- modifying migrations
- writing runtime SQL / SQLite / audit / ledger / snapshot
- connecting broker / network / credentials / endpoint
- placing real orders
- enabling automatic trading

## Formal Authorization Conclusion And Next Task

TASK-221 authorizes future negative fixture scaffold policy only.
TASK-221 does not authorize creation of negative fixture files.
TASK-221 does not authorize creation of the negative fixture directory.
TASK-221 does not authorize negative validator implementation.
TASK-221 does not authorize parser, loader, or reader implementation.
TASK-221 does not authorize replay implementation.
TASK-221 does not authorize AccountingEngine replay calls.
TASK-221 does not authorize runtime SQL / SQLite read/write.
TASK-221 does not authorize audit / ledger / snapshot writes.
TASK-221 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-222 manual entry replay negative fixture scaffold files gate.
