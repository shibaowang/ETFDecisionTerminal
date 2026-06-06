#!/usr/bin/env python3

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Any


DRY_RUN_SCHEMA = "manual-entry-replay-test-only-dry-run-summary/v1"
IMPLEMENTATION_SCHEMA = "manual-entry-replay-test-only-implementation-summary/v1"
TASK_231_HARNESS = Path(
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/"
    "manual_entry_replay_test_only_dry_run_harness.py"
)
TASK_234_IMPLEMENTATION = Path(
    "tests/ShellAccountingManualEntryReplayImplementation/"
    "manual_entry_replay_implementation.py"
)

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


class BridgeError(Exception):
    def __init__(self, issue_code: str, message: str = "bridge contract failed") -> None:
        super().__init__(message)
        self.issue_code = issue_code
        self.message = message


def read_json_file(path: Path) -> dict[str, Any]:
    if not path.exists():
        raise BridgeError("BRIDGE_INPUT_MISSING", "input summary missing")
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        raise BridgeError("BRIDGE_INPUT_MALFORMED_JSON", "input summary malformed") from exc
    if not isinstance(value, dict):
        raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "input summary must be object")
    ensure_input_sanitized(value)
    return value


def run_summary_script(root: Path, script: Path) -> dict[str, Any]:
    completed = subprocess.run(
        [sys.executable, str(root / script), "--source-root", str(root), "--summary-json"],
        cwd=root,
        check=False,
        capture_output=True,
        text=True,
    )
    if completed.returncode != 0:
        raise BridgeError("BRIDGE_INPUT_MISSING", "test-only summary source failed")
    try:
        value = json.loads(completed.stdout)
    except Exception as exc:
        raise BridgeError("BRIDGE_INPUT_MALFORMED_JSON", "test-only summary source returned invalid JSON") from exc
    if not isinstance(value, dict):
        raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "test-only summary source returned non-object JSON")
    ensure_input_sanitized(value)
    return value


def validate_dry_run_summary(summary: dict[str, Any]) -> None:
    if summary.get("syntheticDataOnly", True) is not True:
        raise BridgeError("BRIDGE_INPUT_NON_SYNTHETIC", "dry-run summary must be synthetic")
    if summary.get("schemaVersion") != DRY_RUN_SCHEMA:
        raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "dry-run summary schema mismatch")
    if summary.get("dryRunStatus") != "ok":
        raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "dry-run summary status mismatch")
    entries = summary.get("entries")
    if not isinstance(entries, list) or not entries:
        raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "dry-run summary entries missing")
    seen: set[str] = set()
    for entry in entries:
        if not isinstance(entry, dict):
            raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "dry-run summary entry must be object")
        fixture_id = entry.get("fixtureId")
        if not isinstance(fixture_id, str) or not fixture_id:
            raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "dry-run summary fixture id missing")
        if fixture_id in seen:
            raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "dry-run summary fixture id duplicate")
        seen.add(fixture_id)
        if entry.get("accountingEngineCalled") is not False:
            raise BridgeError("BRIDGE_INPUT_ACCOUNTINGENGINE_CALLED_TRUE", "dry-run summary engine flag rejected")
        if entry.get("replayExecuted") is not False:
            raise BridgeError("BRIDGE_INPUT_REPLAY_EXECUTED_TRUE", "dry-run summary replay flag rejected")
        if entry.get("runtimeWrites") is not False:
            raise BridgeError("BRIDGE_INPUT_RUNTIME_WRITE_TRUE", "dry-run summary runtime write flag rejected")


def validate_implementation_summary(summary: dict[str, Any]) -> None:
    if summary.get("syntheticDataOnly", True) is not True:
        raise BridgeError("BRIDGE_INPUT_NON_SYNTHETIC", "implementation summary must be synthetic")
    if summary.get("schemaVersion") != IMPLEMENTATION_SCHEMA:
        raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "implementation summary schema mismatch")
    if summary.get("implementationMode") != "test-only-in-memory":
        raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "implementation summary mode mismatch")
    if summary.get("implementationStatus") != "ok":
        raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "implementation summary status mismatch")
    entries = summary.get("entries")
    if not isinstance(entries, list) or not entries:
        raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "implementation summary entries missing")
    for entry in entries:
        if not isinstance(entry, dict):
            raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "implementation summary entry must be object")
        if entry.get("accountingEngineCalled") is not False:
            raise BridgeError("BRIDGE_INPUT_ACCOUNTINGENGINE_CALLED_TRUE", "implementation summary engine flag rejected")
        if entry.get("runtimeWrites") is not False:
            raise BridgeError("BRIDGE_INPUT_RUNTIME_WRITE_TRUE", "implementation summary runtime write flag rejected")
        if entry.get("positionCashPnlCalculated") is not False:
            raise BridgeError("BRIDGE_INPUT_MISSING_REQUIRED_FIELD", "implementation summary calculation flag rejected")


def sanitized_fixture_ids(summary: dict[str, Any]) -> list[str]:
    ids: list[str] = []
    for entry in summary.get("entries", []):
        if isinstance(entry, dict) and isinstance(entry.get("fixtureId"), str):
            ids.append(entry["fixtureId"])
    return sorted(set(ids))


def build_bridge_summary(dry_run_summary: dict[str, Any], implementation_summary: dict[str, Any]) -> dict[str, Any]:
    dry_ids = sanitized_fixture_ids(dry_run_summary)
    implementation_ids = sanitized_fixture_ids(implementation_summary)
    summary = {
        "schemaVersion": 1,
        "task": "TASK-246",
        "mode": "test-only-accountingengine-bridge-contract",
        "accountingEngineCalled": False,
        "replayExecuted": False,
        "runtimeWrite": False,
        "bridgeRequest": {
            "schemaVersion": 1,
            "source": "manual-entry-replay-test-only",
            "syntheticDataOnly": True,
            "dryRunSchema": DRY_RUN_SCHEMA,
            "implementationSchema": IMPLEMENTATION_SCHEMA,
            "fixtureCount": len(implementation_ids),
            "fixtureIds": implementation_ids,
        },
        "bridgeResponse": {
            "schemaVersion": 1,
            "status": "not_executed",
            "reason": "AccountingEngine replay is not authorized in TASK-246",
        },
        "issues": [],
    }
    if dry_ids != implementation_ids:
        summary["issues"] = [
            {
                "issueCode": "BRIDGE_FIXTURE_SET_MISMATCH",
                "message": "sanitized bridge issue: fixture coverage mismatch",
            }
        ]
    ensure_sanitized(summary)
    return summary


def sanitized_failure(issue_code: str = "BRIDGE_CONTRACT_INPUT_INVALID", message: str = "bridge contract failed") -> dict[str, Any]:
    return {
        "schemaVersion": 1,
        "task": "TASK-246",
        "mode": "test-only-accountingengine-bridge-contract",
        "accountingEngineCalled": False,
        "replayExecuted": False,
        "runtimeWrite": False,
        "bridgeRequest": {
            "schemaVersion": 1,
            "source": "manual-entry-replay-test-only",
            "syntheticDataOnly": True,
        },
        "bridgeResponse": {
            "schemaVersion": 1,
            "status": "failed",
            "reason": "sanitized failure: " + message,
        },
        "issues": [
            {
                "issueCode": issue_code,
                "message": "sanitized bridge failure",
            }
        ],
    }


def ensure_input_sanitized(summary: dict[str, Any]) -> None:
    lowered = json.dumps(summary, sort_keys=True).lower()
    for token in FORBIDDEN_OUTPUT_TOKENS:
        if token in lowered:
            raise BridgeError("BRIDGE_FORBIDDEN_TOKEN", "bridge input contains forbidden token")


def ensure_sanitized(summary: dict[str, Any]) -> None:
    lowered = json.dumps(summary, sort_keys=True).lower()
    for token in FORBIDDEN_OUTPUT_TOKENS:
        if token in lowered:
            raise BridgeError("BRIDGE_FORBIDDEN_TOKEN", "bridge output contains forbidden token")


def emit_json(summary: dict[str, Any], destination: str | None) -> None:
    encoded = json.dumps(summary, sort_keys=True)
    if destination is None or destination == "-":
        print(encoded)
        return
    path = Path(destination)
    try:
        path.write_text(encoded + "\n", encoding="utf-8")
    except Exception as exc:
        raise BridgeError("BRIDGE_OUTPUT_PATH_INVALID", "bridge output path invalid") from exc


def emit_failure(summary: dict[str, Any], failure_destination: str | None, summary_destination: str | None) -> None:
    destination = failure_destination if failure_destination is not None else summary_destination
    try:
        emit_json(summary, destination)
    except Exception:
        print(json.dumps(summary, sort_keys=True))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    parser.add_argument("--dry-run-summary-json")
    parser.add_argument("--implementation-summary-json")
    parser.add_argument("--summary-json")
    parser.add_argument("--failure-json")
    parser.add_argument("--expect-no-accountingengine-call", action="store_true")
    args = parser.parse_args()

    root = Path(args.source_root)
    try:
        dry_run_summary = (
            read_json_file(Path(args.dry_run_summary_json))
            if args.dry_run_summary_json
            else run_summary_script(root, TASK_231_HARNESS)
        )
        implementation_summary = (
            read_json_file(Path(args.implementation_summary_json))
            if args.implementation_summary_json
            else run_summary_script(root, TASK_234_IMPLEMENTATION)
        )
        validate_dry_run_summary(dry_run_summary)
        validate_implementation_summary(implementation_summary)
        summary = build_bridge_summary(dry_run_summary, implementation_summary)
        if args.expect_no_accountingengine_call and summary["accountingEngineCalled"] is not False:
            raise BridgeError("BRIDGE_INPUT_ACCOUNTINGENGINE_CALLED_TRUE", "AccountingEngine call expectation failed")
        emit_json(summary, args.summary_json)
        return 0
    except BridgeError as exc:
        failure = sanitized_failure(exc.issue_code, exc.message)
        emit_failure(failure, args.failure_json, args.summary_json)
        return 2
    except Exception:
        failure = sanitized_failure("BRIDGE_CONTRACT_INPUT_INVALID")
        emit_failure(failure, args.failure_json, args.summary_json)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
