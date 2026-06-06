#!/usr/bin/env python3

import argparse
import re
import subprocess
from pathlib import Path


TASK_251_DOC = Path("docs/298_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_gate.md")
TASK_251_PLAN = Path("docs/299_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_test_plan.md")
TASK_251_DIR = Path("tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate")
TASK_251_CMAKE = TASK_251_DIR / "CMakeLists.txt"
TASK_251_GATE = TASK_251_DIR / "manual_entry_replay_accountingengine_runtime_integration_authorization_gate.py"
TASK_251_CTEST = "shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization"

BRIDGE_SCRIPT = Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridge/manual_entry_replay_accountingengine_bridge.py")
TASK_249_GATE = Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/manual_entry_replay_accountingengine_bridge_ci_closeout_gate.py")
TASK_250_GATE = Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/manual_entry_replay_accountingengine_bridge_phase_closeout_gate.py")

POSITIVE_DIR = Path("tests/fixtures/manual_entry_replay")
NEGATIVE_DIR = Path("tests/fixtures/manual_entry_replay_negative")
DB_SUFFIXES = (".db", ".sqlite", ".sqlite3", ".db-wal", ".db-shm", ".sqlite-wal", ".sqlite-shm")

TASK_245_TO_250_DOCS = [
    Path("docs/286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md"),
    Path("docs/287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md"),
    Path("docs/288_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_gate.md"),
    Path("docs/289_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_test_plan.md"),
    Path("docs/290_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_gate.md"),
    Path("docs/291_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_test_plan.md"),
    Path("docs/292_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.md"),
    Path("docs/293_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_test_plan.md"),
    Path("docs/294_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_gate.md"),
    Path("docs/295_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_test_plan.md"),
    Path("docs/296_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_gate.md"),
    Path("docs/297_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_test_plan.md"),
]

ACCOUNTINGENGINE_ADEQUACY_DOCS = [
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
]

CRITICAL_GATE_SCRIPTS = [
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.py"),
    TASK_249_GATE,
    TASK_250_GATE,
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py"),
    Path("tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py"),
    TASK_251_GATE,
]

CRITICAL_CTEST_NAMES = [
    TASK_251_CTEST,
    "shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout",
    "shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout",
    "shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening",
    "shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix",
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
    "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
    "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate/manual_entry_replay_accountingengine_runtime_integration_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    TASK_251_DOC.as_posix(),
    TASK_251_PLAN.as_posix(),
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
    "tests/CMakeLists.txt",
    TASK_251_CMAKE.as_posix(),
    TASK_251_GATE.as_posix(),
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice.cpp",
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.py",
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
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/manual_entry_replay_accountingengine_bridge_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/manual_entry_replay_accountingengine_bridge_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
}

REQUIRED_DOC_298_SECTIONS = [
    "## Purpose",
    "## Relationship To TASK-245 Through TASK-250",
    "## Authorization Scope",
    "## Future Runtime Integration Boundary",
    "## Future Integration Entry Points",
    "## Future Data Flow Contract",
    "## AccountingEngine Call Authorization Boundary",
    "## Replay Boundary",
    "## Runtime Write Boundary",
    "## Audit Ledger Snapshot Boundary",
    "## Read Model Refresh Boundary",
    "## AccountingEngine Code Boundary",
    "## Production Code Boundary",
    "## Fixture Boundary",
    "## Broker And Network Boundary",
    "## Go / No-Go Decision",
    "## Formal Conclusion",
    "## Recommended Next Task",
]

REQUIRED_DOC_299_SECTIONS = [
    "## Test Objective",
    "## Required Static Checks",
    "## Required Regression Gates",
    "## Changed-Path Allowlist",
    "## Authorization Checks",
    "## Runtime Integration Boundary Checks",
    "## Failure Modes",
    "## Acceptance Criteria",
    "## Formal Conclusion",
]

FORMAL_CONCLUSION_LINES = [
    "TASK-251 authorizes AccountingEngine runtime integration policy only.",
    "TASK-251 does not implement runtime integration.",
    "TASK-251 does not call AccountingEngine replay.",
    "TASK-251 does not modify AccountingEngine code.",
    "TASK-251 does not implement production bridge.",
    "TASK-251 does not modify fixture JSON.",
    "TASK-251 does not modify production code.",
    "TASK-251 does not authorize runtime SQL / SQLite read/write.",
    "TASK-251 does not authorize audit / ledger / snapshot writes.",
    "TASK-251 does not authorize read model refresh.",
    "TASK-251 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
    "Recommended next task: TASK-252 ShellAccounting manual entry replay AccountingEngine runtime integration implementation authorization gate.",
]

FUTURE_RUNTIME_PATHS = [
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegration"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationImplementation"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationGate"),
    Path("libs/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegration"),
    Path("libs/DataServiceApi/src/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegration.cpp"),
]

PRODUCTION_BRIDGE_PATHS = [
    Path("apps/ETFDecisionShell/src/ShellAccountingManualEntryReplayAccountingEngineBridge.cpp"),
    Path("libs/ShellServices/src/ShellAccountingManualEntryReplayAccountingEngineBridge.cpp"),
    Path("libs/DataServiceApi/src/ShellAccountingManualEntryReplayAccountingEngineBridge.cpp"),
    Path("libs/DataServiceApi/src/ShellAccountingManualEntryReplayAccountingEngineRuntimeBridge.cpp"),
]

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
    depth = 0
    for line in text.splitlines():
        if not skipping and re.match(r"^\s*[A-Z][A-Z0-9_]*\s*=\s*[\(\[\{]", line):
            skipping = True
            depth = line.count("(") + line.count("[") + line.count("{") - line.count(")") - line.count("]") - line.count("}")
            continue
        if skipping:
            depth += line.count("(") + line.count("[") + line.count("{") - line.count(")") - line.count("]") - line.count("}")
            if depth <= 0:
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
        if any(marker in stripped for marker in ("gate.require(", ".require(", ".contains(", "for token in", "if token")):
            continue
        kept.append(line)
    return "\n".join(kept)


def validate_docs(gate: Gate, root: Path) -> None:
    gate.require((root / TASK_251_DOC).exists(), "docs/298 exists")
    gate.require((root / TASK_251_PLAN).exists(), "docs/299 exists")
    doc_298 = read(root / TASK_251_DOC)
    doc_299 = read(root / TASK_251_PLAN)
    gate.contains(doc_298, "# TASK-251 ShellAccounting manual entry replay AccountingEngine runtime integration authorization gate", "docs/298 title")
    gate.contains(doc_299, "# TASK-251 AccountingEngine runtime integration authorization test plan", "docs/299 title")
    for section in REQUIRED_DOC_298_SECTIONS:
        gate.contains(doc_298, section, "docs/298")
    for section in REQUIRED_DOC_299_SECTIONS:
        gate.contains(doc_299, section, "docs/299")
    for line in FORMAL_CONCLUSION_LINES:
        gate.contains(doc_298, line, "docs/298 Formal Conclusion")
        gate.contains(doc_299, line, "docs/299 Formal Conclusion")
    for token in [
        "authorization-only",
        "runtime-integration-policy-only",
        "future runtime integration policy",
        "does not implement runtime integration",
        "does not call AccountingEngine replay",
        "does not modify AccountingEngine code",
        "does not modify fixture JSON",
        "does not modify production code",
        "does not authorize runtime SQL / SQLite read/write",
        "does not authorize audit / ledger / snapshot writes",
        "does not authorize read model refresh",
        "does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading",
    ]:
        gate.contains(doc_298, token, "docs/298 policy")
    for path in TASK_245_TO_250_DOCS + ACCOUNTINGENGINE_ADEQUACY_DOCS:
        gate.require((root / path).exists(), f"{path.as_posix()} exists")
    for index_path in [Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        text = read(root / index_path)
        gate.contains(text, "TASK-251", index_path.as_posix())
        gate.contains(text, TASK_251_DOC.name, index_path.as_posix())
        gate.contains(text, TASK_251_PLAN.name, index_path.as_posix())


def validate_registration(gate: Gate, root: Path) -> None:
    tests_cmake = read(root / "tests/CMakeLists.txt")
    task_cmake = read(root / TASK_251_CMAKE)
    gate.require((root / TASK_251_CMAKE).exists(), "TASK-251 CMakeLists exists")
    gate.require((root / TASK_251_GATE).exists(), "TASK-251 gate exists")
    gate.contains(tests_cmake, "add_subdirectory(ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate)", "tests/CMakeLists")
    gate.contains(task_cmake, TASK_251_CTEST, "TASK-251 CMakeLists")
    gate.contains(task_cmake, TASK_251_GATE.name, "TASK-251 CMakeLists")
    gate.require((root / BRIDGE_SCRIPT).exists(), "TASK-246 bridge script exists")
    gate.require((root / TASK_249_GATE).exists(), "TASK-249 CI closeout gate exists")
    gate.require((root / TASK_250_GATE).exists(), "TASK-250 phase closeout gate exists")
    for path in CRITICAL_GATE_SCRIPTS:
        gate.require((root / path).exists(), f"{path.as_posix()} exists")
    names = registered_ctests(root)
    gate.require(bool(names), "ctest -N returns registered tests")
    for name in CRITICAL_CTEST_NAMES:
        gate.require(name in names, f"{name} registered")


def validate_changed_paths(gate: Gate, root: Path) -> set[str]:
    changes = changed_paths(root)
    unexpected = sorted(path for path in changes if path not in ALLOWED_CHANGED_PATHS)
    gate.require(not unexpected, f"changed paths are exact allowlisted: {unexpected}")
    gate.require(all(not path.endswith("/") for path in ALLOWED_CHANGED_PATHS), "allowlist uses exact files only")
    gate.require(all("*" not in path for path in ALLOWED_CHANGED_PATHS), "allowlist uses no wildcards")
    for wildcard in ["docs/", "tests/", "apps/", "libs/", "migrations/"]:
        gate.require(wildcard not in ALLOWED_CHANGED_PATHS, f"allowlist has no {wildcard} wildcard")
    for path in sorted(changes):
        gate.require(path in ALLOWED_CHANGED_PATHS, f"changed path exact allowlisted: {path}")
        gate.require(not path.startswith(("apps/", "libs/", "migrations/")), f"production path unchanged: {path}")
        gate.require(not path.startswith((POSITIVE_DIR.as_posix(), NEGATIVE_DIR.as_posix())), f"fixture path unchanged: {path}")
        gate.require((path in {"tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_only.json", "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_partial_sell.json", "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_cash_adjustment.json", "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_unsupported_or_issue.json", "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportReadOnlyVerticalSlice/fixtures/TASK255_sanitized_excel_vba_export_buy_partial_sell.json"} or path == "tests/ShellAccountingManualEntryReplayFixtureBackedVbaParityReadOnlyVerticalSlice/fixtures/TASK253_vba_parity_buy_partial_sell.json" or not path.endswith(".json")), f"JSON fixture path unchanged: {path}")
        gate.require(not path.lower().endswith(DB_SUFFIXES), f"database-like path unchanged: {path}")
    for path in ["apps", "libs", "migrations", "libs/AccountingEngine", POSITIVE_DIR.as_posix(), NEGATIVE_DIR.as_posix()]:
        gate.require(git_lines(root, "diff", "--name-only", "main", "--", path) == set(), f"{path} diff empty")
    gate.require(not {path for path in status_paths(root) if path.lower().endswith(DB_SUFFIXES)}, "no untracked database-like files")
    return changes


def validate_runtime_integration_absence(gate: Gate, root: Path) -> None:
    for path in FUTURE_RUNTIME_PATHS:
        gate.require(not (root / path).exists(), f"future runtime integration path absent: {path.as_posix()}")
    for path in PRODUCTION_BRIDGE_PATHS:
        gate.require(not (root / path).exists(), f"production bridge path absent: {path.as_posix()}")


def validate_static_boundaries(gate: Gate, root: Path) -> None:
    scan_paths = [BRIDGE_SCRIPT, *CRITICAL_GATE_SCRIPTS]
    for path in scan_paths:
        text = read(root / path)
        scan = executable_scan_text(text)
        for token in FORBIDDEN_EXECUTABLE_TOKENS:
            gate.require(token not in scan, f"{path.as_posix()} omits executable token `{token}`")
        gate.require("libs/AccountingEngine" not in scan, f"{path.as_posix()} omits AccountingEngine path")
        gate.require("replayReadOnly" not in scan, f"{path.as_posix()} omits replayReadOnly")
        gate.require("runtime integration" not in scan.lower() or path == TASK_251_GATE, f"{path.as_posix()} has no executable runtime integration")
        gate.require("ctest --test-dir build --output-on-failure" not in scan, f"{path.as_posix()} does not run full CTest")
        gate.require("--repeat until-fail:50" not in scan, f"{path.as_posix()} does not run transport repeat")
    task_text = read(root / TASK_251_GATE)
    forbidden_temp_module = "temp" + "file"
    gate.require(forbidden_temp_module not in task_text, "TASK-251 gate does not write temporary files")
    gate.require("write_text(" not in executable_scan_text(task_text), "TASK-251 gate does not write files")
    gate.require("open(" not in executable_scan_text(task_text), "TASK-251 gate does not open files for writing")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()

    root = Path(args.source_root).resolve()
    gate = Gate()
    validate_docs(gate, root)
    validate_registration(gate, root)
    validate_changed_paths(gate, root)
    validate_runtime_integration_absence(gate, root)
    validate_static_boundaries(gate, root)
    gate.require(gate.checks >= 180, f"expected at least 180 checks, got {gate.checks}")
    print(f"TASK_251_CHECKS={gate.checks}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
