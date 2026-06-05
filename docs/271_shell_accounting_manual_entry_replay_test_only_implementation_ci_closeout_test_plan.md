# ShellAccounting Manual Entry Replay Test-Only Implementation CI Closeout Test Plan

## Document Purpose

This document defines the TASK-237 CI closeout validation plan for the ShellAccounting manual entry replay test-only implementation chain.

The plan verifies that TASK-233 through TASK-236 artifacts remain present, registered, clean-main safe, and bounded to test-only behavior.

## Test Matrix

- CI closeout gate registration.
- TASK-233 authorization gate artifact presence.
- TASK-234 test-only implementation artifact presence.
- TASK-235 regression matrix artifact presence.
- TASK-236 failure-mode hardening artifact presence.
- TASK-231 dry-run harness presence.
- TASK-232 dry-run harness regression matrix presence.
- TASK-224 negative fixture static validator presence.
- Critical CTest registration.
- Clean-main changed-path behavior.
- No recursive full CTest in closeout gate.
- No brittle changed-path assertion in critical gates.
- Fixture JSON immutability.
- Implementation success semantics unchanged.
- Dry-run harness business semantics unchanged.
- Validator validation behavior unchanged.
- Production code unchanged.
- No AccountingEngine replay.
- No runtime SQL / SQLite write.
- No audit / ledger / snapshot write.
- No read-model refresh.
- No broker / network / credentials / endpoint.
- No real order placement.
- No automatic trading.
- External full CTest regression.

## Required Probes

- `ctest -N` registration probe.
- docs/270 and docs/271 existence probe.
- README / docs/README / docs/12 index probe.
- TASK-233 through TASK-236 docs existence probe.
- TASK-233 through TASK-236 scripts existence probe.
- TASK-237 changed-path exact allowlist probe.
- Clean-main empty changed-path compatibility probe.
- Fixture JSON hash probe.
- apps / libs / migrations no-diff probe.
- Positive fixture JSON no-diff probe.
- Negative fixture JSON no-diff probe.
- Critical gate brittle assertion scan.
- Recursive full CTest token scan.
- AccountingEngine replay token scan.
- Runtime SQL / SQLite write token scan.
- Broker / network / credentials / endpoint token scan.
- DB/WAL/SHM artifact probe.

## Go / No-Go Checklist

Go only if:

- TASK-237 CI closeout gate passes.
- TASK-236 failure-mode hardening CTest passes.
- TASK-235 regression matrix CTest passes.
- TASK-234 implementation CTest passes.
- TASK-233 authorization gate passes.
- TASK-232 / TASK-231 / TASK-230 / TASK-224 gates pass.
- `shell_diagnostics_facade` passes.
- Broker and real broker gates pass.
- `transport_local_socket_echo` passes 50 times.
- External full CTest passes.
- `git diff --check` passes.
- Fixture JSON is unchanged.
- Implementation success semantics are unchanged.
- Dry-run harness business semantics are unchanged.
- Validator validation behavior is unchanged.
- Production code is unchanged.
- No AccountingEngine / no runtime writes / no real position cash PnL is observed.

No-Go if any validation requires fixture JSON changes, production code changes, AccountingEngine replay, runtime writes, broker/network integration, credentials, endpoints, real order placement, or automatic trading.
