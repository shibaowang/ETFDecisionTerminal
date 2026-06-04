#!/usr/bin/env python3

import argparse
import json
from pathlib import Path
from typing import Any


EXPECTED_FILES = {
    "fixtures_index.json",
    "MRF001_empty_manual_facts.json",
    "MRF002_single_buy.json",
    "MRF003_buy_deposit_baseline.json",
    "MRF004_buy_sell_partial_reduction.json",
    "MRF005_deposit_withdrawal_baseline.json",
    "MRF006_daily_use_combined_baseline.json",
}

EXPECTED_FIXTURES = {
    "MRF001": "MRF001_empty_manual_facts.json",
    "MRF002": "MRF002_single_buy.json",
    "MRF003": "MRF003_buy_deposit_baseline.json",
    "MRF004": "MRF004_buy_sell_partial_reduction.json",
    "MRF005": "MRF005_deposit_withdrawal_baseline.json",
    "MRF006": "MRF006_daily_use_combined_baseline.json",
}

FORBIDDEN_EXTENSIONS = {
    ".db",
    ".sqlite",
    ".sqlite3",
    ".wal",
    ".shm",
}

FORBIDDEN_FILE_MARKERS = {
    "parser",
    "loader",
    "reader",
    "replay_runner",
    "runtime_output",
    "runtime-output",
    "seed",
}

FORBIDDEN_TEXT_TOKENS = {
    "RAW_SQL": "raw sql",
    "SQL_SELECT_ALL": "select" + " *",
    "SQL_INSERT": "insert" + " into",
    "SQL_UPDATE": "update" + " ",
    "SQL_DELETE": "delete" + " from",
    "SQL_DROP": "drop" + " table",
    "CREDENTIALS": "credentials",
    "CREDENTIAL": "credential",
    "ENDPOINT": "endpoint",
    "BROKER_PAYLOAD": "broker payload",
    "REAL_ORDER_ID": "real order id",
    "REAL_BROKER_ORDER_ID": "real broker order id",
    "ACCESS_TOKEN": "access token",
    "SECRET_KEY": "secret key",
    "PRIVATE_KEY": "private key",
    "PASSWORD": "password",
    "SQLITE": "sqlite",
    "WAL": ".wal",
    "SHM": ".shm",
    "TRADE_LOG_RAW": "trade_log raw",
    "CASH_ADJUSTMENT_RAW": "cash_adjustment raw",
    "STACK_TRACE": "stack trace",
}

ALLOWED_EXPECTED_MARKERS = {"SCAFFOLD_ONLY", "UNAVAILABLE_EXPECTED", "TODO_PLACEHOLDER"}
ALLOWED_PRIVACY_MARKERS = {"SYNTHETIC_ONLY", "SANITIZED_MEMO"}


class ValidationFailure(AssertionError):
    pass


class Validator:
    def __init__(self) -> None:
        self.checks = 0

    def require(self, condition: bool, issue_code: str, context: str) -> None:
        self.checks += 1
        if not condition:
            raise ValidationFailure(f"{issue_code}: {context}")


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> Any:
    return json.loads(read_text(path))


def require_bool_field(validator: Validator, payload: dict[str, Any], field: str, expected: bool, context: str) -> None:
    validator.require(field in payload, "REQUIRED_FIELD_MISSING", f"{context}.{field}")
    validator.require(payload[field] is expected, "BOOLEAN_FIELD_MISMATCH", f"{context}.{field}")


def require_string_field(validator: Validator, payload: dict[str, Any], field: str, context: str) -> None:
    validator.require(field in payload, "REQUIRED_FIELD_MISSING", f"{context}.{field}")
    validator.require(isinstance(payload[field], str) and bool(payload[field].strip()), "STRING_FIELD_INVALID", f"{context}.{field}")


def require_no_forbidden_tokens(validator: Validator, text: str, context: str) -> None:
    lowered = text.lower()
    for issue_code, token in FORBIDDEN_TEXT_TOKENS.items():
        validator.require(token not in lowered, f"FORBIDDEN_TOKEN_{issue_code}", context)


def flatten_strings(value: Any) -> list[str]:
    if isinstance(value, str):
        return [value]
    if isinstance(value, list):
        flattened: list[str] = []
        for item in value:
            flattened.extend(flatten_strings(item))
        return flattened
    if isinstance(value, dict):
        flattened = []
        for item in value.values():
            flattened.extend(flatten_strings(item))
        return flattened
    return []


def require_expected_outputs_are_placeholders(
    validator: Validator,
    expected_outputs: Any,
    context: str,
) -> None:
    validator.require(isinstance(expected_outputs, dict) and bool(expected_outputs), "EXPECTED_OUTPUTS_INVALID", context)
    for key, value in expected_outputs.items():
        validator.require(isinstance(key, str) and bool(key.strip()), "EXPECTED_OUTPUT_KEY_INVALID", f"{context}.{key}")
        validator.require(isinstance(value, str), "EXPECTED_OUTPUT_VALUE_INVALID", f"{context}.{key}")
        validator.require(value in ALLOWED_EXPECTED_MARKERS, "EXPECTED_OUTPUT_NOT_PLACEHOLDER", f"{context}.{key}")


def require_source_facts_are_scaffold_only(validator: Validator, source_facts: Any, context: str) -> None:
    validator.require(isinstance(source_facts, dict), "SOURCE_FACTS_INVALID", context)
    validator.require(isinstance(source_facts.get("manualTransactions"), list), "MANUAL_TRANSACTIONS_INVALID", context)
    validator.require(isinstance(source_facts.get("manualCashMovements"), list), "MANUAL_CASH_MOVEMENTS_INVALID", context)
    for value in flatten_strings(source_facts):
        if value in {"BUY", "SELL", "DEPOSIT", "WITHDRAWAL"}:
            continue
        validator.require(
            value.startswith("SYNTHETIC_") or value in {"TODO_PLACEHOLDER", "SANITIZED_MEMO"},
            "SOURCE_FACT_NOT_SYNTHETIC_PLACEHOLDER",
            context,
        )


def validate_fixture_directory(validator: Validator, fixture_dir: Path) -> None:
    validator.require(fixture_dir.exists(), "FIXTURE_DIR_MISSING", fixture_dir.as_posix())
    validator.require(fixture_dir.is_dir(), "FIXTURE_DIR_INVALID", fixture_dir.as_posix())

    children = [child for child in fixture_dir.iterdir() if child.is_file()]
    file_names = {child.name for child in children}
    validator.require(file_names & EXPECTED_FILES == EXPECTED_FILES, "EXPECTED_FILE_MISSING", "manual_entry_replay")

    json_files = {child.name for child in children if child.suffix.lower() == ".json"}
    extra_json = sorted(json_files - EXPECTED_FILES)
    validator.require(not extra_json, "EXTRA_JSON_FILE", ",".join(extra_json))

    forbidden_storage_files = sorted(
        child.name for child in children if child.suffix.lower() in FORBIDDEN_EXTENSIONS
    )
    validator.require(not forbidden_storage_files, "RUNTIME_STORAGE_FILE_PRESENT", ",".join(forbidden_storage_files))

    forbidden_runtime_files = sorted(
        child.name
        for child in children
        if child.name not in EXPECTED_FILES and any(marker in child.name.lower() for marker in FORBIDDEN_FILE_MARKERS)
    )
    validator.require(not forbidden_runtime_files, "RUNTIME_HELPER_FILE_PRESENT", ",".join(forbidden_runtime_files))


def validate_index(validator: Validator, fixture_dir: Path) -> dict[str, str]:
    index_path = fixture_dir / "fixtures_index.json"
    text = read_text(index_path)
    require_no_forbidden_tokens(validator, text, "fixtures_index.json")
    index = load_json(index_path)
    validator.require(isinstance(index, dict), "INDEX_NOT_OBJECT", "fixtures_index.json")
    validator.require(index.get("schemaVersion") == "manual-entry-replay-fixtures.scaffold.v1", "INDEX_SCHEMA_INVALID", "schemaVersion")
    validator.require(index.get("fixtureSet") == "manual_entry_replay_scaffold", "INDEX_FIXTURE_SET_INVALID", "fixtureSet")
    validator.require(index.get("generatedByTask") == "TASK-217", "INDEX_TASK_INVALID", "generatedByTask")
    require_bool_field(validator, index, "runtimeUse", False, "fixtures_index")
    require_bool_field(validator, index, "productionUse", False, "fixtures_index")
    require_bool_field(validator, index, "replayExecuted", False, "fixtures_index")
    require_bool_field(validator, index, "containsSyntheticDataOnly", True, "fixtures_index")

    fixtures = index.get("fixtures")
    validator.require(isinstance(fixtures, list), "INDEX_FIXTURES_INVALID", "fixtures")
    validator.require(len(fixtures) == len(EXPECTED_FIXTURES), "INDEX_FIXTURE_COUNT_INVALID", "fixtures")

    seen_ids: set[str] = set()
    referenced_files: dict[str, str] = {}
    for row in fixtures:
        validator.require(isinstance(row, dict), "INDEX_FIXTURE_ROW_INVALID", "fixtures")
        fixture_id = row.get("fixtureId")
        fixture_file = row.get("file")
        validator.require(isinstance(fixture_id, str), "INDEX_FIXTURE_ID_INVALID", "fixtureId")
        validator.require(fixture_id not in seen_ids, "INDEX_FIXTURE_ID_DUPLICATE", fixture_id)
        seen_ids.add(fixture_id)
        validator.require(fixture_id in EXPECTED_FIXTURES, "INDEX_FIXTURE_ID_UNEXPECTED", fixture_id)
        validator.require(fixture_file == EXPECTED_FIXTURES[fixture_id], "INDEX_FIXTURE_FILE_MISMATCH", fixture_id)
        validator.require(row.get("expectedStatus") == "SCAFFOLD_ONLY", "INDEX_EXPECTED_STATUS_INVALID", fixture_id)
        validator.require(row.get("privacyStatus") == "SYNTHETIC_ONLY", "INDEX_PRIVACY_STATUS_INVALID", fixture_id)
        validator.require(isinstance(row.get("blockingExpected"), bool), "INDEX_BLOCKING_EXPECTED_INVALID", fixture_id)
        for required in ["title", "category"]:
            require_string_field(validator, row, required, f"fixtures.{fixture_id}")
        referenced_files[fixture_id] = fixture_file

    validator.require(seen_ids == set(EXPECTED_FIXTURES), "INDEX_FIXTURE_IDS_INCOMPLETE", "fixtures")
    validator.require(set(referenced_files.values()) == set(EXPECTED_FIXTURES.values()), "INDEX_FILES_INCOMPLETE", "fixtures")
    return referenced_files


def validate_fixture(validator: Validator, fixture_path: Path, fixture_id: str) -> None:
    text = read_text(fixture_path)
    require_no_forbidden_tokens(validator, text, fixture_path.name)
    fixture = load_json(fixture_path)
    validator.require(isinstance(fixture, dict), "FIXTURE_NOT_OBJECT", fixture_path.name)

    validator.require(fixture.get("schemaVersion") == "manual-entry-replay-fixture.scaffold.v1", "FIXTURE_SCHEMA_INVALID", fixture_id)
    validator.require(fixture.get("fixtureId") == fixture_id, "FIXTURE_ID_MISMATCH", fixture_id)
    validator.require(fixture_path.name.startswith(fixture_id), "FIXTURE_FILE_PREFIX_MISMATCH", fixture_path.name)

    for field in ["title", "purpose", "category"]:
        require_string_field(validator, fixture, field, fixture_id)

    require_bool_field(validator, fixture, "scaffoldOnly", True, fixture_id)
    require_bool_field(validator, fixture, "runtimeUse", False, fixture_id)
    require_bool_field(validator, fixture, "productionUse", False, fixture_id)
    require_bool_field(validator, fixture, "replayExecuted", False, fixture_id)
    require_bool_field(validator, fixture, "containsSyntheticDataOnly", True, fixture_id)

    validator.require("sourceFacts" in fixture, "SOURCE_FACTS_MISSING", fixture_id)
    require_source_facts_are_scaffold_only(validator, fixture["sourceFacts"], fixture_id)

    validator.require("expectedOutputs" in fixture, "EXPECTED_OUTPUTS_MISSING", fixture_id)
    require_expected_outputs_are_placeholders(validator, fixture["expectedOutputs"], fixture_id)

    expected_issues = fixture.get("expectedIssues")
    validator.require(isinstance(expected_issues, list) and bool(expected_issues), "EXPECTED_ISSUES_INVALID", fixture_id)
    validator.require(all(issue == "TODO_PLACEHOLDER" for issue in expected_issues), "EXPECTED_ISSUES_NOT_PLACEHOLDER", fixture_id)

    validator.require(isinstance(fixture.get("blockingExpected"), bool), "BLOCKING_EXPECTED_INVALID", fixture_id)

    privacy_expectations = fixture.get("privacyExpectations")
    validator.require(isinstance(privacy_expectations, list) and bool(privacy_expectations), "PRIVACY_EXPECTATIONS_INVALID", fixture_id)
    validator.require(set(privacy_expectations).issubset(ALLOWED_PRIVACY_MARKERS), "PRIVACY_EXPECTATIONS_UNEXPECTED", fixture_id)
    validator.require("SYNTHETIC_ONLY" in privacy_expectations, "PRIVACY_SYNTHETIC_ONLY_MISSING", fixture_id)
    validator.require("SANITIZED_MEMO" in privacy_expectations, "PRIVACY_SANITIZED_MEMO_MISSING", fixture_id)

    metadata = fixture.get("metadata")
    validator.require(isinstance(metadata, dict) and bool(metadata), "METADATA_INVALID", fixture_id)
    validator.require(metadata.get("generatedByTask") == "TASK-217", "METADATA_TASK_INVALID", fixture_id)
    for value in flatten_strings(metadata):
        validator.require(
            value.startswith("SYNTHETIC_")
            or value.startswith("TASK-")
            or value in {"SCAFFOLD_ONLY"}
            or "placeholder" in value.lower()
            or "unavailable" in value.lower(),
            "METADATA_NOT_SCAFFOLD_SAFE",
            fixture_id,
        )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    fixture_dir = root / "tests" / "fixtures" / "manual_entry_replay"
    validator = Validator()

    validate_fixture_directory(validator, fixture_dir)
    referenced_files = validate_index(validator, fixture_dir)
    for fixture_id, fixture_name in sorted(referenced_files.items()):
        validate_fixture(validator, fixture_dir / fixture_name, fixture_id)

    validator.require(validator.checks >= 170, "CHECK_COUNT_TOO_LOW", str(validator.checks))
    print(f"STATIC_VALIDATOR=Passed checks={validator.checks}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
