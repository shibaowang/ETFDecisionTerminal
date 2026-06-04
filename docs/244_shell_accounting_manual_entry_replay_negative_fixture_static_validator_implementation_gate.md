# ShellAccounting Manual Entry Replay Negative Fixture Static Validator Implementation Gate

## Purpose

TASK-224 implements a test-only negative fixture static validator for the
manual entry replay negative fixture scaffold created by TASK-222. The validator
is a CTest static inspection tool only. It reads the negative fixture index,
the ten `NEG_MRF*.json` scaffold files, and the positive fixture index only to
validate schema, flags, source mapping, expected issue codes, privacy markers,
no-replay boundaries, and no-production boundaries.

TASK-224 does not implement production validator. TASK-224 does not implement
production parser, loader, reader. TASK-224 does not implement replay. TASK-224
does not call AccountingEngine replay.

## Scope

The implementation is limited to:

```text
tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/
```

The CTest name is:

```text
shell_accounting_manual_entry_replay_negative_fixture_static_validator
```

The validator may use Python `json` for test-only static inspection. It must
not export a runtime parser, loader, reader, replay harness, data service
action, production dependency, or database reader.

## Input Boundary

The validator may read only:

```text
tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json
tests/fixtures/manual_entry_replay_negative/NEG_MRF*.json
tests/fixtures/manual_entry_replay/fixtures_index.json
```

The positive fixture index is read only to verify that each
`sourcePositiveFixtureId` exists. TASK-224 does not modify negative fixture
JSON. TASK-224 does not modify positive fixture JSON. TASK-224 does not modify
TASK-219 positive fixture static validator.

## Static Validation Contract

The validator checks:

- negative fixture directory exists.
- exact file set is the index plus ten `NEG_MRF*.json` files.
- all JSON files are valid JSON objects.
- the negative index references exactly ten `NEG_MRF` files.
- `negativeFixtureId` values are unique.
- file name and `negativeFixtureId` are aligned.
- `schemaVersion` is correct.
- `scaffoldOnly=true`.
- `runtimeUse=false`.
- `productionUse=false`.
- `replayExecuted=false`.
- `containsSyntheticDataOnly=true`.
- `expectedSeverity="error"`.
- `expectedBlocking=true`.
- `sourcePositiveFixtureId` exists in the positive fixture index.
- expected issue code mapping is exact.
- no DB, WAL, SHM, SQLite, or SQL artifact is present.
- no production path reference is present.
- no credentials, endpoint, broker payload, real order id, or raw SQL is
  present outside explicit sanitized placeholders.
- sanitized placeholders are explicit safe placeholders.
- the validator does not echo raw fixture payload.
- the validator does not output exception stack trace.
- changed-path detection supports clean main with no changes and fails closed
  on non-allowlisted changes.

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

## Sanitized Output Contract

The validator emits a sanitized expected issue summary. Each summary entry
includes:

```text
negativeFixtureId
file
issueCode
severity
blocking
rule
message
```

The summary is constructed from safe identifiers, issue codes, rules, severity,
blocking flag, and short sanitized messages. The validator does not echo full
fixture JSON, raw payload, raw SQL, credentials, endpoint values, broker
payloads, real order ids, or exception stacks.

## No-Replay Boundary

TASK-224 does not implement replay. TASK-224 does not call AccountingEngine
replay. The validator does not compute positions, cash, portfolio values, PnL,
readback output, refresh output, snapshots, audit, or ledger rows.

Negative fixture scaffold files remain static test-only metadata. They are not
runtime replay input, production data, service input, database input, or broker
input.

## No-Runtime Boundary

TASK-224 does not modify production code / QML / DataServiceActions /
repositories / migrations. TASK-224 does not write runtime SQL / SQLite / audit
/ ledger / snapshot. TASK-224 does not connect broker / network / credentials /
endpoint. TASK-224 does not place real orders or enable automatic trading.

## Rollback And Failure Policy

If the validator reports an unsafe fixture, malformed JSON, missing mapping, or
unauthorized changed path, the CTest fails closed with a sanitized issue code
and message. Removing the TASK-224 test-only CTest returns the repo to the
TASK-223 authorization-only state without changing fixtures or runtime code.

## Formal Conclusion And Next Task

TASK-224 implements a test-only negative fixture static validator.
TASK-224 does not implement production validator.
TASK-224 does not implement production parser, loader, reader.
TASK-224 does not implement replay.
TASK-224 does not call AccountingEngine replay.
TASK-224 does not modify negative fixture JSON.
TASK-224 does not modify positive fixture JSON.
TASK-224 does not modify TASK-219 positive fixture static validator.
TASK-224 does not modify production code / QML / DataServiceActions / repositories / migrations.
TASK-224 does not write runtime SQL / SQLite / audit / ledger / snapshot.
TASK-224 does not connect broker / network / credentials / endpoint.
TASK-224 does not place real orders or enable automatic trading.
Recommended next task: TASK-225 manual entry replay negative fixture static validator regression matrix gate.
