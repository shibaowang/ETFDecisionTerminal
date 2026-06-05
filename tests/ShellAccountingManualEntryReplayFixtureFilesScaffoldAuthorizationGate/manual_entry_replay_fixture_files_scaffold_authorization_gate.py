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


def diff_text(root: Path, *paths: str) -> str:
    completed = subprocess.run(
        ["git", "diff", "main", "--", *paths],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return completed.stdout


def added_lines(diff: str) -> str:
    return "\n".join(line[1:] for line in diff.splitlines() if line.startswith("+") and not line.startswith("+++"))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    doc228_path = root / "docs" / "228_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_gate.md"
    doc229_path = root / "docs" / "229_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_test_plan.md"
    test_dir = root / "tests" / "ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate"
    test_cmake_path = test_dir / "CMakeLists.txt"
    test_py_path = test_dir / "manual_entry_replay_fixture_files_scaffold_authorization_gate.py"

    gate.require(doc228_path.exists(), "docs/228 exists")
    gate.require(doc229_path.exists(), "docs/229 exists")
    gate.require(test_cmake_path.exists(), "TASK-216 CMake exists")
    gate.require(test_py_path.exists(), "TASK-216 gate script exists")

    doc228 = read(doc228_path)
    doc229 = read(doc229_path)
    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    test_cmake = read(test_cmake_path)

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        gate.contains(text, "TASK-216", context)
        gate.contains(text, "228_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_gate.md", context)
        gate.contains(text, "229_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_test_plan.md", context)

    gate.contains(tests_cmake, "ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate", "tests/CMakeLists")
    gate.contains(test_cmake, "shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization", "TASK-216 CTest")

    for token in [
        "# ShellAccounting Manual Entry Replay Fixture Files Scaffold Authorization Gate",
        "TASK-216 is gate-only / scaffold-authorization-only.",
        "TASK-216 only authorizes future fixture files scaffold scope.",
        "TASK-216 does not create replay fixture files.",
        "TASK-216 does not create replay fixture JSON.",
        "TASK-216 does not create fixture index.",
        "TASK-216 does not create seed DB.",
        "TASK-216 does not create runtime fixture data.",
        "TASK-216 does not create parser / validator.",
        "TASK-216 does not implement replay.",
        "TASK-216 does not call AccountingEngine replay.",
        "TASK-216 does not modify production code.",
        "TASK-216 does not modify DataServiceActions.",
        "TASK-216 does not modify DataServiceActionRegistrar.",
        "TASK-216 does not modify repositories.",
        "TASK-216 does not modify migrations.",
        "TASK-216 does not modify QML / startup / Presenter / Controller / ShellServices adapter / port.",
        "TASK-216 does not add runtime SQL / SQLite read/write.",
        "TASK-216 does not write audit / ledger.",
        "TASK-216 does not write snapshot.",
        "TASK-216 does not implement backup/export/restore.",
        "TASK-216 does not connect broker / network / credentials / endpoint.",
        "TASK-216 does not place real orders.",
        "TASK-216 does not enable automatic trading.",
        "Broker sandbox new capability remains paused.",
        "## Scaffold Authorization Purpose",
        "## Future Scaffold Directory Boundary",
        "tests/fixtures/manual_entry_replay/",
        "TASK-216 does not create this directory.",
        "must not become a production data source",
        "## Future Scaffold File Set Boundary",
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json",
        "tests/fixtures/manual_entry_replay/MRF002_single_buy.json",
        "tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json",
        "tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json",
        "TASK-216 does not create any of these files.",
        "Future scaffold file creation must be a separate TASK.",
        "seed databases",
        "SQLite files",
        "WAL files",
        "SHM files",
        "runtime output",
        "real account data",
        "real broker data",
        "credentials",
        "endpoint values",
        "real order ids",
        "backup/export/restore payloads",
        "## Future Fixture Index Scaffold Boundary",
        "schemaVersion",
        "fixtureSet",
        "fixtures",
        "fixtureId",
        "title",
        "category",
        "file",
        "expectedStatus",
        "blockingExpected",
        "privacyStatus",
        "TASK-216 does not create `fixtures_index.json`.",
        "must not run replay",
        "read SQLite",
        "write SQLite",
        "call AccountingEngine replay",
        "trigger DataServiceActions behavior",
        "## Future Placeholder Content Boundary",
        "TODO_PLACEHOLDER",
        "SYNTHETIC_ACCOUNT",
        "SYNTHETIC_PORTFOLIO",
        "SYNTHETIC_INSTRUMENT",
        "SYNTHETIC_CURRENCY",
        "SANITIZED_MEMO",
        "UNAVAILABLE_EXPECTED",
        "real users",
        "real accounts",
        "real portfolios",
        "real instruments",
        "real orders",
        "real broker order ids",
        "raw QML payload",
        "raw DataService payload",
        "## Future Fixture JSON Schema Boundary",
        "purpose",
        "sourceFacts",
        "expectedOutputs",
        "expectedIssues",
        "privacyExpectations",
        "metadata",
        "TASK-216 does not create replay fixture JSON.",
        "does not create a JSON schema file",
        "parser",
        "validator",
        "loader",
        "fixture reader",
        "replay runner",
        "## Future Static Validation Boundary",
        "file presence",
        "field presence",
        "stable fixture ids",
        "placeholder-only content",
        "privacy tokens",
        "no seed DB files",
        "no runtime output files",
        "no production path dependencies",
        "TASK-216 does not implement static validation.",
        "Future parser or validator implementation must be a separate TASK",
        "## Privacy And Synthetic Data Boundary",
        "raw SQL",
        "raw payload",
        "full trade_log payload",
        "full cash_adjustment payload",
        "broker payload",
        "secret store values",
        "internal exception stacks",
        "real user data",
        "real account numbers",
        "Privacy expectations must be explicit.",
        "visibly synthetic",
        "not fabricated PnL",
        "fabricated market value",
        "fake ledger output",
        "## No-Runtime-Dependency Boundary",
        "must not add runtime dependencies",
        "DataServiceActions runtime behavior",
        "DataServiceActionRegistrar registration",
        "DataAccess repositories",
        "migrations",
        "AccountingEngine replay",
        "production QML",
        "startup",
        "Presenter",
        "Controller",
        "ShellServices adapter / port",
        "StrategyEngine",
        "MarketEngine",
        "broker adapter",
        "network code",
        "credentials handling",
        "endpoint handling",
        "runtime SQL / SQLite read/write",
        "audit / ledger writes",
        "snapshot writes",
        "backup/export/restore",
        "broker order placement",
        "real order id generation",
        "real broker order id generation",
        "strategy execution",
        "automatic trading",
        "## Out-of-Scope Boundaries",
        "creation of replay fixture files",
        "creation of replay fixture JSON",
        "creation of fixture index files",
        "fixture parser implementation",
        "fixture validator implementation",
        "fixture reader implementation",
        "replay implementation",
        "AccountingEngine replay calls",
        "runtime SQL / SQLite read/write",
        "production code changes",
        "DataServiceActions or DataServiceActionRegistrar changes",
        "repository changes",
        "migration changes",
        "audit / ledger writes",
        "snapshot writes",
        "real order placement",
        "automatic trading",
        "## Formal Authorization Conclusion And Next Task",
        "TASK-216 authorizes replay fixture files scaffold policy only.",
        "TASK-216 does not authorize creation of replay fixture files.",
        "TASK-216 does not authorize creation of replay fixture JSON.",
        "TASK-216 does not authorize creation of fixture index files.",
        "TASK-216 does not authorize parser or validator implementation.",
        "TASK-216 does not authorize replay implementation.",
        "TASK-216 does not authorize AccountingEngine replay calls.",
        "TASK-216 does not authorize runtime SQL / SQLite read/write.",
        "TASK-216 does not authorize audit / ledger writes.",
        "TASK-216 does not authorize snapshot writes.",
        "TASK-216 does not authorize backup/export/restore.",
        "TASK-216 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
        "Recommended next task: TASK-217 manual entry replay fixture files scaffold.",
    ]:
        gate.contains(doc228, token, "docs/228")

    for token in [
        "# ShellAccounting Manual Entry Replay Fixture Files Scaffold Authorization Test Plan",
        "## Document Purpose",
        "## Test Matrix",
        "## Required Probes",
        "## Go / No-Go Checklist",
        "docs/228 exists",
        "docs/229 exists",
        "Documentation registration",
        "Gate-only scope",
        "Scaffold authorization purpose",
        "Future scaffold directory boundary",
        "Future scaffold file set boundary",
        "Future fixture index scaffold boundary",
        "Future placeholder content boundary",
        "Future fixture JSON schema boundary",
        "Future static validation boundary",
        "Privacy and synthetic data boundary",
        "No-runtime-dependency boundary",
        "Out-of-scope boundaries",
        "Formal conclusion",
        "No fixture file creation",
        "No parser / validator creation",
        "Production drift",
        "TASK-215 regression",
        "TASK-214 regression",
        "TASK-213 regression",
        "TASK-212 regression",
        "TASK-211 regression",
        "TASK-209 regression",
        "TASK-207 regression",
        "TASK-204 regression",
        "TASK-198 regression",
        "TASK-196 regression",
        "TASK-192 regression",
        "Broker / real broker gates",
        "transport_local_socket_echo 50-repeat regression",
        "Static source scan",
        "Documentation token scan",
        "Changed-path allowlist scan",
        "Untracked-file detection scan",
        "No fixture directory creation scan",
        "No replay fixture JSON scan",
        "No fixture index file scan",
        "No seed DB or runtime fixture data scan",
        "No parser / validator implementation scan",
        "No loader / fixture reader / replay runner implementation scan",
        "No production code changed scan",
        "No production QML / startup / Presenter / Controller drift scan",
        "No ShellServices adapter / port drift scan",
        "No DataServiceActions drift scan",
        "No DataServiceActionRegistrar drift scan",
        "No DataAccess repository drift scan",
        "No migrations drift scan",
        "No new migration or schema file scan",
        "No runtime SQL / SQLite read/write scan",
        "No AccountingEngine replay implementation scan",
        "No AccountingEngine replay call scan",
        "No audit / ledger write scan",
        "No snapshot write scan",
        "No backup/export/restore implementation scan",
        "No broker / network / credentials / endpoint scan",
        "No real order / automatic trading scan",
        "Retained TASK-215 / TASK-214 / TASK-213 / TASK-212",
        "TASK-216 CTest is registered",
        "TASK-216 authorizes replay fixture files scaffold policy only",
        "No `tests/fixtures/manual_entry_replay/` directory is added",
        "No replay fixture JSON files are added",
        "No `fixtures_index.json` file is added",
        "No seed DB or runtime fixture data is added",
        "No parser, validator, loader, fixture reader, or replay runner is added",
        "Replay fixture files appear",
        "Replay fixture JSON appears",
        "Fixture index files appear",
        "Seed DB / runtime fixture data appears",
        "Fixture parser / validator / loader / reader implementation appears",
        "Replay implementation appears",
        "AccountingEngine replay call appears",
        "Runtime SQL / SQLite read/write appears",
        "DataServiceActions runtime behavior changes",
        "Repository / migration changes appear",
        "Audit / ledger write appears",
        "Snapshot write appears",
        "Backup/export/restore implementation appears",
        "Real order placement or broker order id appears",
        "Automatic trading appears",
        "Fabricated PnL or fabricated market value appears",
    ]:
        gate.contains(doc229, token, "docs/229")

    authorized_task217_fixture_scaffold_paths = {
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json",
        "tests/fixtures/manual_entry_replay/MRF002_single_buy.json",
        "tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json",
        "tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json",
    }
    authorized_task222_negative_fixture_scaffold_paths = {
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

    def is_authorized_task217_fixture_scaffold_path(path: str) -> bool:
        return path in authorized_task217_fixture_scaffold_paths

    allowed_changes = {


        "docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md",

        "docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md",

        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/CMakeLists.txt",

        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",

    "docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md",
    "docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",

        "docs/272_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_gate.md",
        "docs/273_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_test_plan.md",
        "docs/274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md",
        "docs/275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "docs/268_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_gate.md",
    "docs/269_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_test_plan.md",
    "docs/270_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_gate.md",
    "docs/271_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "docs/266_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_gate.md",
    "docs/267_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "docs/258_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_gate.md",
        "docs/259_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_test_plan.md",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/CMakeLists.txt",
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/228_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_gate.md",
        "docs/229_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_test_plan.md",
        "docs/230_shell_accounting_manual_entry_replay_fixture_files_scaffold.md",
        "docs/231_shell_accounting_manual_entry_replay_fixture_files_scaffold_test_plan.md",
        "docs/232_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_gate.md",
        "docs/233_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_test_plan.md",
        "docs/234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md",
        "docs/235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md",
        "docs/236_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_gate.md",
        "docs/237_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_test_plan.md",
        "docs/238_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_gate.md",
        "docs/239_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_test_plan.md",
        "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
        "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
        "docs/254_shell_accounting_manual_entry_replay_next_phase_authorization_planning_gate.md",
        "docs/255_shell_accounting_manual_entry_replay_next_phase_authorization_planning_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/CMakeLists.txt",
        "tests/CMakeLists.txt",
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json",
        "tests/fixtures/manual_entry_replay/MRF002_single_buy.json",
        "tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json",
        "tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/manual_entry_replay_fixture_static_validator.py",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
        "docs/240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md",
        "docs/241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md",
        "docs/242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md",
        "docs/243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md",
        "docs/244_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_gate.md",
        "docs/245_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_test_plan.md",
        "docs/246_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.md",
        "docs/247_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/CMakeLists.txt",
        "docs/248_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.md",
        "docs/249_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_test_plan.md",
        "docs/250_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_gate.md",
        "docs/251_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/CMakeLists.txt",
        "docs/256_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_gate.md",
        "docs/257_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/CMakeLists.txt",
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

        "docs/280_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.md",

        "docs/281_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_test_plan.md",

        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/CMakeLists.txt",

        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "docs/282_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.md",
    "docs/283_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
        "docs/284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md",
        "docs/285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
}
    allowed_changes = allowed_changes | authorized_task222_negative_fixture_scaffold_paths
    changes = changed_paths(root)
    task245_allowed_paths = {
        "docs/286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md",
        "docs/287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    }
    allowed_changes.update(task245_allowed_paths)
    unexpected = sorted(path for path in changes if path not in allowed_changes)
    gate.require(not unexpected, "TASK-216 changed unauthorized paths: " + ", ".join(unexpected))

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
        "tests/fixtures/manual_entry_replay/",
    ]
    for prefix in forbidden_prefixes:
        gate.require(
            not any(
                path.startswith(prefix) and not is_authorized_task217_fixture_scaffold_path(path)
                for path in changes
            ),
            f"TASK-216 must not change {prefix}",
        )

    forbidden_suffixes = [
        ".json",
        ".db",
        ".sqlite",
        ".sqlite3",
        ".wal",
        ".shm",
        ".sql",
    ]
    for path in changes:
        lowered = path.lower()
        gate.require(
            is_authorized_task217_fixture_scaffold_path(path)
            or path in authorized_task222_negative_fixture_scaffold_paths
            or not any(lowered.endswith(suffix) for suffix in forbidden_suffixes),
            f"TASK-216 must not add fixture/schema data file: {path}",
        )

    for forbidden_name in [
        "fixtures_index.json",
        "MRF001_empty_manual_facts.json",
        "MRF002_single_buy.json",
        "MRF003_buy_deposit_baseline.json",
        "MRF004_buy_sell_partial_reduction.json",
        "MRF005_deposit_withdrawal_baseline.json",
        "MRF006_daily_use_combined_baseline.json",
    ]:
        relative_path = f"tests/fixtures/manual_entry_replay/{forbidden_name}"
        gate.require(
            is_authorized_task217_fixture_scaffold_path(relative_path)
            or not (root / relative_path).exists(),
            f"TASK-216 must not create {forbidden_name}",
        )

    production_diff = added_lines(diff_text(root, "apps", "libs", "migrations"))
    for token in [
        "INSERT ",
        "UPDATE ",
        "DELETE ",
        "REPLACE ",
        "audit_log",
        "ledger",
        "snapshot",
        "backup",
        "export",
        "restore",
        "AccountingReplay",
        "runReplay",
        "manual_entry_replay",
        "FixtureReader",
        "FixtureParser",
        "FixtureValidator",
        "validator",
        "parser",
        "Broker",
        "broker",
        "network",
        "credentials",
        "endpoint",
        "realOrder",
        "brokerOrderId",
        "automaticTrading",
    ]:
        gate.require(token not in production_diff, f"production diff must not add `{token}`")

    forbidden_tree = root / "tests" / "fixtures" / "manual_entry_replay"
    if forbidden_tree.exists():
        actual_fixture_paths = {
            path.relative_to(root).as_posix()
            for path in forbidden_tree.iterdir()
            if path.is_file()
        }
        gate.require(
            actual_fixture_paths <= authorized_task217_fixture_scaffold_paths,
            "TASK-216 must not create unauthorized files under tests/fixtures/manual_entry_replay/",
        )
    else:
        gate.require(True, "TASK-216 fixture directory absent or only authorized TASK-217 scaffold files")

    fixture_data_patterns = [
        "tests/fixtures/**/*.json",
        "tests/fixtures/**/*.db",
        "tests/fixtures/**/*.sqlite",
        "tests/fixtures/**/*.sqlite3",
        "tests/fixtures/**/*.wal",
        "tests/fixtures/**/*.shm",
    ]
    for pattern in fixture_data_patterns:
        candidates = {
            p.relative_to(root).as_posix()
            for p in root.glob(pattern)
            if "manual_entry_replay" in p.as_posix()
        }
        unauthorized_candidates = candidates - authorized_task217_fixture_scaffold_paths - authorized_task222_negative_fixture_scaffold_paths
        gate.require(
            not unauthorized_candidates,
            f"TASK-216 must not create manual entry replay fixture data for pattern {pattern}",
        )

    retained_paths = [
        "docs/226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md",
        "docs/227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/CMakeLists.txt",
        "docs/224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md",
        "docs/225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/CMakeLists.txt",
        "docs/222_shell_accounting_manual_entry_replay_policy_authorization_gate.md",
        "docs/223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/CMakeLists.txt",
        "docs/220_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_gate.md",
        "docs/221_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualCashMovementRepositoryDualWriteImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualTransactionRepositoryWriteImplementation/CMakeLists.txt",
        "tests/ShellAccountingBrokerAdapterDisabledWiring/CMakeLists.txt",
        "tests/ShellAccountingBrokerOrderImplementation/CMakeLists.txt",
        "tests/ShellAccountingRealBrokerOrderAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingRealBrokerOrderImplementationGate/CMakeLists.txt",
        "tests/Transport/CMakeLists.txt",
    ]
    for path in retained_paths:
        gate.require((root / path).exists(), f"retained path exists: {path}")
    gate.contains(read(root / "tests" / "Transport" / "CMakeLists.txt"), "transport_local_socket_echo", "transport CTest")

    gate.require(gate.checks >= 110, "TASK-216 gate must execute at least 110 checks")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
