#!/usr/bin/env python3

import argparse
import json
import re
import subprocess
from pathlib import Path
from typing import Any


NEGATIVE_DIR = Path("tests/fixtures/manual_entry_replay_negative")
POSITIVE_DIR = Path("tests/fixtures/manual_entry_replay")
TASK_219_VALIDATOR = Path(
    "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/"
    "manual_entry_replay_fixture_static_validator.py"
)
TASK_224_VALIDATOR = Path(
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/"
    "manual_entry_replay_negative_fixture_static_validator.py"
)

TASK_DOCS = {
    "TASK-220": [
        Path("docs/236_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_gate.md"),
        Path("docs/237_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_test_plan.md"),
    ],
    "TASK-221": [
        Path("docs/238_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_gate.md"),
        Path("docs/239_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_test_plan.md"),
    ],
    "TASK-222": [
        Path("docs/240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md"),
        Path("docs/241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md"),
    ],
    "TASK-223": [
        Path("docs/242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md"),
        Path("docs/243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md"),
    ],
    "TASK-224": [
        Path("docs/244_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_gate.md"),
        Path("docs/245_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_test_plan.md"),
    ],
    "TASK-225": [
        Path("docs/246_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.md"),
        Path("docs/247_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_test_plan.md"),
    ],
    "TASK-226": [
        Path("docs/248_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.md"),
        Path("docs/249_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_test_plan.md"),
    ],
    "TASK-227": [
        Path("docs/250_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_gate.md"),
        Path("docs/251_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_test_plan.md"),
    ],
}

CRITICAL_CTEST_NAMES = [
    "shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout",
    "shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening",
    "shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix",
    "shell_accounting_manual_entry_replay_negative_fixture_static_validator",
    "shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization",
    "shell_accounting_manual_entry_replay_negative_fixture_scaffold_files",
    "shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization",
    "shell_accounting_manual_entry_replay_fixture_negative_fixtures_authorization",
    "shell_accounting_manual_entry_replay_fixture_static_validator",
    "shell_accounting_manual_entry_replay_fixture_static_validator_authorization",
    "shell_diagnostics_facade",
]

CRITICAL_GATE_PATHS = [
    Path("tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py"),
]

EXPECTED_NEGATIVE_FILES = {
    "NEG_MRF001": ("NEG_MRF001_missing_required_field.json", "NEG_FIXTURE_MISSING_REQUIRED_FIELD"),
    "NEG_MRF002": ("NEG_MRF002_wrong_schema_version.json", "NEG_FIXTURE_SCHEMA_VERSION_INVALID"),
    "NEG_MRF003": ("NEG_MRF003_runtime_use_true.json", "NEG_FIXTURE_RUNTIME_USE_TRUE"),
    "NEG_MRF004": ("NEG_MRF004_production_use_true.json", "NEG_FIXTURE_PRODUCTION_USE_TRUE"),
    "NEG_MRF005": ("NEG_MRF005_replay_executed_true.json", "NEG_FIXTURE_REPLAY_EXECUTED_TRUE"),
    "NEG_MRF006": ("NEG_MRF006_non_synthetic_privacy.json", "NEG_FIXTURE_SYNTHETIC_FLAG_FALSE"),
    "NEG_MRF007": ("NEG_MRF007_extra_json_file.json", "NEG_FIXTURE_UNAUTHORIZED_FILE"),
    "NEG_MRF008": ("NEG_MRF008_forbidden_token.sql.json", "NEG_FIXTURE_RAW_SQL_TOKEN"),
    "NEG_MRF009": ("NEG_MRF009_broker_payload_token.json", "NEG_FIXTURE_BROKER_PAYLOAD_TOKEN"),
    "NEG_MRF010": ("NEG_MRF010_real_order_id_token.json", "NEG_FIXTURE_REAL_ORDER_TOKEN"),
}

ACCOUNTING_ENGINE_REPLAY_CALL = "AccountingEngine()" + ".replay"
FORBIDDEN_RUNTIME_TEXT_TOKENS = (
    ("import " + "sqlite3", "does not import sqlite"),
    ("request" + "s", "does not import network client"),
    ("place" + "Order", "does not place orders"),
    ("execute" + "Order", "does not execute orders"),
    ("broker" + "Order", "does not call broker order"),
)

ALLOWED_CHANGED_PATHS = {


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
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/250_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_gate.md",
    "docs/251_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_test_plan.md",
    "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
    "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
    "docs/254_shell_accounting_manual_entry_replay_next_phase_authorization_planning_gate.md",
    "docs/255_shell_accounting_manual_entry_replay_next_phase_authorization_planning_test_plan.md",
    "docs/256_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_gate.md",
    "docs/257_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_test_plan.md",
    "tests/CMakeLists.txt",
    "docs/258_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_gate.md",
    "docs/259_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
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
    "docs/284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md",
    "docs/285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
}

FORBIDDEN_CHANGED_PREFIXES = (
    "apps/",
    "libs/",
    "migrations/",
    "tests/fixtures/manual_entry_replay/",
    "tests/fixtures/manual_entry_replay_negative/",
)

UNSAFE_CLEAN_MAIN_ASSERTIONS = (
    "changed path detection must include " + "current TASK",
    "test-only parameter extension " + "changed",
    "must include " + "current TASK-",
)

RECURSIVE_FULL_CTEST_TOKENS = (
    "ctest --test-dir build " + "--output-on-failure",
    "ctest --test-dir build " + "-j",
    "ctest --test-dir build" + "\r\n",
    "ctest --test-dir build" + "\n",
)


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


def validate_docs(gate: Gate, root: Path) -> None:
    doc250 = read(root / TASK_DOCS["TASK-227"][0])
    doc251 = read(root / TASK_DOCS["TASK-227"][1])
    for task, docs in TASK_DOCS.items():
        for doc in docs:
            gate.require((root / doc).exists(), f"{doc.as_posix()} exists")
            gate.contains(read(root / doc), task, doc.as_posix())

    for section in [
        "## Purpose",
        "## Scope",
        "## Required CTest Set",
        "## Clean-Main Invariants",
        "## No Recursive Full CTest Boundary",
        "## CI Closeout Commands",
        "## No-Production-Drift Boundary",
        "## Fixture Immutability Boundary",
        "## Replay And AccountingEngine Boundary",
        "## Broker And Network Boundary",
        "## Formal Conclusion And Next Task",
    ]:
        gate.contains(doc250, section, "docs/250")

    for token in [
        "TASK-227 is a CI closeout gate for TASK-220 through TASK-226.",
        "TASK-227 does not modify validator validation behavior.",
        "TASK-227 only allows exact-path changed-set self-consistency updates to the TASK-224 test-only validator.",
        "TASK-227 does not modify fixture JSON.",
        "TASK-227 does not implement parser / loader / reader / replay.",
        "TASK-227 does not call AccountingEngine replay.",
        "TASK-227 does not modify production code / QML / DataServiceActions / repositories / migrations.",
        "TASK-227 does not write runtime SQL / SQLite / audit / ledger / snapshot.",
        "TASK-227 does not connect broker / network / credentials / endpoint.",
        "TASK-227 does not place real orders or enable automatic trading.",
        "Recommended next task: TASK-228 manual entry replay negative fixture validator phase closeout and handoff gate.",
    ]:
        gate.contains(doc250, token, "docs/250")

    for section in [
        "## Document Purpose",
        "## Test Matrix",
        "## Required Probes",
        "## Go / No-Go Checklist",
    ]:
        gate.contains(doc251, section, "docs/251")

    for token in [
        "CI closeout gate passes",
        "TASK-220 through TASK-226 critical gates are registered",
        "Clean-main unsafe assertions are absent",
        "No recursive full CTest runs inside the gate",
        "Fixture JSON is unchanged",
        "Validator validation behavior is unchanged",
        "TASK-224 validator changed-set self-consistency is exact-path only",
        "Production code is unchanged",
        "Full CTest is run externally and passes",
    ]:
        gate.contains(doc251, token, "docs/251")

    for index_path in [Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        text = read(root / index_path)
        gate.contains(text, "TASK-227", index_path.as_posix())
        gate.contains(text, TASK_DOCS["TASK-227"][0].name, index_path.as_posix())
        gate.contains(text, TASK_DOCS["TASK-227"][1].name, index_path.as_posix())


def validate_registration(gate: Gate, root: Path) -> None:
    tests_cmake = read(root / "tests/CMakeLists.txt")
    gate.contains(
        tests_cmake,
        "add_subdirectory(ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate)",
        "tests/CMakeLists",
    )
    gate.require(
        (root / "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/CMakeLists.txt").exists(),
        "TASK-227 CMakeLists exists",
    )
    gate.require(
        (root / "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py").exists(),
        "TASK-227 gate script exists",
    )
    cmake = read(root / "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/CMakeLists.txt")
    gate.contains(cmake, "shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout", "TASK-227 CMakeLists")
    gate.contains(cmake, "manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py", "TASK-227 CMakeLists")

    names = ctest_names(root, gate)
    for name in CRITICAL_CTEST_NAMES:
        gate.require(name in names, f"CTest registered: {name}")


def validate_negative_fixture_assets(gate: Gate, root: Path) -> None:
    index_path = root / NEGATIVE_DIR / "negative_fixtures_index.json"
    gate.require(index_path.exists(), "negative fixture index exists")
    index = read_json(index_path)
    gate.require(index.get("schemaVersion") == "manual-entry-replay-negative-fixtures-index/v1", "negative fixture index schema")
    gate.require(index.get("generatedByTask") == "TASK-222", "negative fixture index generated by TASK-222")
    gate.require(index.get("runtimeUse") is False, "negative fixture index runtimeUse false")
    gate.require(index.get("productionUse") is False, "negative fixture index productionUse false")
    gate.require(index.get("replayExecuted") is False, "negative fixture index replayExecuted false")
    fixtures = index.get("fixtures")
    gate.require(isinstance(fixtures, list), "negative fixture index fixtures list")
    gate.require(len(fixtures) == 10, "negative fixture index has ten fixtures")
    by_id = {fixture.get("negativeFixtureId"): fixture for fixture in fixtures}
    gate.require(set(by_id) == set(EXPECTED_NEGATIVE_FILES), "negative fixture ids match expected set")

    for fixture_id, (file_name, issue_code) in EXPECTED_NEGATIVE_FILES.items():
        fixture = by_id[fixture_id]
        path = root / NEGATIVE_DIR / file_name
        gate.require(path.exists(), f"negative fixture exists: {file_name}")
        gate.require(fixture.get("file") == file_name, f"index file mapping: {fixture_id}")
        gate.require(fixture.get("expectedIssueCode") == issue_code, f"index issue mapping: {fixture_id}")
        gate.require(fixture.get("runtimeUse") is False, f"fixture index runtime false: {fixture_id}")
        gate.require(fixture.get("productionUse") is False, f"fixture index production false: {fixture_id}")
        gate.require(fixture.get("replayExecuted") is False, f"fixture index replay false: {fixture_id}")
        payload = read_json(path)
        gate.require(payload.get("negativeFixtureId") == fixture_id, f"fixture id in payload: {fixture_id}")
        gate.require(payload.get("expectedIssueCode") == issue_code, f"fixture issue in payload: {fixture_id}")

    json_names = {path.name for path in (root / NEGATIVE_DIR).glob("*.json")}
    expected_names = {"negative_fixtures_index.json"} | {value[0] for value in EXPECTED_NEGATIVE_FILES.values()}
    gate.require(json_names == expected_names, "negative fixture directory exact JSON set")
    gate.require((root / POSITIVE_DIR / "fixtures_index.json").exists(), "positive fixture index exists")
    gate.require(TASK_219_VALIDATOR.exists(), "TASK-219 positive validator exists")
    gate.require(TASK_224_VALIDATOR.exists(), "TASK-224 negative validator exists")
    gate.require((root / CRITICAL_GATE_PATHS[4]).exists(), "TASK-225 regression matrix gate exists")
    gate.require((root / CRITICAL_GATE_PATHS[5]).exists(), "TASK-226 failure-mode hardening gate exists")


def validate_changed_paths(gate: Gate, root: Path) -> set[str]:
    changes = changed_paths(root)
    ALLOWED_CHANGED_PATHS.update({
        "docs/286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md",
        "docs/287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    })
    for path in sorted(changes):
        gate.require(path in ALLOWED_CHANGED_PATHS, f"changed path exact allowlisted: {path}")
    for prefix in FORBIDDEN_CHANGED_PREFIXES:
        gate.require(not any(path.startswith(prefix) for path in changes), f"no changed path under {prefix}")
    gate.require(TASK_219_VALIDATOR.as_posix() not in changes, "TASK-219 positive validator unchanged")
    if TASK_224_VALIDATOR.as_posix() in changes:
        gate.require(
            TASK_224_VALIDATOR.as_posix() in ALLOWED_CHANGED_PATHS,
            "TASK-224 negative validator exact-path self-consistency allowlisted",
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


def validate_gate_safety(gate: Gate, root: Path) -> None:
    for path in CRITICAL_GATE_PATHS:
        gate.require((root / path).exists(), f"critical gate exists: {path.as_posix()}")
        text = read(root / path)
        for token in UNSAFE_CLEAN_MAIN_ASSERTIONS:
            gate.require(token not in text, f"{path.as_posix()} lacks clean-main unsafe assertion `{token}`")
        for token in RECURSIVE_FULL_CTEST_TOKENS:
            gate.require(token not in text, f"{path.as_posix()} does not recursively run full CTest")
        gate.require(ACCOUNTING_ENGINE_REPLAY_CALL not in text, f"{path.as_posix()} does not call AccountingEngine replay")
        gate.require("subprocess.run([\"ctest\", \"--test-dir\", \"build\"]" not in text, f"{path.as_posix()} no recursive ctest subprocess")

    closeout_text = read(root / CRITICAL_GATE_PATHS[-1])
    gate.contains(closeout_text, "ctest\", \"--test-dir\", str(build), \"-N", "TASK-227 gate")
    gate.require("ctest\", \"--test-dir\", str(build), \"--output-on-failure" not in closeout_text, "TASK-227 gate does not run full CTest")


def validate_no_runtime_boundary(gate: Gate, root: Path, changes: set[str]) -> None:
    gate.require(not any(path.startswith(("apps/", "libs/", "migrations/")) for path in changes), "no production changed paths")
    gate.require(not any(path.endswith(".json") for path in changes), "no fixture JSON changed paths")
    for path in [TASK_DOCS["TASK-227"][0], TASK_DOCS["TASK-227"][1], CRITICAL_GATE_PATHS[-1]]:
        text = read(root / path)
        lowered = text.lower()
        for token, message in FORBIDDEN_RUNTIME_TEXT_TOKENS:
            haystack = lowered if token.islower() else text
            gate.require(token not in haystack, f"{path.as_posix()} {message}")
        gate.require(ACCOUNTING_ENGINE_REPLAY_CALL not in text, f"{path.as_posix()} does not call AccountingEngine replay")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    changes = validate_changed_paths(gate, root)
    validate_docs(gate, root)
    validate_registration(gate, root)
    validate_negative_fixture_assets(gate, root)
    validate_gate_safety(gate, root)
    validate_no_runtime_boundary(gate, root, changes)
    gate.require(gate.checks >= 170, f"expected at least 170 gate checks, got {gate.checks}")
    print(json.dumps({"status": "passed", "checks": gate.checks}, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
