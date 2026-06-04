# ShellAccounting Manual Entry Replay Negative Fixture Validator Phase Closeout And Handoff Gate

## Purpose

TASK-228 closes out the negative fixture validator phase from TASK-220 through
TASK-227. It records the artifact inventory, validation inventory, CTest
inventory, boundary inventory, handoff checklist, and next-phase entry
conditions for the manual entry replay negative fixture validator chain.

TASK-228 does not add validator behavior. TASK-228 does not modify fixture
JSON. TASK-228 does not implement production parser / loader / reader.
TASK-228 does not implement replay. TASK-228 does not call AccountingEngine
replay.

## Phase Scope

The phase covers TASK-220 through TASK-227:

- TASK-220 authorized negative fixture planning.
- TASK-221 authorized negative fixture scaffold planning.
- TASK-222 created the test-only negative fixture scaffold files.
- TASK-223 authorized the negative fixture static validator.
- TASK-224 implemented the test-only negative fixture static validator.
- TASK-225 added the regression matrix gate.
- TASK-226 hardened failure-mode behavior.
- TASK-227 added CI closeout coverage.

TASK-228 is a phase closeout and handoff gate only. It does not create new
fixture payloads, validator validation behavior, production replay behavior, or
runtime integration.

## Artifact Inventory

The required documentation artifacts are docs/236 through docs/253. The
required test artifacts include the TASK-220 through TASK-228 authorization,
scaffold, validator, regression, hardening, CI closeout, and phase closeout
gate scripts.

The required fixture artifacts are:

- `tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json`
- `tests/fixtures/manual_entry_replay_negative/NEG_MRF001_missing_required_field.json`
- `tests/fixtures/manual_entry_replay_negative/NEG_MRF002_wrong_schema_version.json`
- `tests/fixtures/manual_entry_replay_negative/NEG_MRF003_runtime_use_true.json`
- `tests/fixtures/manual_entry_replay_negative/NEG_MRF004_production_use_true.json`
- `tests/fixtures/manual_entry_replay_negative/NEG_MRF005_replay_executed_true.json`
- `tests/fixtures/manual_entry_replay_negative/NEG_MRF006_non_synthetic_privacy.json`
- `tests/fixtures/manual_entry_replay_negative/NEG_MRF007_extra_json_file.json`
- `tests/fixtures/manual_entry_replay_negative/NEG_MRF008_forbidden_token.sql.json`
- `tests/fixtures/manual_entry_replay_negative/NEG_MRF009_broker_payload_token.json`
- `tests/fixtures/manual_entry_replay_negative/NEG_MRF010_real_order_id_token.json`

## Validation Inventory

The phase requires the positive fixture static validator from TASK-219, the
negative fixture static validator from TASK-224, the TASK-225 regression
matrix, the TASK-226 failure-mode hardening gate, and the TASK-227 CI closeout
gate to remain registered and passing.

TASK-228 may allow exact-path changed-set self-consistency updates to the
TASK-224 test-only validator. That allowance is only for changed-path
allowlist self-consistency and does not authorize validator validation behavior
changes.

## CTest Inventory

The phase closeout inventory includes these critical CTest names:

```text
shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout
shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout
shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening
shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix
shell_accounting_manual_entry_replay_negative_fixture_static_validator
shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization
shell_accounting_manual_entry_replay_negative_fixture_scaffold_files
shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization
shell_accounting_manual_entry_replay_fixture_negative_fixtures_authorization
shell_accounting_manual_entry_replay_fixture_static_validator
shell_accounting_manual_entry_replay_fixture_static_validator_authorization
shell_diagnostics_facade
```

## Clean-Main Invariants

Clean main may have an empty changed-path set. Development branches may have
changed paths, but every changed path must be exact allowlisted and must fail
closed otherwise.

Critical gates must not rely on these clean-main-unsafe assertions:

- `changed path detection must include current TASK`
- `test-only parameter extension changed`
- `must include current TASK-`

The phase closeout gate may use `ctest -N` or CMakeLists text to inspect test
registration. It must not run full CTest from inside the gate.

## Boundary Inventory

TASK-228 does not modify production code / QML / DataServiceActions /
repositories / migrations. TASK-228 does not write runtime SQL / SQLite /
audit / ledger / snapshot. TASK-228 does not connect broker / network /
credentials / endpoint. TASK-228 does not place real orders or enable
automatic trading.

## Fixture Immutability Boundary

TASK-228 does not modify fixture JSON. Positive fixture JSON under
`tests/fixtures/manual_entry_replay/` and negative fixture JSON under
`tests/fixtures/manual_entry_replay_negative/` must remain unchanged.

## Validator Behavior Boundary

TASK-228 does not add validator behavior. TASK-228 does not modify validator
validation behavior. If the TASK-224 test-only validator needs a changed-path
allowlist self-consistency update for TASK-228 exact paths, that update must
not change input semantics, schema checks, issue-code mapping, sanitized
summary output, failure JSON behavior, or failure-mode behavior.

## Replay And AccountingEngine Boundary

TASK-228 does not implement replay. TASK-228 does not implement production
parser / loader / reader. TASK-228 does not call AccountingEngine replay and
does not add replay runner behavior.

## Broker And Network Boundary

TASK-228 does not connect broker / network / credentials / endpoint.
TASK-228 does not add broker SDK access, real broker order ids, real order
placement, or automatic trading.

## Handoff Checklist

- TASK-220 through TASK-227 documents exist.
- TASK-220 through TASK-227 gate scripts exist.
- TASK-222 negative fixture index and ten `NEG_MRF` JSON files exist.
- TASK-224 test-only validator exists and passes.
- TASK-225 regression matrix gate exists and passes.
- TASK-226 failure-mode hardening gate exists and passes.
- TASK-227 CI closeout gate exists and passes.
- TASK-228 phase closeout gate is registered and passes.
- Full CTest is run externally and passes.
- Transport local socket echo passes 50 repeats.
- `git diff --check` passes.

## Next-Phase Entry Conditions

The next phase may start only after TASK-228 passes, full CTest passes
externally, fixture JSON remains unchanged, validator validation behavior
remains unchanged, and production boundaries remain unchanged.

Future replay planning, parser implementation, loader implementation, reader
implementation, AccountingEngine replay integration, production runtime
integration, broker integration, real order placement, or automatic trading
must be separately authorized.

## Formal Conclusion And Next Task

TASK-228 formally closes the manual entry replay negative fixture validator
phase from TASK-220 through TASK-227 and hands off to next-phase planning.

Recommended next task: TASK-229 manual entry replay next-phase authorization
planning gate.
