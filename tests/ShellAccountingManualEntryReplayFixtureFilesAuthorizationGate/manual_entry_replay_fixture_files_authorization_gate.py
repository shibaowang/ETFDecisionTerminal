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

    doc226_path = root / "docs" / "226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md"
    doc227_path = root / "docs" / "227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md"
    test_dir = root / "tests" / "ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate"
    test_cmake_path = test_dir / "CMakeLists.txt"
    test_py_path = test_dir / "manual_entry_replay_fixture_files_authorization_gate.py"

    gate.require(doc226_path.exists(), "docs/226 exists")
    gate.require(doc227_path.exists(), "docs/227 exists")
    gate.require(test_cmake_path.exists(), "TASK-215 CMake exists")
    gate.require(test_py_path.exists(), "TASK-215 gate script exists")

    doc226 = read(doc226_path)
    doc227 = read(doc227_path)
    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        gate.contains(text, "TASK-215", context)
        gate.contains(text, "226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md", context)
        gate.contains(text, "227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md", context)

    gate.contains(tests_cmake, "ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate", "tests/CMakeLists")
    gate.contains(read(test_cmake_path), "shell_accounting_manual_entry_replay_fixture_files_authorization", "TASK-215 CTest")

    for token in [
        "# ShellAccounting Manual Entry Replay Fixture Files Authorization Gate",
        "TASK-215 is gate-only / fixture-files-authorization-only.",
        "TASK-215 authorizes replay fixture files contract only.",
        "TASK-215 does not create replay fixture files.",
        "TASK-215 does not create replay fixture JSON.",
        "TASK-215 does not create seed DB.",
        "TASK-215 does not create runtime fixture data.",
        "TASK-215 does not implement replay.",
        "TASK-215 does not call AccountingEngine replay.",
        "TASK-215 does not modify production code.",
        "TASK-215 does not modify DataServiceActions.",
        "TASK-215 does not modify DataServiceActionRegistrar.",
        "TASK-215 does not modify repositories.",
        "TASK-215 does not modify migrations.",
        "TASK-215 does not modify QML / startup / Presenter / Controller / ShellServices adapter / port.",
        "TASK-215 does not add runtime SQL / SQLite read/write.",
        "TASK-215 does not write audit / ledger.",
        "TASK-215 does not write snapshot.",
        "TASK-215 does not implement backup/export/restore.",
        "TASK-215 does not connect broker / network / credentials / endpoint.",
        "TASK-215 does not place real orders.",
        "TASK-215 does not enable automatic trading.",
        "Broker sandbox new capability remains paused.",
        "## Fixture Files Authorization Purpose",
        "inherits the TASK-214 fixture matrix",
        "test input and expected-output acceptance material",
        "must not become a production runtime data source",
        "## Future Fixture Directory Policy",
        "tests/fixtures/manual_entry_replay/",
        "TASK-215 does not create this directory.",
        "must be separately authorized",
        "must not be read by production code",
        "credentials",
        "endpoint values",
        "broker payloads",
        "real account data",
        "real order data",
        "SQLite DB files",
        "WAL files",
        "SHM files",
        "runtime output",
        "## Future Fixture File Naming Policy",
        "MRF001_empty_manual_facts.json",
        "MRF002_single_buy.json",
        "MRF003_buy_deposit_baseline.json",
        "MRF004_buy_sell_partial_reduction.json",
        "MRF005_deposit_withdrawal_baseline.json",
        "MRF006_daily_use_combined_baseline.json",
        "TASK-215 does not create these files.",
        "Future fixture ids must be stable",
        "must not reuse meanings",
        "lowercase snake_case",
        "real account numbers",
        "real names",
        "broker names",
        "endpoint values",
        "credentials",
        "real order ids",
        "must not depend on QML display order",
        "must not depend on test execution order",
        "## Future Fixture Index Policy",
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "TASK-215 does not create this index file.",
        "fixture id",
        "title",
        "category",
        "source file",
        "expected output file or inline expected block",
        "privacy status",
        "blocking status",
        "validate unique fixture ids",
        "stable file names",
        "## Future Fixture JSON Schema Policy",
        "schemaVersion",
        "fixtureId",
        "purpose",
        "sourceFacts",
        "expectedOutputs",
        "expectedIssues",
        "blockingExpected",
        "privacyExpectations",
        "metadata",
        "accounts",
        "portfolios",
        "instruments",
        "tradeExecutionGroups",
        "tradeLogRows",
        "cashAdjustmentRows",
        "requestTrace",
        "positions",
        "cashSummary",
        "portfolioPnlSummary",
        "unavailableFields",
        "calculationMetadata",
        "code",
        "severity",
        "messageToken",
        "blocking",
        "sourceRef",
        "does not create a schema file",
        "does not implement a parser",
        "does not implement a validator",
        "does not execute replay",
        "does not write SQLite",
        "## Future Fixture Content Policy",
        "persisted manual facts candidates only",
        "`trade_execution_group` plus `trade_log`",
        "`trade_log` plus `cash_adjustment`",
        "raw QML payload",
        "DataService raw request body",
        "broker payload",
        "real order id",
        "credentials",
        "endpoint",
        "raw SQL",
        "real user data",
        "backup/export payload",
        "synthetic and sanitized",
        "## Future Fixture Case Coverage Policy",
        "baseline success fixtures",
        "ordering and idempotency fixtures",
        "position safety fixtures",
        "cash safety fixtures",
        "PnL and market dependency fixtures",
        "multi-scope fixtures",
        "unsupported and failure fixtures",
        "MRF001",
        "MRF006",
        "## Privacy And Redaction Policy",
        "internal exception stack",
        "full `trade_log` payload",
        "full `cash_adjustment` payload",
        "real broker order ids",
        "synthetic account / portfolio / instrument identifiers",
        "sanitized issue code",
        "sanitized memo",
        "safe metadata",
        "## Validation Tool Boundary",
        "does not create a validator",
        "does not create a parser",
        "does not read JSON fixtures",
        "does not run replay",
        "documentation token checks",
        "changed-path allowlist checks",
        "untracked-file detection",
        "no-production-drift static checks",
        "Future validator work",
        "future fixture file creation",
        "## Repository And Runtime Boundary",
        "must not be read by production code",
        "QML",
        "Presenter",
        "Controller",
        "ShellServices",
        "must not change DataServiceActions runtime behavior",
        "repositories",
        "migrations",
        "AccountingEngine replay behavior",
        "runtime SQL / SQLite read/write",
        "audit / ledger writes",
        "snapshot writes",
        "backup/export/restore",
        "broker integration",
        "network access",
        "credentials use",
        "endpoint access",
        "real order placement",
        "automatic trading",
        "## Out-of-Scope Boundaries",
        "creation of replay fixture files",
        "fixture parser implementation",
        "fixture validator implementation",
        "replay implementation",
        "AccountingEngine replay calls",
        "runtime SQL / SQLite read/write",
        "production code changes",
        "DataServiceActions or DataServiceActionRegistrar changes",
        "repository changes",
        "migration changes",
        "audit / ledger writes",
        "snapshot writes",
        "real broker order id generation",
        "real order placement",
        "automatic trading",
        "## Formal Authorization Conclusion And Next Task",
        "TASK-215 authorizes replay fixture files contract only.",
        "TASK-215 does not authorize creation of replay fixture files.",
        "TASK-215 does not authorize replay implementation.",
        "TASK-215 does not authorize AccountingEngine replay calls.",
        "TASK-215 does not authorize runtime SQL / SQLite read/write.",
        "TASK-215 does not authorize audit / ledger writes.",
        "TASK-215 does not authorize snapshot writes.",
        "TASK-215 does not authorize backup/export/restore.",
        "TASK-215 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
        "Recommended next task: TASK-216 manual entry replay fixture files scaffold authorization gate.",
    ]:
        gate.contains(doc226, token, "docs/226")

    for token in [
        "# ShellAccounting Manual Entry Replay Fixture Files Authorization Test Plan",
        "## Document Purpose",
        "## Test Matrix",
        "## Required Probes",
        "## Go / No-Go Checklist",
        "docs/226 exists",
        "docs/227 exists",
        "Documentation registration",
        "Gate-only scope",
        "Fixture files authorization purpose",
        "Future fixture directory policy",
        "Future fixture file naming policy",
        "Future fixture index policy",
        "Future fixture JSON schema policy",
        "Future fixture content policy",
        "Future fixture case coverage policy",
        "Privacy and redaction policy",
        "Validation tool boundary",
        "Repository and runtime boundary",
        "Out-of-scope boundaries",
        "Formal conclusion",
        "No fixture file creation",
        "Production drift",
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
        "No seed DB or runtime fixture data scan",
        "No parser / validator implementation scan",
        "No production code changed scan",
        "No production QML / startup / Presenter / Controller drift scan",
        "No ShellServices adapter / port drift scan",
        "No DataServiceActions drift scan",
        "No DataServiceActionRegistrar drift scan",
        "No DataAccess repository drift scan",
        "No migrations drift scan",
        "No runtime SQL / SQLite read/write scan",
        "No AccountingEngine replay implementation scan",
        "No AccountingEngine replay call scan",
        "No audit / ledger write scan",
        "No snapshot write scan",
        "No backup/export/restore implementation scan",
        "No broker / network / credentials / endpoint scan",
        "No real order / automatic trading scan",
        "TASK-215 CTest is registered",
        "TASK-215 authorizes fixture files contract only",
        "TASK-215 does not authorize creation of replay fixture files",
        "No `tests/fixtures/manual_entry_replay/` directory is added",
        "No replay fixture JSON files are added",
        "No seed DB or runtime fixture data is added",
        "Replay implementation appears",
        "AccountingEngine replay call appears",
        "Replay fixture JSON / seed DB / runtime fixture files appear",
        "Fixture parser / validator implementation appears",
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
        gate.contains(doc227, token, "docs/227")

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
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md",
        "docs/227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md",
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
        "tests/CMakeLists.txt",
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json",
        "tests/fixtures/manual_entry_replay/MRF002_single_buy.json",
        "tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json",
        "tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/manual_entry_replay_fixture_static_validator.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
        "docs/240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md",
        "docs/241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md",
        "docs/242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md",
        "docs/243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
    }
    allowed_changes = allowed_changes | authorized_task222_negative_fixture_scaffold_paths
    changes = changed_paths(root)
    unexpected = sorted(path for path in changes if path not in allowed_changes)
    gate.require(not unexpected, "TASK-215 changed unauthorized paths: " + ", ".join(unexpected))

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
            f"TASK-215 must not change {prefix}",
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
            f"TASK-215 must not add fixture/schema data file: {path}",
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
        "fixture",
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
            "TASK-215 must not create unauthorized files under tests/fixtures/manual_entry_replay/",
        )
    else:
        gate.require(True, "TASK-215 fixture directory absent or only authorized TASK-217 scaffold files")

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
            f"TASK-215 must not create manual entry replay fixture data for pattern {pattern}",
        )

    retained_paths = [
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

    gate.require(gate.checks >= 100, "TASK-215 gate must execute at least 100 checks")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
