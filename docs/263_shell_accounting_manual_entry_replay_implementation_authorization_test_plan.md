# ShellAccounting Manual Entry Replay Implementation Authorization Test Plan

## Document Purpose

This test plan defines TASK-233 validation for the manual entry replay
implementation authorization gate. It verifies that future implementation
scope is documented while this task still adds no replay implementation, no
AccountingEngine replay call, no runtime SQL / SQLite read/write, no audit /
ledger / snapshot write, no production integration, and no broker behavior.

## Test Matrix

- docs/262 exists and documents TASK-233.
- docs/263 exists and documents this test plan.
- README, docs/README, and docs/12 reference TASK-233, docs/262, and docs/263.
- tests/CMakeLists registers
  `ShellAccountingManualEntryReplayImplementationAuthorizationGate`.
- CTest `shell_accounting_manual_entry_replay_implementation_authorization` is
  registered.
- TASK-232 regression matrix CTest remains registered.
- TASK-231 dry-run harness CTest remains registered.
- TASK-230 authorization gate remains registered.
- TASK-229 planning gate remains registered.
- TASK-228 phase closeout gate remains registered.
- TASK-224 negative fixture static validator remains registered.
- Future directory `tests/ShellAccountingManualEntryReplayImplementation/`
  remains absent.
- Future CTest `shell_accounting_manual_entry_replay_implementation` remains
  unregistered.
- Future implementation script remains absent.
- Future input boundary is documented.
- Future output boundary is documented.
- AccountingEngine boundary is documented.
- No-write boundary is documented.
- Runtime SQL and ledger boundary is documented.
- Production integration boundary is documented.
- Broker and network boundary is documented.
- Explicit non-authorization list is documented.
- Changed paths support clean-main empty state.
- Non-empty changed paths must be exact allowlisted.
- Fixture JSON remains unchanged.
- TASK-231 harness business semantics remain unchanged.
- TASK-224 validator validation behavior remains unchanged.
- No parser / loader / reader / replay implementation appears.
- No AccountingEngine replay call appears.
- No runtime SQL / SQLite / audit / ledger / snapshot write appears.
- No real position / cash / PnL calculation appears.
- No production code path changes appear.
- No QML, DataServiceActions, repositories, or migrations changes appear.
- No broker / network / credentials / endpoint behavior appears.
- No real order placement or automatic trading appears.
- The gate check count is at least 170.
- The gate does not recursively run full CTest.

## Required Probes

- Documentation existence probe.
- Documentation section probe.
- Documentation index probe.
- CTest directory registration probe.
- CTest name registration probe using `ctest -N`.
- Future implementation directory absent probe.
- Future implementation CTest absent probe.
- Future implementation script absent probe.
- Fixture hash before / after probe.
- Changed-path exact allowlist probe.
- Clean-main empty changed-path probe.
- Production path unchanged probe.
- Harness sanitized summary probe.
- Harness no-replay / no-AccountingEngine / no-runtime-write flag probe.
- Validator registration probe.
- Validator validation behavior immutability probe.
- No parser / loader / reader / replay probe.
- No AccountingEngine replay probe.
- No runtime SQL / SQLite probe.
- No audit / ledger / snapshot write probe.
- No QML / DataServiceActions / repositories / migrations probe.
- No broker / network / credentials / endpoint probe.
- No recursive full CTest probe.
- External full CTest regression.
- External transport 50-repeat regression.
- `git diff --check` probe.

## Go / No-Go Checklist

Go only if:

- TASK-233 implementation authorization gate passes.
- TASK-232 regression matrix gate passes.
- TASK-231 dry-run harness CTest passes.
- TASK-230 authorization gate passes.
- TASK-229 planning gate passes.
- TASK-228 phase closeout gate passes.
- TASK-224 negative fixture static validator passes.
- Future replay implementation directory remains absent.
- Future replay implementation CTest remains unregistered.
- Fixture JSON remains unchanged.
- TASK-231 harness business semantics remain unchanged.
- TASK-224 validator validation behavior remains unchanged.
- Production code remains unchanged.
- No parser / loader / reader / replay implementation is added.
- No AccountingEngine replay call is added.
- No runtime SQL / SQLite / audit / ledger / snapshot write is added.
- No real position / cash / PnL calculation is added.
- No broker / network / credentials / endpoint behavior is added.
- Full CTest is run externally and passes.
- Transport local socket echo passes 50 repeats.
- `git diff --check` passes.

No-Go if:

- Any required CTest pattern matches zero tests.
- The gate depends on changed paths being non-empty.
- Fixture JSON changes.
- Replay implementation appears in this task.
- The future implementation directory is created.
- The future implementation CTest is registered.
- AccountingEngine replay is called.
- Runtime SQL / SQLite / audit / ledger / snapshot writes appear.
- Production code, QML, DataServiceActions, repositories, or migrations change.
- Broker, network, credentials, endpoint, real order, or automatic trading appears.
