# ShellAccounting Manual Entry Replay Negative Fixture Static Validator Regression Matrix Gate

## Purpose

TASK-225 locks the regression matrix for the TASK-224 test-only negative
fixture static validator. The gate verifies the ten TASK-222 `NEG_MRF`
scaffold cases, exact expected issue-code mapping, sanitized summary output,
no-replay boundary, no-runtime boundary, and no-production boundary.

TASK-225 may add a stable sanitized JSON summary mode to the TASK-224
test-only validator. TASK-225 does not change the validator default CTest
validation semantics.

## Scope

TASK-225 adds a test-only regression matrix CTest:

```text
shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix
```

The CTest runs the TASK-224 validator with `--summary-json`, parses the
sanitized JSON summary, and verifies the complete regression matrix.

## Summary JSON Contract

The summary JSON schema is:

```json
{
  "schemaVersion": "manual-entry-replay-negative-fixture-static-validator-summary/v1",
  "issueCount": 10,
  "issues": []
}
```

Each issue entry contains only sanitized fields:

```text
negativeFixtureId
file
issueCode
severity
blocking
rule
message
```

The summary must not echo raw fixture payload, raw SQL, credentials, endpoint
values, broker payloads, real order ids, or exception stacks.

## Regression Matrix

The matrix locks this exact mapping:

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

Every issue must have `severity="error"` and `blocking=true`. File names must
align with their `negativeFixtureId`. Duplicate ids and unknown issue codes
must fail.

## Boundary Policy

TASK-225 does not modify fixture JSON. TASK-225 does not modify positive
fixture JSON. TASK-225 does not modify TASK-219 positive fixture static
validator.

TASK-225 does not implement production validator / parser / loader / reader.
TASK-225 does not implement replay. TASK-225 does not call AccountingEngine
replay. TASK-225 does not modify production code / QML / DataServiceActions /
repositories / migrations.

TASK-225 does not write runtime SQL / SQLite / audit / ledger / snapshot.
TASK-225 does not connect broker / network / credentials / endpoint. TASK-225
does not place real orders or enable automatic trading.

## Changed-Path Policy

The gate supports clean main with an empty changed set. When changed paths are
present, they must be exact allowlisted TASK-225 docs/test paths or exact
old-gate self-consistency paths explicitly authorized by a later prompt. The
gate must fail closed on directory-level allowlists, wildcard allowlists, fixture
JSON changes, production code changes, and runtime path changes.

## Formal Conclusion And Next Task

TASK-225 adds a regression matrix gate for the TASK-224 test-only validator.
TASK-225 may add stable sanitized summary output mode to the test-only
validator.
TASK-225 does not modify fixture JSON.
TASK-225 does not implement production validator / parser / loader / reader.
TASK-225 does not implement replay.
TASK-225 does not call AccountingEngine replay.
TASK-225 does not modify production code / QML / DataServiceActions / repositories / migrations.
TASK-225 does not write runtime SQL / SQLite / audit / ledger / snapshot.
TASK-225 does not connect broker / network / credentials / endpoint.
TASK-225 does not place real orders or enable automatic trading.
Recommended next task: TASK-226 manual entry replay negative fixture static validator failure-mode hardening gate.
