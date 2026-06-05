#!/usr/bin/env python3

import argparse
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Any


SUMMARY_SCHEMA = "manual-entry-replay-test-only-dry-run-summary/v1"

TASK_230_DOC = Path("docs/256_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_gate.md")
TASK_230_PLAN = Path("docs/257_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_test_plan.md")
TASK_231_DOC = Path("docs/258_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_gate.md")
TASK_231_PLAN = Path("docs/259_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_test_plan.md")
TASK_232_DOC = Path("docs/260_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.md")
TASK_232_PLAN = Path("docs/261_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_test_plan.md")

TASK_231_HARNESS = Path(
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/"
    "manual_entry_replay_test_only_dry_run_harness.py"
)
TASK_231_HARNESS_CMAKE = Path("tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/CMakeLists.txt")
TASK_224_VALIDATOR = Path(
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/"
    "manual_entry_replay_negative_fixture_static_validator.py"
)
POSITIVE_INDEX = Path("tests/fixtures/manual_entry_replay/fixtures_index.json")
NEGATIVE_INDEX = Path("tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json")
POSITIVE_DIR = Path("tests/fixtures/manual_entry_replay")
NEGATIVE_DIR = Path("tests/fixtures/manual_entry_replay_negative")

REGRESSION_DIR = Path("tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate")
REGRESSION_CMAKE = REGRESSION_DIR / "CMakeLists.txt"
REGRESSION_GATE = REGRESSION_DIR / "manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py"
REGRESSION_CTEST = "shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix"
HARNESS_CTEST = "shell_accounting_manual_entry_replay_test_only_dry_run_harness"

ALLOWED_CHANGED_PATHS = {
    "docs/268_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_gate.md",
    "docs/269_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_test_plan.md",
    "docs/270_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_gate.md",
    "docs/271_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_test_plan.md",
    "docs/272_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_gate.md",
    "docs/273_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_test_plan.md",
    "docs/274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md",
    "docs/275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md",
    "docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md",
    "docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md",
    "docs/282_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.md",
    "docs/283_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "docs/266_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_gate.md",
    "docs/267_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    TASK_232_DOC.as_posix(),
    TASK_232_PLAN.as_posix(),
    "tests/CMakeLists.txt",
    REGRESSION_CMAKE.as_posix(),
    REGRESSION_GATE.as_posix(),
    TASK_231_HARNESS.as_posix(),
    TASK_224_VALIDATOR.as_posix(),
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
    "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
    "docs/262_shell_accounting_manual_entry_replay_implementation_authorization_gate.md",
    "docs/263_shell_accounting_manual_entry_replay_implementation_authorization_test_plan.md",
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
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


    "docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md",

    "docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/CMakeLists.txt",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "docs/280_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.md",
    "docs/281_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_test_plan.md",
    "docs/284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md",
    "docs/285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",}

FORBIDDEN_CHANGED_PREFIXES = (
    "apps/",
    "libs/",
    "migrations/",
    "tests/fixtures/manual_entry_replay/",
    "tests/fixtures/manual_entry_replay_negative/",
)

FORBIDDEN_OUTPUT_TOKENS = (
    "raw fixture payload",
    "raw payload",
    "full manual entry payload",
    "select *",
    "insert into",
    "update ",
    "delete from",
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

RAW_FIXTURE_FIELD_TOKENS = (
    '"metadata"',
    '"manualEntries"',
    '"transactions"',
    '"cashMovements"',
    '"privacyExpectations"',
    '"sanitizationExpectations"',
    '"brokerPayload"',
    '"rawSql"',
)

FORBIDDEN_RUNTIME_TEXT_TOKENS = (
    ("import " + "sqlite3", "does not import sqlite"),
    ("INSERT " + "INTO", "does not contain insert statement"),
    ("UP" + "DATE ", "does not contain update statement"),
    ("DELETE " + "FROM", "does not contain delete statement"),
    ("write" + "_text(", "does not write text files"),
    ("write" + "_bytes(", "does not write binary files"),
    ("op" + "en(", "does not open files for generic writes"),
    ("request" + "s", "does not import network client"),
    ("place" + "Order", "does not place orders"),
    ("execute" + "Order", "does not execute orders"),
    ("broker" + "Order", "does not call broker order"),
)

RECURSIVE_FULL_CTEST_TOKENS = (
    "ctest --test-dir build " + "--output-on-failure",
    "ctest --test-dir build " + "-j",
    "subprocess.run([\"ctest\", \"--test-dir\", \"build\"]",
)

ACCOUNTING_ENGINE_REPLAY_CALL = "AccountingEngine()" + ".replay"
DB_SUFFIXES = (".db", ".sqlite", ".sqlite3", ".db-wal", ".db-shm", ".sqlite-wal", ".sqlite-shm")


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
        self.require(token in text or normalized_token in normalized_text, f"{context} missing `{token}`")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> Any:
    return json.loads(read(path))


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
    paths = set()
    paths.update(git_lines(root, "diff", "--name-only", "main"))
    paths.update(git_lines(root, "diff", "--cached", "--name-only"))
    paths.update(status_paths(root))
    return {path for path in paths if path}


def fixture_paths(root: Path) -> list[Path]:
    return sorted((root / POSITIVE_DIR).glob("*.json")) + sorted((root / NEGATIVE_DIR).glob("*.json"))


def fixture_hashes(root: Path) -> dict[str, str]:
    return {path.relative_to(root).as_posix(): sha256(path) for path in fixture_paths(root)}


def db_like_untracked_paths(root: Path) -> set[str]:
    return {path for path in status_paths(root) if path.lower().endswith(DB_SUFFIXES)}


def ctest_names(root: Path, gate: Gate) -> set[str]:
    build = root / "build"
    gate.require(build.exists(), "build directory exists for CTest registration probe")
    completed = subprocess.run(
        ["ctest", "--test-dir", str(build), "-N"],
        cwd=root,
        check=False,
        capture_output=True,
        text=True,
    )
    gate.require(completed.returncode == 0, "ctest -N exits zero")
    names: set[str] = set()
    for line in completed.stdout.splitlines():
        match = re.search(r"Test\s+#\d+:\s+(\S+)", line)
        if match:
            names.add(match.group(1))
    gate.require(bool(names), "ctest -N returns registered tests")
    return names


def run_harness(gate: Gate, root: Path) -> tuple[str, dict[str, Any]]:
    completed = subprocess.run(
        [
            sys.executable,
            str(root / TASK_231_HARNESS),
            "--source-root",
            str(root),
            "--summary-json",
        ],
        cwd=root,
        check=False,
        capture_output=True,
        text=True,
    )
    gate.require(completed.returncode == 0, "TASK-231 harness exits zero")
    gate.require(completed.stderr.strip() == "", "TASK-231 harness stderr is empty")
    stdout = completed.stdout.strip()
    gate.require(stdout.startswith("{") and stdout.endswith("}"), "TASK-231 harness stdout is one JSON object")
    return stdout, json.loads(stdout)


def validate_changed_paths(gate: Gate, root: Path) -> set[str]:
    changes = changed_paths(root)
    gate.require(all(not path.endswith("/") for path in ALLOWED_CHANGED_PATHS), "allowlist uses exact files only")
    gate.require(all("*" not in path for path in ALLOWED_CHANGED_PATHS), "allowlist uses no wildcards")
    unexpected = sorted(changes - ALLOWED_CHANGED_PATHS)
    gate.require(not unexpected, "TASK-232 changed unauthorized paths: " + ", ".join(unexpected))
    for path in sorted(changes):
        gate.require(path in ALLOWED_CHANGED_PATHS, f"changed path exact allowlisted: {path}")
        gate.require(not path.startswith(FORBIDDEN_CHANGED_PREFIXES), f"forbidden changed fixture/production path: {path}")
        gate.require(not path.endswith(".json"), f"fixture JSON must not be a changed path: {path}")
        gate.require(not path.endswith(".sql"), f"SQL file must not be a changed path: {path}")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "apps") == set(), "apps diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "libs") == set(), "libs diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "migrations") == set(), "migrations diff empty")
    gate.require(
        git_lines(root, "diff", "--name-only", "main", "--", "tests/fixtures/manual_entry_replay") == set(),
        "positive fixture JSON diff empty",
    )
    gate.require(
        git_lines(root, "diff", "--name-only", "main", "--", "tests/fixtures/manual_entry_replay_negative") == set(),
        "negative fixture JSON diff empty",
    )
    return changes


def validate_docs(gate: Gate, root: Path) -> None:
    for path in [TASK_232_DOC, TASK_232_PLAN, Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        gate.require((root / path).exists(), f"required doc/index exists: {path.as_posix()}")

    doc260 = read(root / TASK_232_DOC)
    doc261 = read(root / TASK_232_PLAN)
    for section in [
        "## Purpose",
        "## Relationship To TASK-231",
        "## Regression Matrix Scope",
        "## Harness Summary Contract",
        "## Sanitized Output Boundary",
        "## Fixture Immutability Boundary",
        "## No-Write Boundary",
        "## No-Replay Boundary",
        "## No-Production Boundary",
        "## Gate Implementation Boundary",
        "## Formal Conclusion And Next Task",
    ]:
        gate.contains(doc260, section, "docs/260")
    for token in [
        "TASK-232 adds a regression matrix gate",
        TASK_231_HARNESS.as_posix(),
        "--source-root <root> --summary-json",
        SUMMARY_SCHEMA,
        "plannedStepCount",
        "replayExecuted = false",
        "accountingEngineCalled = false",
        "runtimeWrites = false",
        REGRESSION_CTEST,
        "TASK-232 does not implement replay",
        "Recommended next task: TASK-233",
    ]:
        gate.contains(doc260, token, "docs/260")

    for section in [
        "## Document Purpose",
        "## Test Matrix",
        "## Required Probes",
        "## Go / No-Go Checklist",
    ]:
        gate.contains(doc261, section, "docs/261")
    for token in [
        "TASK-232 regression matrix gate passes",
        "TASK-231 dry-run harness CTest passes",
        "Summary JSON is sanitized",
        "Replay, AccountingEngine, and runtime write flags remain false",
        "Fixture JSON remains unchanged",
        "Full CTest is run externally and passes",
    ]:
        gate.contains(doc261, token, "docs/261")

    for index_path in [Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        text = read(root / index_path)
        gate.contains(text, "TASK-232", index_path.as_posix())
        gate.contains(text, TASK_232_DOC.name, index_path.as_posix())
        gate.contains(text, TASK_232_PLAN.name, index_path.as_posix())


def validate_registration(gate: Gate, root: Path) -> None:
    tests_cmake = read(root / "tests/CMakeLists.txt")
    gate.contains(
        tests_cmake,
        "add_subdirectory(ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate)",
        "tests/CMakeLists",
    )
    gate.require((root / REGRESSION_CMAKE).exists(), "TASK-232 CMakeLists exists")
    gate.require((root / REGRESSION_GATE).exists(), "TASK-232 gate script exists")
    cmake = read(root / REGRESSION_CMAKE)
    gate.contains(cmake, REGRESSION_CTEST, "TASK-232 CMakeLists")
    gate.contains(cmake, REGRESSION_GATE.name, "TASK-232 CMakeLists")

    names = ctest_names(root, gate)
    for name in [
        REGRESSION_CTEST,
        HARNESS_CTEST,
        "shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization",
        "shell_accounting_manual_entry_replay_next_phase_authorization_planning",
        "shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout",
        "shell_accounting_manual_entry_replay_negative_fixture_static_validator",
        "shell_diagnostics_facade",
    ]:
        gate.require(name in names, f"CTest registered: {name}")


def validate_summary_shape(gate: Gate, stdout: str, summary: dict[str, Any], positive_ids: set[str], negative_ids: set[str]) -> None:
    gate.require(isinstance(summary, dict), "summary is a JSON object")
    gate.require(summary.get("schemaVersion") == SUMMARY_SCHEMA, "summary schemaVersion is exact")
    gate.require(summary.get("dryRunStatus") == "ok", "summary dryRunStatus ok")
    gate.require(summary.get("positiveFixtureCount") == len(positive_ids), "summary positiveFixtureCount matches index")
    gate.require(summary.get("negativeFixtureCount") == len(negative_ids), "summary negativeFixtureCount matches index")
    gate.require(summary.get("negativeFixtureCount") == 10, "summary negativeFixtureCount remains ten")
    entries = summary.get("entries")
    gate.require(isinstance(entries, list), "summary entries list")
    gate.require(len(entries) == len(positive_ids) + len(negative_ids), "summary entry count covers positive and negative fixtures")
    gate.require('"checks"' not in stdout, "summary-json output does not expose harness check count")
    gate.require('"status"' not in stdout, "summary-json output does not use default status payload")
    lowered = stdout.lower()
    for token in FORBIDDEN_OUTPUT_TOKENS:
        gate.require(token not in lowered, f"summary output excludes `{token}`")
    for token in RAW_FIXTURE_FIELD_TOKENS:
        gate.require(token not in stdout, f"summary output excludes raw fixture field {token}")


def validate_entry_matrix(gate: Gate, summary: dict[str, Any], positive_ids: set[str], negative_ids: set[str]) -> None:
    entries = summary["entries"]
    allowed_keys = {
        "fixtureId",
        "source",
        "dryRunStatus",
        "plannedStepCount",
        "replayExecuted",
        "accountingEngineCalled",
        "runtimeWrites",
    }
    seen_positive: set[str] = set()
    seen_negative: set[str] = set()
    for entry in entries:
        gate.require(isinstance(entry, dict), "summary entry is object")
        gate.require(set(entry) == allowed_keys, "summary entry uses sanitized key set")
        fixture_id = entry.get("fixtureId")
        source = entry.get("source")
        gate.require(isinstance(fixture_id, str) and fixture_id, "entry fixtureId is non-empty string")
        gate.require(source in {"positive_fixture", "negative_scaffold"}, f"entry source is allowlisted: {fixture_id}")
        if source == "positive_fixture":
            gate.require(fixture_id in positive_ids, f"positive fixture id covered: {fixture_id}")
            seen_positive.add(fixture_id)
        else:
            gate.require(fixture_id in negative_ids, f"negative fixture id covered: {fixture_id}")
            seen_negative.add(fixture_id)
        gate.require(entry.get("dryRunStatus") == "metadata_checked", f"entry dryRunStatus metadata_checked: {fixture_id}")
        gate.require(isinstance(entry.get("plannedStepCount"), int), f"plannedStepCount integer: {fixture_id}")
        gate.require(entry.get("plannedStepCount") >= 0, f"plannedStepCount non-negative: {fixture_id}")
        gate.require(entry.get("plannedStepCount") == 0, f"plannedStepCount remains zero before replay implementation: {fixture_id}")
        gate.require(entry.get("replayExecuted") is False, f"replayExecuted false: {fixture_id}")
        gate.require(entry.get("accountingEngineCalled") is False, f"accountingEngineCalled false: {fixture_id}")
        gate.require(entry.get("runtimeWrites") is False, f"runtimeWrites false: {fixture_id}")
        gate.require("position" not in entry, f"entry has no real position output: {fixture_id}")
        gate.require("cash" not in entry, f"entry has no real cash output: {fixture_id}")
        gate.require("pnl" not in {key.lower() for key in entry}, f"entry has no real PnL output: {fixture_id}")
    gate.require(seen_positive == positive_ids, "all positive fixture ids covered")
    gate.require(seen_negative == negative_ids, "all negative fixture ids covered")
    gate.require(len(seen_positive) == len(positive_ids), "no duplicate positive fixture coverage")
    gate.require(len(seen_negative) == len(negative_ids), "no duplicate negative fixture coverage")


def validate_no_runtime_text(gate: Gate, root: Path) -> None:
    for relative in [REGRESSION_GATE, TASK_231_HARNESS, TASK_232_DOC, TASK_232_PLAN]:
        text = read(root / relative)
        lowered = text.lower()
        for token in RECURSIVE_FULL_CTEST_TOKENS:
            gate.require(token not in text, f"{relative.as_posix()} does not recursively run full CTest")
        gate.require(ACCOUNTING_ENGINE_REPLAY_CALL not in text, f"{relative.as_posix()} does not call AccountingEngine replay")
        for token, message in FORBIDDEN_RUNTIME_TEXT_TOKENS:
            haystack = lowered if token.islower() else text
            if relative == REGRESSION_GATE and token == "open(":
                continue
            gate.require(token not in haystack, f"{relative.as_posix()} {message}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    changes_before = validate_changed_paths(gate, root)
    validate_docs(gate, root)
    validate_registration(gate, root)
    validate_no_runtime_text(gate, root)

    positive_index = read_json(root / POSITIVE_INDEX)
    negative_index = read_json(root / NEGATIVE_INDEX)
    positive_rows = positive_index.get("fixtures")
    negative_rows = negative_index.get("fixtures")
    gate.require(isinstance(positive_rows, list), "positive fixture index rows list")
    gate.require(isinstance(negative_rows, list), "negative fixture index rows list")
    gate.require(len(negative_rows) == 10, "negative fixture index remains ten")
    positive_ids = {row.get("fixtureId") for row in positive_rows}
    negative_ids = {row.get("negativeFixtureId") for row in negative_rows}
    gate.require(all(isinstance(fixture_id, str) for fixture_id in positive_ids), "positive fixture ids are strings")
    gate.require(all(isinstance(fixture_id, str) for fixture_id in negative_ids), "negative fixture ids are strings")

    before_hashes = fixture_hashes(root)
    before_db_like = db_like_untracked_paths(root)
    stdout, summary = run_harness(gate, root)
    after_hashes = fixture_hashes(root)
    after_db_like = db_like_untracked_paths(root)
    changes_after = changed_paths(root)

    gate.require(before_hashes == after_hashes, "fixture JSON hashes unchanged after harness run")
    gate.require(before_db_like == after_db_like, "no DB/WAL/SHM/SQLite files created")
    gate.require(changes_before == changes_after, "harness run does not change git changed-path set")

    validate_summary_shape(gate, stdout, summary, positive_ids, negative_ids)
    validate_entry_matrix(gate, summary, positive_ids, negative_ids)

    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "apps", "libs", "migrations") == set(), "apps/libs/migrations diff empty")
    gate.require(gate.checks >= 180, f"expected at least 180 gate checks, got {gate.checks}")
    print(json.dumps({"status": "passed", "checks": gate.checks, "entries": len(summary["entries"])}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
