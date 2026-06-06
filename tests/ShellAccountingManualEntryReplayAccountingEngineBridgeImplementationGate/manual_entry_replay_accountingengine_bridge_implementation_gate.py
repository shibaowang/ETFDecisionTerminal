#!/usr/bin/env python3

import argparse
import hashlib
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Any


TASK_246_DOC = Path("docs/288_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_gate.md")
TASK_246_PLAN = Path("docs/289_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_test_plan.md")
BRIDGE_DIR = Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridge")
BRIDGE_CMAKE = BRIDGE_DIR / "CMakeLists.txt"
BRIDGE_SCRIPT = BRIDGE_DIR / "manual_entry_replay_accountingengine_bridge.py"
GATE_DIR = Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate")
GATE_CMAKE = GATE_DIR / "CMakeLists.txt"
GATE_SCRIPT = GATE_DIR / "manual_entry_replay_accountingengine_bridge_implementation_gate.py"
TASK_246_CTEST = "shell_accounting_manual_entry_replay_accountingengine_bridge_implementation"

TASK_245_DOC = Path("docs/286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md")
TASK_245_PLAN = Path("docs/287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md")
TASK_245_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/"
    "manual_entry_replay_accountingengine_bridge_authorization_gate.py"
)
TASK_231_HARNESS = Path(
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/"
    "manual_entry_replay_test_only_dry_run_harness.py"
)
TASK_234_IMPLEMENTATION = Path(
    "tests/ShellAccountingManualEntryReplayImplementation/"
    "manual_entry_replay_implementation.py"
)

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
    TASK_245_DOC,
    TASK_245_PLAN,
]

CRITICAL_CTEST_NAMES = [
    TASK_246_CTEST,
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
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    TASK_246_DOC.as_posix(),
    TASK_246_PLAN.as_posix(),
    "tests/CMakeLists.txt",
    BRIDGE_CMAKE.as_posix(),
    BRIDGE_SCRIPT.as_posix(),
    GATE_CMAKE.as_posix(),
    GATE_SCRIPT.as_posix(),
    (
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/"
        "manual_entry_replay_accountingengine_bridge_authorization_gate.py"
    ),
    (
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/"
        "manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py"
    ),
    (
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/"
        "manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py"
    ),
    (
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/"
        "manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py"
    ),
    (
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/"
        "manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py"
    ),
    (
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/"
        "manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py"
    ),
    (
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/"
        "manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py"
    ),
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
}

TASK_246_SELF_CONSISTENCY_PATHS = {
    "README.md",
    "docs/12_codex_prompt_template.md",
    "docs/288_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_gate.md",
    "docs/289_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_test_plan.md",
    "docs/README.md",
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
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridge/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridge/manual_entry_replay_accountingengine_bridge.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py",
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

ALLOWED_CHANGED_PATHS.update(TASK_246_SELF_CONSISTENCY_PATHS)

TASK_247_SELF_CONSISTENCY_PATHS = {
    "docs/290_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_gate.md",
    "docs/291_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py",
}

ALLOWED_CHANGED_PATHS.update(TASK_247_SELF_CONSISTENCY_PATHS)

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


FORBIDDEN_CHANGED_PREFIXES = (
    "apps/",
    "libs/",
    "migrations/",
    "tests/fixtures/manual_entry_replay/",
    "tests/fixtures/manual_entry_replay_negative/",
)

REQUIRED_DOC_288_SECTIONS = [
    "## Purpose",
    "## Relationship To TASK-245",
    "## Implementation Scope",
    "## Test-Only Bridge Directory",
    "## Bridge Input Contract",
    "## Bridge Output Contract",
    "## Sanitized Failure Contract",
    "## AccountingEngine Call Boundary",
    "## Replay Boundary",
    "## Runtime Isolation Boundary",
    "## No-Write Boundary",
    "## AccountingEngine Code Boundary",
    "## Production Integration Boundary",
    "## Fixture Boundary",
    "## Broker And Network Boundary",
    "## Go / No-Go Decision",
    "## Formal Conclusion",
    "## Recommended Next Task",
]

REQUIRED_DOC_289_SECTIONS = [
    "## Test Objective",
    "## Required Static Checks",
    "## Required Regression Gates",
    "## Changed-Path Allowlist",
    "## Bridge Contract Checks",
    "## Failure Modes",
    "## Acceptance Criteria",
    "## Formal Conclusion",
]

FORMAL_CONCLUSION_LINES = [
    "TASK-246 implements a test-only AccountingEngine bridge contract shell.",
    "TASK-246 does not call AccountingEngine replay.",
    "TASK-246 does not modify AccountingEngine code.",
    "TASK-246 does not implement production bridge.",
    "TASK-246 does not modify fixture JSON.",
    "TASK-246 does not authorize production integration.",
    "TASK-246 does not authorize runtime SQL / SQLite read/write.",
    "TASK-246 does not authorize audit / ledger / snapshot writes.",
    "TASK-246 does not authorize read model refresh.",
    "TASK-246 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
    "Recommended next task: TASK-247 ShellAccounting manual entry replay AccountingEngine bridge regression matrix gate.",
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
    "import sqlite3",
    "INSERT INTO",
    "UPDATE ",
    "DELETE FROM",
    "REPLACE INTO",
    "AccountingReplayEngine(",
    "AccountingReplayEngine::",
    "replayReadOnly(",
    ".replayLedger(",
    "refreshReadModel(",
    "writeAuditLog(",
    "writeLedger(",
    "writeSnapshot(",
    "brokerOrder(",
    "placeOrder(",
    "executeOrder(",
    "requests.",
    "urllib.",
)

DB_SUFFIXES = (".db", ".sqlite", ".sqlite3", ".db-wal", ".db-shm", ".sqlite-wal", ".sqlite-shm")
POSITIVE_DIR = Path("tests/fixtures/manual_entry_replay")
NEGATIVE_DIR = Path("tests/fixtures/manual_entry_replay_negative")


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
        if re.match(r"^[A-Z0-9_]*(FORBIDDEN|BAD|ALLOWED)[A-Z0-9_]*\s*=\s*[\(\{]", line):
            skipping = True
            continue
        if skipping:
            if line.strip() in {")", "}", "}"}:
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
        kept.append(line)
    return "\n".join(kept)


def validate_changed_paths(gate: Gate, root: Path) -> set[str]:
    changes = changed_paths(root)
    unexpected = sorted(path for path in changes if path not in ALLOWED_CHANGED_PATHS)
    gate.require(not unexpected, f"changed paths are exact allowlisted: {unexpected}")
    for path in sorted(changes):
        gate.require(path in ALLOWED_CHANGED_PATHS, f"changed path exact allowlisted: {path}")
        gate.require(not path.startswith(FORBIDDEN_CHANGED_PREFIXES), f"forbidden changed path avoided: {path}")
        gate.require(not path.endswith(".json"), f"fixture JSON unchanged: {path}")
        gate.require(not path.endswith(".sql"), f"SQL file unchanged: {path}")
    for wildcard in ["docs/", "tests/", "apps/", "libs/", "migrations/"]:
        gate.require(wildcard not in ALLOWED_CHANGED_PATHS, f"allowlist has no {wildcard} wildcard")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "apps") == set(), "apps diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "libs") == set(), "libs diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "migrations") == set(), "migrations diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "libs/AccountingEngine") == set(), "AccountingEngine diff empty")
    return changes


def validate_docs(gate: Gate, root: Path) -> None:
    doc_288 = read(root / TASK_246_DOC)
    doc_289 = read(root / TASK_246_PLAN)
    gate.require((root / TASK_246_DOC).exists(), "docs/288 exists")
    gate.require((root / TASK_246_PLAN).exists(), "docs/289 exists")
    for section in REQUIRED_DOC_288_SECTIONS:
        gate.contains(doc_288, section, "docs/288")
    for section in REQUIRED_DOC_289_SECTIONS:
        gate.contains(doc_289, section, "docs/289")
    for line in FORMAL_CONCLUSION_LINES:
        gate.contains(doc_288, line, "docs/288 Formal Conclusion")
        gate.contains(doc_289, line, "docs/289 Formal Conclusion")
    for path in PREVIOUS_DOCS:
        gate.require((root / path).exists(), f"{path.as_posix()} exists")
    for index_path in [Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        text = read(root / index_path)
        gate.contains(text, "TASK-246", index_path.as_posix())
        gate.contains(text, TASK_246_DOC.name, index_path.as_posix())
        gate.contains(text, TASK_246_PLAN.name, index_path.as_posix())


def validate_registration(gate: Gate, root: Path) -> None:
    tests_cmake = read(root / "tests/CMakeLists.txt")
    gate.contains(tests_cmake, "add_subdirectory(ShellAccountingManualEntryReplayAccountingEngineBridge)", "tests/CMakeLists")
    gate.contains(tests_cmake, "add_subdirectory(ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate)", "tests/CMakeLists")
    gate.require((root / BRIDGE_CMAKE).exists(), "bridge CMake exists")
    gate.require((root / BRIDGE_SCRIPT).exists(), "bridge script exists")
    gate.require((root / GATE_CMAKE).exists(), "implementation gate CMake exists")
    gate.require((root / GATE_SCRIPT).exists(), "implementation gate script exists")
    gate.contains(read(root / GATE_CMAKE), TASK_246_CTEST, "TASK-246 gate CMake")
    gate.contains(read(root / GATE_CMAKE), GATE_SCRIPT.name, "TASK-246 gate CMake")
    names = registered_ctests(root)
    gate.require(bool(names), "ctest -N returns registered tests")
    for name in CRITICAL_CTEST_NAMES:
        gate.require(name in names, f"{name} registered")


def validate_bridge_static(gate: Gate, root: Path) -> None:
    bridge_text = read(root / BRIDGE_SCRIPT)
    gate.contains(bridge_text, "--dry-run-summary-json", "bridge script")
    gate.contains(bridge_text, "--summary-json", "bridge script")
    gate.contains(bridge_text, "--expect-no-accountingengine-call", "bridge script")
    gate.contains(bridge_text, "test-only-accountingengine-bridge-contract", "bridge script")
    gate.contains(bridge_text, "accountingEngineCalled", "bridge script")
    gate.contains(bridge_text, "replayExecuted", "bridge script")
    gate.contains(bridge_text, "runtimeWrite", "bridge script")
    scan_text = executable_scan_text(bridge_text)
    for token in FORBIDDEN_EXECUTABLE_TOKENS:
        gate.require(token not in scan_text, f"bridge script omits executable token `{token}`")
    gate.require("libs/AccountingEngine" not in scan_text, "bridge script does not import AccountingEngine path")
    gate.require("AccountingReplayEngine" not in scan_text, "bridge script does not reference replay engine")
    gate.require("replayReadOnly" not in scan_text, "bridge script does not call replayReadOnly")
    gate.require("ctest --test-dir build --output-on-failure" not in scan_text, "bridge script does not run full CTest")


def run_bridge(gate: Gate, root: Path, args: list[str]) -> tuple[int, str, str]:
    completed = run([sys.executable, str(root / BRIDGE_SCRIPT), "--source-root", str(root), *args], root)
    gate.require(completed.stderr.strip() == "", "bridge stderr is empty")
    return completed.returncode, completed.stdout, completed.stderr


def validate_summary_contract(gate: Gate, summary: dict[str, Any]) -> None:
    gate.require(summary.get("schemaVersion") == 1, "summary schemaVersion is 1")
    gate.require(summary.get("task") == "TASK-246", "summary task is TASK-246")
    gate.require(summary.get("mode") == "test-only-accountingengine-bridge-contract", "summary mode is exact")
    gate.require(summary.get("accountingEngineCalled") is False, "summary accountingEngineCalled false")
    gate.require(summary.get("replayExecuted") is False, "summary replayExecuted false")
    gate.require(summary.get("runtimeWrite") is False, "summary runtimeWrite false")
    request = summary.get("bridgeRequest")
    response = summary.get("bridgeResponse")
    gate.require(isinstance(request, dict), "bridgeRequest object")
    gate.require(isinstance(response, dict), "bridgeResponse object")
    gate.require(request.get("schemaVersion") == 1, "bridgeRequest schemaVersion 1")
    gate.require(request.get("source") == "manual-entry-replay-test-only", "bridgeRequest source exact")
    gate.require(request.get("syntheticDataOnly") is True, "bridgeRequest syntheticDataOnly true")
    gate.require(response.get("schemaVersion") == 1, "bridgeResponse schemaVersion 1")
    gate.require(response.get("status") == "not_executed", "bridgeResponse status not_executed")
    gate.require(response.get("reason") == "AccountingEngine replay is not authorized in TASK-246", "bridgeResponse reason exact")
    gate.require(isinstance(summary.get("issues"), list), "issues list")
    gate.require(summary.get("issues") == [], "issues empty for happy path")
    encoded = json.dumps(summary, sort_keys=True).lower()
    for token in FORBIDDEN_OUTPUT_TOKENS:
        gate.require(token not in encoded, f"summary excludes forbidden token `{token}`")


def write_sanitized_input_summaries(root: Path, dry_path: Path, impl_path: Path) -> None:
    index_path = root / "tests/fixtures/manual_entry_replay/fixtures_index.json"
    index = json.loads(index_path.read_text(encoding="utf-8"))
    fixture_ids = [
        entry["fixtureId"]
        for entry in index.get("fixtures", [])
        if isinstance(entry, dict) and isinstance(entry.get("fixtureId"), str)
    ]
    dry_summary = {
        "schemaVersion": "manual-entry-replay-test-only-dry-run-summary/v1",
        "dryRunStatus": "ok",
        "entries": [
            {
                "fixtureId": fixture_id,
                "accountingEngineCalled": False,
                "replayExecuted": False,
                "runtimeWrites": False,
            }
            for fixture_id in fixture_ids
        ],
    }
    implementation_summary = {
        "schemaVersion": "manual-entry-replay-test-only-implementation-summary/v1",
        "implementationMode": "test-only-in-memory",
        "implementationStatus": "ok",
        "entries": [
            {
                "fixtureId": fixture_id,
                "accountingEngineCalled": False,
                "runtimeWrites": False,
                "positionCashPnlCalculated": False,
            }
            for fixture_id in fixture_ids
        ],
    }
    dry_path.write_text(json.dumps(dry_summary, sort_keys=True), encoding="utf-8")
    impl_path.write_text(json.dumps(implementation_summary, sort_keys=True), encoding="utf-8")


def validate_bridge_cli(gate: Gate, root: Path) -> None:
    help_completed = run([sys.executable, str(root / BRIDGE_SCRIPT), "--help"], root)
    gate.require(help_completed.returncode == 0, "bridge --help exits zero")
    gate.contains(help_completed.stdout, "--dry-run-summary-json", "bridge --help")
    gate.contains(help_completed.stdout, "--summary-json", "bridge --help")
    gate.contains(help_completed.stdout, "--expect-no-accountingengine-call", "bridge --help")

    with tempfile.TemporaryDirectory() as tmp:
        tmpdir = Path(tmp)
        dry_path = tmpdir / "dry.json"
        impl_path = tmpdir / "impl.json"
        summary_path = tmpdir / "bridge.json"
        bad_path = tmpdir / "bad.json"
        failure_path = tmpdir / "failure.json"
        write_sanitized_input_summaries(root, dry_path, impl_path)

        code, stdout, _stderr = run_bridge(
            gate,
            root,
            [
                "--dry-run-summary-json",
                str(dry_path),
                "--implementation-summary-json",
                str(impl_path),
                "--expect-no-accountingengine-call",
                "--summary-json",
                "-",
            ],
        )
        gate.require(code == 0, "bridge stdout summary exits zero")
        summary = json.loads(stdout)
        validate_summary_contract(gate, summary)

        code, _stdout, _stderr = run_bridge(
            gate,
            root,
            [
                "--dry-run-summary-json",
                str(dry_path),
                "--implementation-summary-json",
                str(impl_path),
                "--expect-no-accountingengine-call",
                "--summary-json",
                str(summary_path),
            ],
        )
        gate.require(code == 0, "bridge file summary exits zero")
        gate.require(summary_path.exists(), "bridge summary file exists")
        file_summary = json.loads(summary_path.read_text(encoding="utf-8"))
        validate_summary_contract(gate, file_summary)

        bad_path.write_text(json.dumps({"schemaVersion": "bad"}), encoding="utf-8")
        code, _stdout, _stderr = run_bridge(
            gate,
            root,
            ["--dry-run-summary-json", str(bad_path), "--summary-json", str(failure_path)],
        )
        gate.require(code != 0, "bad dry-run summary fails closed")
        gate.require(failure_path.exists(), "failure summary file exists")
        failure = json.loads(failure_path.read_text(encoding="utf-8"))
        gate.require(failure.get("bridgeResponse", {}).get("status") == "failed", "failure status sanitized")
        gate.require(failure.get("accountingEngineCalled") is False, "failure engine flag false")
        gate.require(failure.get("replayExecuted") is False, "failure replay flag false")
        gate.require(failure.get("runtimeWrite") is False, "failure runtime write flag false")
        lowered = json.dumps(failure, sort_keys=True).lower()
        for token in FORBIDDEN_OUTPUT_TOKENS:
            gate.require(token not in lowered, f"failure excludes forbidden token `{token}`")


def validate_no_mutation(gate: Gate, root: Path, before_hashes: dict[str, str], before_db_like: set[str], before_changes: set[str]) -> None:
    after_hashes = fixture_hashes(root)
    after_db_like = db_like_untracked_paths(root)
    after_changes = changed_paths(root)
    gate.require(before_hashes == after_hashes, "fixture JSON hashes unchanged")
    gate.require(before_db_like == after_db_like, "no DB/WAL/SHM/SQLite files created")
    gate.require(before_changes == after_changes, "gate probes do not mutate changed-path set")


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
    validate_bridge_static(gate, root)
    validate_bridge_cli(gate, root)
    validate_no_mutation(gate, root, before_hashes, before_db_like, before_changes)
    gate.require(gate.checks >= 150, f"expected at least 150 checks, got {gate.checks}")
    print(json.dumps({"status": "passed", "checks": gate.checks}, sort_keys=True))
    return 0



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
if __name__ == "__main__":
    raise SystemExit(main())
