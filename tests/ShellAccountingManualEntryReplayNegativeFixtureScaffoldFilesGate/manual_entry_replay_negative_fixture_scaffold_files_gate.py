#!/usr/bin/env python3

import argparse
import json
import re
import subprocess
from pathlib import Path
from typing import Any


EXPECTED_ISSUE_CODES = {
    "NEG_MRF001": "NEG_FIXTURE_MISSING_REQUIRED_FIELD",
    "NEG_MRF002": "NEG_FIXTURE_SCHEMA_VERSION_INVALID",
    "NEG_MRF003": "NEG_FIXTURE_RUNTIME_USE_TRUE",
    "NEG_MRF004": "NEG_FIXTURE_PRODUCTION_USE_TRUE",
    "NEG_MRF005": "NEG_FIXTURE_REPLAY_EXECUTED_TRUE",
    "NEG_MRF006": "NEG_FIXTURE_SYNTHETIC_FLAG_FALSE",
    "NEG_MRF007": "NEG_FIXTURE_UNAUTHORIZED_FILE",
    "NEG_MRF008": "NEG_FIXTURE_RAW_SQL_TOKEN",
    "NEG_MRF009": "NEG_FIXTURE_BROKER_PAYLOAD_TOKEN",
    "NEG_MRF010": "NEG_FIXTURE_REAL_ORDER_TOKEN",
}

EXPECTED_FILES = {
    "negative_fixtures_index.json",
    "NEG_MRF001_missing_required_field.json",
    "NEG_MRF002_wrong_schema_version.json",
    "NEG_MRF003_runtime_use_true.json",
    "NEG_MRF004_production_use_true.json",
    "NEG_MRF005_replay_executed_true.json",
    "NEG_MRF006_non_synthetic_privacy.json",
    "NEG_MRF007_extra_json_file.json",
    "NEG_MRF008_forbidden_token.sql.json",
    "NEG_MRF009_broker_payload_token.json",
    "NEG_MRF010_real_order_id_token.json",
}

EXPECTED_NEGATIVE_FILES = EXPECTED_FILES - {"negative_fixtures_index.json"}

ALLOWED_CHANGED_PATHS = {
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md",
    "docs/241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md",
    "docs/242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md",
    "docs/243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md",
    "docs/244_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_gate.md",
    "docs/245_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_test_plan.md",
    "docs/246_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.md",
    "docs/247_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_test_plan.md",
    "docs/248_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.md",
    "docs/249_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_test_plan.md",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
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
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
    "tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json",
    "tests/fixtures/manual_entry_replay_negative/NEG_MRF001_missing_required_field.json",
    "tests/fixtures/manual_entry_replay_negative/NEG_MRF002_wrong_schema_version.json",
    "tests/fixtures/manual_entry_replay_negative/NEG_MRF003_runtime_use_true.json",
    "tests/fixtures/manual_entry_replay_negative/NEG_MRF004_production_use_true.json",
    "tests/fixtures/manual_entry_replay_negative/NEG_MRF005_replay_executed_true.json",
    "tests/fixtures/manual_entry_replay_negative/NEG_MRF006_non_synthetic_privacy.json",
    "tests/fixtures/manual_entry_replay_negative/NEG_MRF007_extra_json_file.json",
    "tests/fixtures/manual_entry_replay_negative/NEG_MRF008_forbidden_token.sql.json",
    "tests/fixtures/manual_entry_replay_negative/NEG_MRF009_broker_payload_token.json",
    "tests/fixtures/manual_entry_replay_negative/NEG_MRF010_real_order_id_token.json",
}

FORBIDDEN_STORAGE_SUFFIXES = {".db", ".sqlite", ".sqlite3", ".wal", ".shm"}

FORBIDDEN_JSON_TOKENS = {
    "select *",
    "insert into",
    "update ",
    "delete from",
    "drop table",
    "sqlite://",
    "jdbc:",
    "api_key",
    "access token",
    "secret key",
    "private key",
    "password",
    "credential value",
    "https://",
    "http://",
    "real account",
    "real broker",
    "stack trace",
}

FORBIDDEN_CHANGED_PREFIXES = (
    "apps/",
    "libs/",
    "migrations/",
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
        self.require(
            token in text or normalized_token in normalized_text,
            f"{context} missing `{token}`",
        )

    def not_contains(self, text: str, token: str, context: str) -> None:
        self.require(token not in text, f"{context} must not contain `{token}`")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def load_json(path: Path) -> Any:
    return json.loads(read(path))


def git_lines(root: Path, *args: str) -> set[str]:
    completed = subprocess.run(
        ["git", *args],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def changed_paths(root: Path) -> set[str]:
    changed = git_lines(root, "diff", "--name-only", "main")
    staged = git_lines(root, "diff", "--cached", "--name-only")
    untracked = git_lines(root, "ls-files", "--others", "--exclude-standard")
    return changed | staged | untracked


def negative_fixture_id_from_file(name: str) -> str:
    match = re.match(r"^(NEG_MRF\d{3})_.+\.json$", name)
    if not match:
        return ""
    return match.group(1)


def require_bool(gate: Gate, payload: dict[str, Any], field: str, expected: bool, context: str) -> None:
    gate.require(field in payload, f"{context}.{field} missing")
    gate.require(payload[field] is expected, f"{context}.{field} must be {expected}")


def require_string(gate: Gate, payload: dict[str, Any], field: str, context: str) -> str:
    gate.require(field in payload, f"{context}.{field} missing")
    value = payload[field]
    gate.require(isinstance(value, str) and bool(value.strip()), f"{context}.{field} must be non-empty string")
    return value


def require_no_forbidden_json_tokens(gate: Gate, text: str, context: str) -> None:
    lowered = text.lower()
    for token in FORBIDDEN_JSON_TOKENS:
        gate.require(token not in lowered, f"{context} contains forbidden token `{token}`")
    for production_path in ["apps/", "libs/", "migrations/"]:
        gate.require(production_path not in text, f"{context} contains production path `{production_path}`")


def validate_index(gate: Gate, index_path: Path, positive_fixture_ids: set[str]) -> list[str]:
    text = read(index_path)
    require_no_forbidden_json_tokens(gate, text, index_path.name)
    payload = load_json(index_path)
    gate.require(isinstance(payload, dict), "negative_fixtures_index.json must be object")
    gate.require(
        payload.get("schemaVersion") == "manual-entry-replay-negative-fixtures-index/v1",
        "index schemaVersion invalid",
    )
    gate.require(payload.get("fixtureSet") == "manual_entry_replay_negative_scaffold", "index fixtureSet invalid")
    gate.require(payload.get("generatedByTask") == "TASK-222", "index generatedByTask invalid")
    require_bool(gate, payload, "scaffoldOnly", True, "index")
    require_bool(gate, payload, "runtimeUse", False, "index")
    require_bool(gate, payload, "productionUse", False, "index")
    require_bool(gate, payload, "replayExecuted", False, "index")
    require_bool(gate, payload, "containsSyntheticDataOnly", True, "index")

    fixtures = payload.get("fixtures")
    gate.require(isinstance(fixtures, list), "index fixtures must be list")
    gate.require(len(fixtures) == 10, "index must reference 10 negative fixtures")

    seen_ids: set[str] = set()
    seen_files: set[str] = set()
    for row in fixtures:
        gate.require(isinstance(row, dict), "index row must be object")
        fixture_id = require_string(gate, row, "negativeFixtureId", "index row")
        fixture_file = require_string(gate, row, "file", fixture_id)
        source_id = require_string(gate, row, "sourcePositiveFixtureId", fixture_id)
        expected_issue = require_string(gate, row, "expectedIssueCode", fixture_id)
        gate.require(fixture_id in EXPECTED_ISSUE_CODES, f"unexpected negative fixture id in index: {fixture_id}")
        gate.require(fixture_id not in seen_ids, f"duplicate negative fixture id in index: {fixture_id}")
        gate.require(fixture_file in EXPECTED_NEGATIVE_FILES, f"unexpected negative fixture file in index: {fixture_file}")
        gate.require(fixture_file not in seen_files, f"duplicate negative fixture file in index: {fixture_file}")
        gate.require(negative_fixture_id_from_file(fixture_file) == fixture_id, f"index file/id mismatch: {fixture_file}")
        gate.require(source_id in positive_fixture_ids, f"index sourcePositiveFixtureId missing in positive index: {source_id}")
        gate.require(expected_issue == EXPECTED_ISSUE_CODES[fixture_id], f"index expectedIssueCode mismatch: {fixture_id}")
        require_bool(gate, row, "scaffoldOnly", True, fixture_id)
        require_bool(gate, row, "runtimeUse", False, fixture_id)
        require_bool(gate, row, "productionUse", False, fixture_id)
        require_bool(gate, row, "replayExecuted", False, fixture_id)
        require_bool(gate, row, "containsSyntheticDataOnly", True, fixture_id)
        seen_ids.add(fixture_id)
        seen_files.add(fixture_file)

    gate.require(seen_ids == set(EXPECTED_ISSUE_CODES), "index negative fixture ids incomplete")
    gate.require(seen_files == EXPECTED_NEGATIVE_FILES, "index negative fixture files incomplete")
    return sorted(seen_files)


def validate_negative_fixture(gate: Gate, fixture_path: Path, positive_fixture_ids: set[str]) -> None:
    text = read(fixture_path)
    require_no_forbidden_json_tokens(gate, text, fixture_path.name)
    payload = load_json(fixture_path)
    gate.require(isinstance(payload, dict), f"{fixture_path.name} must be object")

    expected_id = negative_fixture_id_from_file(fixture_path.name)
    gate.require(expected_id in EXPECTED_ISSUE_CODES, f"unexpected negative fixture file name: {fixture_path.name}")
    gate.require(
        payload.get("schemaVersion") == "manual-entry-replay-negative-fixture-scaffold/v1",
        f"{expected_id} schemaVersion invalid",
    )
    gate.require(payload.get("negativeFixtureId") == expected_id, f"{expected_id} negativeFixtureId mismatch")

    for field in [
        "title",
        "purpose",
        "sourcePositiveFixtureId",
        "mutationCategory",
        "targetValidatorRule",
        "expectedIssueCode",
        "expectedSeverity",
        "mutationDescription",
    ]:
        require_string(gate, payload, field, expected_id)

    gate.require(payload["sourcePositiveFixtureId"] in positive_fixture_ids, f"{expected_id} source fixture missing")
    gate.require(payload["expectedIssueCode"] == EXPECTED_ISSUE_CODES[expected_id], f"{expected_id} issue mismatch")
    gate.require(payload["expectedSeverity"] == "error", f"{expected_id} severity must be error")
    gate.require(payload.get("expectedBlocking") is True, f"{expected_id} expectedBlocking must be true")
    require_bool(gate, payload, "scaffoldOnly", True, expected_id)
    require_bool(gate, payload, "runtimeUse", False, expected_id)
    require_bool(gate, payload, "productionUse", False, expected_id)
    require_bool(gate, payload, "replayExecuted", False, expected_id)
    require_bool(gate, payload, "containsSyntheticDataOnly", True, expected_id)

    for list_field in ["privacyExpectations", "sanitizationExpectations"]:
        value = payload.get(list_field)
        gate.require(isinstance(value, list) and bool(value), f"{expected_id}.{list_field} must be non-empty list")
        for item in value:
            gate.require(isinstance(item, str) and item.strip(), f"{expected_id}.{list_field} contains invalid value")

    metadata = payload.get("metadata")
    gate.require(isinstance(metadata, dict), f"{expected_id}.metadata must be object")
    gate.require(metadata.get("task") == "TASK-222", f"{expected_id}.metadata.task invalid")
    gate.require(metadata.get("notReplayInput") is True, f"{expected_id}.metadata.notReplayInput invalid")
    gate.require(metadata.get("notProductionInput") is True, f"{expected_id}.metadata.notProductionInput invalid")
    gate.require(metadata.get("notRuntimeDataSource") is True, f"{expected_id}.metadata.notRuntimeDataSource invalid")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    doc240_path = root / "docs" / "240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md"
    doc241_path = root / "docs" / "241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md"
    tests_cmake_path = root / "tests" / "CMakeLists.txt"
    test_dir = root / "tests" / "ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate"
    test_cmake_path = test_dir / "CMakeLists.txt"
    test_py_path = test_dir / "manual_entry_replay_negative_fixture_scaffold_files_gate.py"
    negative_dir = root / "tests" / "fixtures" / "manual_entry_replay_negative"
    positive_index_path = root / "tests" / "fixtures" / "manual_entry_replay" / "fixtures_index.json"
    task219_validator_path = root / "tests" / "ShellAccountingManualEntryReplayFixtureStaticValidator" / "manual_entry_replay_fixture_static_validator.py"

    for path in [
        root / "README.md",
        root / "docs" / "README.md",
        root / "docs" / "12_codex_prompt_template.md",
        doc240_path,
        doc241_path,
        tests_cmake_path,
        test_cmake_path,
        test_py_path,
        positive_index_path,
        task219_validator_path,
    ]:
        gate.require(path.exists(), f"required path exists: {path.relative_to(root).as_posix()}")

    for retained in [
        "docs/238_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_gate.md",
        "docs/239_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/CMakeLists.txt",
    ]:
        gate.require((root / retained).exists(), f"retained prior gate path exists: {retained}")

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc240 = read(doc240_path)
    doc241 = read(doc241_path)
    tests_cmake = read(tests_cmake_path)
    test_cmake = read(test_cmake_path)
    test_py = read(test_py_path)

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        gate.contains(text, "TASK-222", context)
        gate.contains(
            text,
            "240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md",
            context,
        )
        gate.contains(
            text,
            "241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md",
            context,
        )

    gate.contains(
        tests_cmake,
        "ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate",
        "tests/CMakeLists",
    )
    gate.contains(
        test_cmake,
        "shell_accounting_manual_entry_replay_negative_fixture_scaffold_files",
        "TASK-222 CTest",
    )
    gate.contains(test_py, 'git_lines(root, "ls-files", "--others", "--exclude-standard")', "untracked detection")

    for section in [
        "# ShellAccounting Manual Entry Replay Negative Fixture Scaffold Files Gate",
        "## Purpose",
        "## Relationship To TASK-220 And TASK-221",
        "## Created Test-Only Scaffold Files",
        "## Negative Fixture Index Contract",
        "## Negative Fixture JSON Contract",
        "## Case Matrix",
        "## Expected Issue Code Mapping",
        "## Privacy And Sanitization Boundary",
        "## No-Replay Boundary",
        "## No-Runtime-Dependency Boundary",
        "## Out-of-Scope Boundaries",
        "## Formal Conclusion And Next Task",
    ]:
        gate.contains(doc240, section, "docs/240")

    for token in [
        "TASK-222 creates test-only negative fixture scaffold JSON files.",
        "TASK-222 does not implement negative validator.",
        "TASK-222 does not implement parser, loader, reader.",
        "TASK-222 does not implement replay.",
        "TASK-222 does not call AccountingEngine replay.",
        "TASK-222 does not modify positive fixture JSON.",
        "TASK-222 does not modify production code.",
        "TASK-222 does not modify QML / DataServiceActions / repositories / migrations.",
        "TASK-222 does not write runtime SQL / SQLite / audit / ledger / snapshot.",
        "TASK-222 does not connect broker / network / credentials / endpoint.",
        "TASK-222 does not place real orders or enable automatic trading.",
        "Recommended next task: TASK-223 manual entry replay negative fixture static validator authorization gate.",
    ]:
        gate.contains(doc240, token, "docs/240")

    for section in ["## Document Purpose", "## Test Matrix", "## Required Probes", "## Go / No-Go Checklist"]:
        gate.contains(doc241, section, "docs/241")

    for token in [
        "docs/240 and docs/241 exist",
        "TASK-222 CTest is registered",
        "negative fixture directory exists and contains only 11 JSON files",
        "index exactly references 10 `NEG_MRF*.json` files",
        "sourcePositiveFixtureId",
        "Existing positive fixture JSON is not modified",
        "TASK-219 static validator is not modified",
        "No negative validator / parser / loader / reader / replay implementation",
        "No AccountingEngine replay call",
        "No production code / QML / DataServiceActions / repositories / migrations / broker",
    ]:
        gate.contains(doc241, token, "docs/241")

    for filename in EXPECTED_FILES:
        gate.contains(doc240, filename, "docs/240 file set")

    for fixture_id, issue_code in EXPECTED_ISSUE_CODES.items():
        gate.contains(doc240, f"{fixture_id} -> {issue_code}", "docs/240 issue mapping")

    changed = changed_paths(root)
    unexpected_changes = sorted(changed - ALLOWED_CHANGED_PATHS)
    gate.require(not unexpected_changes, f"unexpected changed paths: {unexpected_changes}")
    for path in changed:
        gate.require(not path.startswith(FORBIDDEN_CHANGED_PREFIXES), f"production path changed: {path}")
        gate.require(not path.startswith("tests/fixtures/manual_entry_replay/"), f"positive fixture changed: {path}")
        gate.require(path != task219_validator_path.relative_to(root).as_posix(), "TASK-219 static validator changed")

    gate.require(negative_dir.exists(), "negative fixture directory must exist")
    gate.require(negative_dir.is_dir(), "negative fixture path must be directory")
    children = list(negative_dir.iterdir())
    gate.require(all(child.is_file() for child in children), "negative fixture directory must contain files only")
    actual_files = {child.name for child in children}
    gate.require(actual_files == EXPECTED_FILES, f"negative fixture directory exact file set mismatch: {actual_files}")
    gate.require(len(actual_files) == 11, "negative fixture directory must contain 11 JSON files")
    gate.require(all(child.suffix.lower() == ".json" for child in children), "negative fixture directory only JSON")
    gate.require(
        not any(child.suffix.lower() in FORBIDDEN_STORAGE_SUFFIXES for child in children),
        "negative fixture directory must not contain DB/WAL/SHM",
    )

    positive_index = load_json(positive_index_path)
    positive_fixtures = positive_index.get("fixtures")
    gate.require(isinstance(positive_fixtures, list), "positive fixture index must contain fixtures list")
    positive_ids = {row.get("fixtureId") for row in positive_fixtures if isinstance(row, dict)}
    gate.require({"MRF001", "MRF002", "MRF003", "MRF004", "MRF005", "MRF006"}.issubset(positive_ids), "positive ids incomplete")

    referenced_files = validate_index(gate, negative_dir / "negative_fixtures_index.json", positive_ids)
    gate.require(set(referenced_files) == EXPECTED_NEGATIVE_FILES, "referenced files mismatch")

    seen_fixture_ids: set[str] = set()
    for filename in sorted(EXPECTED_NEGATIVE_FILES):
        fixture_path = negative_dir / filename
        validate_negative_fixture(gate, fixture_path, positive_ids)
        fixture_id = negative_fixture_id_from_file(filename)
        gate.require(fixture_id not in seen_fixture_ids, f"duplicate fixture id: {fixture_id}")
        seen_fixture_ids.add(fixture_id)

    gate.require(seen_fixture_ids == set(EXPECTED_ISSUE_CODES), "negative fixture ids complete")
    gate.require(gate.checks >= 160, f"expected at least 160 gate checks, got {gate.checks}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
