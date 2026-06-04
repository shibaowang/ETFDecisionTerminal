# ShellAccounting Manual Entry Replay Negative Fixture Static Validator Regression Matrix Test Plan

## Document Purpose

This test plan defines TASK-225 validation for the regression matrix around the
TASK-224 test-only negative fixture static validator. The plan proves the
validator summary output is stable, sanitized, complete, and bounded to static
test inspection.

## Test Matrix

- docs/246 exists and documents TASK-225 scope.
- docs/247 exists and documents this test plan.
- README, docs/README, and docs/12 reference docs/246 and docs/247.
- CTest `shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix` is registered.
- The regression matrix gate runs the TASK-224 validator with `--summary-json`.
- Summary JSON has schema version `manual-entry-replay-negative-fixture-static-validator-summary/v1`.
- Summary JSON has `issueCount=10`.
- Summary JSON contains exactly ten issues.
- `NEG_MRF001` through `NEG_MRF010` are all present once.
- Expected issue-code mapping is exact.
- Severity is always `error`.
- Blocking is always `true`.
- File names align with `negativeFixtureId`.
- Duplicate ids fail.
- Unknown issue codes fail.
- Output contains no raw fixture payload.
- Output contains no credentials, endpoint, broker payload, real order id, raw SQL, or stack trace.
- The validator writes no files.
- Negative fixture JSON is unchanged.
- Positive fixture JSON is unchanged.
- TASK-219 positive static validator is unchanged.
- apps / libs / migrations are unchanged.
- No replay, AccountingEngine replay, runtime SQL / SQLite, audit, ledger, snapshot, broker, network, credentials, endpoint, real order, or automatic trading path appears.

## Required Probes

- Documentation existence probe.
- Documentation index probe.
- CTest registration probe.
- `--summary-json` execution probe.
- Summary JSON parse probe.
- Schema version probe.
- Issue count probe.
- Ten-id coverage probe.
- Exact issue-code mapping probe.
- Severity and blocking probes.
- File alignment probe.
- Duplicate id probe.
- Unknown issue code probe.
- Raw payload and forbidden token probe.
- Stack trace probe.
- File-write stability probe.
- Fixture JSON unchanged probe.
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

- docs/246 and docs/247 exist and are indexed.
- The TASK-225 regression matrix CTest exists and passes.
- The TASK-224 validator still passes without `--summary-json`.
- The TASK-224 validator emits stable sanitized summary JSON with `--summary-json`.
- The ten `NEG_MRF` ids and issue-code mapping are exact.
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
