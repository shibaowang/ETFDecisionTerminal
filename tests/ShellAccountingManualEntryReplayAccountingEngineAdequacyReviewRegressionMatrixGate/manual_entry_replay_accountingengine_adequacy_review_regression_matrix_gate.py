#!/usr/bin/env python3

import argparse
import hashlib
import json
import os
import re
import subprocess
from pathlib import Path


TASK_241_DOC = Path("docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md")
TASK_241_PLAN = Path("docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md")
TASK_241_DIR = Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate")
TASK_241_CMAKE = TASK_241_DIR / "CMakeLists.txt"
TASK_241_GATE = TASK_241_DIR / "manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py"
TASK_241_CTEST = "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix"

TASK_240_DOC = Path("docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md")
TASK_240_PLAN = Path("docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md")
TASK_240_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/"
    "manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py"
)
TASK_240_CTEST = "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation"

TASK_239_DOC = Path("docs/274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md")
TASK_239_PLAN = Path("docs/275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md")
TASK_239_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/"
    "manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py"
)
TASK_239_CTEST = "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization"

TASK_231_HARNESS = Path(
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/"
    "manual_entry_replay_test_only_dry_run_harness.py"
)
TASK_224_VALIDATOR = Path(
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/"
    "manual_entry_replay_negative_fixture_static_validator.py"
)
TASK_234_IMPLEMENTATION = Path(
    "tests/ShellAccountingManualEntryReplayImplementation/"
    "manual_entry_replay_implementation.py"
)

REVIEW_INPUTS = {
    "implementation summary": TASK_240_DOC,
    "dry-run summary": TASK_231_HARNESS,
    "fixture index": Path("tests/fixtures/manual_entry_replay/fixtures_index.json"),
    "negative fixture index": Path("tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json"),
}

CRITICAL_CTEST_NAMES = [
    TASK_241_CTEST,
    TASK_240_CTEST,
    TASK_239_CTEST,
    "shell_accounting_manual_entry_replay_implementation_phase_closeout",
    "shell_accounting_manual_entry_replay_implementation_ci_closeout",
    "shell_accounting_manual_entry_replay_implementation_failure_mode_hardening",
    "shell_accounting_manual_entry_replay_implementation_regression_matrix",
    "shell_accounting_manual_entry_replay_implementation",
    "shell_accounting_manual_entry_replay_implementation_authorization",
    "shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix",
    "shell_accounting_manual_entry_replay_test_only_dry_run_harness",
    "shell_diagnostics_facade",
    "transport_local_socket_echo",
]

ALLOWED_CHANGED_PATHS = {

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/CMakeLists.txt",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",

    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",

    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",

    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",

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

    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/manual_entry_replay_negative_fixture_validator_phase_closeout_gate.py",

    "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/manual_entry_replay_next_phase_authorization_planning_gate.py",

    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    TASK_241_DOC.as_posix(),
    TASK_241_PLAN.as_posix(),
    "tests/CMakeLists.txt",
    TASK_241_CMAKE.as_posix(),
    TASK_241_GATE.as_posix(),
    TASK_240_GATE.as_posix(),
    TASK_234_IMPLEMENTATION.as_posix(),
    TASK_231_HARNESS.as_posix(),
    TASK_224_VALIDATOR.as_posix(),


    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",

    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",

    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",

    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",

    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",

    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",

    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",    "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
    "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
}

ALLOWED_CHANGED_PATHS.update(
    {
        "docs/280_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.md",
        "docs/281_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
        "docs/282_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.md",
        "docs/283_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    }
)

SANITIZED_FAILURE_ENV = "ETFDT_SANITIZED_FAILURE_OUTPUT"

FORBIDDEN_CHANGED_PREFIXES = (
    "apps/",
    "libs/",
    "migrations/",
    "tests/fixtures/manual_entry_replay/",
    "tests/fixtures/manual_entry_replay_negative/",
)

FUTURE_RUNTIME_PATHS = (
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReview"),
    Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridge"),
    Path("libs/AccountingEngineManualEntryReplayBridge"),
    Path("libs/ManualEntryReplayAccountingEngineBridge"),
    Path("libs/ShellAccountingReplayAccountingEngineBridge"),
    Path("apps/ETFDecisionShell/qml/pages/ShellAccountingReplayPage.qml"),
)

BAD_CLEAN_MAIN_ASSERTIONS = (
    "changed path detection " + "must include current TASK",
    "must include current TASK-",
    "implementation extension " + "changed",
    "test-only parameter extension " + "changed",
)

RECURSIVE_FULL_CTEST_TOKENS = (
    "ctest --test-dir build " + "--output-on-failure",
    'subprocess.run(["ctest", "--test-dir", "build"]',
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

POSITIVE_DIR = Path("tests/fixtures/manual_entry_replay")
NEGATIVE_DIR = Path("tests/fixtures/manual_entry_replay_negative")
DB_SUFFIXES = (".db", ".sqlite", ".sqlite3", ".db-wal", ".db-shm", ".sqlite-wal", ".sqlite-shm")


class Gate:
    def __init__(self) -> None:
        self.checks = 0
        self.results: list[dict[str, object]] = []
        self.sanitized_output = os.environ.get(SANITIZED_FAILURE_ENV) == "1"

    def require(self, condition: bool, message: str) -> None:
        self.checks += 1
        passed = bool(condition)
        self.results.append({"check": self.checks, "passed": passed, "message": message})
        visible_message = "check passed" if self.sanitized_output and passed else message
        if self.sanitized_output and not passed:
            visible_message = "sanitized failure"
        print(f"CHECK {self.checks:03d} {'PASS' if passed else 'FAIL'} {visible_message}")
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


def fixture_paths(root: Path) -> list[Path]:
    return sorted((root / POSITIVE_DIR).glob("*.json")) + sorted((root / NEGATIVE_DIR).glob("*.json"))


def fixture_hashes(root: Path) -> dict[str, str]:
    return {path.relative_to(root).as_posix(): sha256(path) for path in fixture_paths(root)}


def db_like_untracked_paths(root: Path) -> set[str]:
    return {path for path in status_paths(root) if path.lower().endswith(DB_SUFFIXES)}


def has_recursive_full_ctest_invocation(text: str) -> bool:
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        if stripped.startswith(("RECURSIVE_FULL_CTEST_TOKENS", '"', "'")):
            continue
        if stripped.startswith(('if "ctest --test-dir build"', "if 'ctest --test-dir build'")):
            continue
        if stripped.startswith(('if \'subprocess.run(["ctest"', 'if "subprocess.run([\\"ctest"')):
            continue
        if "ctest --test-dir build" in line and "--output-on-failure" in line and "not in" not in line:
            return True
        if 'subprocess.run(["ctest", "--test-dir", "build"]' in line:
            return True
    return False


def has_clean_main_brittle_assertion(text: str, token: str) -> bool:
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        if stripped.startswith(("BAD_CLEAN_MAIN_ASSERTIONS", '"', "'")):
            continue
        if token in line:
            return True
    return False


def token_in_executable_lines(text: str, token: str) -> bool:
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        if stripped.startswith(("FORBIDDEN_EXECUTABLE_TOKENS", '"', "'")):
            continue
        if "not in text" in stripped or "not in haystack" in stripped:
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
    gate.require(not unexpected, "TASK-241 changed unauthorized paths: " + ", ".join(unexpected))
    for path in sorted(changes):
        gate.require(path in ALLOWED_CHANGED_PATHS, f"changed path exact allowlisted: {path}")
        gate.require(not path.startswith(FORBIDDEN_CHANGED_PREFIXES), f"forbidden changed prefix not used: {path}")
        gate.require(not path.endswith(".json"), f"fixture JSON unchanged: {path}")
        gate.require(not path.endswith(".sql"), f"SQL file unchanged: {path}")
    for path in ["apps", "libs", "migrations", "libs/AccountingEngine", POSITIVE_DIR.as_posix(), NEGATIVE_DIR.as_posix()]:
        gate.require(git_lines(root, "diff", "--name-only", "main", "--", path) == set(), f"{path} diff empty")
    return changes


def validate_docs(gate: Gate, root: Path) -> None:
    doc278 = read(root / TASK_241_DOC)
    doc279 = read(root / TASK_241_PLAN)
    for path in [TASK_241_DOC, TASK_241_PLAN, TASK_240_DOC, TASK_240_PLAN, TASK_239_DOC, TASK_239_PLAN, TASK_240_GATE, TASK_239_GATE]:
        gate.require((root / path).exists(), f"{path.as_posix()} exists")
    gate.contains(doc278, "# TASK-241 ShellAccounting manual entry replay AccountingEngine adequacy review regression matrix gate", "docs/278 title")
    gate.contains(doc279, "# TASK-241 AccountingEngine adequacy review regression matrix test plan", "docs/279 title")
    for section in [
        "## Purpose",
        "## Relationship To TASK-239 And TASK-240",
        "## Matrix Scope",
        "## Review Input Matrix",
        "## Static Adequacy Matrix",
        "## No-Bridge Boundary",
        "## No-Replay Boundary",
        "## No-Write Boundary",
        "## Runtime SQL And Ledger Boundary",
        "## AccountingEngine Code Boundary",
        "## Production Integration Boundary",
        "## Go/No-Go Matrix",
        "## Formal Conclusion",
        "## Next Phase Handoff",
    ]:
        gate.contains(doc278, section, "docs/278")
    for section in [
        "## Test Objective",
        "## Matrix Checks",
        "## Changed-Path Allowlist",
        "## Regression Gates",
        "## Failure Modes",
        "## Acceptance Criteria",
        "## Formal Conclusion",
    ]:
        gate.contains(doc279, section, "docs/279")
    for token in [
        "implementation summary",
        "dry-run summary",
        "fixture index",
        "negative fixture index",
        "No AccountingEngine call.",
        "No runtime integration.",
        "No fixture JSON mutation.",
        "No validator semantic change.",
        "TASK-241 does not call AccountingEngine replay.",
        "TASK-241 does not implement AccountingEngine bridge.",
        "TASK-241 does not implement replay.",
        "TASK-241 does not modify AccountingEngine code.",
        "TASK-241 does not authorize runtime SQL / SQLite read/write.",
        "TASK-241 does not authorize audit / ledger / snapshot writes.",
        "TASK-241 does not authorize read model refresh.",
        "TASK-241 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
        "Recommended next task: TASK-242 ShellAccounting manual entry replay AccountingEngine adequacy review failure-mode hardening gate.",
    ]:
        gate.contains(doc278, token, "docs/278")
        gate.contains(doc279, token, "docs/279")
    for index_path in [Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        text = read(root / index_path)
        gate.contains(text, "TASK-241", index_path.as_posix())
        gate.contains(text, TASK_241_DOC.name, index_path.as_posix())
        gate.contains(text, TASK_241_PLAN.name, index_path.as_posix())


def validate_registration(gate: Gate, root: Path) -> None:
    tests_cmake = read(root / "tests/CMakeLists.txt")
    gate.contains(
        tests_cmake,
        "add_subdirectory(ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate)",
        "tests/CMakeLists",
    )
    gate.require((root / TASK_241_CMAKE).exists(), "TASK-241 CMakeLists exists")
    gate.require((root / TASK_241_GATE).exists(), "TASK-241 gate script exists")
    cmake_text = read(root / TASK_241_CMAKE)
    gate.contains(cmake_text, TASK_241_CTEST, "TASK-241 CMakeLists")
    for name in CRITICAL_CTEST_NAMES:
        gate.contains(tests_cmake + cmake_text + read(root / TASK_240_GATE), name, f"CTest name retained: {name}")


def validate_review_inputs(gate: Gate, root: Path) -> None:
    for name, path in REVIEW_INPUTS.items():
        gate.require((root / path).exists(), f"review input exists: {name}")
        gate.require((root / path).is_file(), f"review input is file: {name}")
    doc278 = read(root / TASK_241_DOC)
    for name in REVIEW_INPUTS:
        gate.contains(doc278, name, f"review input matrix includes {name}")


def validate_future_runtime_absent(gate: Gate, root: Path) -> None:
    for path in FUTURE_RUNTIME_PATHS:
        gate.require(not (root / path).exists(), f"future runtime/bridge path absent: {path.as_posix()}")


def validate_clean_main_invariants(gate: Gate, root: Path) -> None:
    for path in [TASK_241_GATE, TASK_240_GATE, TASK_234_IMPLEMENTATION, TASK_231_HARNESS, TASK_224_VALIDATOR]:
        text = read(root / path)
        for token in BAD_CLEAN_MAIN_ASSERTIONS:
            gate.require(not has_clean_main_brittle_assertion(text, token), f"{path.as_posix()} has no brittle assertion `{token}`")
        gate.require(not has_recursive_full_ctest_invocation(text), f"{path.as_posix()} does not recursively run full CTest")


def validate_static_boundaries(gate: Gate, root: Path) -> None:
    gate_text = read(root / TASK_241_GATE)
    for token in FORBIDDEN_EXECUTABLE_TOKENS:
        gate.require(not token_in_executable_lines(gate_text, token), f"TASK-241 gate does not execute token `{token}`")
    for path in ["apps", "libs", "migrations", "libs/AccountingEngine", POSITIVE_DIR.as_posix(), NEGATIVE_DIR.as_posix()]:
        gate.require(git_lines(root, "diff", "--name-only", "main", "--", path) == set(), f"{path} remains unchanged")


def validate_fixture_immutability(gate: Gate, before: dict[str, str], after: dict[str, str]) -> None:
    gate.require(before == after, "fixture JSON hashes unchanged")
    gate.require(bool(before), "fixture hash set is non-empty")
    gate.require(any(path.startswith(POSITIVE_DIR.as_posix()) for path in before), "positive fixtures included")
    gate.require(any(path.startswith(NEGATIVE_DIR.as_posix()) for path in before), "negative fixtures included")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    before_hashes = fixture_hashes(root)
    before_db_like = db_like_untracked_paths(root)
    changes_before = validate_changed_paths(gate, root)
    validate_docs(gate, root)
    validate_registration(gate, root)
    validate_review_inputs(gate, root)
    validate_future_runtime_absent(gate, root)
    validate_clean_main_invariants(gate, root)
    validate_static_boundaries(gate, root)
    after_hashes = fixture_hashes(root)
    after_db_like = db_like_untracked_paths(root)
    changes_after = changed_paths(root)

    validate_fixture_immutability(gate, before_hashes, after_hashes)
    gate.require(before_db_like == after_db_like, "no DB/WAL/SHM/SQLite files created")
    gate.require(changes_before == changes_after, "regression matrix gate does not mutate changed-path set")
    gate.require(gate.checks >= 60, f"expected at least 60 gate checks, got {gate.checks}")
    print(json.dumps({"status": "passed", "checks": gate.checks}, sort_keys=True))
    return 0


def run_entrypoint() -> int:
    try:
        return main()
    except Exception as exc:
        if os.environ.get(SANITIZED_FAILURE_ENV) != "1":
            raise
        print(
            json.dumps(
                {
                    "status": "failed",
                    "error": "sanitized failure",
                    "reason": type(exc).__name__,
                },
                sort_keys=True,
            )
        )
        return 1


if __name__ == "__main__":
    raise SystemExit(run_entrypoint())
