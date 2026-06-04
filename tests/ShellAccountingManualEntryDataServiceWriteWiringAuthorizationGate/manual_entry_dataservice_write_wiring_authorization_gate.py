#!/usr/bin/env python3

import argparse
import re
import subprocess
from pathlib import Path


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def require_contains(text: str, token: str, context: str) -> None:
    require(token in text, f"{context} missing `{token}`")


def changed_paths(root: Path) -> set[str]:
    completed = subprocess.run(
        ["git", "diff", "--name-only", "main"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def diff_text(root: Path, *paths: str) -> str:
    completed = subprocess.run(
        ["git", "diff", "main", "--", *paths],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return completed.stdout


def files_under(root: Path, suffixes: set[str] | None = None) -> list[Path]:
    if not root.exists():
        return []
    files = [path for path in root.rglob("*") if path.is_file()]
    if suffixes is not None:
        files = [path for path in files if path.suffix in suffixes]
    return files


def joined(paths: list[Path]) -> str:
    return "\n".join(read(path) for path in paths if path.exists())


def assert_not_changed(changes: set[str], relative_path: str) -> None:
    normalized = relative_path.replace("\\", "/")
    require(normalized not in changes, f"{relative_path} must not be changed by TASK-197")


def function_body(source: str, function_name: str) -> str:
    start = source.find(function_name + "(")
    require(start >= 0, f"{function_name} must exist")
    brace = source.find("{", start)
    require(brace >= 0, f"{function_name} body must start")
    depth = 0
    for index in range(brace, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[brace : index + 1]
    raise AssertionError(f"{function_name} body must end")


def added_lines(diff: str) -> str:
    return "\n".join(line for line in diff.splitlines() if line.startswith("+") and not line.startswith("+++"))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc188 = read(root / "docs" / "188_shell_accounting_manual_cash_movement_repository_dual_write_implementation.md")
    doc189 = read(root / "docs" / "189_shell_accounting_manual_cash_movement_repository_dual_write_implementation_test_plan.md")
    doc190_path = root / "docs" / "190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md"
    doc191_path = root / "docs" / "191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md"
    doc192_path = root / "docs" / "192_shell_accounting_manual_entry_dataservice_write_wiring_implementation.md"
    doc193_path = root / "docs" / "193_shell_accounting_manual_entry_dataservice_write_wiring_implementation_test_plan.md"
    doc190 = read(doc190_path)
    doc191 = read(doc191_path)
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    data_service_actions = read(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp")
    data_service_actions_h = read(root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" / "DataServiceActions.h")
    registrar = read(root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp")
    validation_wiring_cmake = read(
        root / "tests" / "ShellAccountingManualEntryDataServiceActionValidationWiring" / "CMakeLists.txt"
    )
    task196_cmake = read(root / "tests" / "ShellAccountingManualCashMovementRepositoryDualWriteImplementation" / "CMakeLists.txt")
    task192_cmake = read(root / "tests" / "ShellAccountingManualTransactionRepositoryWriteImplementation" / "CMakeLists.txt")

    require(doc190_path.exists(), "docs/190 exists")
    require(doc191_path.exists(), "docs/191 exists")
    require(doc192_path.exists(), "docs/192 exists")
    require(doc193_path.exists(), "docs/193 exists")
    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-197", context)
        require_contains(text, "TASK-198", context)
    require_contains(readme, "docs/190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md", "README")
    require_contains(readme, "docs/191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md", "README")
    require_contains(readme, "docs/192_shell_accounting_manual_entry_dataservice_write_wiring_implementation.md", "README")
    require_contains(readme, "docs/193_shell_accounting_manual_entry_dataservice_write_wiring_implementation_test_plan.md", "README")
    require_contains(docs_index, "190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md", "docs/README")
    require_contains(docs_index, "191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md", "docs/README")
    require_contains(docs_index, "192_shell_accounting_manual_entry_dataservice_write_wiring_implementation.md", "docs/README")
    require_contains(docs_index, "193_shell_accounting_manual_entry_dataservice_write_wiring_implementation_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate", "tests/CMakeLists")
    require_contains(tests_cmake, "ShellAccountingManualEntryDataServiceWriteWiringImplementation", "tests/CMakeLists")
    require_contains(tests_cmake, "ShellAccountingBrokerAdapterDisabledWiring", "tests/CMakeLists broker disabled retained")
    require_contains(tests_cmake, "ShellAccountingBrokerOrderImplementation", "tests/CMakeLists broker order retained")
    require_contains(tests_cmake, "ShellAccountingRealBrokerOrderAuthorizationGate", "tests/CMakeLists real broker authorization retained")
    require_contains(tests_cmake, "ShellAccountingRealBrokerOrderImplementationGate", "tests/CMakeLists real broker implementation retained")

    doc190_tokens = [
        "TASK-197",
        "DataService write wiring authorization gate-only",
        "TASK-198 has now evolved the authorized path",
        "did not modify `DataServiceActions.cpp`",
        "did not modify `DataServiceActions.h`",
        "does not modify `DataServiceActionRegistrar.cpp`",
        "did not wire repositories",
        "did not write database rows",
        "did not execute runtime SQL",
        "did not return persistent id",
        "accounting.manual_transaction.create",
        "accounting.manual_cash_movement.create",
        "sanitized local write results",
        "ShellAccountingManualTransactionRepository",
        "ShellAccountingManualCashMovementRepository",
        "TASK-198 separately authorized and implemented DataService write wiring",
        "validation-first",
        "must not scatter SQL in DataService action handlers",
        "SQL location remains DataAccess only",
        "sanitized local write result",
        "raw SQL",
        "raw payloads",
        "credentials",
        "endpoint",
        "tokens",
        "internal stack traces",
        "Future audit write",
        "Future replay",
        "read model",
        "UI integration",
        "No broker semantics",
        "No replay trigger",
        "Repository transaction boundaries remain authoritative",
        "Duplicate idempotency",
        "stable protocol error",
        "Broker sandbox new capability development remains paused",
        "no real broker",
        "no automatic trading",
        "no real order placement",
    ]
    for token in doc190_tokens:
        require_contains(doc190, token, "docs/190")

    doc191_tokens = [
        "TASK-197",
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "docs/190 exists",
        "docs/191 exists",
        "DataServiceActions.cpp is modified only for TASK-198 repository wiring",
        "DataServiceActions.h is not modified",
        "DataServiceActionRegistrar.cpp is not modified",
        "TASK-178 validation production code is not modified",
        "TASK-192 repository header/source are not modified",
        "TASK-196 repository header/source are not modified",
        "TASK-182 validation-first behavior is retained",
        "Valid manual transaction payload writes through",
        "Valid manual cash movement payload writes through",
        "Invalid payloads still do not write database rows",
        "DataServiceActions calls only `ShellAccountingManualTransactionRepository`",
        "DataServiceActions calls only `ShellAccountingManualCashMovementRepository`",
        "DataServiceApi does not add runtime SQL",
        "No TradeDraft implementation",
        "No broker SDK",
        "No network or endpoint",
        "No credentials",
        "No real broker order id",
        "No real order placement",
        "No automatic trading",
        "TASK-196 dual-write repository tests pass",
        "TASK-192 manual transaction repository tests pass",
        "TASK-182 validation-first wiring tests pass",
    ]
    for token in doc191_tokens:
        require_contains(doc191, token, "docs/191")

    for token in ["TASK-197", "DataService write wiring authorization gate"]:
        require_contains(doc188, token, "docs/188")
    for token in ["TASK-197", "DataService write wiring authorization gate tests pass"]:
        require_contains(doc189, token, "docs/189")

    changes = changed_paths(root)
    allowed_changes = {
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/188_shell_accounting_manual_cash_movement_repository_dual_write_implementation.md",
        "docs/189_shell_accounting_manual_cash_movement_repository_dual_write_implementation_test_plan.md",
        "docs/190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md",
        "docs/191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md",
        "docs/192_shell_accounting_manual_entry_dataservice_write_wiring_implementation.md",
        "docs/193_shell_accounting_manual_entry_dataservice_write_wiring_implementation_test_plan.md",
        "docs/194_shell_accounting_manual_entry_qml_presenter_authorization_gate.md",
        "docs/195_shell_accounting_manual_entry_qml_presenter_authorization_test_plan.md",
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
        "docs/212_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_gate.md",
        "docs/213_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_test_plan.md",
        "docs/216_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.md",
        "docs/217_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_test_plan.md",
        "docs/218_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance.md",
        "docs/219_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance_test_plan.md",
        "apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml",
        "libs/ShellServices/include/ShellServices/ShellAccountingDataServiceAdapter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingDataServiceClientPort.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingDataServiceClientPortAdapter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingPresenter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingReadOnlyController.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingServiceAdapter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingServiceTypes.h",
        "libs/ShellServices/src/ShellAccountingDataServiceAdapter.cpp",
        "libs/ShellServices/src/ShellAccountingDataServiceClientPort.cpp",
        "libs/ShellServices/src/ShellAccountingDataServiceClientPortAdapter.cpp",
        "libs/ShellServices/src/ShellAccountingPresenter.cpp",
        "libs/ShellServices/src/ShellAccountingReadOnlyController.cpp",
        "libs/ShellServices/src/ShellAccountingServiceAdapter.cpp",
        "libs/DataServiceApi/src/DataServiceActions.cpp",
        "tests/CMakeLists.txt",
        "tests/DevDocs/test_readonly_demo_acceptance.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryDataServiceActionAuthorizationGate/manual_entry_dataservice_action_authorization_gate.cpp",
        "tests/ShellAccountingManualEntryDataServiceActionImplementationAuthorizationGate/manual_entry_dataservice_action_implementation_authorization_gate.cpp",
        "tests/ShellAccountingManualEntryDataServiceActionScaffold/manual_entry_dataservice_action_scaffold.cpp",
        "tests/ShellAccountingManualEntryDataServiceActionValidationWiring/manual_entry_dataservice_action_validation_wiring.cpp",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualTransactionCashMovementValidationScaffold/manual_transaction_cash_movement_validation_scaffold.cpp",
        "tests/ShellAccountingProductionQmlBindingGate/ShellAccountingProductionQmlBindingGate.cpp",
        "tests/ShellAccountingProductionQmlBindingImplementation/ShellAccountingProductionQmlBindingImplementation.cpp",
        "tests/ShellAccountingPresenterLifecycleGate/ShellAccountingPresenterLifecycleGate.cpp",
        "tests/ShellAccountingPresenterLifecycleImplementation/ShellAccountingPresenterLifecycleImplementation.cpp",
        "tests/ShellAccountingRealDataAdapterGate/ShellAccountingRealDataAdapterGate.cpp",
        "tests/ShellAccountingRealDataAdapterImplementation/ShellAccountingRealDataAdapterImplementation.cpp",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/manual_entry_mvp_runtime_e2e_acceptance.cpp",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryPersistenceAuthorizationGate/manual_entry_persistence_authorization_gate.cpp",
        "tests/ShellAccountingManualEntryRepositoryImplementationAuthorizationGate/manual_entry_repository_implementation_authorization_gate.cpp",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryRepositoryScaffold/manual_entry_repository_scaffold.cpp",
        "tests/ShellAccountingManualEntryRepositoryScaffoldAuthorizationGate/manual_entry_repository_scaffold_authorization_gate.cpp",
        "tests/ShellAccountingManualEntrySchemaAdequacyReviewGate/manual_entry_schema_adequacy_review_gate.cpp",
        "tests/ShellAccountingManualEntrySchemaGapAuthorizationGate/manual_entry_schema_gap_authorization_gate.cpp",
        "tests/ShellAccountingManualEntrySchemaImplementationAuthorizationGate/manual_entry_schema_implementation_authorization_gate.cpp",
        "tests/ShellAccountingManualTransactionCashMovementMvpAuthorizationGate/manual_transaction_cash_movement_mvp_authorization_gate.cpp",
        "tests/ShellAccountingManualCashMovementRepositoryDualWriteImplementation/manual_cash_movement_repository_dual_write_implementation.cpp",
        "tests/ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate/manual_cash_movement_repository_write_authorization_gate.py",
        "tests/ShellAccountingManualCashMovementSchemaContractAlignmentGate/manual_cash_movement_schema_contract_alignment_gate.py",
        "tests/ShellAccountingManualTransactionRepositoryWriteImplementation/manual_transaction_repository_write_implementation.cpp",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringImplementation/manual_entry_dataservice_write_wiring_implementation.cpp",
        "docs/222_shell_accounting_manual_entry_replay_policy_authorization_gate.md",
        "docs/223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
        "docs/224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md",
        "docs/225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
        "docs/226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md",
        "docs/227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
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
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
        "docs/234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md",
        "docs/235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/manual_entry_replay_fixture_static_validator.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
    }
    unexpected = sorted(path for path in changes if path not in allowed_changes)
    require(not unexpected, "TASK-197 changed unauthorized paths: " + ", ".join(unexpected))

    protected_paths = [
        "migrations/001_initial_schema.sql",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
        "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
        "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
        "libs/DataServiceApi/include/DataServiceApi/ShellAccountingManualTransactionCashMovementValidation.h",
        "libs/DataServiceApi/src/ShellAccountingManualTransactionCashMovementValidation.cpp",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
        "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualCashMovementRepository.h",
        "libs/DataAccess/src/ShellAccountingManualCashMovementRepository.cpp",
        "apps/ETFDecisionShell/src/main.cpp",
    ]
    for path in protected_paths:
        assert_not_changed(changes, path)
    require(not any(path.startswith("migrations/") for path in changes), "TASK-197 must not modify migrations")
    require(not any(path.endswith(".sql") for path in changes), "TASK-197 must not add schema or SQL files")
    allowed_task200_qml = {"apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml"}
    unexpected_qml_changes = [
        path for path in changes
        if path.startswith("apps/ETFDecisionShell/qml/") and path not in allowed_task200_qml
    ]
    require(not unexpected_qml_changes, "TASK-197/TASK-200 must not modify unauthorized production QML: " + ", ".join(unexpected_qml_changes))
    require(not any(path.startswith("libs/AccountingEngine/") for path in changes), "TASK-197 must not modify AccountingEngine")
    require(not any(path.startswith("libs/StrategyEngine/") for path in changes), "TASK-197 must not modify StrategyEngine")
    require(not any(path.startswith("libs/MarketEngine/") for path in changes), "TASK-197 must not modify MarketEngine")
    allowed_task200_presenter_controller = {
        "libs/ShellServices/include/ShellServices/ShellAccountingDataServiceAdapter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingDataServiceClientPort.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingDataServiceClientPortAdapter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingPresenter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingReadOnlyController.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingServiceAdapter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingServiceTypes.h",
        "libs/ShellServices/src/ShellAccountingDataServiceAdapter.cpp",
        "libs/ShellServices/src/ShellAccountingDataServiceClientPort.cpp",
        "libs/ShellServices/src/ShellAccountingDataServiceClientPortAdapter.cpp",
        "libs/ShellServices/src/ShellAccountingPresenter.cpp",
        "libs/ShellServices/src/ShellAccountingReadOnlyController.cpp",
        "libs/ShellServices/src/ShellAccountingServiceAdapter.cpp",
    }
    unexpected_presenter_controller_changes = [
        path for path in changes
        if path not in allowed_task200_presenter_controller
        and (path.startswith("apps/") or path.startswith("libs/"))
        and ("Presenter" in path or "Controller" in path)
    ]
    require(
        not unexpected_presenter_controller_changes,
        "TASK-197/TASK-200 must not modify unauthorized Presenter/Controller: "
        + ", ".join(unexpected_presenter_controller_changes),
    )

    require_contains(data_service_actions, "validateManualTransactionEntry", "DataServiceActions")
    require_contains(data_service_actions, "validateManualCashMovement", "DataServiceActions")
    require_contains(data_service_actions, "manualEntryValidationRejectedResponse", "DataServiceActions")
    require_contains(data_service_actions, "manualTransactionWriteResponse", "DataServiceActions")
    require_contains(data_service_actions, "manualCashMovementWriteResponse", "DataServiceActions")
    require_contains(data_service_actions, "ShellAccountingManualTransactionRepository repository(connection)", "DataServiceActions")
    require_contains(data_service_actions, "ShellAccountingManualCashMovementRepository repository(connection)", "DataServiceActions")
    require_contains(data_service_actions, "persistManualTransaction", "DataServiceActions")
    require_contains(data_service_actions, "persistManualCashMovement", "DataServiceActions")
    require_contains(data_service_actions, '\\"writeImplemented\\":true', "DataServiceActions")
    require_contains(data_service_actions, '\\"writeEnabled\\":true', "DataServiceActions")
    require_contains(data_service_actions, '\\"databaseWritten\\":false', "DataServiceActions")
    require_contains(data_service_actions, '\\"tradeLogWritten\\":false', "DataServiceActions")
    require_contains(data_service_actions, '\\"cashAdjustmentWritten\\":false', "DataServiceActions")
    require_contains(data_service_actions, '\\"cashFactsWritten\\":false', "DataServiceActions")
    require_contains(data_service_actions, '\\"auditWritten\\":false', "DataServiceActions")
    require_contains(data_service_actions, '\\"ledgerWritten\\":false', "DataServiceActions")
    require_contains(data_service_actions_h, "handleAccountingManualEntryTransactionCreate", "DataServiceActions.h")
    require_contains(data_service_actions_h, "handleAccountingManualEntryCashMovementCreate", "DataServiceActions.h")
    require_contains(registrar, "kActionAccountingManualTransactionCreate", "DataServiceActionRegistrar")
    require_contains(registrar, "kActionAccountingManualCashMovementCreate", "DataServiceActionRegistrar")

    transaction_body = function_body(data_service_actions, "handleAccountingManualEntryTransactionCreate")
    cash_body = function_body(data_service_actions, "handleAccountingManualEntryCashMovementCreate")
    for body, context, validator in [
        (transaction_body, "manual transaction handler", "validateManualTransactionEntry"),
        (cash_body, "manual cash movement handler", "validateManualCashMovement"),
    ]:
        require_contains(body, validator, context)
        require_contains(body, "Repository repository(connection)", context)
        require("executeStatement" not in body, f"{context} must not execute SQL")
        require("INSERT" not in body and "UPDATE" not in body and "DELETE" not in body and "REPLACE" not in body,
                f"{context} must not contain DML")

    runtime_diff = added_lines(diff_text(root, "libs", "apps"))
    for token in [
        "INSERT INTO",
        "UPDATE ",
        "DELETE FROM",
        "REPLACE INTO",
        "cashFactsWritten\":true",
        "auditWritten\":true",
        "ledgerWritten\":true",
        "TradeDraft",
        "trade suggestion",
        "brokerOrder",
        "placeOrder",
        "real order",
        "automaticTrading\\\":true",
        "http://",
        "https://",
        "api_key",
        "apiKey",
        "password=",
        "secret=",
        "endpoint=",
    ]:
        require(token not in runtime_diff, f"TASK-197 must not add runtime token {token}")

    require_contains(validation_wiring_cmake, "manual_transaction_success", "TASK-182 CMake")
    require_contains(validation_wiring_cmake, "manual_cash_movement_success", "TASK-182 CMake")
    require_contains(task192_cmake, "shell_accounting_manual_transaction_repository_write_implementation_manual_buy_success", "TASK-192 CMake")
    require_contains(task192_cmake, "shell_accounting_manual_transaction_repository_write_implementation_manual_sell_success", "TASK-192 CMake")
    require_contains(task192_cmake, "no_cash_adjustment_write", "TASK-192 CMake")
    require_contains(task192_cmake, "no_broker", "TASK-192 CMake")
    require_contains(task196_cmake, "deposit_success", "TASK-196 CMake")
    require_contains(task196_cmake, "withdrawal_success", "TASK-196 CMake")
    require_contains(task196_cmake, "duplicate_idempotency_no_extra_trade_log", "TASK-196 CMake")
    require_contains(task196_cmake, "no_audit_log_write", "TASK-196 CMake")
    require_contains(task196_cmake, "no_broker", "TASK-196 CMake")
    require_contains(task196_cmake, "no_network_endpoint", "TASK-196 CMake")
    require_contains(task196_cmake, "no_credentials", "TASK-196 CMake")
    require_contains(task196_cmake, "no_real_order", "TASK-196 CMake")
    require_contains(task196_cmake, "no_automatic_trading", "TASK-196 CMake")

    devdocs = read(root / "tests" / "DevDocs" / "test_readonly_demo_acceptance.py")
    for token in [
        "docs/190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md",
        "docs/191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md",
        "shell_accounting_manual_entry_dataservice_write_wiring_authorization",
    ]:
        require_contains(devdocs, token, "DevDocs acceptance")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
