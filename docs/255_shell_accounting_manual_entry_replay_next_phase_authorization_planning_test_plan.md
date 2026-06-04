# ShellAccounting Manual Entry Replay Next-Phase Authorization Planning Test Plan

## Document Purpose

This test plan defines TASK-229 validation for the manual entry replay
next-phase authorization planning gate. It verifies that TASK-220 through
TASK-228 artifacts remain intact, TASK-229 planning documents are indexed,
future work tracks are documented as candidates only, and no replay, runtime,
fixture, validator, production, broker, network, real order, or automatic
trading behavior is added.

## Test Matrix

- docs/254 exists and documents TASK-229 next-phase planning scope.
- docs/255 exists and documents the TASK-229 test plan.
- README, docs/README, and docs/12 reference TASK-229, docs/254, and docs/255.
- tests/CMakeLists registers `ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate`.
- CTest `shell_accounting_manual_entry_replay_next_phase_authorization_planning` is registered.
- TASK-228 phase closeout artifacts exist.
- TASK-220 through TASK-228 critical artifacts exist.
- docs/254 contains next-phase scope.
- docs/254 contains future work tracks.
- docs/254 contains required preconditions.
- docs/254 contains a risk register.
- docs/254 contains an explicit non-authorization list.
- docs/254 contains Go / No-Go handoff rules.
- docs/255 contains Test Matrix, Required Probes, and Go / No-Go Checklist.
- No replay implementation path is added.
- No AccountingEngine replay call is added.
- No runtime SQL / SQLite read/write is added.
- No audit / ledger / snapshot write is added.
- No broker / network / credentials / endpoint behavior is added.
- Fixture JSON unchanged.
- Validator validation behavior unchanged.
- Production code unchanged.
- Changed-path checking accepts clean main with an empty changed set.
- Non-empty changed paths must be exact allowlisted and fail closed otherwise.
- Gate checks do not depend on changed paths being non-empty.

## Required Probes

- Documentation existence probe.
- Documentation index probe.
- CTest directory registration probe.
- CTest name registration probe using `ctest -N`.
- TASK-228 phase closeout artifact probe.
- TASK-220 through TASK-228 artifact inventory probe.
- Next-phase scope documentation probe.
- Future work track documentation probe.
- Required precondition documentation probe.
- Risk register documentation probe.
- Explicit non-authorization list probe.
- Go / No-Go handoff documentation probe.
- Changed-path exact allowlist probe.
- Clean-main empty changed-path support probe.
- Fixture JSON unchanged probe.
- Validator validation behavior unchanged probe.
- Production path unchanged probe.
- No replay / AccountingEngine replay probe.
- No runtime SQL / SQLite / audit / ledger / snapshot write probe.
- No broker / network / credentials / endpoint probe.
- External full CTest regression.
- External transport 50-repeat regression.
- `git diff --check` probe.

## Go / No-Go Checklist

Go only if:

- TASK-229 planning gate passes.
- TASK-228 phase closeout still passes.
- TASK-227 CI closeout still passes.
- TASK-226, TASK-225, and TASK-224 validator gates still pass.
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
- The planning gate depends on changed paths being non-empty.
- Fixture JSON changes.
- Validator validation behavior changes.
- Production code, QML, DataServiceActions, repositories, or migrations change.
- Parser, loader, reader, replay, or AccountingEngine replay behavior appears.
- Runtime SQL / SQLite / audit / ledger / snapshot write appears.
- Broker, network, credentials, endpoint, real order, or automatic trading appears.
