# ShellAccounting Manual Entry Replay Test-Only Dry-Run Harness Implementation Test Plan

## Document Purpose

This test plan defines TASK-231 validation for the manual entry replay test-only
dry-run harness implementation. It verifies that the harness reads only
authorized test fixture metadata and fixture JSON, emits a sanitized dry-run
summary, preserves fixture immutability, and avoids replay, AccountingEngine,
production modules, runtime SQL, audit, ledger, broker, network, real order, and
automatic trading behavior.

## Test Matrix

- docs/258 exists and documents TASK-231.
- docs/259 exists and documents this test plan.
- README, docs/README, and docs/12 reference TASK-231, docs/258, and docs/259.
- tests/CMakeLists registers `ShellAccountingManualEntryReplayTestOnlyDryRunHarness`.
- CTest `shell_accounting_manual_entry_replay_test_only_dry_run_harness` is registered.
- TASK-230 authorization gate remains registered and passes.
- The harness supports `--source-root`.
- The harness supports `--summary-json`.
- The harness reads the positive fixture index.
- The harness reads the negative fixture index.
- Positive fixture JSON files referenced by the positive index exist.
- Negative fixture JSON files referenced by the negative index exist.
- Negative fixture count remains ten.
- The dry-run summary schema is
  `manual-entry-replay-test-only-dry-run-summary/v1`.
- Summary entries contain only sanitized metadata fields.
- Summary entries report `plannedStepCount: 0`.
- Summary entries report `replayExecuted: false`.
- Summary entries report `accountingEngineCalled: false`.
- Summary entries report `runtimeWrites: false`.
- The harness does not output raw fixture payload.
- The harness does not output credentials, endpoints, broker payloads, real
  order ids, raw SQL, stack traces, or internal exception payloads.
- Fixture JSON hashes are unchanged before and after harness execution.
- The TASK-219 positive validator remains unchanged.
- TASK-224 validator validation behavior remains unchanged.
- Production code remains unchanged.
- No production parser / loader / reader is implemented.
- No replay implementation is added.
- No AccountingEngine replay call appears.
- No runtime SQL / SQLite / audit / ledger / snapshot write appears.
- No broker / network / credentials / endpoint behavior appears.

## Required Probes

- Documentation existence probe.
- Documentation index probe.
- CTest directory registration probe.
- CTest name registration probe using `ctest -N`.
- CLI support probe for `--source-root`.
- CLI support probe for `--summary-json`.
- Positive fixture index existence probe.
- Negative fixture index existence probe.
- Positive fixture reference existence probe.
- Negative fixture reference existence probe.
- Negative fixture count probe.
- Sanitized summary schema probe.
- Sanitized summary entry key probe.
- No raw payload output probe.
- No SQL / credential / endpoint / broker / real-order output probe.
- Fixture hash before / after probe.
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

- TASK-231 dry-run harness CTest passes.
- TASK-230 authorization gate passes.
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
- The implementation gate depends on changed paths being non-empty.
- Fixture JSON changes.
- Validator validation behavior changes.
- Production code, QML, DataServiceActions, repositories, or migrations change.
- Parser, loader, reader, replay, or AccountingEngine replay behavior appears.
- Runtime SQL / SQLite / audit / ledger / snapshot write appears.
- Broker, network, credentials, endpoint, real order, or automatic trading appears.
