#!/usr/bin/env python3

import argparse
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path
from typing import Any


AUTH_DIR = Path("tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate")
AUTH_CMAKE = AUTH_DIR / "CMakeLists.txt"
AUTH_GATE = AUTH_DIR / "manual_entry_replay_implementation_authorization_gate.py"
AUTH_CTEST = "shell_accounting_manual_entry_replay_implementation_authorization"

TASK_232_CTEST = "shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix"
TASK_231_CTEST = "shell_accounting_manual_entry_replay_test_only_dry_run_harness"
TASK_230_CTEST = "shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization"
TASK_229_CTEST = "shell_accounting_manual_entry_replay_next_phase_authorization_planning"
TASK_228_CTEST = "shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout"
TASK_224_CTEST = "shell_accounting_manual_entry_replay_negative_fixture_static_validator"

FUTURE_REPLAY_DIR = Path("tests/ShellAccountingManualEntryReplayImplementation")
FUTURE_REPLAY_SCRIPT = FUTURE_REPLAY_DIR / "manual_entry_replay_implementation.py"
FUTURE_REPLAY_CTEST = "shell_accounting_manual_entry_replay_implementation"

TASK_231_HARNESS = Path(
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/"
    "manual_entry_replay_test_only_dry_run_harness.py"
)
TASK_224_VALIDATOR = Path(
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/"
    "manual_entry_replay_negative_fixture_static_validator.py"
)
TASK_233_DOC = Path("docs/262_shell_accounting_manual_entry_replay_implementation_authorization_gate.md")
TASK_233_PLAN = Path("docs/263_shell_accounting_manual_entry_replay_implementation_authorization_test_plan.md")
TASK_234_DOC = Path("docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md")
TASK_234_PLAN = Path("docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md")
TASK_234_CMAKE = FUTURE_REPLAY_DIR / "CMakeLists.txt"
POSITIVE_INDEX = Path("tests/fixtures/manual_entry_replay/fixtures_index.json")
NEGATIVE_INDEX = Path("tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json")
POSITIVE_DIR = Path("tests/fixtures/manual_entry_replay")
NEGATIVE_DIR = Path("tests/fixtures/manual_entry_replay_negative")

SUMMARY_SCHEMA = "manual-entry-replay-test-only-dry-run-summary/v1"
VALIDATOR_SUMMARY_SCHEMA = "manual-entry-replay-negative-fixture-static-validator-summary/v1"

ALLOWED_CHANGED_PATHS = {
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
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    TASK_233_DOC.as_posix(),
    TASK_233_PLAN.as_posix(),
    TASK_234_DOC.as_posix(),
    TASK_234_PLAN.as_posix(),
    "tests/CMakeLists.txt",
    AUTH_CMAKE.as_posix(),
    AUTH_GATE.as_posix(),
    TASK_234_CMAKE.as_posix(),
    FUTURE_REPLAY_SCRIPT.as_posix(),
    TASK_231_HARNESS.as_posix(),
    TASK_224_VALIDATOR.as_posix(),
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
}

FORBIDDEN_CHANGED_PREFIXES = (
    "apps/",
    "libs/",
    "migrations/",
    "tests/fixtures/manual_entry_replay/",
    "tests/fixtures/manual_entry_replay_negative/",
)

REQUIRED_DOC_262_SECTIONS = (
    "## Purpose",
    "## Relationship To TASK-230 Through TASK-232",
    "## Future Implementation Scope",
    "## Future Directory Boundary",
    "## Future CTest Name",
    "## Future Input Boundary",
    "## Future Output Boundary",
    "## AccountingEngine Boundary",
    "## No-Write Boundary",
    "## Runtime SQL And Ledger Boundary",
    "## Production Integration Boundary",
    "## Broker And Network Boundary",
    "## Explicit Non-Authorization List",
    "## Go / No-Go Handoff Rules",
    "## Formal Conclusion And Next Task",
)

REQUIRED_DOC_263_SECTIONS = (
    "## Document Purpose",
    "## Test Matrix",
    "## Required Probes",
    "## Go / No-Go Checklist",
)

FUTURE_INPUT_TOKENS = (
    "tests/fixtures/manual_entry_replay/fixtures_index.json",
    "tests/fixtures/manual_entry_replay/*.json",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness summary-json output",
)

FUTURE_OUTPUT_TOKENS = (
    "fixtureId",
    "replayPlanStatus",
    "sanitizedReplaySummary",
    "plannedEventCount",
    "accountingEngineMode",
    "runtimeWrites",
    "warnings",
)

ACCOUNTING_BOUNDARY_TOKENS = (
    "TASK-233 does not call AccountingEngine replay.",
    "TASK-233 does not authorize production AccountingEngine integration.",
    "Any future AccountingEngine use must be test-only, no-write, deterministic, and separately implemented in a later task.",
    "Future implementation must not write audit / ledger / snapshot.",
    "Future implementation must not refresh read models.",
)

NON_AUTH_TOKENS = (
    "TASK-233 does not authorize replay implementation in this task.",
    "TASK-233 does not authorize AccountingEngine replay calls in this task.",
    "TASK-233 does not authorize parser / loader / reader implementation in this task.",
    "TASK-233 does not authorize runtime SQL / SQLite read/write.",
    "TASK-233 does not authorize audit / ledger / snapshot writes.",
    "TASK-233 does not authorize production integration.",
    "TASK-233 does not authorize QML / DataServiceActions / repositories changes.",
    "TASK-233 does not authorize broker / network / credentials / endpoint.",
    "TASK-233 does not authorize real order placement or automatic trading.",
)

FORMAL_CONCLUSION_TOKENS = (
    "TASK-233 authorizes future manual entry replay implementation boundaries only.",
    "TASK-233 does not implement replay.",
    "TASK-233 does not call AccountingEngine replay.",
    "TASK-233 does not authorize production AccountingEngine integration.",
    "TASK-233 does not authorize runtime SQL / SQLite read/write.",
    "TASK-233 does not authorize audit / ledger / snapshot writes.",
    "TASK-233 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
    "Recommended next task: TASK-234 manual entry replay test-only implementation gate.",
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

FORBIDDEN_RUNTIME_TEXT_TOKENS = (
    ("import " + "sqlite3", "does not import sqlite"),
    ("INSERT " + "INTO", "does not contain insert statement"),
    ("UP" + "DATE ", "does not contain update statement"),
    ("DELETE " + "FROM", "does not contain delete statement"),
    ("RE" + "PLACE INTO", "does not contain replace statement"),
    ("write" + "_text(", "does not write text files"),
    ("write" + "_bytes(", "does not write binary files"),
    ("request" + "s", "does not import network client"),
    ("url" + "lib", "does not import url client"),
    ("place" + "Order", "does not place orders"),
    ("execute" + "Order", "does not execute orders"),
    ("broker" + "Order", "does not call broker order"),
)

RECURSIVE_FULL_CTEST_TOKENS = (
    "ctest --test-dir build " + "--output-on-failure",
    "ctest --test-dir build " + "-j",
    "subprocess.run([\"ctest\", \"--test-dir\", \"build\"]",
)

ACCOUNTING_ENGINE_REPLAY_TOKENS = (
    "AccountingEngine()" + ".replay",
    "AccountingReplayEngine" + "().replay",
    "replay(" + "ledger",
    "run" + "AccountingReplay",
)

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


def run_json_command(gate: Gate, root: Path, command: list[str], label: str) -> dict[str, Any]:
    completed = subprocess.run(command, cwd=root, check=False, capture_output=True, text=True)
    gate.require(completed.returncode == 0, f"{label} exits zero")
    gate.require(completed.stderr.strip() == "", f"{label} stderr is empty")
    stdout = completed.stdout.strip()
    gate.require(stdout.startswith("{") and stdout.endswith("}"), f"{label} stdout is one JSON object")
    return json.loads(stdout)


def validate_changed_paths(gate: Gate, root: Path) -> set[str]:
    changes = changed_paths(root)
    gate.require(all(not path.endswith("/") for path in ALLOWED_CHANGED_PATHS), "allowlist uses exact files only")
    gate.require(all("*" not in path for path in ALLOWED_CHANGED_PATHS), "allowlist uses no wildcards")
    gate.require("docs/" not in ALLOWED_CHANGED_PATHS, "allowlist does not include docs directory")
    gate.require("tests/" not in ALLOWED_CHANGED_PATHS, "allowlist does not include tests directory")
    gate.require("apps/" not in ALLOWED_CHANGED_PATHS, "allowlist does not include apps directory")
    gate.require("libs/" not in ALLOWED_CHANGED_PATHS, "allowlist does not include libs directory")
    unexpected = sorted(changes - ALLOWED_CHANGED_PATHS)
    gate.require(not unexpected, "TASK-233 changed unauthorized paths: " + ", ".join(unexpected))
    for path in sorted(changes):
        gate.require(path in ALLOWED_CHANGED_PATHS, f"changed path exact allowlisted: {path}")
        gate.require(not path.startswith(FORBIDDEN_CHANGED_PREFIXES), f"forbidden changed fixture/production path: {path}")
        gate.require(not path.endswith(".json"), f"fixture JSON must not be a changed path: {path}")
        gate.require(not path.endswith(".sql"), f"SQL file must not be a changed path: {path}")
        if path.startswith(FUTURE_REPLAY_DIR.as_posix() + "/"):
            gate.require(
                path in {TASK_234_CMAKE.as_posix(), FUTURE_REPLAY_SCRIPT.as_posix()},
                f"TASK-234 allows only exact test-only implementation path: {path}",
            )
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
    for path in [TASK_233_DOC, TASK_233_PLAN, Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        gate.require((root / path).exists(), f"required doc/index exists: {path.as_posix()}")

    doc262 = read(root / TASK_233_DOC)
    doc263 = read(root / TASK_233_PLAN)
    for section in REQUIRED_DOC_262_SECTIONS:
        gate.contains(doc262, section, "docs/262")
    for section in REQUIRED_DOC_263_SECTIONS:
        gate.contains(doc263, section, "docs/263")
    for token in FUTURE_INPUT_TOKENS:
        gate.contains(doc262, token, "docs/262 future input")
    for token in FUTURE_OUTPUT_TOKENS:
        gate.contains(doc262, token, "docs/262 future output")
    for token in ACCOUNTING_BOUNDARY_TOKENS:
        gate.contains(doc262, token, "docs/262 AccountingEngine boundary")
    for token in NON_AUTH_TOKENS:
        gate.contains(doc262, token, "docs/262 non-authorization")
    for token in FORMAL_CONCLUSION_TOKENS:
        gate.contains(doc262, token, "docs/262 formal conclusion")
    for token in [
        "TASK-233 validation",
        AUTH_CTEST,
        FUTURE_REPLAY_DIR.as_posix() + "/",
        FUTURE_REPLAY_CTEST,
        "The gate check count is at least 170.",
        "The gate does not recursively run full CTest.",
    ]:
        gate.contains(doc263, token, "docs/263")

    for index_path in [Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        text = read(root / index_path)
        gate.contains(text, "TASK-233", index_path.as_posix())
        gate.contains(text, TASK_233_DOC.name, index_path.as_posix())
        gate.contains(text, TASK_233_PLAN.name, index_path.as_posix())


def validate_registration(gate: Gate, root: Path) -> None:
    tests_cmake = read(root / "tests/CMakeLists.txt")
    gate.contains(
        tests_cmake,
        "add_subdirectory(ShellAccountingManualEntryReplayImplementationAuthorizationGate)",
        "tests/CMakeLists",
    )
    gate.require((root / AUTH_CMAKE).exists(), "TASK-233 CMakeLists exists")
    gate.require((root / AUTH_GATE).exists(), "TASK-233 gate script exists")
    cmake = read(root / AUTH_CMAKE)
    gate.contains(cmake, AUTH_CTEST, "TASK-233 CMakeLists")
    gate.contains(cmake, AUTH_GATE.name, "TASK-233 CMakeLists")

    names = ctest_names(root, gate)
    for name in [
        AUTH_CTEST,
        TASK_232_CTEST,
        TASK_231_CTEST,
        TASK_230_CTEST,
        TASK_229_CTEST,
        TASK_228_CTEST,
        TASK_224_CTEST,
        "shell_diagnostics_facade",
    ]:
        gate.require(name in names, f"CTest registered: {name}")
    gate.require(FUTURE_REPLAY_CTEST in names, f"TASK-234 implementation CTest registered: {FUTURE_REPLAY_CTEST}")


def validate_test_only_implementation_present(gate: Gate, root: Path) -> None:
    gate.require((root / FUTURE_REPLAY_DIR).exists(), "TASK-234 test-only replay implementation directory exists")
    gate.require((root / TASK_234_CMAKE).exists(), "TASK-234 test-only replay implementation CMakeLists exists")
    gate.require((root / FUTURE_REPLAY_SCRIPT).exists(), "TASK-234 test-only replay implementation script exists")
    implementation_scripts = {
        path.relative_to(root).as_posix()
        for path in (root / "tests").glob("**/manual_entry_replay_implementation.py")
    }
    gate.require(implementation_scripts == {FUTURE_REPLAY_SCRIPT.as_posix()}, "only exact TASK-234 replay implementation script exists under tests")
    gate.require(not any((root / "tests").glob("**/manual_entry_replay_implementation.cpp")), "no replay implementation C++ exists under tests")
    gate.require(not any((root / "tests").glob("**/manual_entry_replay_implementation_test.py")), "no replay implementation test script exists")
    gate.require(not any((root / "tests").glob("**/manual_entry_replay_implementation_test.cpp")), "no replay implementation test C++ exists")
    gate.require(
        "add_subdirectory(ShellAccountingManualEntryReplayImplementation)" in read(root / "tests/CMakeLists.txt"),
        "TASK-234 test-only replay implementation directory registered",
    )


def validate_summary(gate: Gate, summary: dict[str, Any], positive_ids: set[str], negative_ids: set[str]) -> None:
    gate.require(summary.get("schemaVersion") == SUMMARY_SCHEMA, "harness summary schema exact")
    gate.require(summary.get("dryRunStatus") == "ok", "harness summary dryRunStatus ok")
    gate.require(summary.get("positiveFixtureCount") == len(positive_ids), "harness positive count matches")
    gate.require(summary.get("negativeFixtureCount") == len(negative_ids), "harness negative count matches")
    entries = summary.get("entries")
    gate.require(isinstance(entries, list), "harness entries list")
    gate.require(len(entries) == len(positive_ids) + len(negative_ids), "harness entry count covers fixtures")
    encoded = json.dumps(summary, sort_keys=True)
    lowered = encoded.lower()
    for token in FORBIDDEN_OUTPUT_TOKENS:
        gate.require(token not in lowered, f"harness summary excludes `{token}`")
    seen_positive: set[str] = set()
    seen_negative: set[str] = set()
    allowed_keys = {
        "fixtureId",
        "source",
        "dryRunStatus",
        "plannedStepCount",
        "replayExecuted",
        "accountingEngineCalled",
        "runtimeWrites",
    }
    for entry in entries:
        gate.require(isinstance(entry, dict), "harness entry object")
        gate.require(set(entry) == allowed_keys, "harness entry sanitized key set")
        fixture_id = entry.get("fixtureId")
        source = entry.get("source")
        gate.require(isinstance(fixture_id, str) and fixture_id, "fixtureId non-empty")
        gate.require(source in {"positive_fixture", "negative_scaffold"}, f"source allowlisted: {fixture_id}")
        if source == "positive_fixture":
            gate.require(fixture_id in positive_ids, f"positive fixture covered: {fixture_id}")
            seen_positive.add(fixture_id)
        else:
            gate.require(fixture_id in negative_ids, f"negative fixture covered: {fixture_id}")
            seen_negative.add(fixture_id)
        gate.require(entry.get("dryRunStatus") == "metadata_checked", f"dryRunStatus metadata_checked: {fixture_id}")
        gate.require(entry.get("plannedStepCount") == 0, f"plannedStepCount zero before replay: {fixture_id}")
        gate.require(entry.get("replayExecuted") is False, f"replayExecuted false: {fixture_id}")
        gate.require(entry.get("accountingEngineCalled") is False, f"accountingEngineCalled false: {fixture_id}")
        gate.require(entry.get("runtimeWrites") is False, f"runtimeWrites false: {fixture_id}")
        gate.require("replayPlanStatus" not in entry, f"future replay output absent before implementation: {fixture_id}")
        gate.require("sanitizedReplaySummary" not in entry, f"future replay summary absent before implementation: {fixture_id}")
        gate.require("accountingEngineMode" not in entry, f"future AccountingEngine mode absent before implementation: {fixture_id}")
    gate.require(seen_positive == positive_ids, "all positive fixture ids covered by harness")
    gate.require(seen_negative == negative_ids, "all negative fixture ids covered by harness")


def validate_validator_summary(gate: Gate, summary: dict[str, Any]) -> None:
    gate.require(summary.get("schemaVersion") == VALIDATOR_SUMMARY_SCHEMA, "validator summary schema exact")
    gate.require(summary.get("issueCount") == 10, "validator issue count remains ten")
    issues = summary.get("issues")
    gate.require(isinstance(issues, list), "validator issues list")
    gate.require(len(issues) == 10, "validator emits ten sanitized issues")
    encoded = json.dumps(summary, sort_keys=True).lower()
    for token in FORBIDDEN_OUTPUT_TOKENS:
        gate.require(token not in encoded, f"validator summary excludes `{token}`")
    seen: set[str] = set()
    for issue in issues:
        gate.require(isinstance(issue, dict), "validator issue object")
        gate.require(set(issue) == {"negativeFixtureId", "file", "issueCode", "severity", "blocking", "rule", "message"}, "validator issue sanitized key set")
        fixture_id = issue.get("negativeFixtureId")
        gate.require(isinstance(fixture_id, str) and fixture_id.startswith("NEG_MRF"), "validator negative id sanitized")
        gate.require(fixture_id not in seen, f"validator no duplicate issue: {fixture_id}")
        seen.add(fixture_id)
        gate.require(issue.get("severity") == "error", f"validator severity error: {fixture_id}")
        gate.require(issue.get("blocking") is True, f"validator blocking true: {fixture_id}")
        gate.require(str(issue.get("file", "")).endswith(".json"), f"validator file sanitized JSON name: {fixture_id}")
        gate.require("sanitized expected issue" in str(issue.get("message", "")), f"validator sanitized message: {fixture_id}")


def validate_no_runtime_text(gate: Gate, root: Path) -> None:
    text_paths = [AUTH_GATE, TASK_231_HARNESS, TASK_224_VALIDATOR, FUTURE_REPLAY_SCRIPT]
    for relative in text_paths:
        text = read(root / relative)
        lowered = text.lower()
        for token in RECURSIVE_FULL_CTEST_TOKENS:
            gate.require(token not in text, f"{relative.as_posix()} does not recursively run full CTest")
        for token in ACCOUNTING_ENGINE_REPLAY_TOKENS:
            gate.require(token not in text, f"{relative.as_posix()} does not call AccountingEngine replay")
        for token, message in FORBIDDEN_RUNTIME_TEXT_TOKENS:
            haystack = lowered if token.islower() else text
            gate.require(token not in haystack, f"{relative.as_posix()} {message}")
    for relative in [TASK_233_DOC, TASK_233_PLAN]:
        text = read(root / relative)
        for token in RECURSIVE_FULL_CTEST_TOKENS:
            gate.require(token not in text, f"{relative.as_posix()} does not ask gate to recursively run full CTest")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    changes_before = validate_changed_paths(gate, root)
    validate_docs(gate, root)
    validate_registration(gate, root)
    validate_test_only_implementation_present(gate, root)
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
    harness_summary = run_json_command(
        gate,
        root,
        [sys.executable, str(root / TASK_231_HARNESS), "--source-root", str(root), "--summary-json"],
        "TASK-231 harness",
    )
    validator_summary = run_json_command(
        gate,
        root,
        [
            sys.executable,
            str(root / TASK_224_VALIDATOR),
            "--source-root",
            str(root),
            "--summary-json",
        ],
        "TASK-224 validator",
    )
    after_hashes = fixture_hashes(root)
    after_db_like = db_like_untracked_paths(root)
    changes_after = changed_paths(root)

    gate.require(before_hashes == after_hashes, "fixture JSON hashes unchanged after probes")
    gate.require(before_db_like == after_db_like, "no DB/WAL/SHM/SQLite files created by probes")
    gate.require(changes_before == changes_after, "probes do not change git changed-path set")
    validate_summary(gate, harness_summary, positive_ids, negative_ids)
    validate_validator_summary(gate, validator_summary)

    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "apps", "libs", "migrations") == set(), "apps/libs/migrations diff empty")
    gate.require(gate.checks >= 170, f"expected at least 170 gate checks, got {gate.checks}")
    print(json.dumps({"status": "passed", "checks": gate.checks}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
