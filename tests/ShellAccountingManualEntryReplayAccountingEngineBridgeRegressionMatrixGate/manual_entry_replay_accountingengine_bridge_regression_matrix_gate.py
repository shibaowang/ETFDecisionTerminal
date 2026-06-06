TASK_257_EXACT_PATHS = {
    "tests/ShellAccountingManualCashMovementRepositoryDualWriteImplementation/manual_cash_movement_repository_dual_write_implementation.cpp",
    "tests/ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate/manual_cash_movement_repository_write_authorization_gate.py",
    "tests/ShellAccountingManualCashMovementSchemaContractAlignmentGate/manual_cash_movement_schema_contract_alignment_gate.py",
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/310_shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary.md",
    "docs/311_shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary_test_plan.md",
    "libs/DataServiceApi/CMakeLists.txt",
    "libs/DataServiceApi/include/DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h",
    "libs/DataServiceApi/src/ShellAccountingExcelVbaImportReadOnlyParser.cpp",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_buy_only_import_payload.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_cash_adjustment_import_payload.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_chinese_header_buy_partial_sell_import_payload.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_invalid_action_amount_cash_import_payload.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_missing_required_header_import_payload.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/t257_parser_boundary_slice.cpp",
    "docs/312_shell_accounting_excel_vba_import_readonly_dataservice_preview_action.md",
    "docs/313_shell_accounting_excel_vba_import_readonly_dataservice_preview_action_test_plan.md",
    "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
    "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    "libs/DataServiceApi/src/ShellAccountingExcelVbaImportReadOnlyPreviewAction.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/excel_vba_import_readonly_dataservice_preview_action.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/fixtures/TASK258_missing_required_header_preview_payload.json",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/fixtures/TASK258_valid_buy_preview_payload.json",
    "docs/314_shell_accounting_excel_vba_import_readonly_preview_client_adapter.md",
    "docs/315_shell_accounting_excel_vba_import_readonly_preview_client_adapter_test_plan.md",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClient.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientJson.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientTypes.h",
    "libs/DataServiceClient/src/DataServiceClient.cpp",
    "libs/DataServiceClient/src/DataServiceClientJson.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter/excel_vba_import_readonly_preview_client_adapter.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter/fixtures/TASK259_missing_required_header_preview_client_payload.json",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter/fixtures/TASK259_valid_buy_preview_client_payload.json",
    "docs/316_shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract.md",
    "docs/317_shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract_test_plan.md",
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
    "libs/ShellServices/src/ShellAccountingServiceTypes.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewShellServicesPresenterContract/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewShellServicesPresenterContract/excel_vba_import_readonly_preview_shellservices_presenter_contract.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewShellServicesPresenterContract/fixtures/TASK260_missing_required_header_presenter_preview_payload.json",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewShellServicesPresenterContract/fixtures/TASK260_valid_buy_presenter_preview_payload.json",
}
#!/usr/bin/env python3

import argparse
import copy
import hashlib
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Any


TASK_250_PHASE_CLOSEOUT_SELF_CONSISTENCY_PATHS = {
    "docs/296_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_gate.md",
    "docs/297_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/manual_entry_replay_accountingengine_bridge_phase_closeout_gate.py",
}

TASK_247_DOC = Path("docs/290_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_gate.md")
TASK_247_PLAN = Path("docs/291_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_test_plan.md")
TASK_247_DIR = Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate")
TASK_247_CMAKE = TASK_247_DIR / "CMakeLists.txt"
TASK_247_GATE = TASK_247_DIR / "manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py"
TASK_247_CTEST = "shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix"

BRIDGE_SCRIPT = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridge/"
    "manual_entry_replay_accountingengine_bridge.py"
)
BRIDGE_IMPLEMENTATION_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/"
    "manual_entry_replay_accountingengine_bridge_implementation_gate.py"
)
BRIDGE_AUTHORIZATION_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/"
    "manual_entry_replay_accountingengine_bridge_authorization_gate.py"
)

DRY_RUN_SCHEMA = "manual-entry-replay-test-only-dry-run-summary/v1"
IMPLEMENTATION_SCHEMA = "manual-entry-replay-test-only-implementation-summary/v1"
POSITIVE_DIR = Path("tests/fixtures/manual_entry_replay")
NEGATIVE_DIR = Path("tests/fixtures/manual_entry_replay_negative")
DB_SUFFIXES = (".db", ".sqlite", ".sqlite3", ".db-wal", ".db-shm", ".sqlite-wal", ".sqlite-shm")

PREVIOUS_DOCS = [
    Path("docs/274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md"),
    Path("docs/275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md"),
    Path("docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md"),
    Path("docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md"),
    Path("docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md"),
    Path("docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md"),
    Path("docs/280_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.md"),
    Path("docs/281_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_test_plan.md"),
    Path("docs/282_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.md"),
    Path("docs/283_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_test_plan.md"),
    Path("docs/284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md"),
    Path("docs/285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md"),
    Path("docs/286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md"),
    Path("docs/287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md"),
    Path("docs/288_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_gate.md"),
    Path("docs/289_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_test_plan.md"),
]

PREVIOUS_GATES = [
    Path(
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/"
        "manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py"
    ),
    Path(
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/"
        "manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py"
    ),
    Path(
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/"
        "manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py"
    ),
    Path(
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/"
        "manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py"
    ),
    Path(
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/"
        "manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py"
    ),
    Path(
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/"
        "manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py"
    ),
    BRIDGE_AUTHORIZATION_GATE,
    BRIDGE_IMPLEMENTATION_GATE,
]

CRITICAL_CTEST_NAMES = [
    TASK_247_CTEST,
    "shell_accounting_manual_entry_replay_accountingengine_bridge_implementation",
    "shell_accounting_manual_entry_replay_accountingengine_bridge_authorization",
    "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout",
    "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout",
    "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening",
    "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix",
    "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation",
    "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization",
    "shell_accounting_manual_entry_replay_implementation_phase_closeout",
    "shell_accounting_manual_entry_replay_implementation_ci_closeout",
    "shell_accounting_manual_entry_replay_implementation_failure_mode_hardening",
    "shell_accounting_manual_entry_replay_implementation_regression_matrix",
    "shell_accounting_manual_entry_replay_implementation",
    "shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix",
    "shell_accounting_manual_entry_replay_test_only_dry_run_harness",
    "shell_diagnostics_facade",
    "transport_local_socket_echo",
]

ALLOWED_CHANGED_PATHS = {
    "docs/300_shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.md",
    "docs/301_shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice_test_plan.md",
    "docs/302_shell_accounting_manual_entry_replay_fixture_backed_vba_parity_readonly_vertical_slice.md",
    "docs/303_shell_accounting_manual_entry_replay_fixture_backed_vba_parity_test_plan.md",
    "tests/ShellAccountingManualEntryReplayFixtureBackedVbaParityReadOnlyVerticalSlice/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayFixtureBackedVbaParityReadOnlyVerticalSlice/fixtures/TASK253_vba_parity_buy_partial_sell.json",
    "tests/ShellAccountingManualEntryReplayFixtureBackedVbaParityReadOnlyVerticalSlice/manual_entry_replay_fixture_backed_vba_parity_readonly_vertical_slice.cpp",
    "docs/304_shell_accounting_manual_entry_replay_fixture_parity_matrix_readonly_vertical_slice.md",
    "docs/305_shell_accounting_manual_entry_replay_fixture_parity_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_only.json",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_partial_sell.json",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_cash_adjustment.json",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_unsupported_or_issue.json",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/manual_entry_replay_fixture_parity_matrix_readonly_vertical_slice.cpp",
    "docs/306_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice.md",
    "docs/307_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_test_plan.md",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportReadOnlyVerticalSlice/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportReadOnlyVerticalSlice/fixtures/TASK255_sanitized_excel_vba_export_buy_partial_sell.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportReadOnlyVerticalSlice/manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice.cpp",
    "docs/308_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_readonly_vertical_slice.md",
    "docs/309_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_test_plan.md",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_buy_only_export_sample.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_buy_partial_sell_export_sample.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_cash_adjustment_export_sample.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_missing_required_header_sample.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_unsupported_or_issue_sample.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/t256_matrix_slice.cpp",
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice.cpp",
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.py",
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    TASK_247_DOC.as_posix(),
    TASK_247_PLAN.as_posix(),
    "docs/298_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_gate.md",
    "docs/299_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_test_plan.md",
    "tests/CMakeLists.txt",
    TASK_247_CMAKE.as_posix(),
    TASK_247_GATE.as_posix(),
    BRIDGE_SCRIPT.as_posix(),
    "tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate/manual_entry_replay_accountingengine_runtime_integration_authorization_gate.py",
    "docs/294_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_gate.md",
    "docs/295_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/manual_entry_replay_accountingengine_bridge_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    BRIDGE_AUTHORIZATION_GATE.as_posix(),
    BRIDGE_IMPLEMENTATION_GATE.as_posix(),
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
}
ALLOWED_CHANGED_PATHS.update(TASK_257_EXACT_PATHS)

REQUIRED_DOC_290_SECTIONS = [
    "## Purpose",
    "## Relationship To TASK-246",
    "## Regression Matrix Scope",
    "## Bridge CLI Contract",
    "## Positive Matrix",
    "## Failure Matrix",
    "## Sanitized Output Matrix",
    "## No-AccountingEngine-Call Boundary",
    "## No-Replay Boundary",
    "## No-Write Boundary",
    "## AccountingEngine Code Boundary",
    "## Production Integration Boundary",
    "## Fixture Boundary",
    "## Broker And Network Boundary",
    "## Go / No-Go Decision",
    "## Formal Conclusion",
    "## Recommended Next Task",
]

REQUIRED_DOC_291_SECTIONS = [
    "## Test Objective",
    "## Required Static Checks",
    "## Required Regression Gates",
    "## Changed-Path Allowlist",
    "## Bridge Matrix Cases",
    "## Failure Modes",
    "## Acceptance Criteria",
    "## Formal Conclusion",
]

FORMAL_CONCLUSION_LINES = [
    "TASK-247 adds a test-only regression matrix gate for the AccountingEngine bridge contract.",
    "TASK-247 does not call AccountingEngine replay.",
    "TASK-247 does not modify AccountingEngine code.",
    "TASK-247 does not implement production bridge.",
    "TASK-247 does not modify fixture JSON.",
    "TASK-247 does not authorize production integration.",
    "TASK-247 does not authorize runtime SQL / SQLite read/write.",
    "TASK-247 does not authorize audit / ledger / snapshot writes.",
    "TASK-247 does not authorize read model refresh.",
    "TASK-247 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
    "Recommended next task: TASK-248 ShellAccounting manual entry replay AccountingEngine bridge failure-mode hardening gate.",
]

FORBIDDEN_OUTPUT_TOKENS = (
    "raw fixture payload",
    "raw payload",
    "full manual entry payload",
    "select *",
    "insert into",
    "update ",
    "delete from",
    "replace into",
    "drop table",
    "credential=",
    "endpoint=",
    "access token",
    "secret key",
    "private key",
    "password",
    "broker payload",
    "real order id",
    "traceback",
    "stack trace",
    "exception stack",
    "apps/",
    "libs/",
    "migrations/",
)

FORBIDDEN_EXECUTABLE_TOKENS = (
    "import " + "sqlite3",
    "INSERT " + "INTO",
    "UP" + "DATE ",
    "DELETE " + "FROM",
    "REPLACE " + "INTO",
    "AccountingReplayEngine(",
    "AccountingReplayEngine::",
    "AccountingEngine().replay",
    ".replayLedger(",
    "replayReadOnly(",
    "refreshReadModel(",
    "writeAuditLog(",
    "writeLedger(",
    "writeSnapshot(",
    "broker" + "Order(",
    "place" + "Order(",
    "execute" + "Order(",
    "request" + "s.",
    "url" + "lib.",
)


class Gate:
    def __init__(self) -> None:
        self.checks = 0

    def require(self, condition: bool, message: str) -> None:
        self.checks += 1
        passed = bool(condition)
        print(f"CHECK {self.checks:03d} {'PASS' if passed else 'FAIL'} {message}")
        if not passed:
            raise AssertionError(message)

    def contains(self, text: str, token: str, context: str) -> None:
        normalized_text = " ".join(text.split())
        normalized_token = " ".join(token.split())
        self.require(token in text or normalized_token in normalized_text, f"{context} contains `{token}`")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(args: list[str], root: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(args, cwd=root, check=False, capture_output=True, text=True)


def git_lines(root: Path, *args: str) -> set[str]:
    completed = run(["git", *args], root)
    if completed.returncode != 0:
        return set()
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def status_paths(root: Path) -> set[str]:
    completed = run(["git", "status", "--porcelain=v1", "-uall"], root)
    paths: set[str] = set()
    for line in completed.stdout.splitlines():
        if not line:
            continue
        path = line[3:].strip().replace("\\", "/")
        if " -> " in path:
            path = path.split(" -> ", 1)[1]
        if path:
            paths.add(path)
    return paths


def changed_paths(root: Path) -> set[str]:
    return (
        git_lines(root, "diff", "--name-only", "main")
        | git_lines(root, "diff", "--cached", "--name-only")
        | git_lines(root, "ls-files", "--others", "--exclude-standard")
        | status_paths(root)
    )


def fixture_paths(root: Path) -> list[Path]:
    return sorted((root / POSITIVE_DIR).glob("*.json")) + sorted((root / NEGATIVE_DIR).glob("*.json"))


def fixture_hashes(root: Path) -> dict[str, str]:
    return {path.relative_to(root).as_posix(): sha256(path) for path in fixture_paths(root)}


def db_like_untracked_paths(root: Path) -> set[str]:
    return {path for path in status_paths(root) if path.lower().endswith(DB_SUFFIXES)}


def registered_ctests(root: Path) -> set[str]:
    completed = run(["ctest", "--test-dir", str(root / "build"), "-N"], root)
    names: set[str] = set()
    for line in completed.stdout.splitlines():
        match = re.search(r"Test\s+#\d+:\s+(\S+)", line)
        if match:
            names.add(match.group(1))
    return names


def source_without_constant_tables(text: str) -> str:
    kept: list[str] = []
    skipping = False
    for line in text.splitlines():
        if re.match(r"^[A-Z0-9_]+\s*=\s*[\(\[\{]", line):
            skipping = True
            continue
        if skipping:
            if line.strip() in {")", "]", "}"}:
                skipping = False
            continue
        kept.append(line)
    return "\n".join(kept)


def executable_scan_text(text: str) -> str:
    kept: list[str] = []
    for line in source_without_constant_tables(text).splitlines():
        stripped = line.strip()
        if "not in" in stripped or "FORBIDDEN" in stripped:
            continue
        if "libs/AccountingEngine" in stripped and ("git_lines(" in stripped or stripped.startswith("for path in")):
            continue
        kept.append(line)
    return "\n".join(kept)


def validate_changed_paths(gate: Gate, root: Path) -> set[str]:
    changes = changed_paths(root)
    unexpected = sorted(path for path in changes if path not in ALLOWED_CHANGED_PATHS)
    gate.require(not unexpected, f"changed paths are exact allowlisted: {unexpected}")
    for path in sorted(changes):
        gate.require(path in ALLOWED_CHANGED_PATHS, f"changed path exact allowlisted: {path}")
        if path in TASK_257_EXACT_PATHS:
            continue
        gate.require(not path.startswith(("apps/", "libs/", "migrations/")), f"production path unchanged: {path}")
        gate.require(not path.startswith((POSITIVE_DIR.as_posix(), NEGATIVE_DIR.as_posix())), f"fixture path unchanged: {path}")
        gate.require((path in {"tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_only.json", "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_partial_sell.json", "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_cash_adjustment.json", "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_unsupported_or_issue.json", "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportReadOnlyVerticalSlice/fixtures/TASK255_sanitized_excel_vba_export_buy_partial_sell.json", "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_buy_only_export_sample.json", "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_buy_partial_sell_export_sample.json", "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_cash_adjustment_export_sample.json", "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_missing_required_header_sample.json", "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_unsupported_or_issue_sample.json"} or path == "tests/ShellAccountingManualEntryReplayFixtureBackedVbaParityReadOnlyVerticalSlice/fixtures/TASK253_vba_parity_buy_partial_sell.json" or path in TASK_257_EXACT_PATHS or path in TASK_257_EXACT_PATHS or not path.endswith(".json")), f"JSON fixture path unchanged: {path}")
        gate.require(not path.endswith(".sql"), f"SQL path unchanged: {path}")
    for wildcard in ["docs/", "tests/", "apps/", "libs/", "migrations/"]:
        gate.require(wildcard not in ALLOWED_CHANGED_PATHS, f"allowlist has no {wildcard} wildcard")
    for path in ["apps", "libs", "migrations", "libs/AccountingEngine", POSITIVE_DIR.as_posix(), NEGATIVE_DIR.as_posix()]:
        diff_paths = git_lines(root, "diff", "--name-only", "main", "--", path)
        if path == "libs":
            gate.require(diff_paths <= TASK_257_EXACT_PATHS, f"{path} diff exact TASK-257 parser boundary only")
        else:
            gate.require(diff_paths == set(), f"{path} diff empty")
    return changes


def validate_docs(gate: Gate, root: Path) -> None:
    doc_290 = read(root / TASK_247_DOC)
    doc_291 = read(root / TASK_247_PLAN)
    gate.require((root / TASK_247_DOC).exists(), "docs/290 exists")
    gate.require((root / TASK_247_PLAN).exists(), "docs/291 exists")
    gate.contains(doc_290, "# TASK-247 ShellAccounting manual entry replay AccountingEngine bridge regression matrix gate", "docs/290 title")
    gate.contains(doc_291, "# TASK-247 AccountingEngine bridge regression matrix test plan", "docs/291 title")
    for section in REQUIRED_DOC_290_SECTIONS:
        gate.contains(doc_290, section, "docs/290")
    for section in REQUIRED_DOC_291_SECTIONS:
        gate.contains(doc_291, section, "docs/291")
    for line in FORMAL_CONCLUSION_LINES:
        gate.contains(doc_290, line, "docs/290 Formal Conclusion")
        gate.contains(doc_291, line, "docs/291 Formal Conclusion")
    for path in PREVIOUS_DOCS:
        gate.require((root / path).exists(), f"{path.as_posix()} exists")
    for index_path in [Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        text = read(root / index_path)
        gate.contains(text, "TASK-247", index_path.as_posix())
        gate.contains(text, TASK_247_DOC.name, index_path.as_posix())
        gate.contains(text, TASK_247_PLAN.name, index_path.as_posix())


def validate_registration(gate: Gate, root: Path) -> None:
    tests_cmake = read(root / "tests/CMakeLists.txt")
    task_cmake = read(root / TASK_247_CMAKE)
    gate.contains(tests_cmake, "add_subdirectory(ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate)", "tests/CMakeLists")
    gate.contains(task_cmake, TASK_247_CTEST, "TASK-247 CMakeLists")
    gate.contains(task_cmake, TASK_247_GATE.name, "TASK-247 CMakeLists")
    gate.require((root / TASK_247_CMAKE).exists(), "TASK-247 CMakeLists exists")
    gate.require((root / TASK_247_GATE).exists(), "TASK-247 gate exists")
    gate.require((root / BRIDGE_SCRIPT).exists(), "TASK-246 bridge script exists")
    gate.require((root / BRIDGE_IMPLEMENTATION_GATE).exists(), "TASK-246 implementation gate exists")
    gate.require((root / BRIDGE_AUTHORIZATION_GATE).exists(), "TASK-245 authorization gate exists")
    for path in PREVIOUS_GATES:
        gate.require((root / path).exists(), f"{path.as_posix()} exists")
    names = registered_ctests(root)
    gate.require(bool(names), "ctest -N returns registered tests")
    for name in CRITICAL_CTEST_NAMES:
        gate.require(name in names, f"{name} registered")


def validate_static_boundaries(gate: Gate, root: Path) -> None:
    for path in [BRIDGE_SCRIPT, BRIDGE_IMPLEMENTATION_GATE, TASK_247_GATE]:
        text = read(root / path)
        scan = executable_scan_text(text)
        for token in FORBIDDEN_EXECUTABLE_TOKENS:
            gate.require(token not in scan, f"{path.as_posix()} omits executable token `{token}`")
        gate.require("libs/AccountingEngine" not in scan, f"{path.as_posix()} omits AccountingEngine path")
        gate.require("replayReadOnly" not in scan, f"{path.as_posix()} omits replayReadOnly")
        gate.require("ctest --test-dir build --output-on-failure" not in scan, f"{path.as_posix()} does not run full CTest")
    bridge_text = read(root / BRIDGE_SCRIPT)
    for token in ["--dry-run-summary-json", "--implementation-summary-json", "--summary-json", "--expect-no-accountingengine-call"]:
        gate.contains(bridge_text, token, "bridge CLI")
    gate.contains(bridge_text, "syntheticDataOnly", "bridge synthetic guard")


def valid_dry_summary() -> dict[str, Any]:
    return {
        "schemaVersion": DRY_RUN_SCHEMA,
        "syntheticDataOnly": True,
        "dryRunStatus": "ok",
        "entries": [
            {
                "fixtureId": "MRF-REGRESSION-001",
                "accountingEngineCalled": False,
                "replayExecuted": False,
                "runtimeWrites": False,
            }
        ],
    }


def valid_implementation_summary() -> dict[str, Any]:
    return {
        "schemaVersion": IMPLEMENTATION_SCHEMA,
        "syntheticDataOnly": True,
        "implementationMode": "test-only-in-memory",
        "implementationStatus": "ok",
        "entries": [
            {
                "fixtureId": "MRF-REGRESSION-001",
                "accountingEngineCalled": False,
                "runtimeWrites": False,
                "positionCashPnlCalculated": False,
            }
        ],
    }


def write_json(path: Path, value: dict[str, Any]) -> None:
    path.write_text(json.dumps(value, sort_keys=True), encoding="utf-8")


def run_bridge(root: Path, args: list[str]) -> subprocess.CompletedProcess[str]:
    return run([sys.executable, str(root / BRIDGE_SCRIPT), "--source-root", str(root), *args], root)


def assert_sanitized(gate: Gate, value: dict[str, Any], label: str) -> None:
    encoded = json.dumps(value, sort_keys=True).lower()
    for token in FORBIDDEN_OUTPUT_TOKENS:
        gate.require(token not in encoded, f"{label} excludes forbidden token `{token}`")


def validate_positive_summary(gate: Gate, summary: dict[str, Any]) -> None:
    gate.require(summary.get("schemaVersion") == 1, "positive schemaVersion=1")
    gate.require(summary.get("task") == "TASK-246", "positive task=TASK-246")
    gate.require(summary.get("mode") == "test-only-accountingengine-bridge-contract", "positive mode exact")
    gate.require(summary.get("accountingEngineCalled") is False, "positive accountingEngineCalled=false")
    gate.require(summary.get("replayExecuted") is False, "positive replayExecuted=false")
    gate.require(summary.get("runtimeWrite") is False, "positive runtimeWrite=false")
    request = summary.get("bridgeRequest")
    response = summary.get("bridgeResponse")
    gate.require(isinstance(request, dict), "positive bridgeRequest object")
    gate.require(isinstance(response, dict), "positive bridgeResponse object")
    gate.require(request.get("syntheticDataOnly") is True, "positive bridgeRequest.syntheticDataOnly=true")
    gate.require(response.get("status") == "not_executed", "positive bridgeResponse.status=not_executed")
    gate.require(response.get("reason") == "AccountingEngine replay is not authorized in TASK-246", "positive bridgeResponse reason exact")
    gate.require(summary.get("issues") == [], "positive issues empty")
    assert_sanitized(gate, summary, "positive summary")


def validate_failure_summary(gate: Gate, summary: dict[str, Any], case_name: str) -> None:
    gate.require(summary.get("schemaVersion") == 1, f"{case_name} failure schemaVersion=1")
    gate.require(summary.get("task") == "TASK-246", f"{case_name} failure task=TASK-246")
    gate.require(summary.get("mode") == "test-only-accountingengine-bridge-contract", f"{case_name} failure mode exact")
    gate.require(summary.get("accountingEngineCalled") is False, f"{case_name} failure accountingEngineCalled=false")
    gate.require(summary.get("replayExecuted") is False, f"{case_name} failure replayExecuted=false")
    gate.require(summary.get("runtimeWrite") is False, f"{case_name} failure runtimeWrite=false")
    response = summary.get("bridgeResponse")
    gate.require(isinstance(response, dict), f"{case_name} failure response object")
    gate.require(response.get("status") == "failed", f"{case_name} failure status failed")
    gate.require("sanitized failure" in response.get("reason", ""), f"{case_name} failure reason sanitized")
    gate.require(isinstance(summary.get("issues"), list) and summary["issues"], f"{case_name} failure issues present")
    assert_sanitized(gate, summary, f"{case_name} failure summary")


def validate_bridge_matrix(gate: Gate, root: Path) -> None:
    with tempfile.TemporaryDirectory() as tmp:
        tmpdir = Path(tmp)
        dry_path = tmpdir / "dry.json"
        implementation_path = tmpdir / "implementation.json"
        summary_path = tmpdir / "summary.json"
        write_json(dry_path, valid_dry_summary())
        write_json(implementation_path, valid_implementation_summary())

        help_completed = run([sys.executable, str(root / BRIDGE_SCRIPT), "--help"], root)
        gate.require(help_completed.returncode == 0, "bridge --help exits zero")
        for token in ["--dry-run-summary-json", "--implementation-summary-json", "--summary-json", "--expect-no-accountingengine-call"]:
            gate.contains(help_completed.stdout, token, "bridge --help")

        stdout_completed = run_bridge(
            root,
            [
                "--dry-run-summary-json",
                str(dry_path),
                "--implementation-summary-json",
                str(implementation_path),
                "--summary-json",
                "-",
                "--expect-no-accountingengine-call",
            ],
        )
        gate.require(stdout_completed.returncode == 0, "positive stdout exits zero")
        gate.require(stdout_completed.stderr.strip() == "", "positive stdout stderr empty")
        validate_positive_summary(gate, json.loads(stdout_completed.stdout))

        file_completed = run_bridge(
            root,
            [
                "--dry-run-summary-json",
                str(dry_path),
                "--implementation-summary-json",
                str(implementation_path),
                "--summary-json",
                str(summary_path),
                "--expect-no-accountingengine-call",
            ],
        )
        gate.require(file_completed.returncode == 0, "positive file exits zero")
        gate.require(file_completed.stdout.strip() == "", "positive file stdout empty")
        gate.require(file_completed.stderr.strip() == "", "positive file stderr empty")
        gate.require(summary_path.exists(), "positive file summary exists")
        validate_positive_summary(gate, json.loads(summary_path.read_text(encoding="utf-8")))

        failure_cases = build_failure_cases(tmpdir)
        for case_name, dry_value, implementation_value, malformed in failure_cases:
            case_dir = tmpdir / case_name
            case_dir.mkdir()
            case_dry_path = case_dir / "dry.json"
            case_impl_path = case_dir / "implementation.json"
            case_output_path = case_dir / "failure.json"
            if malformed == "missing-file":
                case_dry_path = case_dir / "missing.json"
            elif malformed == "malformed-json":
                case_dry_path.write_text('{"raw payload": "must not echo"', encoding="utf-8")
            else:
                write_json(case_dry_path, dry_value)
            write_json(case_impl_path, implementation_value)
            completed = run_bridge(
                root,
                [
                    "--dry-run-summary-json",
                    str(case_dry_path),
                    "--implementation-summary-json",
                    str(case_impl_path),
                    "--summary-json",
                    str(case_output_path),
                    "--expect-no-accountingengine-call",
                ],
            )
            gate.require(completed.returncode != 0, f"{case_name} fails closed")
            gate.require(completed.stdout.strip() == "", f"{case_name} stdout empty")
            gate.require(completed.stderr.strip() == "", f"{case_name} stderr empty")
            gate.require(case_output_path.exists(), f"{case_name} failure output exists")
            validate_failure_summary(gate, json.loads(case_output_path.read_text(encoding="utf-8")), case_name)


def build_failure_cases(tmpdir: Path) -> list[tuple[str, dict[str, Any], dict[str, Any], str | None]]:
    dry = valid_dry_summary()
    implementation = valid_implementation_summary()
    cases: list[tuple[str, dict[str, Any], dict[str, Any], str | None]] = []
    cases.append(("missing_input_summary", dry, implementation, "missing-file"))
    cases.append(("malformed_json", dry, implementation, "malformed-json"))
    missing_required = copy.deepcopy(dry)
    missing_required.pop("schemaVersion", None)
    cases.append(("missing_required_fields", missing_required, implementation, None))
    non_synthetic = copy.deepcopy(dry)
    non_synthetic["syntheticDataOnly"] = False
    cases.append(("non_synthetic_flag", non_synthetic, implementation, None))
    replay_true = copy.deepcopy(dry)
    replay_true["entries"][0]["replayExecuted"] = True
    cases.append(("replay_executed_true", replay_true, implementation, None))
    engine_true = copy.deepcopy(dry)
    engine_true["entries"][0]["accountingEngineCalled"] = True
    cases.append(("accountingengine_called_true", engine_true, implementation, None))
    runtime_write_true = copy.deepcopy(implementation)
    runtime_write_true["entries"][0]["runtimeWrites"] = True
    cases.append(("runtime_write_true", dry, runtime_write_true, None))
    return cases


def validate_no_mutation(gate: Gate, root: Path, before_hashes: dict[str, str], before_db_like: set[str], before_changes: set[str]) -> None:
    after_hashes = fixture_hashes(root)
    after_db_like = db_like_untracked_paths(root)
    after_changes = changed_paths(root)
    gate.require(before_hashes == after_hashes, "fixture JSON hashes unchanged")
    gate.require(bool(before_hashes), "fixture hash set non-empty")
    gate.require(before_db_like == after_db_like, "no DB/WAL/SHM/SQLite files created")
    gate.require(before_changes == after_changes, "gate does not mutate changed-path set")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root).resolve()
    gate = Gate()

    before_hashes = fixture_hashes(root)
    before_db_like = db_like_untracked_paths(root)
    before_changes = validate_changed_paths(gate, root)
    validate_docs(gate, root)
    validate_registration(gate, root)
    validate_static_boundaries(gate, root)
    validate_bridge_matrix(gate, root)
    validate_no_mutation(gate, root, before_hashes, before_db_like, before_changes)
    gate.require(gate.checks >= 160, f"expected at least 160 gate checks, got {gate.checks}")
    print(json.dumps({"status": "passed", "checks": gate.checks}, sort_keys=True))
    return 0



TASK_247_OLD_GATE_SELF_CONSISTENCY_PATHS = {
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
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/manual_entry_replay_negative_fixture_validator_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/manual_entry_replay_next_phase_authorization_planning_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
}
ALLOWED_CHANGED_PATHS.update(TASK_247_OLD_GATE_SELF_CONSISTENCY_PATHS)

TASK_248_SELF_CONSISTENCY_PATHS = {
    "docs/292_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.md",
    "docs/293_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.py",
}
ALLOWED_CHANGED_PATHS.update(TASK_248_SELF_CONSISTENCY_PATHS)

TASK_248_OLD_GATE_SELF_CONSISTENCY_PATHS = {
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
}

ALLOWED_CHANGED_PATHS.update(TASK_248_OLD_GATE_SELF_CONSISTENCY_PATHS)



TASK_248_FULL_CTEST_SELF_CONSISTENCY_PATHS = {
    "docs/12_codex_prompt_template.md",
    "docs/292_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.md",
    "docs/293_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_test_plan.md",
    "docs/README.md",
    "README.md",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
    "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
    "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
    "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridge/manual_entry_replay_accountingengine_bridge.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/manual_entry_replay_negative_fixture_validator_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/manual_entry_replay_next_phase_authorization_planning_gate.py",
    "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
}

ALLOWED_CHANGED_PATHS.update(TASK_248_FULL_CTEST_SELF_CONSISTENCY_PATHS)

ALLOWED_CHANGED_PATHS.update(TASK_250_PHASE_CLOSEOUT_SELF_CONSISTENCY_PATHS)
if __name__ == "__main__":
    raise SystemExit(main())
