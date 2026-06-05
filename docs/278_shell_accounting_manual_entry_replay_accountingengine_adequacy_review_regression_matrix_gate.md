# TASK-241 ShellAccounting manual entry replay AccountingEngine adequacy review regression matrix gate

## Purpose

TASK-241 adds a review-only regression matrix for the ShellAccounting manual entry replay AccountingEngine adequacy review boundary.

The gate records static review inputs, matrix rows, forbidden boundaries, and Go / No-Go conclusions after TASK-240. It is deterministic, test-only, no-write, and does not execute replay.

## Relationship To TASK-239 And TASK-240

TASK-239 authorized AccountingEngine adequacy review planning only.

TASK-240 implemented the AccountingEngine adequacy review implementation gate only.

TASK-241 adds the regression matrix for that review gate. It does not expand the boundary into AccountingEngine bridge implementation, AccountingEngine replay calls, runtime integration, read model refresh, production parser / loader / reader work, or ledger writes.

## Matrix Scope

The matrix covers these static review dimensions:

- Review input inventory.
- Static adequacy rows.
- No-bridge boundary.
- No-replay boundary.
- No-write boundary.
- Runtime SQL and ledger boundary.
- AccountingEngine code boundary.
- Production integration boundary.
- Go / No-Go decision rows.

The matrix is a review artifact. It is not ledger truth, not an AccountingEngine replay result, not a production read model, and not broker authorization.

## Review Input Matrix

| Input | Required Review Use | Boundary |
| --- | --- | --- |
| TASK-240 implementation summary | Verify the review gate conclusion remains test-only and no-write. | No AccountingEngine call. |
| TASK-231 dry-run summary | Verify dry-run replay harness remains test-only. | No runtime integration. |
| Positive fixture index | Verify fixture inventory is available for review. | No fixture JSON mutation. |
| Negative fixture index | Verify negative fixture inventory is available for review. | No validator semantic change. |
| TASK-239 authorization docs | Verify authorization remains planning-only. | No bridge authorization. |
| TASK-240 implementation docs | Verify implementation means review gate implementation only. | No replay implementation. |

The review input matrix must not accept QML raw payload, DataService runtime payload, SQLite state, broker payload, network data, credentials, endpoints, command-line secrets, or environment secrets as replay facts.

The review input matrix explicitly includes the negative fixture index as a review input only.

## Static Adequacy Matrix

| Matrix Row | Required Conclusion |
| --- | --- |
| Artifact inventory | TASK-239 and TASK-240 docs and gates remain present. |
| CTest registration | TASK-241 regression matrix gate is registered by CMake. |
| Review inputs | Implementation summary, dry-run summary, and fixture indexes are review inputs only. |
| Clean-main handling | Changed paths may be empty after merge. |
| Development changed paths | Staged, unstaged, and untracked files are collected. |
| Exact allowlist | Changed paths fail closed unless exact allowlisted. |
| Production drift | `apps/`, `libs/`, and `migrations/` remain unchanged. |
| Fixture drift | Fixture JSON remains unchanged. |
| AccountingEngine drift | `libs/AccountingEngine/` remains unchanged. |
| Side effects | No runtime SQL, ledger, audit, snapshot, broker, network, endpoint, order, or automatic trading side effect is authorized. |

## No-Bridge Boundary

TASK-241 does not implement AccountingEngine bridge.

No AccountingEngine bridge directory, bridge CTest, production bridge adapter, parser, loader, reader, or runtime integration path is authorized by this task.

## No-Replay Boundary

TASK-241 does not call AccountingEngine replay.

TASK-241 does not implement replay. It does not call `AccountingReplayEngine`, `replayLedger`, `replayReadOnly`, or any equivalent AccountingEngine replay entrypoint.

## No-Write Boundary

TASK-241 does not write runtime SQL or SQLite state.

TASK-241 does not write audit rows, ledger rows, snapshot rows, cash facts, trade facts, read model rows, temporary database files, WAL files, or SHM files.

## Runtime SQL And Ledger Boundary

TASK-241 does not authorize runtime SQL / SQLite read/write.

TASK-241 does not authorize audit / ledger / snapshot writes.

TASK-241 does not authorize read model refresh.

## AccountingEngine Code Boundary

TASK-241 does not modify AccountingEngine code.

The matrix must fail closed if `libs/AccountingEngine/` is changed by this task.

## Production Integration Boundary

TASK-241 does not modify production code, production QML, DataServiceActions, repositories, migrations, ShellServices, Presenter / Controller paths, or production startup.

TASK-241 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

## Go/No-Go Matrix

Go only if:

- TASK-241 regression matrix gate passes.
- TASK-240 adequacy review implementation gate passes.
- TASK-239 adequacy review authorization gate passes.
- TASK-238 through TASK-233 replay gates pass.
- TASK-232 and TASK-231 dry-run gates pass.
- `shell_diagnostics_facade` passes.
- `transport_local_socket_echo` passes externally.
- External full CTest passes.
- `git diff --check` passes.
- Production code, AccountingEngine code, migrations, QML, fixture JSON, broker, network, credentials, endpoint, runtime writes, replay calls, bridge implementation, and read model refresh paths remain unchanged.

No-Go if AccountingEngine bridge implementation, AccountingEngine replay calls, production replay, runtime SQL / SQLite writes, audit / ledger / snapshot writes, read model refresh, real position / cash / PnL calculation, fixture JSON mutation, production drift, broker / network / credential / endpoint capability, real order placement, or automatic trading appears without separate authorization.

## Formal Conclusion

TASK-241 adds a review-only regression matrix for AccountingEngine adequacy review.

TASK-241 does not call AccountingEngine replay.

TASK-241 does not implement AccountingEngine bridge.

TASK-241 does not implement replay.

TASK-241 does not modify AccountingEngine code.

TASK-241 does not authorize runtime SQL / SQLite read/write.

TASK-241 does not authorize audit / ledger / snapshot writes.

TASK-241 does not authorize read model refresh.

TASK-241 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-242 ShellAccounting manual entry replay AccountingEngine adequacy review failure-mode hardening gate.

## Next Phase Handoff

Future AccountingEngine adequacy review failure-mode hardening must reference `docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md` and `docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md`.

Any future AccountingEngine bridge, runtime integration, replay implementation, read model refresh, production parser / loader / reader work, or AccountingEngine replay call must be separately authorized in a later TASK.
