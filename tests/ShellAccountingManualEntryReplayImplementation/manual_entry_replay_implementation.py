#!/usr/bin/env python3

import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path
from typing import Any


SUMMARY_SCHEMA = "manual-entry-replay-test-only-implementation-summary/v1"
DRY_RUN_SCHEMA = "manual-entry-replay-test-only-dry-run-summary/v1"
FAILURE_SCHEMA = "manual-entry-replay-test-only-implementation-failure/v1"
TASK_231_HARNESS = Path(
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/"
    "manual_entry_replay_test_only_dry_run_harness.py"
)
POSITIVE_DIR = Path("tests/fixtures/manual_entry_replay")
NEGATIVE_DIR = Path("tests/fixtures/manual_entry_replay_negative")

ALLOWED_CHANGED_PATHS = {
    "docs/268_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_gate.md",
    "docs/269_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "docs/270_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_gate.md",
    "docs/271_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "docs/266_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_gate.md",
    "docs/267_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
    "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
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
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/manual_entry_replay_next_phase_authorization_planning_gate.py",
}

FORBIDDEN_CHANGED_PREFIXES = (
    "apps/",
    "libs/",
    "migrations/",
    "tests/fixtures/manual_entry_replay/",
    "tests/fixtures/manual_entry_replay_negative/",
)

FORBIDDEN_SUMMARY_TOKENS = (
    "raw " + "fixture payload",
    "raw " + "payload",
    "select " + "*",
    "insert " + "into",
    "up" + "date ",
    "delete " + "from",
    "drop " + "table",
    "credential=",
    "endpoint=",
    "access " + "token",
    "secret " + "key",
    "private " + "key",
    "password",
    "broker " + "payload",
    "real " + "order id",
    "traceback",
    "stack " + "trace",
)

FORBIDDEN_DRY_RUN_INPUT_TOKENS = FORBIDDEN_SUMMARY_TOKENS + (
    "api_key",
    "credential",
    "endpoint",
    "broker payload",
    "real order",
    "apps/",
    "libs/",
    "migrations/",
    ".db",
    ".sqlite",
    ".sqlite3",
    ".db-wal",
    ".db-shm",
    ".sqlite-wal",
    ".sqlite-shm",
)

DB_SUFFIXES = (".db", ".sqlite", ".sqlite3", ".db-wal", ".db-shm", ".sqlite-wal", ".sqlite-shm")


class Check:
    def __init__(self) -> None:
        self.count = 0

    def require(self, condition: bool, message: str) -> None:
        self.count += 1
        if not condition:
            raise AssertionError(message)


def sanitized_failure(message: str = "dry-run summary contract failed") -> dict[str, Any]:
    return {
        "schemaVersion": FAILURE_SCHEMA,
        "ok": False,
        "failureCount": 1,
        "failures": [
            {
                "issueCode": "REPLAY_IMPLEMENTATION_CONTRACT_VIOLATION",
                "rule": "dry-run-summary-contract",
                "file": "dry-run-summary",
                "message": "sanitized failure: " + message,
            }
        ],
    }


def git_lines(root: Path, *args: str) -> set[str]:
    completed = subprocess.run(["git", *args], cwd=root, check=True, capture_output=True, text=True)
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def changed_paths(root: Path) -> set[str]:
    return (
        git_lines(root, "diff", "--name-only", "main")
        | git_lines(root, "diff", "--cached", "--name-only")
        | git_lines(root, "ls-files", "--others", "--exclude-standard")
    )


def fixture_hashes(root: Path) -> dict[str, str]:
    fixture_paths = sorted((root / POSITIVE_DIR).glob("*.json")) + sorted((root / NEGATIVE_DIR).glob("*.json"))
    return {
        path.relative_to(root).as_posix(): hashlib.sha256(path.read_bytes()).hexdigest()
        for path in fixture_paths
    }


def db_like_untracked_paths(root: Path) -> set[str]:
    return {
        path
        for path in git_lines(root, "ls-files", "--others", "--exclude-standard")
        if path.lower().endswith(DB_SUFFIXES)
    }


def validate_changed_paths(check: Check, root: Path) -> None:
    changes = changed_paths(root)
    check.require(all(not path.endswith("/") for path in ALLOWED_CHANGED_PATHS), "allowlist uses exact files")
    check.require(all("*" not in path for path in ALLOWED_CHANGED_PATHS), "allowlist uses no wildcards")
    check.require("docs/" not in ALLOWED_CHANGED_PATHS, "allowlist does not include docs directory")
    check.require("tests/" not in ALLOWED_CHANGED_PATHS, "allowlist does not include tests directory")
    unexpected = sorted(changes - ALLOWED_CHANGED_PATHS)
    check.require(not unexpected, "TASK-234 changed unauthorized paths: " + ", ".join(unexpected))
    for path in sorted(changes):
        check.require(path in ALLOWED_CHANGED_PATHS, f"changed path exact allowlisted: {path}")
        check.require(not path.startswith(FORBIDDEN_CHANGED_PREFIXES), f"forbidden changed path: {path}")
        check.require(not path.endswith(".json"), f"fixture JSON unchanged: {path}")
    check.require(git_lines(root, "diff", "--name-only", "main", "--", "apps") == set(), "apps diff empty")
    check.require(git_lines(root, "diff", "--name-only", "main", "--", "libs") == set(), "libs diff empty")
    check.require(git_lines(root, "diff", "--name-only", "main", "--", "migrations") == set(), "migrations diff empty")
    check.require(
        git_lines(root, "diff", "--name-only", "main", "--", "tests/fixtures/manual_entry_replay") == set(),
        "positive fixture JSON diff empty",
    )
    check.require(
        git_lines(root, "diff", "--name-only", "main", "--", "tests/fixtures/manual_entry_replay_negative") == set(),
        "negative fixture JSON diff empty",
    )


def run_dry_run_summary(check: Check, root: Path) -> dict[str, Any]:
    harness_path = root / TASK_231_HARNESS
    check.require(harness_path.exists(), "TASK-231 dry-run harness script exists")
    completed = subprocess.run(
        [sys.executable, str(harness_path), "--source-root", str(root), "--summary-json"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    summary = json.loads(completed.stdout)
    check.require(isinstance(summary, dict), "dry-run summary object")
    validate_dry_run_summary(check, summary)
    return summary


def read_dry_run_summary_override(check: Check, path: Path) -> dict[str, Any]:
    check.require(path.exists(), "dry-run summary override exists")
    check.require(path.is_file(), "dry-run summary override is file")
    try:
        summary = json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        raise AssertionError("dry-run summary JSON is invalid") from exc
    check.require(isinstance(summary, dict), "dry-run summary object")
    validate_dry_run_summary(check, summary)
    return summary


def validate_dry_run_summary(check: Check, summary: dict[str, Any]) -> None:
    encoded = json.dumps(summary, sort_keys=True).lower()
    for token in FORBIDDEN_DRY_RUN_INPUT_TOKENS:
        check.require(token not in encoded, "dry-run summary input excludes forbidden token")
    check.require(summary.get("schemaVersion") == DRY_RUN_SCHEMA, "dry-run summary schema correct")
    check.require(summary.get("dryRunStatus") == "ok", "dry-run summary status ok")
    entries = summary.get("entries")
    check.require(isinstance(entries, list), "dry-run entries list")
    check.require(bool(entries), "dry-run entries present")
    seen: set[str] = set()
    for entry in entries:
        check.require(isinstance(entry, dict), "dry-run entry object")
        fixture_id = entry.get("fixtureId")
        check.require(isinstance(fixture_id, str) and fixture_id, "dry-run fixture id is present")
        check.require(fixture_id not in seen, "dry-run fixture id is unique")
        seen.add(fixture_id)
        source = entry.get("source")
        check.require(isinstance(source, str) and source, "dry-run entry source is present")
        check.require(entry.get("dryRunStatus") == "metadata_checked", "dry-run entry status is metadata checked")
        planned_count = entry.get("plannedStepCount")
        check.require(isinstance(planned_count, int), "planned step count is integer")
        check.require(planned_count >= 0, "planned step count is non-negative")
        check.require(entry.get("replayExecuted") is False, "dry-run replay executed flag false")
        check.require(entry.get("accountingEngineCalled") is False, "dry-run engine flag false")
        check.require(entry.get("runtimeWrites") is False, "dry-run runtime write flag false")


def sanitize_entry(check: Check, entry: dict[str, Any]) -> dict[str, Any]:
    fixture_id = entry.get("fixtureId")
    check.require(isinstance(fixture_id, str) and fixture_id, "dry-run fixture id is present")
    check.require(entry.get("accountingEngineCalled") is False, f"dry-run engine flag false: {fixture_id}")
    check.require(entry.get("runtimeWrites") is False, f"dry-run runtime write flag false: {fixture_id}")
    planned_count = entry.get("plannedStepCount", 0)
    check.require(isinstance(planned_count, int), f"planned step count is integer: {fixture_id}")
    check.require(planned_count == 0, f"planned step count remains zero: {fixture_id}")
    return {
        "fixtureId": fixture_id,
        "source": "dry-run-summary",
        "replayPlanStatus": "planned",
        "plannedEventCount": planned_count,
        "accountingEngineCalled": False,
        "runtimeWrites": False,
        "positionCashPnlCalculated": False,
    }


def build_summary(check: Check, dry_run_summary: dict[str, Any]) -> dict[str, Any]:
    dry_entries = dry_run_summary["entries"]
    entries = [sanitize_entry(check, entry) for entry in dry_entries]
    summary = {
        "schemaVersion": SUMMARY_SCHEMA,
        "implementationMode": "test-only-in-memory",
        "implementationStatus": "ok",
        "fixtureCount": len(entries),
        "entries": entries,
    }
    check.require(summary["fixtureCount"] == len(dry_entries), "fixture count matches dry-run entries")
    encoded = json.dumps(summary, sort_keys=True)
    lowered = encoded.lower()
    for token in FORBIDDEN_SUMMARY_TOKENS:
        check.require(token not in lowered, f"summary excludes forbidden token: {token}")
    for entry in entries:
        check.require(set(entry) == {
            "fixtureId",
            "source",
            "replayPlanStatus",
            "plannedEventCount",
            "accountingEngineCalled",
            "runtimeWrites",
            "positionCashPnlCalculated",
        }, "implementation entry uses sanitized key set")
        check.require(entry["source"] == "dry-run-summary", "implementation entry source is dry-run summary")
        check.require(entry["replayPlanStatus"] == "planned", "implementation entry plan status")
        check.require(entry["accountingEngineCalled"] is False, "implementation entry engine flag false")
        check.require(entry["runtimeWrites"] is False, "implementation entry write flag false")
        check.require(entry["positionCashPnlCalculated"] is False, "implementation entry no real position cash pnl")
    return summary


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    parser.add_argument("--summary-json", action="store_true")
    parser.add_argument("--dry-run-summary-json-file")
    args = parser.parse_args()

    root = Path(args.source_root)
    check = Check()
    try:
        validate_changed_paths(check, root)
        before_hashes = fixture_hashes(root)
        before_db_like = db_like_untracked_paths(root)
        if args.dry_run_summary_json_file:
            dry_run_summary = read_dry_run_summary_override(check, Path(args.dry_run_summary_json_file))
        else:
            dry_run_summary = run_dry_run_summary(check, root)
        summary = build_summary(check, dry_run_summary)
        after_hashes = fixture_hashes(root)
        after_db_like = db_like_untracked_paths(root)
        check.require(before_hashes == after_hashes, "fixture JSON hashes unchanged")
        check.require(before_db_like == after_db_like, "no DB/WAL/SHM/SQLite files created")
        check.require(check.count >= 80, f"expected at least 80 implementation checks, got {check.count}")

        if args.summary_json:
            print(json.dumps(summary, sort_keys=True))
        else:
            print(json.dumps({"status": "passed", "checks": check.count}, sort_keys=True))
        return 0
    except Exception:
        if args.summary_json:
            print(json.dumps(sanitized_failure(), sort_keys=True))
        else:
            print("sanitized failure: dry-run summary contract failed", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
