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
    completed = subprocess.run(
        ["git", "diff", "--name-only", "main"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def added_lines(root: Path, *paths: str) -> str:
    completed = subprocess.run(
        ["git", "diff", "main", "--", *paths],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return "\n".join(
        line for line in completed.stdout.splitlines() if line.startswith("+") and not line.startswith("+++")
    )


def files_under(root: Path) -> list[Path]:
    if not root.exists():
        return []
    return [path for path in root.rglob("*") if path.is_file()]


def joined(files: list[Path]) -> str:
    return "\n".join(read(path) for path in files if path.exists())


def assert_not_changed(changes: set[str], relative_path: str, task: str = "TASK-199") -> None:
    normalized = relative_path.replace("\\", "/")
    require(normalized not in changes, f"{task} must not change {relative_path}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc192 = read(root / "docs" / "192_shell_accounting_manual_entry_dataservice_write_wiring_implementation.md")
    doc193 = read(root / "docs" / "193_shell_accounting_manual_entry_dataservice_write_wiring_implementation_test_plan.md")
    doc194_path = root / "docs" / "194_shell_accounting_manual_entry_qml_presenter_authorization_gate.md"
    doc195_path = root / "docs" / "195_shell_accounting_manual_entry_qml_presenter_authorization_test_plan.md"
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    devdocs = read(root / "tests" / "DevDocs" / "test_readonly_demo_acceptance.py")
    task198_cmake = read(root / "tests" / "ShellAccountingManualEntryDataServiceWriteWiringImplementation" / "CMakeLists.txt")
    task196_cmake = read(root / "tests" / "ShellAccountingManualCashMovementRepositoryDualWriteImplementation" / "CMakeLists.txt")
    task192_cmake = read(root / "tests" / "ShellAccountingManualTransactionRepositoryWriteImplementation" / "CMakeLists.txt")

    require(doc194_path.exists(), "docs/194 exists")
    require(doc195_path.exists(), "docs/195 exists")
    doc194 = read(doc194_path)
    doc195 = read(doc195_path)

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-199", context)
    require_contains(readme, "docs/194_shell_accounting_manual_entry_qml_presenter_authorization_gate.md", "README")
    require_contains(readme, "docs/195_shell_accounting_manual_entry_qml_presenter_authorization_test_plan.md", "README")
    require_contains(docs_index, "194_shell_accounting_manual_entry_qml_presenter_authorization_gate.md", "docs/README")
    require_contains(docs_index, "195_shell_accounting_manual_entry_qml_presenter_authorization_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingManualEntryQmlPresenterAuthorizationGate", "tests/CMakeLists")

    doc194_tokens = [
        "TASK-199",
        "QML / Presenter authorization gate-only",
        "does not implement UI",
        "does not modify production QML",
        "does not modify startup",
        "does not modify Presenter or Controller",
        "does not modify DataServiceActions",
        "does not modify repositories",
        "does not write the database",
        "does not trigger replay",
        "does not write audit or ledger rows",
        "does not add broker, network, credentials, endpoint",
        "accounting.manual_transaction.create",
        "accounting.manual_cash_movement.create",
        "Future Manual Transaction Form Boundary",
        "Future Manual Cash Movement Form Boundary",
        "DataService-Only UI Policy",
        "must not directly call DataAccess repositories",
        "must not directly call DataAccess repositories",
        "directly write SQLite",
        "sanitized repository issues",
        "raw SQL",
        "raw payloads",
        "credentials",
        "endpoints",
        "tokens",
        "internal stack traces",
        "broker payloads",
        "real broker order ids",
        "real order ids",
        "Failure states must not be silent success",
        "Broker sandbox new capability development remains paused",
        "no real order placement",
        "automatic trading",
        "Future readback, refresh, or AccountingEngine replay integration must be separately authorized",
        "Future audit write or ledger policy changes must be separately authorized",
    ]
    for token in doc194_tokens:
        require_contains(doc194, token, "docs/194")

    doc195_tokens = [
        "TASK-199",
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "docs/194 exists",
        "docs/195 exists",
        "Authorization-Only Boundary",
        "Future Manual Entry UI Boundary",
        "Privacy And Response Boundary",
        "No Production Drift",
        "Future manual transaction UI may only call",
        "Future manual cash movement UI may only call",
        "Future UI must call DataService actions only",
        "Future UI must not call DataAccess repositories directly",
        "Future UI must not write SQLite directly",
        "Production QML is not modified",
        "Production startup is not modified",
        "Presenter and Controller code are not modified",
        "DataServiceActions.cpp",
        "DataServiceActions.h",
        "DataServiceActionRegistrar.cpp",
        "Migrations 001 and 002 are not modified",
        "No migration or schema file is added",
        "No QML type or UI form is added",
        "No direct SQLite write",
        "No real order placement",
        "automatic trading",
        "TASK-198 DataService write wiring implementation tests pass",
        "TASK-196 manual cash movement repository tests pass",
        "TASK-192 manual transaction repository tests pass",
        "Broker disabled, broker order, real broker authorization, and real broker",
    ]
    for token in doc195_tokens:
        require_contains(doc195, token, "docs/195")

    for token in ["TASK-199", "UI / Presenter authorization gate", "does not change TASK-198"]:
        require_contains(doc192, token, "docs/192")
    for token in ["TASK-199", "QML / Presenter authorization gate", "No Production Drift"]:
        require_contains(doc193, token, "docs/193")

    changes = changed_paths(root)
    allowed_changes = {
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
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
        "tests/CMakeLists.txt",
        "tests/DevDocs/test_readonly_demo_acceptance.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingProductionQmlBindingGate/ShellAccountingProductionQmlBindingGate.cpp",
        "tests/ShellAccountingProductionQmlBindingImplementation/ShellAccountingProductionQmlBindingImplementation.cpp",
        "tests/ShellAccountingPresenterLifecycleGate/ShellAccountingPresenterLifecycleGate.cpp",
        "tests/ShellAccountingPresenterLifecycleImplementation/ShellAccountingPresenterLifecycleImplementation.cpp",
        "tests/ShellAccountingRealDataAdapterGate/ShellAccountingRealDataAdapterGate.cpp",
        "tests/ShellAccountingRealDataAdapterImplementation/ShellAccountingRealDataAdapterImplementation.cpp",
        "tests/ShellAccountingManualTransactionCashMovementMvpAuthorizationGate/manual_transaction_cash_movement_mvp_authorization_gate.cpp",
        "tests/ShellAccountingManualTransactionCashMovementValidationScaffold/manual_transaction_cash_movement_validation_scaffold.cpp",
        "tests/ShellAccountingManualEntryDataServiceActionAuthorizationGate/manual_entry_dataservice_action_authorization_gate.cpp",
        "tests/ShellAccountingManualEntryDataServiceActionScaffold/manual_entry_dataservice_action_scaffold.cpp",
        "tests/ShellAccountingManualEntryDataServiceActionImplementationAuthorizationGate/manual_entry_dataservice_action_implementation_authorization_gate.cpp",
        "tests/ShellAccountingManualEntryPersistenceAuthorizationGate/manual_entry_persistence_authorization_gate.cpp",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate/manual_cash_movement_repository_write_authorization_gate.py",
        "tests/ShellAccountingManualCashMovementSchemaContractAlignmentGate/manual_cash_movement_schema_contract_alignment_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
    }
    unexpected = sorted(path for path in changes if path not in allowed_changes)
    require(not unexpected, "TASK-199 changed unauthorized paths: " + ", ".join(unexpected))

    protected_paths = [
        "migrations/001_initial_schema.sql",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
        "libs/DataServiceApi/src/DataServiceActions.cpp",
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
    allowed_task200_qml = {"apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml"}
    unexpected_qml_changes = [
        path for path in changes
        if path.startswith("apps/ETFDecisionShell/qml/") and path not in allowed_task200_qml
    ]
    require(
        not unexpected_qml_changes,
        "TASK-199/TASK-200 must not modify unauthorized production QML: "
        + ", ".join(unexpected_qml_changes),
    )
    allowed_task200_shellservices = {
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
    unexpected_shellservices_changes = [
        path for path in changes
        if path.startswith("libs/ShellServices/") and path not in allowed_task200_shellservices
    ]
    require(
        not unexpected_shellservices_changes,
        "TASK-199/TASK-200 must not modify unauthorized ShellServices files: "
        + ", ".join(unexpected_shellservices_changes),
    )
    production_presenter_controller_changes = [
        path
      for path in changes
      if path not in allowed_task200_shellservices
      if (
          path.startswith("apps/ETFDecisionShell/")
            or path.startswith("libs/ShellServices/")
            or path.startswith("libs/ShellCore/")
        )
        and ("presenter" in path.lower() or "controller" in path.lower())
    ]
    require(
        not production_presenter_controller_changes,
        "TASK-199 must not modify production Presenter/Controller: "
        + ", ".join(production_presenter_controller_changes),
    )
    require(not any(path.startswith("migrations/") for path in changes), "TASK-199 must not modify migrations")
    require(not any(path.endswith(".sql") for path in changes), "TASK-199 must not add schema or SQL files")
    require(not any(path.startswith("libs/AccountingEngine/") for path in changes), "TASK-199 must not modify AccountingEngine")
    require(not any(path.startswith("libs/StrategyEngine/") for path in changes), "TASK-199 must not modify StrategyEngine")
    require(not any(path.startswith("libs/MarketEngine/") for path in changes), "TASK-199 must not modify MarketEngine")

    production_qml = joined(files_under(root / "apps" / "ETFDecisionShell" / "qml"))
    for token in [
        "ManualTransactionRepository",
        "ManualCashMovementRepository",
        "INSERT INTO",
        "UPDATE ",
        "DELETE FROM",
        "REPLACE INTO",
        "brokerOrder",
        "placeOrder",
        "automaticTrading",
    ]:
        require_not_contains(production_qml, token, "production QML")

    runtime_added = added_lines(root, "apps", "libs", "migrations")
    for token in [
        "INSERT INTO",
        "UPDATE ",
        "DELETE FROM",
        "REPLACE INTO",
        "qmlRegisterType",
        "ShellAccountingManualTransactionRepository",
        "ShellAccountingManualCashMovementRepository",
        "brokerOrder",
        "placeOrder",
        "broker SDK",
        "http://",
        "https://",
        "endpoint=",
        "api_key",
        "apiKey",
        "secret=",
        "password=",
        "realBrokerOrderId",
        "realOrderId",
        "automaticTrading",
    ]:
        require_not_contains(runtime_added, token, "TASK-199 runtime diff")

    for token in [
        "shell_accounting_manual_entry_dataservice_write_wiring_implementation",
    ]:
        require_contains(task198_cmake, token, "TASK-198 CMake")
    for token in ["deposit_success", "withdrawal_success", "no_broker", "no_network_endpoint", "no_credentials"]:
        require_contains(task196_cmake, token, "TASK-196 CMake")
    for token in ["manual_buy_success", "manual_sell_success", "no_broker", "no_network_endpoint", "no_credentials"]:
        require_contains(task192_cmake, token, "TASK-192 CMake")
    require_contains(tests_cmake, "ShellAccountingBrokerAdapterDisabledWiring", "broker disabled gate retained")
    require_contains(tests_cmake, "ShellAccountingBrokerOrderImplementation", "broker order gate retained")
    require_contains(tests_cmake, "ShellAccountingRealBrokerOrderAuthorizationGate", "real broker authorization gate retained")
    require_contains(tests_cmake, "ShellAccountingRealBrokerOrderImplementationGate", "real broker implementation gate retained")

    for token in [
        "docs/194_shell_accounting_manual_entry_qml_presenter_authorization_gate.md",
        "docs/195_shell_accounting_manual_entry_qml_presenter_authorization_test_plan.md",
        "docs/196_shell_accounting_manual_entry_qml_presenter_implementation.md",
        "docs/197_shell_accounting_manual_entry_qml_presenter_implementation_test_plan.md",
        "shell_accounting_manual_entry_qml_presenter_authorization",
        "shell_accounting_manual_entry_qml_presenter_implementation",
        "submitManualTransaction",
        "submitManualCashMovement",
    ]:
        require_contains(devdocs, token, "DevDocs acceptance")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
