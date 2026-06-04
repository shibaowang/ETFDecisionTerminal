# ShellAccounting Manual Entry Replay Negative Fixture Validator Phase Closeout And Handoff Test Plan

## Document Purpose

This test plan defines TASK-228 validation for the negative fixture validator
phase closeout and handoff. It verifies that TASK-220 through TASK-227
artifacts are complete, critical CTest registrations exist, clean-main behavior
is safe, fixture JSON is unchanged, validator validation behavior is unchanged,
and production boundaries remain unchanged.

## Test Matrix

- docs/252 exists and documents TASK-228 phase closeout scope.
- docs/253 exists and documents the TASK-228 test plan.
- README, docs/README, and docs/12 reference TASK-228, docs/252, and docs/253.
- tests/CMakeLists registers `ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate`.
- CTest `shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout` is registered.
- TASK-220 through TASK-227 docs exist.
- TASK-220 through TASK-227 gate scripts exist.
- TASK-222 negative fixture index exists.
- All ten `NEG_MRF*.json` scaffold files exist.
- TASK-224 validator exists and validation behavior remains unchanged.
- TASK-225 regression matrix gate exists.
- TASK-226 failure-mode hardening gate exists.
- TASK-227 CI closeout gate exists.
- Critical CTest names are registered.
- Clean-main unsafe assertions are absent.
- No recursive full CTest runs inside phase closeout gates.
- Changed-path checking accepts clean main with an empty changed set.
- Non-empty changed paths must be exact allowlisted and fail closed otherwise.
- Fixture JSON unchanged.
- Validator validation behavior unchanged.
- Production code unchanged.
- No parser / loader / reader / replay implementation is added.
- No AccountingEngine replay call is added.
- No broker / network / credentials / endpoint is added.

## Required Probes

- Documentation existence probe.
- Documentation index probe.
- CTest directory registration probe.
- CTest name registration probe using `ctest -N`.
- TASK-220 through TASK-227 artifact inventory probe.
- TASK-222 negative fixture index parse probe.
- Ten `NEG_MRF` fixture existence probe.
- Expected issue-code mapping probe.
- TASK-224 validator existence probe.
- TASK-225 regression matrix gate existence probe.
- TASK-226 failure-mode hardening gate existence probe.
- TASK-227 CI closeout gate existence probe.
- Clean-main unsafe assertion scan.
- No recursive full CTest scan.
- Changed-path exact allowlist probe.
- Fixture JSON unchanged probe.
- Validator validation behavior unchanged probe.
- Production path unchanged probe.
- No replay / AccountingEngine replay probe.
- No broker / network / credentials / endpoint probe.
- External full CTest regression.
- External transport 50-repeat regression.
- `git diff --check` probe.

## Go / No-Go Checklist

Go only if:

- Phase closeout gate passes.
- TASK-220 through TASK-227 critical artifacts exist.
- TASK-220 through TASK-227 critical CTest names are registered.
- Clean-main unsafe assertions are absent.
- No recursive full CTest runs inside the gate.
- Fixture JSON unchanged.
- Validator validation behavior unchanged.
- Production code unchanged.
- No parser / loader / reader / replay implementation is added.
- No AccountingEngine replay call is added.
- No broker / network / credentials / endpoint is added.
- Full CTest is run externally and passes.
- Transport local socket echo passes 50 repeats.
- `git diff --check` passes.

No-Go if:

- Any required CTest pattern matches zero tests.
- The phase closeout gate depends on changed paths being non-empty.
- Fixture JSON changes.
- Validator validation behavior changes.
- Production code, QML, DataServiceActions, repositories, or migrations change.
- Parser, loader, reader, replay, or AccountingEngine replay behavior is added.
- Runtime SQL / SQLite / audit / ledger / snapshot write appears.
- Broker, network, credentials, endpoint, real order, or automatic trading appears.
