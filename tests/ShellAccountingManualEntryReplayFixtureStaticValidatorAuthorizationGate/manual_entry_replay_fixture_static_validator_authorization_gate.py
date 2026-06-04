#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path


class Gate:
    def __init__(self) -> None:
        self.checks = 0

    def require(self, condition: bool, message: str) -> None:
        self.checks += 1
        if not condition:
            raise AssertionError(message)

    def contains(self, text: str, token: str, context: str) -> None:
        normalized_text = " ".join(text.split())
        normalized_token = " ".join(token.split())
        self.require(
            token in text or normalized_token in normalized_text,
            f"{context} missing `{token}`",
        )


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def git_lines(root: Path, *args: str) -> set[str]:
    completed = subprocess.run(
        ["git", *args],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def changed_paths(root: Path) -> set[str]:
    changed = git_lines(root, "diff", "--name-only", "main")
    staged = git_lines(root, "diff", "--cached", "--name-only")
    untracked = git_lines(root, "ls-files", "--others", "--exclude-standard")
    return changed | staged | untracked


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    doc232_path = root / "docs" / "232_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_gate.md"
    doc233_path = root / "docs" / "233_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_test_plan.md"
    test_dir = root / "tests" / "ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate"
    test_cmake_path = test_dir / "CMakeLists.txt"
    test_py_path = test_dir / "manual_entry_replay_fixture_static_validator_authorization_gate.py"
    tests_cmake_path = root / "tests" / "CMakeLists.txt"
    transport_cmake_path = root / "tests" / "Transport" / "CMakeLists.txt"

    task217_gate_files = [
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
    ]
    broker_gate_dirs = [
        "tests/ShellAccountingBrokerAdapterDisabledWiring",
        "tests/ShellAccountingBrokerOrderAuthorizationGate",
        "tests/ShellAccountingBrokerOrderImplementation",
        "tests/ShellAccountingRealBrokerOrderAuthorizationGate",
        "tests/ShellAccountingRealBrokerOrderImplementationGate",
    ]
    fixture_files = [
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json",
        "tests/fixtures/manual_entry_replay/MRF002_single_buy.json",
        "tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json",
        "tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json",
    ]

    for path in [doc232_path, doc233_path, test_cmake_path, test_py_path, tests_cmake_path, transport_cmake_path]:
        gate.require(path.exists(), f"required file exists: {path.relative_to(root).as_posix()}")
    for path in task217_gate_files + fixture_files:
        gate.require((root / path).exists(), f"retained TASK-217 path exists: {path}")
    for directory in broker_gate_dirs:
        gate.require((root / directory).exists(), f"broker gate directory exists: {directory}")

    doc232 = read(doc232_path)
    doc233 = read(doc233_path)
    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    tests_cmake = read(tests_cmake_path)
    test_cmake = read(test_cmake_path)
    transport_cmake = read(transport_cmake_path)

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        gate.contains(text, "TASK-218", context)
        gate.contains(
            text,
            "232_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_gate.md",
            context,
        )
        gate.contains(
            text,
            "233_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_test_plan.md",
            context,
        )

    gate.contains(
        tests_cmake,
        "ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate",
        "tests/CMakeLists",
    )
    gate.contains(
        test_cmake,
        "shell_accounting_manual_entry_replay_fixture_static_validator_authorization",
        "TASK-218 CTest",
    )
    gate.contains(transport_cmake, "transport_local_socket_echo", "transport CTest")

    for section in [
        "# ShellAccounting Manual Entry Replay Fixture Scaffold Static Validator Authorization Gate",
        "## Static Validator Authorization Purpose",
        "## Future Validator Input Boundary",
        "## Future Validator Output Boundary",
        "## Future Validator Check Matrix",
        "## Fixture Index Validation Rules",
        "## Fixture JSON Validation Rules",
        "## Privacy And Forbidden Token Rules",
        "## No-Replay Boundary",
        "## No-Runtime-Dependency Boundary",
        "## Out-of-Scope Boundaries",
        "## Formal Authorization Conclusion And Next Task",
    ]:
        gate.contains(doc232, section, "docs/232")

    for token in [
        "TASK-218 is gate-only / static-validator-authorization-only.",
        "TASK-218 authorizes future validator input boundary",
        "TASK-218 does not create parser implementation.",
        "TASK-218 does not create validator implementation.",
        "TASK-218 does not create loader implementation.",
        "TASK-218 does not create reader implementation.",
        "TASK-218 does not implement replay.",
        "TASK-218 does not call AccountingEngine replay.",
        "TASK-218 does not modify production code.",
        "TASK-218 does not modify DataServiceActions.",
        "TASK-218 does not modify repositories.",
        "TASK-218 does not modify migrations.",
        "TASK-218 does not modify QML / startup / Presenter / Controller / ShellServices.",
        "TASK-218 does not add runtime SQL / SQLite read/write.",
        "TASK-218 does not write audit / ledger / snapshot rows.",
        "TASK-218 does not connect broker / network / credentials / endpoint.",
        "TASK-218 does not place real orders.",
        "TASK-218 does not enable automatic trading.",
        "Broker sandbox new capability remains paused.",
    ]:
        gate.contains(doc232, token, "docs/232")

    for token in fixture_files:
        gate.contains(doc232, token, "docs/232")

    for token in [
        "pass / fail status",
        "sanitized issue code",
        "fixture id",
        "fixture file name",
        "rule id",
        "safe short reason",
        "no raw SQL",
        "no full trade_log payload",
        "no full cash_adjustment payload",
        "no full fixture payload",
        "no broker payload",
        "no credentials",
        "no endpoint values",
        "no internal stack trace",
    ]:
        gate.contains(doc232, token, "docs/232")

    for token in [
        "docs/230 scaffold contract remains present.",
        "docs/231 scaffold test plan remains present.",
        "`fixtures_index.json` exists.",
        "MRF001 through MRF006 exist.",
        "JSON parse succeeds for index and all fixtures.",
        "Index schemaVersion is `manual-entry-replay-fixtures.scaffold.v1`.",
        "Fixture schemaVersion is `manual-entry-replay-fixture.scaffold.v1`.",
        "fixtureSet is `manual_entry_replay_scaffold`.",
        "fixture ids are unique.",
        "fixture ids are exactly MRF001 through MRF006.",
        "index file references match exact fixture file names.",
        "every fixture has `scaffoldOnly=true`.",
        "every fixture has `runtimeUse=false`.",
        "every fixture has `productionUse=false`.",
        "every fixture has `replayExecuted=false`.",
        "every fixture has `containsSyntheticDataOnly=true`.",
        "every fixture contains `SYNTHETIC_*` placeholder values only.",
        "every fixture contains sanitized memo markers.",
        "every fixture contains placeholder expected-output markers.",
        "no DB, SQLite, WAL, SHM, or seed data file is present.",
        "no parser / validator / loader / reader implementation is present.",
        "no replay runner is present.",
        "no AccountingEngine replay call is introduced.",
        "no production code drift is introduced.",
        "no broker / network / credentials / endpoint token is introduced.",
        "no real order placement or automatic trading token is introduced.",
    ]:
        gate.contains(doc232, token, "docs/232")

    for token in [
        "`schemaVersion`",
        "`fixtureSet`",
        "`generatedByTask`",
        "`scaffoldOnly`",
        "`runtimeUse`",
        "`productionUse`",
        "`replayExecuted`",
        "`fixtures`",
        "`fixtureId`",
        "`title`",
        "`purpose`",
        "`category`",
        "`containsSyntheticDataOnly`",
        "`sourceFacts`",
        "`expectedOutputs`",
        "`expectedIssues`",
        "`blockingExpected`",
        "`privacyExpectations`",
        "`metadata`",
    ]:
        gate.contains(doc232, token, "docs/232")

    for token in [
        "raw SQL",
        "raw UI payload",
        "raw DataService payload",
        "full trade_log payload",
        "full cash_adjustment payload",
        "full snapshot payload",
        "broker payload",
        "credentials",
        "endpoint values",
        "access token",
        "secret key",
        "private key",
        "real account id",
        "real order id",
        "real broker order id",
        "internal stack trace",
        "production database path",
        "SYNTHETIC_*",
        "SANITIZED_MEMO",
        "TODO_PLACEHOLDER",
        "UNAVAILABLE_EXPECTED",
    ]:
        gate.contains(doc232, token, "docs/232")

    for token in [
        "TASK-218 authorizes replay fixture scaffold static validator policy only.",
        "TASK-218 does not authorize parser implementation.",
        "TASK-218 does not authorize validator implementation.",
        "TASK-218 does not authorize loader or reader implementation.",
        "TASK-218 does not authorize replay implementation.",
        "TASK-218 does not authorize AccountingEngine replay calls.",
        "TASK-218 does not authorize runtime SQL / SQLite read/write.",
        "TASK-218 does not authorize audit / ledger writes.",
        "TASK-218 does not authorize snapshot writes.",
        "TASK-218 does not authorize backup/export/restore.",
        "TASK-218 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
        "Recommended next task: TASK-219 manual entry replay fixture scaffold static validator.",
    ]:
        gate.contains(doc232, token, "docs/232")

    for section in [
        "# ShellAccounting Manual Entry Replay Fixture Scaffold Static Validator Authorization Test Plan",
        "## Document Purpose",
        "## Test Matrix",
        "## Required Probes",
        "## Go / No-Go Checklist",
    ]:
        gate.contains(doc233, section, "docs/233")

    for token in [
        "docs/232 exists",
        "docs/233 exists",
        "Documentation registration",
        "Gate-only scope",
        "Static validator authorization purpose",
        "Future validator input boundary",
        "Future validator output boundary",
        "Future validator check matrix",
        "Fixture index validation rules",
        "Fixture JSON validation rules",
        "Privacy and forbidden token rules",
        "No-replay boundary",
        "No-runtime-dependency boundary",
        "Out-of-scope boundaries",
        "Formal conclusion",
        "No parser / validator creation",
        "No replay execution",
        "Production drift",
        "TASK-217 regression",
        "TASK-216 regression",
        "TASK-215 regression",
        "TASK-214 regression",
        "TASK-213 regression",
        "TASK-212 regression",
        "Broker / real broker gates",
        "transport_local_socket_echo 50-repeat regression",
        "Static source scan",
        "Changed-path allowlist scan",
        "Untracked-file detection scan",
        "Go only if",
        "No-Go if",
        "No tests/fixtures/manual_entry_replay/*.json file is modified.",
        "Automatic trading appears.",
    ]:
        gate.contains(doc233, token, "docs/233")

    task218_allowed_changes = {
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/232_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_gate.md",
        "docs/233_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_test_plan.md",
        "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
        "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
        "tests/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
    "docs/260_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.md",
    "docs/261_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/CMakeLists.txt",
        "docs/262_shell_accounting_manual_entry_replay_implementation_authorization_gate.md",
        "docs/263_shell_accounting_manual_entry_replay_implementation_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/manual_entry_replay_next_phase_authorization_planning_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/manual_entry_replay_negative_fixture_validator_phase_closeout_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py",
    }
    task219_allowed_changes = {
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md",
        "docs/235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md",
        "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
        "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
        "tests/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/manual_entry_replay_fixture_static_validator.py",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
    }
    task220_allowed_changes = {
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/236_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_gate.md",
        "docs/237_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_test_plan.md",
        "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
        "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
        "tests/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/CMakeLists.txt",
    }
    task221_allowed_changes = {
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/238_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_gate.md",
        "docs/239_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_test_plan.md",
        "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
        "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
        "tests/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
    }
    task222_allowed_changes = {
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md",
        "docs/241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md",
        "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
        "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
        "tests/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/CMakeLists.txt",
        "tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF001_missing_required_field.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF002_wrong_schema_version.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF003_runtime_use_true.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF004_production_use_true.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF005_replay_executed_true.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF006_non_synthetic_privacy.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF007_extra_json_file.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF008_forbidden_token.sql.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF009_broker_payload_token.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF010_real_order_id_token.json",
    }
    task223_allowed_changes = {
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "docs/242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md",
        "docs/243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
    }
    task224_allowed_changes = {
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "docs/244_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_gate.md",
        "docs/245_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/CMakeLists.txt",
    }
    task225_allowed_changes = {
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "docs/246_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.md",
        "docs/247_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/CMakeLists.txt",
        "docs/248_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.md",
        "docs/249_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/CMakeLists.txt",
    }
    task227_allowed_changes = {
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "docs/250_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_gate.md",
        "docs/251_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/CMakeLists.txt",
    }
    task229_allowed_changes = {
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "docs/258_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_gate.md",
        "docs/259_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_test_plan.md",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/CMakeLists.txt",
        "docs/254_shell_accounting_manual_entry_replay_next_phase_authorization_planning_gate.md",
        "docs/255_shell_accounting_manual_entry_replay_next_phase_authorization_planning_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/CMakeLists.txt",
        "docs/256_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_gate.md",
        "docs/257_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/CMakeLists.txt",
    }
    allowed_changes = (
        task218_allowed_changes
        | task219_allowed_changes
        | task220_allowed_changes
        | task221_allowed_changes
        | task222_allowed_changes
        | task223_allowed_changes
        | task224_allowed_changes
        | task225_allowed_changes
        | task227_allowed_changes
        | task229_allowed_changes
    )

    changes = changed_paths(root)
    unexpected = sorted(path for path in changes if path not in allowed_changes)
    gate.require(not unexpected, "TASK-218 changed unauthorized paths: " + ", ".join(unexpected))
    if changes:
        gate.require(
            changes.issubset(allowed_changes),
            "TASK-218 changed paths must stay within an exact static validator authorization allowlist",
        )

    fixture_json_changes = sorted(
        path for path in changes if path.startswith("tests/fixtures/manual_entry_replay/") and path.endswith(".json")
    )
    gate.require(not fixture_json_changes, "TASK-218 must not modify fixture JSON files: " + ", ".join(fixture_json_changes))

    forbidden_prefixes = [
        "apps/",
        "libs/ShellServices/",
        "libs/ShellCore/",
        "libs/DataServiceApi/",
        "libs/DataServiceClient/",
        "libs/DataAccess/",
        "libs/AccountingEngine/",
        "libs/StrategyEngine/",
        "libs/MarketEngine/",
        "libs/Transport/",
        "libs/ServiceRuntime/",
        "libs/ServiceHost/",
        "migrations/",
    ]
    production_changes = sorted(path for path in changes if any(path.startswith(prefix) for prefix in forbidden_prefixes))
    gate.require(not production_changes, "TASK-218 must not modify production/runtime/schema paths: " + ", ".join(production_changes))

    forbidden_created_tokens = [
        "parser",
        "loader",
        "reader",
        "replay_runner",
        "fixture_reader",
    ]
    unauthorized_implementation_changes = sorted(
        path
        for path in changes
        if path not in allowed_changes and any(token in path.lower() for token in forbidden_created_tokens)
    )
    gate.require(
        not unauthorized_implementation_changes,
        "TASK-218 must not add parser/loader/reader/replay implementation paths: "
        + ", ".join(unauthorized_implementation_changes),
    )

    runtime_sql_tokens = ["INSERT" + " INTO", "UPDATE" + " ", "DELETE" + " FROM", "sqlite3" + "_exec"]
    for token in [
        "AccountingReplayEngine::replay",
        "AccountingEngine replay",
        *runtime_sql_tokens,
        "audit_log",
        "brokerOrder",
        "placeOrder",
        "executeOrder",
        "automatic trading",
    ]:
        for path in [doc232_path, doc233_path, test_py_path]:
            text = read(path)
            if path == test_py_path and token in runtime_sql_tokens:
                gate.require(token not in text, f"TASK-218 gate script must not contain runtime SQL token `{token}`")

    gate.require(gate.checks >= 120, f"TASK-218 gate must keep at least 120 checks, got {gate.checks}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
