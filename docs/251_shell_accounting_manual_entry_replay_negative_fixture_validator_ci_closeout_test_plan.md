# ShellAccounting Manual Entry Replay Negative Fixture Validator CI Closeout Test Plan

## Document Purpose

This test plan defines TASK-227 closeout validation for the manual entry replay
negative fixture validator phase. It confirms that TASK-220 through TASK-226
are registered, clean-main-safe, fixture-immutable, production-safe, and
externally verifiable by full CTest.

## Test Matrix

- docs/250 exists and documents TASK-227 CI closeout scope.
- docs/251 exists and documents this test plan.
- README, docs/README, and docs/12 reference TASK-227, docs/250, and docs/251.
- tests/CMakeLists registers `ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate`.
- CTest `shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout` is registered.
- TASK-220 through TASK-226 critical docs, gates, scripts, and CTest names exist.
- TASK-222 negative fixture index exists.
- All ten `NEG_MRF*.json` scaffold files exist.
- TASK-224 validator exists and remains unchanged.
- TASK-225 regression matrix gate exists.
- TASK-226 failure-mode hardening gate exists.
- Known clean-main-unsafe changed-path assertions are absent.
- The closeout gate does not recursively run full CTest.
- Changed-path checking supports clean main with an empty changed set.
- Non-empty changed paths must be exact allowlisted and fail closed otherwise.
- Fixture JSON remains unchanged.
- Production code remains unchanged.
- No parser / loader / reader / replay implementation is added.
- No AccountingEngine replay call is added.
- No broker / network / credentials / endpoint path is added.

## Required Probes

- Documentation existence probe.
- Documentation index probe.
- CTest directory registration probe.
- CTest name registration probe using `ctest -N`.
- TASK-220 through TASK-226 file existence probe.
- Negative fixture index parse probe.
- Ten `NEG_MRF` fixture existence probe.
- Expected issue-code mapping probe.
- Clean-main unsafe assertion scan.
- No recursive full CTest scan.
- Changed-path exact allowlist probe.
- Fixture JSON unchanged probe.
- Validator unchanged probe.
- Production path unchanged probe.
- No replay / AccountingEngine replay probe.
- No broker / network / credentials / endpoint probe.
- External full CTest regression.
- External transport 50-repeat regression.
- `git diff --check` probe.

## Go / No-Go Checklist

Go only if:

- CI closeout gate passes.
- TASK-220 through TASK-226 critical gates are registered.
- Clean-main unsafe assertions are absent.
- No recursive full CTest runs inside the gate.
- Fixture JSON is unchanged.
- Validator validation behavior is unchanged.
- TASK-224 validator changed-set self-consistency is exact-path only.
- Production code is unchanged.
- No parser / loader / reader / replay implementation is added.
- No AccountingEngine replay call is added.
- No broker / network / credentials / endpoint is added.
- Full CTest is run externally and passes.
- Transport local socket echo passes 50 repeats.
- `git diff --check` passes.

No-Go if:

- Any required CTest pattern matches zero tests.
- The closeout gate depends on changed paths being non-empty.
- Fixture JSON changes.
- Validator files change.
- Production code, QML, DataServiceActions, repositories, or migrations change.
- Parser, loader, reader, replay, or AccountingEngine replay behavior is added.
- Runtime SQL / SQLite / audit / ledger / snapshot write appears.
- Broker, network, credentials, endpoint, real order, or automatic trading appears.
