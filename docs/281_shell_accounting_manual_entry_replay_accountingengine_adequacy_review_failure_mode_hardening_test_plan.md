# TASK-242 AccountingEngine adequacy review failure-mode hardening test plan

## Test Objective

TASK-242 verifies that the TASK-240 and TASK-241 review-only AccountingEngine
adequacy gates fail closed under controlled bad inputs while preserving their
normal success behavior. The test plan is documentation and CTest only.

## Failure Matrix

- Missing TASK-240 implementation docs.
- Missing TASK-241 regression matrix docs.
- Missing Formal Conclusion in TASK-240 documentation.
- Missing No-Bridge, No-Write, or Go/No-Go matrix sections in TASK-241
  documentation.
- Missing README, docs/README, or tests/CMakeLists references.
- Unauthorized changed paths under apps, libs/AccountingEngine, migrations, or
  fixture JSON.
- Forbidden static tokens for AccountingEngine replay, replayReadOnly,
  SQLite write, audit / ledger / snapshot write, broker, network, and order
  placement.

## Temporary Input Probes

The gate uses temporary git roots copied from the current workspace. Each probe
mutates only that temporary root, runs the relevant review gate with a
test-only source-root override, and requires sanitized nonzero failure. The
real workspace fixture JSON and production files remain unchanged.

## Changed-Path Allowlist

The TASK-242 changed-path allowlist is exact-file only. It may include TASK-242
docs, TASK-242 CTest registration, the TASK-242 gate script, and minimal
test-only support in TASK-240 / TASK-241 gates for sanitized failure output and
temporary source-root failure probes. It must not include directory-level
allowlist entries or wildcards.

## Regression Gates

Each targeted CTest pattern must be probed first with `ctest -N -R` and must
match at least one registered test. Required regression gates include
TASK-242, TASK-241, TASK-240, TASK-239, replay implementation closeout,
replay implementation failure-mode and regression matrix, dry-run harness,
shell diagnostics facade, and transport local socket echo.

## Acceptance Criteria

- TASK-242 CTest passes.
- TASK-240 and TASK-241 gates still pass on the real source root.
- Temporary bad docs and changed-set probes fail closed.
- Failure output is sanitized.
- Gate check count is at least 80.
- No recursive full CTest invocation exists inside the gate.
- No production code, fixture JSON, migrations, AccountingEngine code, broker,
  network, credentials, endpoint, real order placement, or automatic trading
  code changes are introduced.

## Formal Conclusion

TASK-242 hardens failure-mode behavior for review-only AccountingEngine adequacy gates.
TASK-242 does not call AccountingEngine replay.
TASK-242 does not implement AccountingEngine bridge.
TASK-242 does not implement replay.
TASK-242 does not modify AccountingEngine code.
TASK-242 does not authorize runtime SQL / SQLite read/write.
TASK-242 does not authorize audit / ledger / snapshot writes.
TASK-242 does not authorize read model refresh.
TASK-242 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-243 ShellAccounting manual entry replay AccountingEngine adequacy review CI closeout gate.
