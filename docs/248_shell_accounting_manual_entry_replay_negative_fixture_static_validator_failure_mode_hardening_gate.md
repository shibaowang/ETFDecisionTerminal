# ShellAccounting Manual Entry Replay Negative Fixture Static Validator Failure-Mode Hardening Gate

## Purpose

TASK-226 hardens TASK-224 test-only validator failure-mode behavior. The gate
proves that malformed or policy-violating negative fixture inputs fail closed,
exit non-zero, and emit sanitized failure output without modifying real fixture
JSON.

TASK-226 uses temporary mutated copies only. It may extend the TASK-224
test-only validator with explicit input path arguments for test harness use,
while preserving default no-argument CTest behavior and preserving the
successful `--summary-json` output contract.

## Scope

TASK-226 adds a test-only CTest:

```text
shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening
```

The CTest copies `tests/fixtures/manual_entry_replay_negative/` and
`tests/fixtures/manual_entry_replay/fixtures_index.json` into a temporary
directory, mutates only those temporary copies, invokes the TASK-224 validator,
and verifies non-zero sanitized failures.

## Failure-Mode Contract

Bad input must fail closed. The validator must not report silent success for:

- missing required fields
- wrong schema version
- duplicate negative fixture ids
- file name / negativeFixtureId mismatch
- index missing `NEG_MRF` references
- index references to unauthorized JSON
- extra unauthorized JSON files
- `runtimeUse=true`
- `productionUse=true`
- `replayExecuted=true`
- `containsSyntheticDataOnly=false`
- unknown `expectedIssueCode`
- unknown `sourcePositiveFixtureId`
- raw SQL tokens
- credential tokens
- endpoint tokens
- broker payload tokens
- real order id tokens
- DB / WAL / SHM artifacts
- production path references

Failure output must be sanitized. It must not expose raw JSON payload, raw SQL,
credentials, endpoint values, broker payloads, real order ids, production paths,
or traceback / stack traces.

## Boundary Policy

TASK-226 does not modify fixture JSON. TASK-226 does not modify positive
fixture JSON. TASK-226 does not modify the TASK-219 positive fixture static
validator.

TASK-226 does not implement production validator, parser, loader, or reader.
TASK-226 does not implement replay. TASK-226 does not call AccountingEngine
replay. TASK-226 does not modify production code, production QML,
DataServiceActions, repositories, or migrations.

TASK-226 does not write runtime SQL / SQLite / audit / ledger / snapshot.
TASK-226 does not connect broker / network / credentials / endpoint. TASK-226
does not place real orders or enable automatic trading.

## Changed-Path Policy

The gate supports clean main with an empty changed set. When changed paths are
present, they must be exact allowlisted TASK-226 docs/test paths, the TASK-224
test-only validator parameter extension, or exact old-gate self-consistency
paths explicitly authorized by a later prompt.

The gate fails closed on directory-level allowlists, wildcard allowlists,
fixture JSON changes, production code changes, runtime path changes, and
production validator / parser / loader / reader / replay paths.

## Formal Conclusion And Next Task

TASK-226 hardens TASK-224 test-only validator failure-mode behavior.
TASK-226 uses temporary mutated copies only.
TASK-226 does not modify fixture JSON.
TASK-226 does not implement production validator / parser / loader / reader.
TASK-226 does not implement replay.
TASK-226 does not call AccountingEngine replay.
TASK-226 does not modify production code / QML / DataServiceActions / repositories / migrations.
TASK-226 does not write runtime SQL / SQLite / audit / ledger / snapshot.
TASK-226 does not connect broker / network / credentials / endpoint.
TASK-226 does not place real orders or enable automatic trading.
Recommended next task: TASK-227 CI closeout gate.
