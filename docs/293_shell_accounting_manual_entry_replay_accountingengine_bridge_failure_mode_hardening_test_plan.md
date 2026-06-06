# TASK-248 AccountingEngine bridge failure-mode hardening test plan

## Test Objective

Prove that the TASK-246 test-only AccountingEngine bridge fails closed, emits sanitized failure output, preserves the TASK-247 regression matrix contract, and does not introduce replay, production integration, fixture mutation, broker access, or runtime writes.

## Required Static Checks

- docs/292 and docs/293 exist with required sections.
- README, docs/README, and docs/12 reference TASK-248 and both new documents.
- tests/CMakeLists registers the TASK-248 gate directory.
- The TASK-248 CMakeLists registers `shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening`.
- The bridge script supports `--dry-run-summary-json`, `--implementation-summary-json`, `--summary-json`, `--failure-json`, and `--expect-no-accountingengine-call`.
- Bridge and gate sources do not import AccountingEngine, call replay, call `replayReadOnly`, write runtime SQL / SQLite / audit / ledger / snapshot, or access broker / network / credentials / endpoint.

## Required Regression Gates

Run the TASK-248 gate, TASK-247 bridge regression matrix, TASK-246 bridge implementation, TASK-245 bridge authorization, TASK-244 through TASK-239 AccountingEngine adequacy gates, replay implementation gates, dry-run harness gates, shell diagnostics, transport repeat, and full CTest.

## Changed-Path Allowlist

Changed-path detection must include staged, unstaged, and untracked files. Non-empty changed paths must be exact allowlisted. Clean main with an empty changed set must also pass. The allowlist must not use broad `docs/`, `tests/`, `apps/`, `libs/`, or `migrations/` wildcards.

## Failure-Mode Cases

The bridge failure-mode matrix must cover:

- missing input summary
- malformed JSON
- missing required fields
- non-synthetic flag
- `replayExecuted=true`
- `accountingEngineCalled=true`
- `runtimeWrite=true` or equivalent runtime write flags
- invalid output path
- forbidden token placeholder

## Sanitization Checks

Sanitized output must contain stable issue codes and must not echo raw input payloads, SQL, credentials, endpoints, broker payloads, production paths, or exception stacks.

## Acceptance Criteria

- TASK-248 gate prints at least 180 meaningful checks.
- Failure cases return non-zero and include stable issue codes.
- Success summary contract remains compatible with TASK-246 and TASK-247.
- Fixture JSON hashes are unchanged.
- No production source, AccountingEngine source, migrations, QML, repositories, DataServiceActions, broker, network, credentials, endpoint, real order placement, or automatic trading code changes are present.
- Full CTest, transport repeat, and `git diff --check` pass.

## Formal Conclusion

TASK-248 hardens test-only AccountingEngine bridge failure modes.
TASK-248 does not call AccountingEngine replay.
TASK-248 does not modify AccountingEngine code.
TASK-248 does not implement production bridge.
TASK-248 does not modify fixture JSON.
TASK-248 does not authorize production integration.
TASK-248 does not authorize runtime SQL / SQLite read/write.
TASK-248 does not authorize audit / ledger / snapshot writes.
TASK-248 does not authorize read model refresh.
TASK-248 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-249 ShellAccounting manual entry replay AccountingEngine bridge CI closeout gate.
