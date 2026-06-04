#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def require_contains(text: str, token: str, context: str) -> None:
    require(token in text, f"{context} missing `{token}`")


def require_not_contains(text: str, token: str, context: str) -> None:
    require(token not in text, f"{context} unexpectedly contains `{token}`")


def changed_paths(root: Path) -> set[str]:
    diff = subprocess.run(
        ["git", "diff", "--name-only", "main"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    untracked = subprocess.run(
        ["git", "ls-files", "--others", "--exclude-standard"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    paths = set()
    for output in (diff.stdout, untracked.stdout):
        paths.update(line.strip().replace("\\", "/") for line in output.splitlines() if line.strip())
    return paths


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc196 = read(root / "docs" / "196_shell_accounting_manual_entry_qml_presenter_implementation.md")
    doc197 = read(root / "docs" / "197_shell_accounting_manual_entry_qml_presenter_implementation_test_plan.md")
    doc198_path = root / "docs" / "198_shell_accounting_manual_entry_post_write_readback_refresh_authorization_gate.md"
    doc199_path = root / "docs" / "199_shell_accounting_manual_entry_post_write_readback_refresh_authorization_test_plan.md"
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    devdocs = read(root / "tests" / "DevDocs" / "test_readonly_demo_acceptance.py")
    task201_cmake = read(
        root
        / "tests"
        / "ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate"
        / "CMakeLists.txt"
    )
    production_qml = read(root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml")
    dataservice_actions = read(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp")

    require(doc198_path.exists(), "docs/198 exists")
    require(doc199_path.exists(), "docs/199 exists")
    doc198 = read(doc198_path)
    doc199 = read(doc199_path)

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-201", context)
    require_contains(
        readme,
        "docs/198_shell_accounting_manual_entry_post_write_readback_refresh_authorization_gate.md",
        "README",
    )
    require_contains(
        readme,
        "docs/199_shell_accounting_manual_entry_post_write_readback_refresh_authorization_test_plan.md",
        "README",
    )
    require_contains(
        docs_index,
        "198_shell_accounting_manual_entry_post_write_readback_refresh_authorization_gate.md",
        "docs/README",
    )
    require_contains(
        docs_index,
        "199_shell_accounting_manual_entry_post_write_readback_refresh_authorization_test_plan.md",
        "docs/README",
    )
    require_contains(
        tests_cmake,
        "ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate",
        "tests/CMakeLists",
    )
    require_contains(
        task201_cmake,
        "shell_accounting_manual_entry_post_write_readback_refresh_authorization",
        "TASK-201 CMake",
    )

    for token in [
        "TASK-201",
        "authorization-only",
        "does not implement readback",
        "refresh",
        "AccountingEngine replay integration",
        "read model refresh",
        "snapshot refresh",
        "UI auto-refresh",
        "refresh buttons",
        "does not modify production QML",
        "ShellServices adapter / port code",
        "DataServiceActions",
        "DataServiceActionRegistrar",
        "DataAccess repositories",
        "migrations",
        "accounting.manual_transaction.create",
        "accounting.manual_cash_movement.create",
        "Manual Transaction Post-Write Boundary",
        "Manual Cash Movement Post-Write Boundary",
        "Replay / Read Model Boundary",
        "DataService Read Boundary Policy",
        "No Direct UI Data Access Policy",
        "Future post-write readback may use existing read actions",
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
        "QML must not directly access SQLite",
        "Presenter and Controller must not directly access SQLite",
        "QML must not calculate accounting facts",
        "Future audit write or ledger policy changes",
        "Broker sandbox new capability development remains paused",
        "no real broker",
        "no automatic trading",
        "no real order placement",
        "raw SQL",
        "raw manual entry payloads",
        "sensitive memo content",
        "must be disableable",
        "must not fabricate readback data",
        "must not trigger replay",
    ]:
        require_contains(doc198, token, "docs/198")

    for token in [
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "docs/198 exists",
        "docs/199 exists",
        "Authorization-Only Policy",
        "Manual Transaction Boundary",
        "Manual Cash Movement Boundary",
        "No Production Implementation Drift",
        "Forbidden Capability Probes",
        "Production QML is not modified",
        "Startup files are not modified",
        "Presenter and Controller files are not modified",
        "ShellServices adapter / port files are not modified",
        "DataServiceActions.cpp",
        "DataServiceActionRegistrar.cpp",
        "DataAccess repositories are not modified",
        "No migration or schema file is added",
        "No direct SQLite read/write is introduced",
        "No AccountingEngine replay integration is introduced",
        "No read model refresh implementation is introduced",
        "No UI auto-refresh is introduced",
        "No refresh button is introduced",
        "No audit_log or ledger write is introduced",
        "TASK-200 QML Presenter implementation tests pass",
        "TASK-198 DataService write wiring implementation tests pass",
        "TASK-196 manual cash movement repository tests pass",
        "TASK-192 manual transaction repository tests pass",
    ]:
        require_contains(doc199, token, "docs/199")

    for token in ["TASK-201 Authorization Gate Update", "Post-write readback", "refresh"]:
        require_contains(doc196, token, "docs/196")
        require_contains(doc197, token, "docs/197")

    for token in [
        "docs/198_shell_accounting_manual_entry_post_write_readback_refresh_authorization_gate.md",
        "docs/199_shell_accounting_manual_entry_post_write_readback_refresh_authorization_test_plan.md",
        "shell_accounting_manual_entry_post_write_readback_refresh_authorization",
    ]:
        require_contains(devdocs, token, "DevDocs acceptance")

    changes = changed_paths(root)
    allowed = {
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/196_shell_accounting_manual_entry_qml_presenter_implementation.md",
        "docs/197_shell_accounting_manual_entry_qml_presenter_implementation_test_plan.md",
        "docs/198_shell_accounting_manual_entry_post_write_readback_refresh_authorization_gate.md",
        "docs/199_shell_accounting_manual_entry_post_write_readback_refresh_authorization_test_plan.md",
        "docs/200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md",
        "docs/201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md",
        "docs/202_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_gate.md",
        "docs/203_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_test_plan.md",
        "docs/204_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance.md",
        "docs/205_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance_test_plan.md",
        "docs/206_shell_accounting_manual_entry_readback_replay_adequacy_review_gate.md",
        "docs/207_shell_accounting_manual_entry_readback_replay_adequacy_review_test_plan.md",
        "docs/208_shell_accounting_manual_entry_readback_mapping_authorization_gate.md",
        "docs/209_shell_accounting_manual_entry_readback_mapping_authorization_test_plan.md",
        "docs/210_shell_accounting_manual_entry_readback_mapping_implementation.md",
        "docs/211_shell_accounting_manual_entry_readback_mapping_implementation_test_plan.md",
        "docs/214_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance.md",
        "docs/215_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance_test_plan.md",
        "docs/216_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.md",
        "docs/217_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_test_plan.md",
        "docs/218_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance.md",
        "docs/219_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance_test_plan.md",
        "docs/220_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_gate.md",
        "docs/221_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_test_plan.md",
        "libs/DataServiceApi/src/DataServiceActions.cpp",
        "apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml",
        "libs/ShellServices/include/ShellServices/ShellAccountingPresenter.h",
        "libs/ShellServices/src/ShellAccountingPresenter.cpp",
        "libs/ShellServices/include/ShellServices/ShellAccountingReadOnlyController.h",
        "libs/ShellServices/src/ShellAccountingReadOnlyController.cpp",
        "tests/CMakeLists.txt",
        "tests/DevDocs/test_readonly_demo_acceptance.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/manual_entry_mvp_runtime_e2e_acceptance.cpp",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/manual_entry_readback_mapping_implementation.cpp",
        "docs/212_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_gate.md",
        "docs/213_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance/manual_entry_readback_daily_use_runtime_acceptance.cpp",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseRuntimeAcceptance/manual_entry_sell_withdrawal_daily_use_runtime_acceptance.cpp",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
        "docs/222_shell_accounting_manual_entry_replay_policy_authorization_gate.md",
        "docs/223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
        "docs/224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md",
        "docs/225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
        "docs/226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md",
        "docs/227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md",
        "docs/228_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_gate.md",
        "docs/229_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_test_plan.md",
        "docs/232_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_gate.md",
        "docs/233_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_test_plan.md",
        "docs/236_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_gate.md",
        "docs/237_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_test_plan.md",
        "docs/238_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_gate.md",
        "docs/239_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
        "docs/230_shell_accounting_manual_entry_replay_fixture_files_scaffold.md",
        "docs/231_shell_accounting_manual_entry_replay_fixture_files_scaffold_test_plan.md",
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json",
        "tests/fixtures/manual_entry_replay/MRF002_single_buy.json",
        "tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json",
        "tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
        "docs/234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md",
        "docs/235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/manual_entry_replay_fixture_static_validator.py",
        "docs/240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md",
        "docs/241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md",
        "docs/242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md",
        "docs/243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md",
        "docs/244_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_gate.md",
        "docs/245_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_test_plan.md",
        "docs/246_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.md",
        "docs/247_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
        "docs/248_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.md",
        "docs/249_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_test_plan.md",
        "docs/250_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_gate.md",
        "docs/251_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py",
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
    unexpected = sorted(path for path in changes if path not in allowed)
    require(not unexpected, "TASK-201 changed unauthorized paths: " + ", ".join(unexpected))

    forbidden_prefixes = [
        "libs/DataAccess/",
        "libs/AccountingEngine/",
        "libs/StrategyEngine/",
        "libs/MarketEngine/",
        "migrations/",
    ]
    for prefix in forbidden_prefixes:
        require(not any(path.startswith(prefix) for path in changes), f"TASK-201 must not change {prefix}")

    forbidden_files = [
        "apps/ETFDecisionShell/src/main.cpp",
        "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
        "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
        "migrations/001_initial_schema.sql",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
    ]
    for path in forbidden_files:
        require(path not in changes, f"TASK-201 must not change {path}")
    for path in changes:
        if path.startswith("libs/DataServiceApi/"):
            require(
                path == "libs/DataServiceApi/src/DataServiceActions.cpp",
                f"TASK-201/TASK-207 only permits DataServiceActions readback mapping, not {path}",
            )

    for token in [
        "DataServiceClient",
        "SQLite",
        "DataAccess",
        "INSERT INTO",
        "UPDATE ",
        "DELETE FROM",
        "REPLACE INTO",
        "brokerOrder",
        "placeOrder",
        "realOrderId",
        "automaticTrading",
    ]:
        require_not_contains(production_qml, token, "production QML")

    for token in ["persistManualTransaction", "persistManualCashMovement"]:
        require_contains(dataservice_actions, token, "DataServiceActions")
    for token in ["INSERT INTO", "UPDATE ", "DELETE FROM", "REPLACE INTO"]:
        require_not_contains(dataservice_actions, token, "DataServiceActions")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
