# ShellAccounting Manual Entry Replay AccountingEngine Adequacy Review Authorization Test Plan

## Document Purpose

This test plan defines the TASK-239 static probes for the manual entry replay AccountingEngine adequacy review authorization gate. The plan keeps the future review separate from AccountingEngine bridge implementation, runtime replay, read model refresh, production integration, broker access, and runtime writes.

## Test Matrix

- TASK-239 authorization gate registration.
- docs/274 and docs/275 existence and index references.
- TASK-233 through TASK-238 artifact inventory remains present.
- Future adequacy review directory is documented but not created.
- Future adequacy review CTest is documented but not registered.
- Future adequacy review CTest candidate is documented only:
  `shell_accounting_manual_entry_replay_accountingengine_adequacy_review`.
- Future review input candidates are documentation only.
- AccountingEngine bridge is not implemented.
- AccountingEngine replay is not called.
- AccountingEngine code is not modified.
- Fixture JSON is unchanged.
- Implementation success semantics unchanged.
- Dry-run harness business semantics unchanged.
- Validator validation behavior unchanged.
- No production parser / loader / reader.
- No runtime SQL / SQLite read/write.
- No audit / ledger / snapshot writes.
- No read model refresh.
- No production position / cash / PnL calculation.
- No QML / startup / Presenter / Controller changes.
- No DataServiceActions / repositories / migrations changes.
- No broker / network / credentials / endpoint.
- No real order placement.
- No automatic trading.
- Clean-main changed-path behavior.
- Exact-path changed-set fail-closed behavior.
- External full CTest regression.

## Required Probes

- `ctest -N` registration probe for the TASK-239 authorization CTest.
- docs/274 and docs/275 existence probe.
- README / docs/README / docs/12 index probe.
- tests/CMakeLists registration probe.
- TASK-233 through TASK-238 docs existence probe.
- TASK-233 through TASK-238 scripts existence probe.
- TASK-231 dry-run harness existence probe.
- TASK-224 validator existence probe.
- Future review directory absence probe.
- Future review CTest absence probe.
- Future review CTest name documentation probe.
- Future review input candidate documentation probe.
- Clean-main empty changed-path support probe.
- Changed-path exact allowlist probe.
- Unauthorized changed-path fail-closed probe.
- Fixture JSON immutability probe.
- AccountingEngine code drift probe.
- AccountingEngine replay token probe.
- Runtime SQL / SQLite token probe.
- Audit / ledger / snapshot token probe.
- Read model refresh token probe.
- Broker / network / credentials / endpoint token probe.
- No recursive full CTest token scan inside the gate.

## Go / No-Go Checklist

- TASK-239 authorization gate passes.
- Future review directory is not created.
- Future review CTest is not registered.
- AccountingEngine bridge is not implemented.
- AccountingEngine replay is not called.
- Fixture JSON unchanged.
- Implementation / harness / validator semantics unchanged.
- Production code unchanged.
- TASK-238 phase closeout gate passes.
- TASK-237 CI closeout gate passes.
- TASK-236 failure-mode hardening CTest passes.
- TASK-235 regression matrix CTest passes.
- TASK-234 implementation CTest passes.
- TASK-233 authorization gate passes.
- TASK-232 dry-run regression matrix CTest passes.
- TASK-231 dry-run harness CTest passes.
- TASK-224 negative fixture static validator CTest passes.
- Broker / real broker gates pass.
- `transport_local_socket_echo` passes 50 times.
- External full CTest passes.
- `git diff --check` passes.
