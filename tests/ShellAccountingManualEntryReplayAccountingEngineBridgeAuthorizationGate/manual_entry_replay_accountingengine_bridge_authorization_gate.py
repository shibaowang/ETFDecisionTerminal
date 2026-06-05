#!/usr/bin/env python3

import argparse
import re
import subprocess
from pathlib import Path


TASK_245_DOC = Path("docs/286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md")
TASK_245_PLAN = Path("docs/287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md")
TASK_245_DIR = Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate")
TASK_245_CMAKE = TASK_245_DIR / "CMakeLists.txt"
TASK_245_GATE = TASK_245_DIR / "manual_entry_replay_accountingengine_bridge_authorization_gate.py"
TASK_245_CTEST = "shell_accounting_manual_entry_replay_accountingengine_bridge_authorization"
FUTURE_BRIDGE_DIR = Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridge")

TASK_239_DOC = Path("docs/274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md")
TASK_239_PLAN = Path("docs/275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md")
TASK_239_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/"
    "manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py"
)

TASK_240_DOC = Path("docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md")
TASK_240_PLAN = Path("docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md")
TASK_240_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/"
    "manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py"
)

TASK_241_DOC = Path("docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md")
TASK_241_PLAN = Path("docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md")
TASK_241_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/"
    "manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py"
)

TASK_242_DOC = Path("docs/280_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.md")
TASK_242_PLAN = Path("docs/281_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_test_plan.md")
TASK_242_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/"
    "manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py"
)

TASK_243_DOC = Path("docs/282_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.md")
TASK_243_PLAN = Path("docs/283_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_test_plan.md")
TASK_243_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/"
    "manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py"
)

TASK_244_DOC = Path("docs/284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md")
TASK_244_PLAN = Path("docs/285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md")
TASK_244_GATE = Path(
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/"
    "manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py"
)

PREVIOUS_TASK_DOCS = [
    TASK_239_DOC,
    TASK_239_PLAN,
    TASK_240_DOC,
    TASK_240_PLAN,
    TASK_241_DOC,
    TASK_241_PLAN,
    TASK_242_DOC,
    TASK_242_PLAN,
    TASK_243_DOC,
    TASK_243_PLAN,
    TASK_244_DOC,
    TASK_244_PLAN,
]

CRITICAL_GATE_SCRIPTS = [
    TASK_239_GATE,
    TASK_240_GATE,
    TASK_241_GATE,
    TASK_242_GATE,
    TASK_243_GATE,
    TASK_244_GATE,
    Path("tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py"),
    Path("tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py"),
    Path("tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py"),
]

CRITICAL_CTEST_NAMES = [
    TASK_245_CTEST,
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
    TASK_245_DOC.as_posix(),
    TASK_245_PLAN.as_posix(),
    "tests/CMakeLists.txt",
    TASK_245_CMAKE.as_posix(),
    TASK_245_GATE.as_posix(),
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
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

REQUIRED_DOC_286_SECTIONS = [
    "## Purpose",
    "## Relationship To TASK-239 Through TASK-244",
    "## Authorization Scope",
    "## Future Bridge Boundary",
    "## Future Test-Only Bridge Directory",
    "## Future Bridge Input Contract",
    "## Future Bridge Output Contract",
    "## Future AccountingEngine Call Boundary",
    "## Future Replay Boundary",
    "## Runtime Isolation Boundary",
    "## No-Write Boundary",
    "## AccountingEngine Code Boundary",
    "## Production Integration Boundary",
    "## Fixture Boundary",
    "## Broker And Network Boundary",
    "## Authorization Go / No-Go Decision",
    "## Formal Conclusion",
    "## Recommended Next Task",
]

REQUIRED_DOC_287_SECTIONS = [
    "## Test Objective",
    "## Required Static Checks",
    "## Required Regression Gates",
    "## Changed-Path Allowlist",
    "## Authorization Checks",
    "## Future Bridge Contract Checks",
    "## Failure Modes",
    "## Acceptance Criteria",
    "## Formal Conclusion",
]

FORMAL_CONCLUSION_LINES = [
    "TASK-245 authorizes AccountingEngine bridge policy only.",
    "TASK-245 does not implement AccountingEngine bridge.",
    "TASK-245 does not call AccountingEngine replay.",
    "TASK-245 does not implement replay.",
    "TASK-245 does not modify AccountingEngine code.",
    "TASK-245 does not modify fixture JSON.",
    "TASK-245 does not authorize production integration.",
    "TASK-245 does not authorize runtime SQL / SQLite read/write.",
    "TASK-245 does not authorize audit / ledger / snapshot writes.",
    "TASK-245 does not authorize read model refresh.",
    "TASK-245 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
    "Recommended next task: TASK-246 ShellAccounting manual entry replay AccountingEngine bridge test-only implementation gate.",
]

FORBIDDEN_CHANGED_PREFIXES = (
    "apps/",
    "libs/",
    "migrations/",
    "tests/fixtures/manual_entry_replay/",
    "tests/fixtures/manual_entry_replay_negative/",
)

FORBIDDEN_EXECUTABLE_TOKENS = [
    "AccountingReplayEngine(",
    "AccountingReplayEngine::",
    "replayReadOnly(",
    ".replayLedger(",
    "import " + "sqlite3",
    "INSERT " + "INTO",
    "UP" + "DATE ",
    "DELETE " + "FROM",
    "REPLACE " + "INTO",
    "writeAuditLog(",
    "writeLedger(",
    "writeSnapshot(",
    "refreshReadModel(",
    "broker" + "Order(",
    "place" + "Order(",
    "execute" + "Order(",
    "request" + "s.",
    "url" + "lib.",
]

FORBIDDEN_BRIDGE_IMPLEMENTATION_TOKENS = [
    "ShellAccountingManualEntryReplayAccountingEngineBridge/",
    "manual_entry_replay_accountingengine_bridge.py",
    "AccountingEngineBridge(",
    "AccountingEngineBridge::",
    "bridgeToAccountingEngine(",
    "buildAccountingEngineReplayInput(",
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


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def run(args: list[str], cwd: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(args, cwd=cwd, check=False, capture_output=True, text=True)


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


def text_without_policy_constant_tables(text: str) -> str:
    kept: list[str] = []
    skipping = False
    for line in text.splitlines():
        if re.match(r"^[A-Z0-9_]*(BAD|FORBIDDEN|KNOWN)[A-Z0-9_]*\s*=\s*[\(\[]", line):
            skipping = True
            continue
        if skipping:
            if line.strip() in {")", "]"}:
                skipping = False
            continue
        kept.append(line)
    return "\n".join(kept)


def text_without_meta_changed_path_assertion_checks(text: str) -> str:
    kept: list[str] = []
    for line in text.splitlines():
        if '"changed path detection must include current TASK" not in source_scan_text' in line:
            continue
        if '"must include current TASK-" not in source_scan_text' in line:
            continue
        kept.append(line)
    return "\n".join(kept)


def text_without_static_guard_lines(text: str) -> str:
    kept: list[str] = []
    for line in text_without_policy_constant_tables(text).splitlines():
        stripped = line.strip()
        if any(
            marker in stripped
            for marker in (
                "gate.require(",
                ".require(",
                "for token in",
                "if token",
                "not in text",
                "not in source_scan_text",
                "not in executable_scan_text",
            )
        ):
            continue
        kept.append(line)
    return "\n".join(kept)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", default=".")
    args = parser.parse_args()
    root = Path(args.source_root).resolve()
    gate = Gate()

    doc_286 = root / TASK_245_DOC
    doc_287 = root / TASK_245_PLAN
    readme = read(root / "README.md")
    docs_readme = read(root / "docs/README.md")
    prompt_template = read(root / "docs/12_codex_prompt_template.md")
    tests_cmake = read(root / "tests/CMakeLists.txt")
    task_cmake = read(root / TASK_245_CMAKE)
    task_gate_text = read(root / TASK_245_GATE)
    doc_286_text = read(doc_286) if doc_286.exists() else ""
    doc_287_text = read(doc_287) if doc_287.exists() else ""

    gate.require(doc_286.exists(), "docs/286 exists")
    gate.require(doc_287.exists(), "docs/287 exists")
    gate.require((root / TASK_245_CMAKE).exists(), "TASK-245 CMakeLists exists")
    gate.require((root / TASK_245_GATE).exists(), "TASK-245 gate script exists")

    gate.contains(readme, "TASK-245", "README")
    gate.contains(readme, TASK_245_DOC.as_posix(), "README")
    gate.contains(readme, TASK_245_PLAN.as_posix(), "README")
    gate.contains(docs_readme, TASK_245_DOC.name, "docs/README")
    gate.contains(docs_readme, TASK_245_PLAN.name, "docs/README")
    gate.contains(prompt_template, "TASK-245", "docs/12")
    gate.contains(prompt_template, TASK_245_DOC.as_posix(), "docs/12")
    gate.contains(prompt_template, TASK_245_PLAN.as_posix(), "docs/12")

    gate.contains(tests_cmake, "add_subdirectory(ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate)", "tests/CMakeLists")
    gate.contains(task_cmake, TASK_245_CTEST, "TASK-245 CMakeLists")
    gate.contains(task_cmake, "--source-root", "TASK-245 CMakeLists")

    for section in REQUIRED_DOC_286_SECTIONS:
        gate.contains(doc_286_text, section, "docs/286")
    for section in REQUIRED_DOC_287_SECTIONS:
        gate.contains(doc_287_text, section, "docs/287")
    for line in FORMAL_CONCLUSION_LINES:
        gate.contains(doc_286_text, line, "docs/286 Formal Conclusion")
        gate.contains(doc_287_text, line, "docs/287 Formal Conclusion")

    gate.contains(doc_286_text, FUTURE_BRIDGE_DIR.as_posix() + "/", "docs/286 future bridge directory")
    gate.contains(doc_287_text, FUTURE_BRIDGE_DIR.as_posix() + "/", "docs/287 future bridge directory")
    gate.contains(doc_286_text, "TASK-245 does not create this directory", "docs/286 no bridge directory creation")
    gate.contains(doc_287_text, "The gate must verify that the future directory does not exist in TASK-245", "docs/287 no bridge directory creation")

    for path in PREVIOUS_TASK_DOCS:
        gate.require((root / path).exists(), f"{path.as_posix()} exists")
    for path in [TASK_239_GATE, TASK_240_GATE, TASK_241_GATE, TASK_242_GATE, TASK_243_GATE, TASK_244_GATE]:
        gate.require((root / path).exists(), f"{path.as_posix()} exists")

    names = registered_ctests(root)
    gate.require(bool(names), "ctest -N returns registered tests")
    for ctest_name in CRITICAL_CTEST_NAMES:
        gate.require(ctest_name in names, f"{ctest_name} registered")

    current_paths = changed_paths(root)
    unauthorized = sorted(path for path in current_paths if path not in ALLOWED_CHANGED_PATHS)
    gate.require(not unauthorized, f"changed paths are exact allowlisted: {unauthorized}")
    gate.require(all(not path.startswith(prefix) for path in current_paths for prefix in FORBIDDEN_CHANGED_PREFIXES), "changed paths avoid forbidden production and fixture prefixes")
    gate.require(TASK_245_DOC.as_posix() in ALLOWED_CHANGED_PATHS, "TASK-245 docs/286 exact path allowlisted")
    gate.require(TASK_245_PLAN.as_posix() in ALLOWED_CHANGED_PATHS, "TASK-245 docs/287 exact path allowlisted")
    gate.require(TASK_245_GATE.as_posix() in ALLOWED_CHANGED_PATHS, "TASK-245 gate exact path allowlisted")
    gate.require(TASK_245_CMAKE.as_posix() in ALLOWED_CHANGED_PATHS, "TASK-245 CMake exact path allowlisted")
    gate.require("docs/" not in ALLOWED_CHANGED_PATHS, "changed-path allowlist has no docs directory wildcard")
    gate.require("tests/" not in ALLOWED_CHANGED_PATHS, "changed-path allowlist has no tests directory wildcard")
    gate.require("apps/" not in ALLOWED_CHANGED_PATHS, "changed-path allowlist has no apps directory wildcard")
    gate.require("libs/" not in ALLOWED_CHANGED_PATHS, "changed-path allowlist has no libs directory wildcard")
    gate.require("migrations/" not in ALLOWED_CHANGED_PATHS, "changed-path allowlist has no migrations directory wildcard")

    for script in CRITICAL_GATE_SCRIPTS:
        script_path = root / script
        gate.require(script_path.exists(), f"{script.as_posix()} exists")
        text = read(script_path)
        source_scan_text = text_without_meta_changed_path_assertion_checks(text_without_policy_constant_tables(text))
        executable_scan_text = text_without_static_guard_lines(text)
        gate.require("changed path detection must include current TASK" not in source_scan_text, f"{script.as_posix()} omits clean-main brittle changed-path assertion")
        gate.require("must include current TASK-" not in source_scan_text, f"{script.as_posix()} omits current-task changed-path dependency")
        gate.require("ctest --test-dir build --output-on-failure" not in executable_scan_text, f"{script.as_posix()} does not run recursive full CTest internally")
        gate.require("--repeat until-fail:50" not in executable_scan_text, f"{script.as_posix()} does not run transport loop internally")
        for token in FORBIDDEN_EXECUTABLE_TOKENS:
            gate.require(token not in executable_scan_text, f"{script.as_posix()} omits forbidden executable token `{token}`")

    task_gate_scan_text = text_without_static_guard_lines(task_gate_text)
    for token in FORBIDDEN_EXECUTABLE_TOKENS:
        gate.require(token not in task_gate_scan_text, f"TASK-245 gate omits forbidden executable token `{token}`")
    gate.require("ctest --test-dir build --output-on-failure" not in task_gate_scan_text, "TASK-245 gate does not run recursive full CTest")
    gate.require("--repeat until-fail:50" not in task_gate_scan_text, "TASK-245 gate does not run transport repeat loop internally")

    for token in FORBIDDEN_BRIDGE_IMPLEMENTATION_TOKENS:
        gate.require(token not in task_gate_scan_text, f"TASK-245 gate has no bridge implementation token `{token}`")

    fixture_paths = [
        root / "tests/fixtures/manual_entry_replay/fixtures_index.json",
        root / "tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json",
    ]
    for fixture in fixture_paths:
        gate.require(fixture.exists(), f"{fixture.relative_to(root).as_posix()} exists")
        gate.require(fixture.relative_to(root).as_posix() not in current_paths, f"{fixture.relative_to(root).as_posix()} not changed")

    gate.require(not (root / FUTURE_BRIDGE_DIR).exists(), "future AccountingEngine bridge test directory absent")
    gate.require(not any(path.startswith("libs/AccountingEngine/") for path in current_paths), "AccountingEngine code unchanged")
    gate.require(not any(path.startswith("apps/") for path in current_paths), "apps production paths unchanged")
    gate.require(not any(path.startswith("libs/") for path in current_paths), "libs production paths unchanged")
    gate.require(not any(path.startswith("migrations/") for path in current_paths), "migrations unchanged")
    gate.require(not any(path.endswith(".qml") for path in current_paths), "QML unchanged")

    gate.contains(doc_286_text, "TASK-245 authorizes AccountingEngine bridge policy only", "docs/286 bridge policy only")
    gate.contains(doc_286_text, "TASK-245 does not implement AccountingEngine bridge", "docs/286 no bridge implementation")
    gate.contains(doc_286_text, "TASK-245 does not call AccountingEngine replay", "docs/286 no replay call")
    gate.contains(doc_286_text, "TASK-245 does not authorize production integration", "docs/286 no production integration")
    gate.contains(doc_287_text, "Clean main may have an empty changed-path set", "docs/287 clean-main policy")
    gate.contains(doc_287_text, "The allowlist must not use directory-level `docs/`, `tests/`, `apps/`, `libs/`, or `migrations/` entries", "docs/287 no directory allowlist")
    gate.contains(doc_287_text, "The gate must prove TASK-245 authorizes bridge policy only", "docs/287 authorization policy")
    gate.contains(doc_287_text, "TASK-246 ShellAccounting manual entry replay AccountingEngine bridge test-only implementation gate", "docs/287 next task")

    gate.require(gate.checks >= 120, f"expected at least 120 checks, got {gate.checks}")
    print(f"TASK_245_CHECKS={gate.checks}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
