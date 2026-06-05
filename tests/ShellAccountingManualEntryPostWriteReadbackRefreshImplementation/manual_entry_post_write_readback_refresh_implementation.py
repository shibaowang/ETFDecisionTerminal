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


def function_body(text: str, signature: str) -> str:
    start = text.find(signature)
    require(start >= 0, f"missing function `{signature}`")
    brace = text.find("{", start)
    require(brace >= 0, f"missing function body `{signature}`")
    depth = 0
    for index in range(brace, len(text)):
        if text[index] == "{":
            depth += 1
        elif text[index] == "}":
            depth -= 1
            if depth == 0:
                return text[brace:index + 1]
    raise AssertionError(f"unterminated function body `{signature}`")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc198 = read(root / "docs" / "198_shell_accounting_manual_entry_post_write_readback_refresh_authorization_gate.md")
    doc199 = read(root / "docs" / "199_shell_accounting_manual_entry_post_write_readback_refresh_authorization_test_plan.md")
    doc200_path = root / "docs" / "200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md"
    doc201_path = root / "docs" / "201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md"
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    devdocs = read(root / "tests" / "DevDocs" / "test_readonly_demo_acceptance.py")
    qml = read(root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml")
    presenter_h = read(root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h")
    presenter_cpp = read(root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp")
    controller_h = read(root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h")
    controller_cpp = read(root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp")
    dataservice_actions = read(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp")

    require(doc200_path.exists(), "docs/200 exists")
    require(doc201_path.exists(), "docs/201 exists")
    doc200 = read(doc200_path)
    doc201 = read(doc201_path)

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-202", context)
    require_contains(
        readme,
        "docs/200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md",
        "README",
    )
    require_contains(
        readme,
        "docs/201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md",
        "README",
    )
    require_contains(docs_index, "200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md", "docs/README")
    require_contains(docs_index, "201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingManualEntryPostWriteReadbackRefreshImplementation", "tests/CMakeLists")
    require_contains(doc198, "TASK-202 Implementation Update", "docs/198")
    require_contains(doc199, "TASK-202 Implementation Update", "docs/199")

    for token in [
        "TASK-202",
        "post-write readback refresh",
        "postWriteRefreshEnabled",
        "postWriteRefreshBusy",
        "lastPostWriteRefreshStatus",
        "lastPostWriteRefreshIssue",
        "lastPostWriteRefreshSummary",
        "shellAccountingPostWriteRefreshStatus",
        "shellAccountingPostWriteRefreshIssue",
        "shellAccountingPostWriteRefreshSummary",
        "accounting.manual_transaction.create",
        "accounting.manual_cash_movement.create",
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
        "does not add DataService actions",
        "does not modify DataServiceActions",
        "does not modify DataServiceActionRegistrar",
        "does not trigger AccountingEngine replay",
        "snapshot rebuild after write",
        "derived table refresh after write",
        "write `audit_log` or ledger rows",
        "real order placement",
        "automatic trading",
    ]:
        require_contains(doc200, token, "docs/200")

    for token in [
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "QML Status Boundary",
        "Presenter And Controller Boundary",
        "No Production Drift Outside Scope",
        "Successful manual transaction writes trigger post-write refresh",
        "Successful manual cash movement writes trigger post-write refresh",
        "Validation failures do not trigger refresh",
        "Write failures do not trigger refresh",
        "DataServiceActions.cpp is not modified by TASK-202",
        "No audit_log or ledger write is introduced",
        "No snapshot rebuild after write is introduced",
        "No derived table refresh after write is introduced",
        "No real order placement is introduced",
        "No automatic trading is introduced",
    ]:
        require_contains(doc201, token, "docs/201")

    for token in [
        "shellAccountingPostWriteRefreshStatus",
        "shellAccountingPostWriteRefreshIssue",
        "shellAccountingPostWriteRefreshSummary",
        "accountingPresenter.lastPostWriteRefreshStatus",
        "accountingPresenter.lastPostWriteRefreshIssue",
        "accountingPresenter.lastPostWriteRefreshSummary",
    ]:
        require_contains(qml, token, "ShellAccountingReadOnlyPage.qml")
    for token in [
        "DataServiceClient",
        "SQLite",
        "DataAccess",
        "INSERT INTO",
        "UPDATE ",
        "DELETE FROM",
        "REPLACE INTO",
        "positionValue",
        "cashBalance",
        "pnlAmount",
        "brokerOrder",
        "placeOrder",
        "realOrderId",
        "automaticTrading",
    ]:
        require_not_contains(qml, token, "production QML")

    for token in [
        "Q_PROPERTY(bool postWriteRefreshEnabled",
        "Q_PROPERTY(bool postWriteRefreshBusy",
        "Q_PROPERTY(QString lastPostWriteRefreshStatus",
        "Q_PROPERTY(QString lastPostWriteRefreshIssue",
        "Q_PROPERTY(QString lastPostWriteRefreshSummary",
        "postWriteRefreshStateChanged",
        "Q_INVOKABLE void resetPostWriteRefreshState",
        "Q_INVOKABLE bool refreshManualEntryReadback",
        "refreshAfterManualEntryWrite",
    ]:
        require_contains(presenter_h, token, "ShellAccountingPresenter.h")
    for token in [
        "ShellAccountingPresenter::postWriteRefreshEnabled",
        "ShellAccountingPresenter::postWriteRefreshBusy",
        "ShellAccountingPresenter::lastPostWriteRefreshStatus",
        "ShellAccountingPresenter::lastPostWriteRefreshIssue",
        "ShellAccountingPresenter::lastPostWriteRefreshSummary",
        "ShellAccountingPresenter::resetPostWriteRefreshState",
        "ShellAccountingPresenter::refreshManualEntryReadback",
        "ShellAccountingPresenter::refreshAfterManualEntryWrite",
        "refreshAfterManualEntryWrite(true, \"manual_transaction\")",
        "refreshAfterManualEntryWrite(false, \"manual_cash_movement\")",
        "\"position.list\"",
        "\"cash.summary\"",
        "\"portfolio.pnl.summary\"",
        "controller_->submitManualTransaction",
        "controller_->submitManualCashMovement",
        "controller_->currentState()",
        "Post-write readback refresh failed.",
    ]:
        require_contains(presenter_cpp, token, "ShellAccountingPresenter.cpp")

    transaction_body = function_body(presenter_cpp, "bool ShellAccountingPresenter::submitManualTransaction")
    cash_body = function_body(presenter_cpp, "bool ShellAccountingPresenter::submitManualCashMovement")
    for body, context, refresh_call in [
        (transaction_body, "manual transaction submit", 'refreshAfterManualEntryWrite(true, "manual_transaction")'),
        (cash_body, "manual cash movement submit", 'refreshAfterManualEntryWrite(false, "manual_cash_movement")'),
    ]:
        require_contains(body, "if (!valid) {\n        return false;\n    }", context)
        require_contains(body, "const bool writeSucceeded", context)
        require_contains(body, "if (writeSucceeded)", context)
        require_contains(body, refresh_call, context)
        require(body.find("if (!valid)") < body.find("const auto result"), f"{context} validates before write")
        require(body.find("const bool writeSucceeded") > body.find("applyManualEntryResult"), f"{context} evaluates write result after mapping")

    refresh_body = function_body(presenter_cpp, "void ShellAccountingPresenter::refreshAfterManualEntryWrite")
    require_contains(refresh_body, "ShellAccountingReadOnlyController::refreshPositionList", "refresh helper")
    require_contains(refresh_body, "ShellAccountingReadOnlyController::refreshCashSummary", "refresh helper")
    require_contains(refresh_body, "ShellAccountingReadOnlyController::refreshPortfolioPnlSummary", "refresh helper")
    require_contains(refresh_body, "includePositionList", "refresh helper")
    require_contains(refresh_body, "cashMovementSteps", "refresh helper")
    for token in ["DataAccess", "SQLite", "INSERT INTO", "UPDATE ", "DELETE FROM", "REPLACE INTO", "AccountingReplay", "StrategyEngine", "BrokerOrderPort"]:
        require_not_contains(presenter_h, token, "ShellAccountingPresenter.h")
        require_not_contains(presenter_cpp, token, "ShellAccountingPresenter.cpp")
        require_not_contains(controller_h, token, "ShellAccountingReadOnlyController.h")
        require_not_contains(controller_cpp, token, "ShellAccountingReadOnlyController.cpp")

    for token in ["position.list", "cash.summary", "portfolio.pnl.summary"]:
        require_contains(controller_cpp, token, "ShellAccountingReadOnlyController.cpp")
    for token in ["DataAccess", "SQLite", "INSERT INTO", "UPDATE ", "DELETE FROM", "REPLACE INTO", "AccountingReplay", "StrategyEngine", "BrokerOrderPort"]:
        require_not_contains(controller_cpp, token, "ShellAccountingReadOnlyController.cpp")

    changes = changed_paths(root)
    allowed = {

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
        "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
        "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
        "docs/254_shell_accounting_manual_entry_replay_next_phase_authorization_planning_gate.md",
        "docs/255_shell_accounting_manual_entry_replay_next_phase_authorization_planning_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/CMakeLists.txt",
        "tests/CMakeLists.txt",
        "tests/DevDocs/test_readonly_demo_acceptance.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/manual_entry_mvp_runtime_e2e_acceptance.cpp",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/manual_entry_readback_mapping_implementation.cpp",
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
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
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
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/CMakeLists.txt",
        "docs/248_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.md",
        "docs/249_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_test_plan.md",
        "docs/250_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_gate.md",
        "docs/251_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/CMakeLists.txt",
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
}
    unexpected = sorted(path for path in changes if path not in allowed)
    require(not unexpected, "TASK-202 changed unauthorized paths: " + ", ".join(unexpected))
    for forbidden in [
        "libs/DataAccess/",
        "libs/AccountingEngine/",
        "libs/StrategyEngine/",
        "libs/MarketEngine/",
        "migrations/",
    ]:
        require(not any(path.startswith(forbidden) for path in changes), f"TASK-202 must not change {forbidden}")
    for forbidden in [
        "apps/ETFDecisionShell/src/main.cpp",
        "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
        "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
        "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualCashMovementRepository.h",
        "libs/DataAccess/src/ShellAccountingManualCashMovementRepository.cpp",
        "migrations/001_initial_schema.sql",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
    ]:
        require(forbidden not in changes, f"TASK-202 must not change {forbidden}")
    for path in changes:
        if path.startswith("libs/DataServiceApi/"):
            require(
                path == "libs/DataServiceApi/src/DataServiceActions.cpp",
                f"TASK-202/TASK-207 only permits DataServiceActions readback mapping, not {path}",
            )

    for token in ["persistManualTransaction", "persistManualCashMovement"]:
        require_contains(dataservice_actions, token, "DataServiceActions")
    for token in ["INSERT INTO", "UPDATE ", "DELETE FROM", "REPLACE INTO"]:
        require_not_contains(dataservice_actions, token, "DataServiceActions")

    for token in [
        "docs/200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md",
        "docs/201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md",
        "shell_accounting_manual_entry_post_write_readback_refresh_implementation",
    ]:
        require_contains(devdocs, token, "DevDocs acceptance")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
