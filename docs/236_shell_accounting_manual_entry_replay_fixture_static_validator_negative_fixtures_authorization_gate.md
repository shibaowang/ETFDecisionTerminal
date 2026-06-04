# ShellAccounting Manual Entry Replay Fixture Static Validator Negative Fixtures Authorization Gate

## Negative Fixtures Authorization Purpose

TASK-220 is gate-only / negative-fixtures-authorization-only. It records the
future policy for negative fixture coverage for the manual entry replay fixture
static validator. The task does not create negative fixture files and does not
implement a negative-case validator.

The purpose is to fix the authorization boundary before any future negative
fixture scaffold or static-validator implementation task. The current
TASK-219 static validator remains unchanged and continues to validate only the
existing TASK-217 positive scaffold files.

## Future Negative Fixture Directory Boundary

Future negative fixtures may only be proposed under a separate task. The
candidate directory is `tests/fixtures/manual_entry_replay_negative/`.

TASK-220 does not create that directory. A future task must prove that the
directory is test-only, contains no runtime database artifacts, and cannot be
used as production seed data. The future directory must not be used as
production seed data. The directory must not contain DB, SQLite, WAL, SHM,
export, backup, restore, replay output, runtime output, broker, network,
credential, endpoint, or real order artifacts.

## Future Negative Fixture Naming Policy

Future negative fixture names are reserved as candidate names only:

- `NEG_MRF001_missing_required_field.json`
- `NEG_MRF002_wrong_schema_version.json`
- `NEG_MRF003_runtime_use_true.json`
- `NEG_MRF004_production_use_true.json`
- `NEG_MRF005_replay_executed_true.json`
- `NEG_MRF006_non_synthetic_privacy.json`
- `NEG_MRF007_extra_json_file.json`
- `NEG_MRF008_forbidden_token.sql.json`
- `NEG_MRF009_broker_payload_token.json`
- `NEG_MRF010_real_order_id_token.json`

Future implementation must require exact file names, unique negative fixture
ids, one issue target per fixture unless explicitly documented otherwise, and
sanitized file content. Extra JSON files must fail closed.

## Future Negative Fixture Schema Policy

Future negative fixtures must use a structured schema and must not be raw
payload escape hatches. Candidate schema fields are:

- `negativeFixtureId`
- `title`
- `purpose`
- `targetValidatorRule`
- `expectedIssueCode`
- `expectedSeverity`
- `expectedBlocking`
- `mutatedFile`
- `mutationDescription`
- `inputFixture`
- `privacyExpectations`
- `metadata`

The schema must identify the validator rule being exercised, the expected
sanitized issue code, whether the issue is blocking, and which scaffold input is
being mutated. It must not embed full fixture payloads, raw SQL, credentials,
endpoint values, broker payloads, real order ids, or internal stack traces.

## Future Negative Fixture Case Matrix

Future negative fixture coverage must include:

- missing required field
- wrong schema version
- fixture id mismatch
- duplicate fixture id
- unauthorized negative fixture file
- unauthorized fixture path
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

Each negative case must remain static validation only. It must not run replay,
call AccountingEngine, read SQLite, write SQLite, write audit, write ledger,
write snapshots, or start any runtime service.

## Future Negative Expected Issue Codes

Future negative validator output must use sanitized issue codes only:

- `NEG_FIXTURE_MISSING_REQUIRED_FIELD`
- `NEG_FIXTURE_SCHEMA_VERSION_INVALID`
- `NEG_FIXTURE_ID_MISMATCH`
- `NEG_FIXTURE_DUPLICATE_ID`
- `NEG_FIXTURE_UNAUTHORIZED_FILE`
- `NEG_FIXTURE_UNAUTHORIZED_PATH`
- `NEG_FIXTURE_RUNTIME_USE_TRUE`
- `NEG_FIXTURE_PRODUCTION_USE_TRUE`
- `NEG_FIXTURE_REPLAY_EXECUTED_TRUE`
- `NEG_FIXTURE_SYNTHETIC_FLAG_FALSE`
- `NEG_FIXTURE_FORBIDDEN_TOKEN`
- `NEG_FIXTURE_RAW_SQL_TOKEN`
- `NEG_FIXTURE_CREDENTIAL_TOKEN`
- `NEG_FIXTURE_ENDPOINT_TOKEN`
- `NEG_FIXTURE_BROKER_PAYLOAD_TOKEN`
- `NEG_FIXTURE_REAL_ORDER_TOKEN`
- `NEG_FIXTURE_DB_ARTIFACT_PRESENT`
- `NEG_FIXTURE_PRODUCTION_PATH_REFERENCE`

Diagnostics must not echo raw fixture payload, raw SQL, full trade_log payload,
full cash_adjustment payload, full snapshot payload, broker payload, credential
material, endpoint values, real order ids, or internal stack traces.

## Static Validator Negative-Case Boundary

Future negative-case validation must remain a test-only static source check.
It may parse JSON from the authorized negative fixture directory and compare
fields against the negative fixture policy. It must not import production
modules, DataServiceApi, DataAccess, DataServiceClient, ShellServices,
AccountingEngine, broker adapters, or SQLite runtime helpers.

The static validator may report pass / fail status, sanitized issue code,
negative fixture id, target validator rule, mutated file name, and safe short
reason. It must not generate concrete replay output or use negative fixtures as
runtime facts.

## Privacy And Sanitization Boundary

Future negative fixtures must remain synthetic-only and sanitized. Privacy
expectations must reject raw SQL, raw UI payload, raw DataService payload, full
trade_log payload, full cash_adjustment payload, full snapshot payload, broker
payload, credentials, endpoint values, access tokens, secret keys, private
keys, passwords, real account ids, real order ids, real broker order ids,
internal stack traces, and production database paths.

Negative fixtures may use safe placeholder markers such as `SYNTHETIC_*`,
`SANITIZED_MEMO`, `TODO_PLACEHOLDER`, and `UNAVAILABLE_EXPECTED`.

## No-Replay Boundary

TASK-220 does not authorize replay. Future negative fixture work must not call
AccountingEngine replay, must not run a replay preview, must not refresh
readback models, must not compute position / cash / PnL / market value, and
must not write snapshots.

Negative fixtures are meant to prove fail-closed static validation behavior,
not runtime accounting behavior.

## No-Runtime-Dependency Boundary

TASK-220 does not authorize runtime dependencies. Future negative fixture
validation must not read SQLite, must not write SQLite, must not start
services, must not connect sockets, must not call DataService actions, must not
write audit rows, must not write ledger rows, must not write ledger, must not
create backup/export/restore artifacts, and must not depend on broker, network,
credentials, endpoint, real order placement, or automatic trading capabilities.

## Out-of-Scope Boundaries

Out of scope for TASK-220:

- creation of `tests/fixtures/manual_entry_replay_negative/`
- creation of negative fixture JSON files
- modification of existing `tests/fixtures/manual_entry_replay/*.json`
- negative validator implementation
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

## Formal Authorization Conclusion And Next Task

TASK-220 authorizes replay fixture static validator negative fixture policy only.
TASK-220 does not authorize creation of negative fixture files.
TASK-220 does not authorize negative validator implementation.
TASK-220 does not authorize parser, loader, or reader implementation.
TASK-220 does not authorize replay implementation.
TASK-220 does not authorize AccountingEngine replay calls.
TASK-220 does not authorize runtime SQL / SQLite read/write.
TASK-220 does not authorize audit / ledger writes.
TASK-220 does not authorize snapshot writes.
TASK-220 does not authorize backup/export/restore.
TASK-220 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-221 manual entry replay fixture negative fixtures scaffold authorization gate.
