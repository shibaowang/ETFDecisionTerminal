# ShellAccounting Manual Entry Replay Negative Fixture Static Validator Failure-Mode Hardening Test Plan

## Document Purpose

This test plan defines TASK-226 validation for failure-mode hardening around
the TASK-224 test-only negative fixture static validator. The plan proves bad
input fails closed with sanitized output, while real fixture JSON and production
paths remain unchanged.

## Test Matrix

- docs/248 exists and documents TASK-226 scope.
- docs/249 exists and documents this test plan.
- README, docs/README, and docs/12 reference docs/248 and docs/249.
- CTest `shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening` is registered.
- The gate uses temporary mutated copies only.
- Missing required field fails non-zero.
- Wrong schema version fails non-zero.
- Duplicate negative fixture id fails non-zero.
- File name / negativeFixtureId mismatch fails non-zero.
- Index missing `NEG_MRF` reference fails non-zero.
- Index reference to unauthorized JSON fails non-zero.
- Extra unauthorized JSON file fails non-zero.
- `runtimeUse=true` fails non-zero.
- `productionUse=true` fails non-zero.
- `replayExecuted=true` fails non-zero.
- `containsSyntheticDataOnly=false` fails non-zero.
- Unknown `expectedIssueCode` fails non-zero.
- Unknown `sourcePositiveFixtureId` fails non-zero.
- Raw SQL token fails non-zero.
- Credential token fails non-zero.
- Endpoint token fails non-zero.
- Broker payload token fails non-zero.
- Real order id token fails non-zero.
- DB / WAL / SHM artifact fails non-zero.
- Production path reference fails non-zero.
- Failure output uses schema `manual-entry-replay-negative-fixture-static-validator-failure/v1`.
- Failure output is sanitized and contains no raw JSON payload.
- Failure output contains no traceback or stack trace.
- Failure output contains no raw SQL, credentials, endpoint, broker payload, or real order id.
- Failure output contains no `apps/`, `libs/`, or `migrations/` production path.
- Negative fixture JSON is unchanged.
- Positive fixture JSON is unchanged.
- TASK-219 positive validator is unchanged.
- apps / libs / migrations are unchanged.
- No replay, AccountingEngine replay, runtime SQL / SQLite, audit, ledger, snapshot, broker, network, credentials, endpoint, real order, or automatic trading path appears.

## Required Probes

- Documentation existence probe.
- Documentation index probe.
- CTest registration probe.
- Temporary-copy mutation probe.
- Non-zero exit probe for every failure case.
- Failure JSON parse probe.
- Failure schema version probe.
- Failure count probe.
- Sanitized key-set probe.
- Sanitized message probe.
- Raw JSON payload exclusion probe.
- Stack trace exclusion probe.
- Raw SQL / credential / endpoint / broker payload / real order exclusion probe.
- Production path exclusion probe.
- Real fixture hash stability probe.
- Positive fixture hash stability probe.
- TASK-219 positive validator unchanged probe.
- No production path drift probe.
- No replay / AccountingEngine replay probe.
- No broker / network / credentials / endpoint probe.
- Clean-main empty changed-set support probe.
- Exact changed-path allowlist fail-closed probe.
- Full CTest regression.
- transport_local_socket_echo 50-repeat regression.
- git diff whitespace probe.

## Go / No-Go Checklist

Go only if:

- docs/248 and docs/249 exist and are indexed.
- The TASK-226 failure-mode hardening CTest exists and passes.
- The TASK-224 validator still passes without test-only path arguments.
- The TASK-225 regression matrix still passes with `--summary-json`.
- Every required failure case exits non-zero.
- Failure output is sanitized.
- Negative fixture JSON is unchanged.
- Positive fixture JSON is unchanged.
- TASK-219 positive validator is unchanged.
- All targeted gates and full CTest pass.
- `git diff --check` passes.

No-Go if:

- Fixture JSON changes.
- Production code, QML, DataServiceActions, repositories, or migrations change.
- Production validator, parser, loader, reader, replay, or AccountingEngine replay is implemented.
- Runtime SQL / SQLite / audit / ledger / snapshot write appears.
- Broker, network, credentials, endpoint, real order, or automatic trading appears.
- Any required `ctest -R` pattern matches zero tests.
