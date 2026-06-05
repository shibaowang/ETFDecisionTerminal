# TASK-244 ShellAccounting manual entry replay AccountingEngine adequacy review phase closeout and handoff gate

## Purpose

TASK-244 closes out the TASK-239 through TASK-243 AccountingEngine adequacy review phase and records the handoff boundary for the next authorization task. This is a review-only, test-only, no-write gate. It does not implement the bridge, does not call AccountingEngine replay, and does not authorize runtime integration.

## Relationship To TASK-239 Through TASK-243

TASK-239 added the AccountingEngine adequacy review authorization gate.

TASK-240 added the review-only adequacy review implementation gate.

TASK-241 added the adequacy review regression matrix gate.

TASK-242 added failure-mode hardening for the adequacy review gates.

TASK-243 added the CI closeout gate for the TASK-239 through TASK-242 chain.

TASK-244 records that this review phase is complete enough to hand off to a future authorization task, without changing any runtime boundary.

## Phase Closeout Scope

TASK-244 only records closeout evidence and static handoff rules. It adds documentation, a deterministic CTest gate, and index registration.

The CTest is `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout`.

The gate verifies docs, indexes, CTest registration, previous review-gate presence, clean-main changed-path behavior, exact changed-path allowlists, no bridge, no replay, no runtime write, no production drift, and no broker drift.

## Completed Review Gates

The phase closeout depends on these prior CTests remaining registered:

- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization`
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation`
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix`
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening`
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout`

The closeout also keeps the broader replay implementation gates in scope:

- `shell_accounting_manual_entry_replay_implementation_phase_closeout`
- `shell_accounting_manual_entry_replay_implementation_ci_closeout`
- `shell_accounting_manual_entry_replay_implementation_failure_mode_hardening`
- `shell_accounting_manual_entry_replay_implementation_regression_matrix`
- `shell_accounting_manual_entry_replay_implementation`
- `shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix`
- `shell_accounting_manual_entry_replay_test_only_dry_run_harness`

## CI Evidence

TASK-244 requires each targeted CTest pattern to be probed with `ctest -N -R` before execution. The gate itself must not invoke full CTest recursively and must not depend on test execution order.

Clean main may have an empty changed-path set. Feature branch work must keep every changed path inside the exact TASK-244 allowlist unless a later supplemental authorization names additional exact paths.

The phase closeout requires:

- configure succeeds.
- build succeeds.
- targeted AccountingEngine adequacy review gates succeed.
- replay implementation regression gates succeed.
- full CTest succeeds.
- `transport_local_socket_echo` succeeds with `--repeat until-fail:50`.
- `git diff --check` succeeds.
- final `git status --short` is clean after commit and push.

## Static Adequacy Findings

The static adequacy phase finds that the current repository has enough documented review evidence to authorize a future bridge authorization gate, but not enough authority to implement the bridge in TASK-244.

The current phase remains blocked from runtime integration until a future TASK explicitly authorizes:

- AccountingEngine bridge wiring.
- replay invocation.
- runtime SQL / SQLite read/write.
- audit / ledger / snapshot writes.
- read model refresh.
- production parser, loader, reader, or UI integration.

## Handoff Preconditions

Future bridge authorization work must reference this document and the test plan in `docs/285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md`.

The next task must prove:

- bridge scope is explicitly authorized.
- replay call sites are explicitly authorized.
- runtime read/write boundaries are explicitly authorized.
- sanitized failure mapping is preserved.
- no broker, network, credentials, endpoint, real order placement, or automatic trading capability is introduced unless separately authorized.

## Handoff Non-Goals

TASK-244 does not implement any handoff target. It only records the closeout and next-task guardrails.

TASK-244 does not add a parser, loader, reader, bridge adapter, replay runtime, read model refresh, ShellServices binding, production QML, broker integration, or schema change.

## No-Bridge Boundary

TASK-244 does not implement AccountingEngine bridge.

The future bridge must be a separate authorization task. It must not be inferred from the closeout gate, CI evidence, or documentation handoff.

## No-Replay Boundary

TASK-244 does not call AccountingEngine replay.

The gate must not invoke replay directly or indirectly. It must not call `AccountingReplayEngine`, `replayReadOnly`, a parser, a loader, a reader, or an AccountingEngine bridge.

## No-Write Boundary

TASK-244 does not authorize runtime SQL / SQLite read/write.

TASK-244 does not authorize audit / ledger / snapshot writes.

TASK-244 does not authorize read model refresh.

The gate must not write files, mutate fixtures, update databases, or create runtime outputs.

## AccountingEngine Code Boundary

TASK-244 does not modify AccountingEngine code.

`libs/AccountingEngine/` must remain unchanged. Any future AccountingEngine code change must be separately authorized.

## Production Integration Boundary

TASK-244 does not modify production QML, startup, Presenter, Controller, ShellServices, DataServiceActions, DataServiceActionRegistrar, DataAccess repositories, migrations, AccountingEngine, StrategyEngine, or MarketEngine.

Future production parser / loader / reader work must be separately authorized.

## Broker And Network Boundary

TASK-244 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

The closeout and handoff gate must not introduce broker SDK references, network calls, credentials, endpoints, real broker order ids, order placement, strategy execution, or automatic trading.

## Go / No-Go Decision

Go for TASK-244 means the review-only phase closeout gate passes and the prior adequacy review gates remain registered and passing.

No-Go for TASK-244 means any production code drift, AccountingEngine code drift, replay invocation, runtime write, fixture mutation, broker/network drift, missing documentation, missing CTest registration, or unauthorized changed path.

## Formal Conclusion

TASK-244 closes out the AccountingEngine adequacy review phase only.

TASK-244 does not call AccountingEngine replay.

TASK-244 does not implement AccountingEngine bridge.

TASK-244 does not implement replay.

TASK-244 does not modify AccountingEngine code.

TASK-244 does not authorize runtime SQL / SQLite read/write.

TASK-244 does not authorize audit / ledger / snapshot writes.

TASK-244 does not authorize read model refresh.

TASK-244 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-245 ShellAccounting manual entry replay AccountingEngine bridge authorization gate.

## Recommended Next Task

Recommended next task: TASK-245 ShellAccounting manual entry replay AccountingEngine bridge authorization gate.

TASK-245 must remain an authorization gate unless its prompt explicitly authorizes bridge implementation. It must not treat TASK-244 closeout as permission to call AccountingEngine replay.
