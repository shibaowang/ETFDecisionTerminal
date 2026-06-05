# ShellAccounting Manual Entry Replay Test-Only Implementation Phase Closeout And Handoff Test Plan

## Document Purpose

This test plan defines the TASK-238 phase closeout and handoff probes for the TASK-233 through TASK-237 manual entry replay test-only implementation chain.

## Test Matrix

- Phase closeout gate registration.
- TASK-233 through TASK-237 artifact inventory.
- TASK-233 through TASK-237 CTest inventory.
- Clean-main changed-path behavior.
- Exact-path changed-set fail-closed policy.
- No recursive full CTest inside the gate.
- No fixture JSON changes.
- Implementation success semantics unchanged.
- Dry-run harness business semantics unchanged.
- Validator validation behavior unchanged.
- No production parser / loader / reader.
- No production replay.
- No AccountingEngine replay.
- No runtime SQL / SQLite / audit / ledger / snapshot write.
- No read model refresh.
- No real position / cash / PnL calculation.
- No broker / network / credentials / endpoint.
- No real orders.
- No automatic trading.
- External full CTest regression.

## Required Probes

- `ctest -N` registration probe.
- docs/272 and docs/273 existence probe.
- README / docs/README / docs/12 index probe.
- TASK-233 through TASK-237 docs existence probe.
- TASK-233 through TASK-237 scripts existence probe.
- TASK-231 harness existence probe.
- TASK-232 regression matrix existence probe.
- TASK-224 validator existence probe.
- Clean-main brittle assertion scan.
- Recursive full CTest token scan.
- Fixture JSON hash immutability probe.
- Production path drift probe.
- Runtime SQL / SQLite token probe.
- AccountingEngine replay token probe.
- Broker / network / credentials / endpoint token probe.
- Changed-path exact allowlist probe.

## Go / No-Go Checklist

- TASK-238 phase closeout gate passes.
- TASK-237 CI closeout gate passes.
- TASK-236 failure-mode hardening CTest passes.
- TASK-235 regression matrix CTest passes.
- TASK-234 implementation CTest passes.
- TASK-233 authorization gate passes.
- TASK-232 dry-run regression matrix CTest passes.
- TASK-231 dry-run harness CTest passes.
- TASK-230 dry-run harness authorization CTest passes.
- TASK-224 negative fixture static validator CTest passes.
- Fixture JSON is unchanged.
- Implementation success semantics are unchanged.
- Dry-run harness business semantics are unchanged.
- Validator validation behavior is unchanged.
- Production code is unchanged.
- No AccountingEngine / no runtime writes / no real position cash PnL is observed.
- Broker / real broker gates pass.
- `transport_local_socket_echo` passes 50 times.
- External full CTest passes.
- `git diff --check` passes.
