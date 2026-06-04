# ShellAccounting Manual Entry Replay Test-Only Dry-Run Harness Authorization Test Plan

## Document Purpose

This test plan defines TASK-230 validation for the manual entry replay
test-only dry-run harness authorization gate. It verifies that TASK-230
documents only the future harness boundary, keeps the future implementation
absent, preserves fixture immutability, preserves TASK-224 validator validation
behavior, and blocks replay, AccountingEngine, production, runtime SQL, audit,
ledger, broker, network, real order, and automatic trading behavior.

## Test Matrix

- docs/256 exists and documents TASK-230.
- docs/257 exists and documents this test plan.
- README, docs/README, and docs/12 reference TASK-230, docs/256, and docs/257.
- tests/CMakeLists registers `ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate`.
- CTest `shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization` is registered.
- TASK-229 planning artifacts remain present and indexed.
- Future harness directory is documented but not created.
- Future CTest name is documented but not registered as implementation.
- Future input boundary is documented.
- Future output boundary is documented.
- No-write boundary is documented.
- No-replay boundary is documented.
- No-AccountingEngine boundary is documented.
- No runtime SQL / SQLite boundary is documented.
- No broker / no network boundary is documented.
- Go / No-Go handoff rules are documented.
- The authorization gate does not recursively run full CTest.
- The future harness implementation directory is absent.
- The future harness implementation script is absent.
- Fixture JSON unchanged.
- Validator validation behavior unchanged.
- Production code unchanged.
- No parser / loader / reader / replay implementation appears.
- No AccountingEngine replay call appears.
- No runtime SQL / SQLite read/write appears.
- No audit / ledger / snapshot write appears.
- No broker / network / credentials / endpoint behavior appears.
- Changed-path checking accepts clean main with an empty changed set.
- Non-empty changed paths must be exact allowlisted and fail closed otherwise.
- Gate check count is at least 160 without depending on changed paths being non-empty.

## Required Probes

- Documentation existence probe.
- Documentation index probe.
- CTest directory registration probe.
- CTest name registration probe using `ctest -N`.
- TASK-229 planning artifact probe.
- Future harness directory absence probe.
- Future implementation script absence probe.
- Future input boundary documentation probe.
- Future output boundary documentation probe.
- No-write documentation probe.
- No-replay documentation probe.
- No-AccountingEngine documentation probe.
- Runtime SQL / SQLite boundary probe.
- Broker / network boundary probe.
- Changed-path exact allowlist probe.
- Clean-main empty changed-path support probe.
- Fixture JSON unchanged probe.
- Validator validation behavior unchanged probe.
- Production path unchanged probe.
- No parser / loader / reader / replay probe.
- No AccountingEngine replay probe.
- No runtime SQL / SQLite / audit / ledger / snapshot write probe.
- No broker / network / credentials / endpoint probe.
- External full CTest regression.
- External transport 50-repeat regression.
- `git diff --check` probe.

## Go / No-Go Checklist

Go only if:

- TASK-230 authorization gate passes.
- Future harness is not implemented.
- Fixture JSON unchanged.
- Validator validation behavior unchanged.
- Production code unchanged.
- No parser / loader / reader / replay implementation is added.
- No AccountingEngine replay call is added.
- No runtime SQL / SQLite / audit / ledger / snapshot write is added.
- No broker / network / credentials / endpoint behavior is added.
- Full CTest is run externally and passes.
- Transport local socket echo passes 50 repeats.
- `git diff --check` passes.

No-Go if:

- Any required CTest pattern matches zero tests.
- The authorization gate depends on changed paths being non-empty.
- Future harness directory or implementation appears.
- Fixture JSON changes.
- Validator validation behavior changes.
- Production code, QML, DataServiceActions, repositories, or migrations change.
- Parser, loader, reader, replay, or AccountingEngine replay behavior appears.
- Runtime SQL / SQLite / audit / ledger / snapshot write appears.
- Broker, network, credentials, endpoint, real order, or automatic trading appears.
