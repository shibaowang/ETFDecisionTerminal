#!/usr/bin/env python3

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path


TASK_242_DOC = Path("docs/280_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.md")
TASK_242_PLAN = Path("docs/281_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_test_plan.md")
TASK_242_DIR = Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate")
TASK_242_CMAKE = TASK_242_DIR / "CMakeLists.txt"
TASK_242_GATE = TASK_242_DIR / "manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py"
TASK_242_CTEST = "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening"

TASK_241_DOC = Path("docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md")
TASK_241_PLAN = Path("docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md")
TASK_241_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/"
    "manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py"
)
TASK_241_CTEST = "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix"

TASK_240_DOC = Path("docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md")
TASK_240_PLAN = Path("docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md")
TASK_240_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/"
    "manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py"
)
TASK_240_CTEST = "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation"

TASK_239_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/"
    "manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py"
)

TASK_234_IMPLEMENTATION = Path(
    "tests/ShellAccountingManualEntryReplayImplementation/"
    "manual_entry_replay_implementation.py"
)
TASK_231_HARNESS = Path(
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/"
    "manual_entry_replay_test_only_dry_run_harness.py"
)
TASK_224_VALIDATOR = Path(
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/"
    "manual_entry_replay_negative_fixture_static_validator.py"
)

POSITIVE_DIR = Path("tests/fixtures/manual_entry_replay")
NEGATIVE_DIR = Path("tests/fixtures/manual_entry_replay_negative")

ALLOWED_CHANGED_PATHS = {
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    TASK_242_DOC.as_posix(),
    TASK_242_PLAN.as_posix(),
    "tests/CMakeLists.txt",
    TASK_242_CMAKE.as_posix(),
    TASK_242_GATE.as_posix(),
    TASK_239_GATE.as_posix(),
    TASK_240_GATE.as_posix(),
    TASK_241_GATE.as_posix(),
    TASK_234_IMPLEMENTATION.as_posix(),
    TASK_231_HARNESS.as_posix(),
    TASK_224_VALIDATOR.as_posix(),
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
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
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/manual_entry_replay_negative_fixture_validator_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/manual_entry_replay_next_phase_authorization_planning_gate.py",
    "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
}

ALLOWED_CHANGED_PATHS.update(
    {
        "docs/282_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.md",
        "docs/283_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_test_plan.md",
        "docs/284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md",
        "docs/285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
    }
)

FORBIDDEN_CHANGED_PREFIXES = (
    "apps/",
    "libs/",
    "migrations/",
    POSITIVE_DIR.as_posix() + "/",
    NEGATIVE_DIR.as_posix() + "/",
)

FORBIDDEN_OUTPUT_TOKENS = (
    "Trace" + "back",
    "SELECT" + " ",
    "INSERT" + " ",
    "UPDATE" + " ",
    "DELETE" + " ",
    "password",
    "credential",
    "api" + "_key",
    "endpoint",
    "broker" + " payload",
    "real" + " order",
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
    "AccountingEngine()" + ".replay",
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

REQUIRED_COPY_PATHS = [
    Path("README.md"),
    Path("docs/README.md"),
    Path("docs/12_codex_prompt_template.md"),
    Path("docs/274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md"),
    Path("docs/275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md"),
    TASK_240_DOC,
    TASK_240_PLAN,
    TASK_241_DOC,
    TASK_241_PLAN,
    Path("tests/CMakeLists.txt"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate"),
    Path("tests/ShellAccountingManualEntryReplayImplementation"),
    Path("tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate"),
    Path("tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate"),
    Path("tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate"),
    Path("tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate"),
    Path("tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate"),
    Path("tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness"),
    Path("tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate"),
    Path("tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator"),
    POSITIVE_DIR,
    NEGATIVE_DIR,
]


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


@dataclass(frozen=True)
class FailureCase:
    name: str
    target_gate: Path
    mutate: object


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def run(args: list[str], cwd: Path, env: dict[str, str] | None = None) -> subprocess.CompletedProcess[str]:
    merged_env = os.environ.copy()
    if env:
        merged_env.update(env)
    return subprocess.run(args, cwd=cwd, check=False, capture_output=True, text=True, env=merged_env)


def git_lines(root: Path, *args: str) -> set[str]:
    completed = subprocess.run(["git", *args], cwd=root, check=True, capture_output=True, text=True)
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def status_paths(root: Path) -> set[str]:
    completed = subprocess.run(
        ["git", "status", "--porcelain=v1", "-uall"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
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


def token_in_executable_lines(text: str, token: str) -> bool:
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        if stripped.startswith(("FORBIDDEN_EXECUTABLE_TOKENS", "FORBIDDEN_OUTPUT_TOKENS", '"', "'")):
            continue
        if "not in output" in stripped or "not in text" in stripped:
            continue
        if token in line:
            return True
    return False


def validate_changed_paths(gate: Gate, root: Path) -> set[str]:
    changes = changed_paths(root)
    gate.require(all(not path.endswith("/") for path in ALLOWED_CHANGED_PATHS), "allowlist uses exact files only")
    gate.require(all("*" not in path for path in ALLOWED_CHANGED_PATHS), "allowlist uses no wildcards")
    for forbidden_dir in ["docs/", "tests/", "apps/", "libs/", "migrations/"]:
        gate.require(forbidden_dir not in ALLOWED_CHANGED_PATHS, f"allowlist does not include {forbidden_dir} directory")
    unexpected = sorted(changes - ALLOWED_CHANGED_PATHS)
    gate.require(not unexpected, "TASK-242 changed unauthorized paths: " + ", ".join(unexpected))
    for path in sorted(changes):
        gate.require(path in ALLOWED_CHANGED_PATHS, f"changed path exact allowlisted: {path}")
        gate.require(not path.startswith(FORBIDDEN_CHANGED_PREFIXES), f"forbidden changed prefix absent: {path}")
        gate.require(not path.endswith(".json"), f"fixture JSON unchanged: {path}")
        gate.require(not path.endswith(".sql"), f"SQL file unchanged: {path}")
    for path in ["apps", "libs", "migrations", "libs/AccountingEngine", POSITIVE_DIR.as_posix(), NEGATIVE_DIR.as_posix()]:
        gate.require(git_lines(root, "diff", "--name-only", "main", "--", path) == set(), f"{path} diff empty")
    return changes


def validate_docs(gate: Gate, root: Path) -> None:
    doc280 = read(root / TASK_242_DOC)
    doc281 = read(root / TASK_242_PLAN)
    gate.contains(doc280, "# TASK-242 ShellAccounting manual entry replay AccountingEngine adequacy review failure-mode hardening gate", "docs/280 title")
    gate.contains(doc281, "# TASK-242 AccountingEngine adequacy review failure-mode hardening test plan", "docs/281 title")
    for section in [
        "## Purpose",
        "## Relationship To TASK-240 And TASK-241",
        "## Failure-Mode Scope",
        "## Temporary Input Boundary",
        "## Failure Case Matrix",
        "## Sanitized Failure Output",
        "## No-Bridge Boundary",
        "## No-Replay Boundary",
        "## No-Write Boundary",
        "## AccountingEngine Code Boundary",
        "## Production Integration Boundary",
        "## Formal Conclusion",
        "## Next Phase Handoff",
    ]:
        gate.contains(doc280, section, "docs/280")
    for section in [
        "## Test Objective",
        "## Failure Matrix",
        "## Temporary Input Probes",
        "## Changed-Path Allowlist",
        "## Regression Gates",
        "## Acceptance Criteria",
        "## Formal Conclusion",
    ]:
        gate.contains(doc281, section, "docs/281")
    for token in [
        "TASK-242 hardens failure-mode behavior for review-only AccountingEngine adequacy gates.",
        "TASK-242 does not call AccountingEngine replay.",
        "TASK-242 does not implement AccountingEngine bridge.",
        "TASK-242 does not implement replay.",
        "TASK-242 does not modify AccountingEngine code.",
        "TASK-242 does not authorize runtime SQL / SQLite read/write.",
        "TASK-242 does not authorize audit / ledger / snapshot writes.",
        "TASK-242 does not authorize read model refresh.",
        "TASK-242 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
        "Recommended next task: TASK-243 ShellAccounting manual entry replay AccountingEngine adequacy review CI closeout gate.",
    ]:
        gate.contains(doc280, token, "docs/280 formal conclusion")
        gate.contains(doc281, token, "docs/281 formal conclusion")
    for index_path in [Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        text = read(root / index_path)
        gate.contains(text, "TASK-242", index_path.as_posix())
        gate.contains(text, TASK_242_DOC.name, index_path.as_posix())
        gate.contains(text, TASK_242_PLAN.name, index_path.as_posix())


def validate_registration(gate: Gate, root: Path) -> None:
    tests_cmake = read(root / "tests/CMakeLists.txt")
    gate.contains(
        tests_cmake,
        "add_subdirectory(ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate)",
        "tests/CMakeLists",
    )
    gate.require((root / TASK_242_CMAKE).exists(), "TASK-242 CMakeLists exists")
    gate.require((root / TASK_242_GATE).exists(), "TASK-242 gate script exists")
    cmake_text = read(root / TASK_242_CMAKE)
    gate.contains(cmake_text, TASK_242_CTEST, "TASK-242 CMakeLists")


def validate_support_scripts(gate: Gate, root: Path) -> None:
    for path in [TASK_240_GATE, TASK_241_GATE]:
        text = read(root / path)
        gate.contains(text, "ETFDT_SANITIZED_FAILURE_OUTPUT", path.as_posix())
        gate.contains(text, "sanitized failure", path.as_posix())
        gate.contains(text, "except Exception", path.as_posix())
    for path in [TASK_242_GATE, TASK_240_GATE, TASK_241_GATE, TASK_234_IMPLEMENTATION, TASK_231_HARNESS, TASK_224_VALIDATOR]:
        gate.require((root / path).exists(), f"{path.as_posix()} exists")
        gate.require((root / path).is_file(), f"{path.as_posix()} is file")


def validate_static_boundaries(gate: Gate, root: Path) -> None:
    gate_text = read(root / TASK_242_GATE)
    full_ctest_token = "ctest --test-dir build " + "--output-on-failure"
    full_ctest_subprocess = 'subprocess.run(["ctest", "--test-dir", ' + '"build"]'
    gate.require(full_ctest_token not in gate_text, "TASK-242 gate does not recursively run full CTest")
    gate.require(full_ctest_subprocess not in gate_text, "TASK-242 gate has no full CTest subprocess")
    for token in FORBIDDEN_EXECUTABLE_TOKENS:
        gate.require(not token_in_executable_lines(gate_text, token), f"TASK-242 gate does not execute token `{token}`")
    for path in [
        "apps",
        "libs",
        "migrations",
        "libs/AccountingEngine",
        POSITIVE_DIR.as_posix(),
        NEGATIVE_DIR.as_posix(),
    ]:
        gate.require(git_lines(root, "diff", "--name-only", "main", "--", path) == set(), f"{path} remains unchanged")


def copy_path(source_root: Path, temp_root: Path, relative: Path) -> None:
    source = source_root / relative
    destination = temp_root / relative
    if source.is_dir():
        shutil.copytree(source, destination, dirs_exist_ok=True)
    else:
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination)


def prepare_temp_root(source_root: Path) -> tempfile.TemporaryDirectory[str]:
    temp = tempfile.TemporaryDirectory()
    temp_root = Path(temp.name) / "source"
    temp_root.mkdir()
    for relative in REQUIRED_COPY_PATHS:
        copy_path(source_root, temp_root, relative)
    git_env = {
        "GIT_AUTHOR_NAME": "TASK-242 Gate",
        "GIT_AUTHOR_EMAIL": "task242@example.invalid",
        "GIT_COMMITTER_NAME": "TASK-242 Gate",
        "GIT_COMMITTER_EMAIL": "task242@example.invalid",
    }
    for command in [
        ["git", "init"],
        ["git", "checkout", "-b", "main"],
        ["git", "add", "."],
        ["git", "commit", "-m", "TASK-242 temp baseline"],
    ]:
        completed = run(command, temp_root, git_env)
        if completed.returncode != 0:
            raise RuntimeError("temporary git setup failed")
    return temp


def replace_token(path: Path, old: str, new: str) -> None:
    text = read(path)
    if old not in text:
        raise AssertionError(f"mutation token missing in {path.as_posix()}")
    write(path, text.replace(old, new, 1))


def replace_all(path: Path, old: str, new: str) -> None:
    text = read(path)
    if old not in text:
        raise AssertionError(f"mutation token missing in {path.as_posix()}")
    write(path, text.replace(old, new))


def remove_path(path: Path) -> None:
    path.unlink()


def create_temp_changed_file(path: Path, text: str = "TASK-242 temporary probe\n") -> None:
    write(path, text)


def append_forbidden_token(path: Path, token: str) -> None:
    text = read(path)
    write(path, text + "\n# TASK-242 temporary failure probe\nTASK_242_PROBE_TOKEN = " + repr(token) + "\n")


def assert_sanitized_failure_output(gate: Gate, case_name: str, output: str) -> None:
    gate.contains(output, "sanitized failure", f"{case_name} sanitized output")
    gate.contains(output, '"status": "failed"', f"{case_name} failure status")
    for token in FORBIDDEN_OUTPUT_TOKENS:
        gate.require(token.lower() not in output.lower(), f"{case_name} output excludes `{token}`")


def assert_no_runtime_artifacts(gate: Gate, temp_root: Path, case_name: str) -> None:
    db_suffixes = (".db", ".sqlite", ".sqlite3", ".db-wal", ".db-shm", ".sqlite-wal", ".sqlite-shm")
    generated = [path for path in temp_root.rglob("*") if path.is_file() and path.suffix.lower() in db_suffixes]
    gate.require(not generated, f"{case_name} creates no DB/WAL/SHM/SQLite files")


def run_failure_case(gate: Gate, source_root: Path, failure_case: FailureCase) -> None:
    with prepare_temp_root(source_root) as temp_name:
        temp_root = Path(temp_name) / "source"
        failure_case.mutate(temp_root)
        target = temp_root / failure_case.target_gate
        result = run(
            [sys.executable, str(target), "--source-root", str(temp_root)],
            temp_root,
            {"ETFDT_SANITIZED_FAILURE_OUTPUT": "1"},
        )
        output = result.stdout + "\n" + result.stderr
        gate.require(result.returncode != 0, f"{failure_case.name} exits nonzero")
        assert_sanitized_failure_output(gate, failure_case.name, output)
        assert_no_runtime_artifacts(gate, temp_root, failure_case.name)


def failure_cases() -> list[FailureCase]:
    return [
        FailureCase("missing docs/276", TASK_240_GATE, lambda root: remove_path(root / TASK_240_DOC)),
        FailureCase("missing docs/277", TASK_240_GATE, lambda root: remove_path(root / TASK_240_PLAN)),
        FailureCase("missing docs/278", TASK_241_GATE, lambda root: remove_path(root / TASK_241_DOC)),
        FailureCase("missing docs/279", TASK_241_GATE, lambda root: remove_path(root / TASK_241_PLAN)),
        FailureCase("docs/276 missing Formal Conclusion", TASK_240_GATE, lambda root: replace_token(root / TASK_240_DOC, "## Formal Conclusion", "## Formal Outcome")),
        FailureCase("docs/278 missing No-Bridge Boundary", TASK_241_GATE, lambda root: replace_token(root / TASK_241_DOC, "## No-Bridge Boundary", "## Bridge Boundary Review")),
        FailureCase("docs/278 missing No-Write Boundary", TASK_241_GATE, lambda root: replace_token(root / TASK_241_DOC, "## No-Write Boundary", "## Write Boundary Review")),
        FailureCase("docs/278 missing Go/No-Go Matrix", TASK_241_GATE, lambda root: replace_token(root / TASK_241_DOC, "## Go/No-Go Matrix", "## Review Matrix")),
        FailureCase("README missing TASK-240 reference", TASK_240_GATE, lambda root: replace_all(root / Path("README.md"), "TASK-240", "TASK_240_REMOVED")),
        FailureCase("docs/README missing TASK-241 reference", TASK_241_GATE, lambda root: replace_all(root / Path("docs/README.md"), "TASK-241", "TASK_241_REMOVED")),
        FailureCase(
            "tests/CMakeLists missing TASK-240 registration",
            TASK_240_GATE,
            lambda root: replace_token(
                root / Path("tests/CMakeLists.txt"),
                "add_subdirectory(ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate)",
                "# TASK-242 removed TASK-240 registration",
            ),
        ),
        FailureCase(
            "tests/CMakeLists missing TASK-241 registration",
            TASK_241_GATE,
            lambda root: replace_token(
                root / Path("tests/CMakeLists.txt"),
                "add_subdirectory(ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate)",
                "# TASK-242 removed TASK-241 registration",
            ),
        ),
        FailureCase("changed path includes libs/AccountingEngine", TASK_241_GATE, lambda root: create_temp_changed_file(root / "libs/AccountingEngine/task242_probe.txt")),
        FailureCase("changed path includes apps production path", TASK_241_GATE, lambda root: create_temp_changed_file(root / "apps/TASK242_probe.txt")),
        FailureCase("changed path includes fixture JSON", TASK_241_GATE, lambda root: create_temp_changed_file(root / POSITIVE_DIR / "TASK242_probe.json", "{}\n")),
        FailureCase("changed path includes migrations", TASK_241_GATE, lambda root: create_temp_changed_file(root / "migrations/TASK242_probe.sql", "-- probe\n")),
        FailureCase("gate source contains AccountingEngine replay call token", TASK_241_GATE, lambda root: append_forbidden_token(root / TASK_241_GATE, "AccountingEngine()" + ".replay")),
        FailureCase("gate source contains replayReadOnly token", TASK_241_GATE, lambda root: append_forbidden_token(root / TASK_241_GATE, "replay" + "ReadOnly(")),
        FailureCase("gate source contains SQLite write token", TASK_241_GATE, lambda root: append_forbidden_token(root / TASK_241_GATE, "INSERT " + "INTO")),
        FailureCase("gate source contains audit ledger snapshot write token", TASK_241_GATE, lambda root: append_forbidden_token(root / TASK_241_GATE, "write" + "Ledger(")),
        FailureCase("gate source contains broker/network/order placement token", TASK_241_GATE, lambda root: append_forbidden_token(root / TASK_241_GATE, "place" + "Order(")),
    ]


def validate_failure_cases(gate: Gate, root: Path) -> None:
    for case in failure_cases():
        run_failure_case(gate, root, case)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    before_changes = validate_changed_paths(gate, root)
    validate_docs(gate, root)
    validate_registration(gate, root)
    validate_support_scripts(gate, root)
    validate_static_boundaries(gate, root)
    validate_failure_cases(gate, root)
    after_changes = changed_paths(root)

    gate.require(before_changes == after_changes, "TASK-242 gate does not mutate changed paths")
    gate.require(gate.checks >= 80, f"expected at least 80 gate checks, got {gate.checks}")
    print(json.dumps({"status": "passed", "checks": gate.checks}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
