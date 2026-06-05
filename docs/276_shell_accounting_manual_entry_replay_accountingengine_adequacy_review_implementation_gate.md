# TASK-240 ShellAccounting manual entry replay AccountingEngine adequacy review implementation gate

## Scope

TASK-240 implements the AccountingEngine adequacy review implementation gate for the ShellAccounting manual entry replay chain.

Implementation in this task means review gate implementation only. It creates a deterministic, test-only, no-write CTest that verifies the adequacy review boundary after TASK-239. TASK-240 does not implement an AccountingEngine bridge, does not call AccountingEngine replay, and does not modify AccountingEngine code.

The gate records whether the TASK-233 through TASK-239 replay artifacts are ready for a separately authorized future AccountingEngine adequacy review. It does not perform production replay, production parser / loader / reader work, read model refresh, runtime SQL / SQLite access, audit / ledger / snapshot writes, broker access, real order placement, or automatic trading.

## Preconditions

Required predecessor tasks and artifacts:

- TASK-233 manual entry replay implementation authorization.
- TASK-234 test-only replay implementation.
- TASK-235 replay implementation regression matrix.
- TASK-236 replay implementation failure-mode hardening.
- TASK-237 replay implementation CI closeout.
- TASK-238 replay implementation phase closeout and handoff.
- TASK-239 AccountingEngine adequacy review authorization.

Required predecessor documents:

- `docs/262_shell_accounting_manual_entry_replay_implementation_authorization_gate.md`
- `docs/263_shell_accounting_manual_entry_replay_implementation_authorization_test_plan.md`
- `docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md`
- `docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md`
- `docs/266_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_gate.md`
- `docs/267_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_test_plan.md`
- `docs/268_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_gate.md`
- `docs/269_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_test_plan.md`
- `docs/270_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_gate.md`
- `docs/271_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_test_plan.md`
- `docs/272_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_gate.md`
- `docs/273_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_test_plan.md`
- `docs/274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md`
- `docs/275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md`
- `docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md`
- `docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md`

## Review Inputs

TASK-240 may statically inspect these test-only artifacts:

- `tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py`
- `tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py`
- `tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py`
- `tests/fixtures/manual_entry_replay/fixtures_index.json`
- `tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json`

The gate must not treat those inputs as production ledger facts. The gate must not mutate fixture JSON. The gate must not accept QML raw payload, DataService runtime payload, SQLite state, broker payload, network data, credentials, endpoints, command-line secrets, or environment secrets as replay facts.

## Static Adequacy Matrix

The implementation gate verifies these static review dimensions:

- Artifact inventory for TASK-233 through TASK-239 remains present.
- CTest registration exists for TASK-240 and predecessor gates.
- Clean-main changed-path behavior allows an empty changed set after merge.
- Development changed-path behavior includes unstaged, staged, and untracked files.
- Changed paths fail closed unless every path is exact allowlisted.
- Production code paths are not modified.
- `libs/AccountingEngine/` is not modified.
- Fixture JSON is not modified.
- No future AccountingEngine review runtime directory is created.
- No future AccountingEngine bridge directory is created.
- No production parser / loader / reader is created.
- No AccountingEngine replay call appears in the gate path.
- No runtime SQL / SQLite write appears in the gate path.
- No audit / ledger / snapshot write appears in the gate path.
- No read model refresh appears in the gate path.
- No broker / network / credentials / endpoint / order placement appears in the gate path.
- No recursive full CTest invocation appears inside the gate.

## Explicit Non-Goals

TASK-240 does not implement the future AccountingEngine adequacy review runtime.

TASK-240 does not implement an AccountingEngine bridge.

TASK-240 does not call AccountingEngine replay.

TASK-240 does not modify AccountingEngine source code.

TASK-240 does not implement production replay.

TASK-240 does not implement production parser / loader / reader behavior.

TASK-240 does not calculate real position, cash, PnL, market value, realized PnL, unrealized PnL, or ledger balances.

TASK-240 does not refresh read models.

TASK-240 does not create or update audit, ledger, snapshot, cash fact, trade fact, or SQLite state.

## Forbidden Side Effects

The gate must fail closed if TASK-240 changes:

- `apps/`
- `libs/`
- `migrations/`
- `tests/fixtures/manual_entry_replay/`
- `tests/fixtures/manual_entry_replay_negative/`

The gate must also fail closed if any TASK-240 artifact attempts:

- AccountingEngine replay calls.
- Runtime SQL / SQLite read or write.
- Audit / ledger / snapshot writes.
- Read model refresh.
- Broker SDK, broker adapter, network, credentials, endpoint, real order placement, or automatic trading.
- Recursive full CTest execution inside the gate.
- Fixture JSON mutation.

## Formal Conclusion

TASK-240 implements a review gate only. The gate gives a static Go / No-Go signal for whether the current test-only replay chain is ready for a separately authorized future AccountingEngine adequacy review.

The conclusion is not an AccountingEngine result. It is not a replay result. It is not ledger truth. It is not a production read model. It is not broker authorization.

## Go/No-Go Decision

Go only if:

- TASK-240 implementation gate passes.
- TASK-239 authorization gate passes.
- TASK-238 phase closeout gate passes.
- TASK-237 through TASK-233 replay gates pass.
- TASK-232 and TASK-231 dry-run harness gates pass.
- `shell_diagnostics_facade` passes.
- `transport_local_socket_echo` passes 50 times externally.
- External full CTest passes.
- `git diff --check` passes.
- Production code, AccountingEngine code, migrations, QML, fixture JSON, broker, network, credentials, endpoint, runtime write paths, and read model refresh paths remain unchanged.

No-Go if AccountingEngine bridge implementation, AccountingEngine replay calls, production replay, runtime SQL / SQLite writes, audit / ledger / snapshot writes, read model refresh, real position / cash / PnL calculation, fixture JSON mutation, production drift, broker / network / credential / endpoint capability, real order placement, or automatic trading appears without separate authorization.

## Next Phase Handoff

Future AccountingEngine bridge, runtime integration, replay implementation, read model refresh, production parser / loader / reader work, and any AccountingEngine replay call must be separately authorized in a later TASK.

Future TASKs must reference `docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md` and `docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md` before expanding the boundary.
