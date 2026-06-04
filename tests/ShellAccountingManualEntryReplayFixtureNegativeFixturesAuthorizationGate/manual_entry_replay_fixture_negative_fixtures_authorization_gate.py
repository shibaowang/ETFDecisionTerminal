#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path


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


def is_negative_fixture_json_path(relative_path: str) -> bool:
    normalized = relative_path.replace("\\", "/")
    filename = normalized.rsplit("/", 1)[-1]
    return (
        normalized.startswith("tests/fixtures/manual_entry_replay_negative/")
        and normalized.endswith(".json")
    ) or (
        normalized.startswith("tests/fixtures/")
        and filename.startswith("NEG_MRF")
        and filename.endswith(".json")
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    doc236_path = root / "docs" / "236_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_gate.md"
    doc237_path = root / "docs" / "237_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_test_plan.md"
    tests_cmake_path = root / "tests" / "CMakeLists.txt"
    test_dir = root / "tests" / "ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate"
    test_cmake_path = test_dir / "CMakeLists.txt"
    test_py_path = test_dir / "manual_entry_replay_fixture_negative_fixtures_authorization_gate.py"
    transport_cmake_path = root / "tests" / "Transport" / "CMakeLists.txt"
    task219_validator_path = root / "tests" / "ShellAccountingManualEntryReplayFixtureStaticValidator" / "manual_entry_replay_fixture_static_validator.py"
    negative_dir = root / "tests" / "fixtures" / "manual_entry_replay_negative"

    required_paths = [
        root / "README.md",
        root / "docs" / "README.md",
        root / "docs" / "12_codex_prompt_template.md",
        doc236_path,
        doc237_path,
        tests_cmake_path,
        test_cmake_path,
        test_py_path,
        transport_cmake_path,
        task219_validator_path,
    ]
    for path in required_paths:
        gate.require(path.exists(), f"required file exists: {path.relative_to(root).as_posix()}")

    retained_paths = [
        "docs/222_shell_accounting_manual_entry_replay_policy_authorization_gate.md",
        "docs/223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md",
        "docs/224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md",
        "docs/225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md",
        "docs/226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md",
        "docs/227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md",
        "docs/228_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_gate.md",
        "docs/229_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_test_plan.md",
        "docs/230_shell_accounting_manual_entry_replay_fixture_files_scaffold.md",
        "docs/231_shell_accounting_manual_entry_replay_fixture_files_scaffold_test_plan.md",
        "docs/232_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_gate.md",
        "docs/233_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_test_plan.md",
        "docs/234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md",
        "docs/235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/CMakeLists.txt",
    ]
    for path in retained_paths:
        gate.require((root / path).exists(), f"retained replay gate path exists: {path}")

    broker_gate_dirs = [
        "tests/ShellAccountingBrokerAdapterDisabledWiring",
        "tests/ShellAccountingBrokerOrderAuthorizationGate",
        "tests/ShellAccountingBrokerOrderImplementation",
        "tests/ShellAccountingRealBrokerOrderAuthorizationGate",
        "tests/ShellAccountingRealBrokerOrderImplementationGate",
    ]
    for directory in broker_gate_dirs:
        gate.require((root / directory).exists(), f"broker gate directory exists: {directory}")

    positive_fixture_files = [
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json",
        "tests/fixtures/manual_entry_replay/MRF002_single_buy.json",
        "tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json",
        "tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json",
    ]
    for path in positive_fixture_files:
        gate.require((root / path).exists(), f"positive fixture retained: {path}")

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc236 = read(doc236_path)
    doc237 = read(doc237_path)
    tests_cmake = read(tests_cmake_path)
    test_cmake = read(test_cmake_path)
    test_py = read(test_py_path)
    transport_cmake = read(transport_cmake_path)

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        gate.contains(text, "TASK-220", context)
        gate.contains(
            text,
            "236_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_gate.md",
            context,
        )
        gate.contains(
            text,
            "237_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_test_plan.md",
            context,
        )

    gate.contains(
        tests_cmake,
        "ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate",
        "tests/CMakeLists",
    )
    gate.contains(
        test_cmake,
        "shell_accounting_manual_entry_replay_fixture_negative_fixtures_authorization",
        "TASK-220 CTest",
    )
    gate.contains(transport_cmake, "transport_local_socket_echo", "transport CTest")
    gate.contains(test_py, "git_lines(root, \"ls-files\", \"--others\", \"--exclude-standard\")", "TASK-220 untracked detection")

    for section in [
        "# ShellAccounting Manual Entry Replay Fixture Static Validator Negative Fixtures Authorization Gate",
        "## Negative Fixtures Authorization Purpose",
        "## Future Negative Fixture Directory Boundary",
        "## Future Negative Fixture Naming Policy",
        "## Future Negative Fixture Schema Policy",
        "## Future Negative Fixture Case Matrix",
        "## Future Negative Expected Issue Codes",
        "## Static Validator Negative-Case Boundary",
        "## Privacy And Sanitization Boundary",
        "## No-Replay Boundary",
        "## No-Runtime-Dependency Boundary",
        "## Out-of-Scope Boundaries",
        "## Formal Authorization Conclusion And Next Task",
    ]:
        gate.contains(doc236, section, "docs/236")

    for token in [
        "TASK-220 is gate-only / negative-fixtures-authorization-only.",
        "TASK-219 static validator remains unchanged",
        "TASK-220 does not create that directory.",
        "tests/fixtures/manual_entry_replay_negative/",
        "test-only",
        "must not be used as production seed data",
        "Extra JSON files must fail closed.",
        "structured schema",
        "must not be raw payload escape hatches",
        "one issue target per fixture",
        "sanitized issue codes only",
        "safe short reason",
        "must not generate concrete replay output",
        "must not run replay",
        "must not call AccountingEngine",
        "must not read SQLite",
        "must not write SQLite",
        "must not write audit",
        "must not write ledger",
        "must not write snapshots",
        "must not start services",
        "must not connect sockets",
        "must not call DataService actions",
        "Negative fixtures are meant to prove fail-closed static validation behavior",
    ]:
        gate.contains(doc236, token, "docs/236")

    negative_names = [
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
    ]
    for token in negative_names:
        gate.contains(doc236, token, "docs/236 negative fixture names")

    schema_fields = [
        "negativeFixtureId",
        "title",
        "purpose",
        "targetValidatorRule",
        "expectedIssueCode",
        "expectedSeverity",
        "expectedBlocking",
        "mutatedFile",
        "mutationDescription",
        "inputFixture",
        "privacyExpectations",
        "metadata",
    ]
    for token in schema_fields:
        gate.contains(doc236, token, "docs/236 schema fields")

    case_matrix = [
        "missing required field",
        "wrong schema version",
        "fixture id mismatch",
        "duplicate fixture id",
        "unauthorized negative fixture file",
        "unauthorized fixture path",
        "`runtimeUse=true`",
        "`productionUse=true`",
        "`replayExecuted=true`",
        "`containsSyntheticDataOnly=false`",
        "forbidden token",
        "raw SQL token",
        "credential token",
        "endpoint token",
        "broker payload token",
        "real order token",
        "DB artifact present",
        "production path reference",
    ]
    for token in case_matrix:
        gate.contains(doc236, token, "docs/236 case matrix")

    issue_codes = [
        "NEG_FIXTURE_MISSING_REQUIRED_FIELD",
        "NEG_FIXTURE_SCHEMA_VERSION_INVALID",
        "NEG_FIXTURE_ID_MISMATCH",
        "NEG_FIXTURE_DUPLICATE_ID",
        "NEG_FIXTURE_UNAUTHORIZED_FILE",
        "NEG_FIXTURE_UNAUTHORIZED_PATH",
        "NEG_FIXTURE_RUNTIME_USE_TRUE",
        "NEG_FIXTURE_PRODUCTION_USE_TRUE",
        "NEG_FIXTURE_REPLAY_EXECUTED_TRUE",
        "NEG_FIXTURE_SYNTHETIC_FLAG_FALSE",
        "NEG_FIXTURE_FORBIDDEN_TOKEN",
        "NEG_FIXTURE_RAW_SQL_TOKEN",
        "NEG_FIXTURE_CREDENTIAL_TOKEN",
        "NEG_FIXTURE_ENDPOINT_TOKEN",
        "NEG_FIXTURE_BROKER_PAYLOAD_TOKEN",
        "NEG_FIXTURE_REAL_ORDER_TOKEN",
        "NEG_FIXTURE_DB_ARTIFACT_PRESENT",
        "NEG_FIXTURE_PRODUCTION_PATH_REFERENCE",
    ]
    for token in issue_codes:
        gate.contains(doc236, token, "docs/236 issue codes")

    privacy_tokens = [
        "raw SQL",
        "raw UI payload",
        "raw DataService payload",
        "full trade_log payload",
        "full cash_adjustment payload",
        "full snapshot payload",
        "broker payload",
        "credentials",
        "endpoint values",
        "access tokens",
        "secret keys",
        "private keys",
        "passwords",
        "real account ids",
        "real order ids",
        "real broker order ids",
        "internal stack traces",
        "production database paths",
        "SYNTHETIC_*",
        "SANITIZED_MEMO",
        "TODO_PLACEHOLDER",
        "UNAVAILABLE_EXPECTED",
    ]
    for token in privacy_tokens:
        gate.contains(doc236, token, "docs/236 privacy")

    for token in [
        "TASK-220 authorizes replay fixture static validator negative fixture policy only.",
        "TASK-220 does not authorize creation of negative fixture files.",
        "TASK-220 does not authorize negative validator implementation.",
        "TASK-220 does not authorize parser, loader, or reader implementation.",
        "TASK-220 does not authorize replay implementation.",
        "TASK-220 does not authorize AccountingEngine replay calls.",
        "TASK-220 does not authorize runtime SQL / SQLite read/write.",
        "TASK-220 does not authorize audit / ledger writes.",
        "TASK-220 does not authorize snapshot writes.",
        "TASK-220 does not authorize backup/export/restore.",
        "TASK-220 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
        "Recommended next task: TASK-221 manual entry replay fixture negative fixtures scaffold authorization gate.",
    ]:
        gate.contains(doc236, token, "docs/236 conclusion")

    for section in [
        "# ShellAccounting Manual Entry Replay Fixture Static Validator Negative Fixtures Authorization Test Plan",
        "## Document Purpose",
        "## Test Matrix",
        "## Required Probes",
        "## Go / No-Go Checklist",
    ]:
        gate.contains(doc237, section, "docs/237")

    for token in [
        "docs/236 exists",
        "docs/237 exists",
        "README references TASK-220",
        "docs/README references docs/236 / docs/237",
        "docs/12 references TASK-220",
        "gate-only",
        "no negative fixture creation",
        "no negative validator implementation",
        "no parser / loader / reader implementation",
        "no replay implementation",
        "no AccountingEngine replay call",
        "no runtime SQL / SQLite read/write",
        "no audit / ledger / snapshot writes",
        "no backup/export/restore",
        "no broker / network / credentials / endpoint / real order / automatic trading",
        "Future negative fixture directory boundary",
        "Future negative fixture naming policy",
        "NEG_MRF001 through NEG_MRF010",
        "Future negative fixture schema policy",
        "Required schema fields",
        "Future negative fixture case matrix",
        "Future negative expected issue codes",
        "Static validator negative-case boundary",
        "Privacy and sanitization boundary",
        "No-replay boundary",
        "No-runtime-dependency boundary",
        "Formal next task is TASK-221",
        "Changed-path allowlist",
        "Untracked-file detection",
        "No negative fixture directory probe",
        "No negative fixture JSON probe",
        "Existing positive fixture JSON unchanged probe",
        "TASK-219 validator script unchanged probe",
        "No parser / loader / reader implementation scan",
        "No replay implementation scan",
        "No AccountingEngine replay call scan",
        "No runtime SQL / SQLite read/write scan",
        "No audit / ledger / snapshot write scan",
        "No backup/export/restore scan",
        "No production code change scan",
        "No QML / startup / Presenter / Controller / ShellServices scan",
        "No DataServiceActions / DataServiceActionRegistrar scan",
        "No DataAccess repository change scan",
        "No migration change scan",
        "No broker / network / credentials / endpoint scan",
        "No real order / automatic trading scan",
        "TASK-219 static validator regression",
        "TASK-218 static validator authorization regression",
        "TASK-217 fixture scaffold regression",
        "TASK-216 through TASK-212 replay policy regressions",
        "transport_local_socket_echo 50-repeat regression",
        "Go only if",
        "No-Go if",
    ]:
        gate.contains(doc237, token, "docs/237")

    allowed_changes = {
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/236_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_gate.md",
        "docs/237_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_test_plan.md",
        "tests/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
    }
    task221_allowed_changes = {
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/238_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_gate.md",
        "docs/239_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_test_plan.md",
        "tests/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
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
    }
    task222_negative_fixture_paths = {
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
    task222_allowed_changes = {
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md",
        "docs/241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md",
        "tests/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
    } | task222_negative_fixture_paths
    task223_allowed_changes = {
        "docs/242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md",
        "docs/243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
    }
    task224_allowed_changes = {
        "docs/244_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_gate.md",
        "docs/245_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
    }
    task225_allowed_changes = {
        "docs/246_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.md",
        "docs/247_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
    }
    allowed_changes = (
        allowed_changes
        | task221_allowed_changes
        | task222_allowed_changes
        | task223_allowed_changes
        | task224_allowed_changes
        | task225_allowed_changes
    )
    gate.require(all(not path.endswith("/") for path in allowed_changes), "TASK-220 allowlist must be exact file paths")
    gate.require(all("*" not in path for path in allowed_changes), "TASK-220 allowlist must not use wildcards")
    changes = changed_paths(root)
    unexpected = sorted(path for path in changes if path not in allowed_changes)
    gate.require(not unexpected, "TASK-220 changed unauthorized paths: " + ", ".join(unexpected))

    if changes:
        gate.require(
            changes.issubset(allowed_changes),
            "TASK-220 changed paths must stay within an exact negative fixtures authorization allowlist",
        )

    fixture_json_changes = sorted(
        path for path in changes if path.startswith("tests/fixtures/manual_entry_replay/") and path.endswith(".json")
    )
    gate.require(not fixture_json_changes, "TASK-220 must not modify positive fixture JSON files: " + ", ".join(fixture_json_changes))

    if negative_dir.exists():
        actual_negative_files = {
            f"tests/fixtures/manual_entry_replay_negative/{child.name}"
            for child in negative_dir.iterdir()
            if child.is_file()
        }
        gate.require(
            actual_negative_files == task222_negative_fixture_paths,
            "TASK-220 permits only TASK-222 exact negative fixture scaffold files: "
            + ", ".join(sorted(actual_negative_files)),
        )
    negative_json_changes = sorted(
        path for path in changes if is_negative_fixture_json_path(path) and path not in task222_negative_fixture_paths
    )
    gate.require(
        not negative_json_changes,
        "TASK-220 must not create unauthorized negative fixture JSON files: " + ", ".join(negative_json_changes),
    )

    gate.require(
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/manual_entry_replay_fixture_static_validator.py" not in changes,
        "TASK-220 must not modify TASK-219 static validator script",
    )

    forbidden_prefixes = [
        "apps/",
        "libs/",
        "migrations/",
    ]
    production_changes = sorted(path for path in changes if any(path.startswith(prefix) for prefix in forbidden_prefixes))
    gate.require(not production_changes, "TASK-220 must not modify production/runtime/schema paths: " + ", ".join(production_changes))

    forbidden_implementation_markers = [
        "parser",
        "loader",
        "reader",
        "replay_runner",
        "fixture_reader",
        "runtime_output",
        "runtime-output",
    ]
    unauthorized_implementation_changes = sorted(
        path
        for path in changes
        if path not in allowed_changes and any(marker in path.lower() for marker in forbidden_implementation_markers)
    )
    gate.require(
        not unauthorized_implementation_changes,
        "TASK-220 must not add parser/loader/reader/replay implementation paths: "
        + ", ".join(unauthorized_implementation_changes),
    )

    for token in [
        "AccountingReplayEngine" + "::replay",
        "AccountingEngine" + "::replay",
        "sqlite3" + "_exec",
        "INSERT" + " INTO",
        "UPDATE" + " ",
        "DELETE" + " FROM",
        "REPLACE" + " INTO",
        "backup" + " export" + " restore" + " implementation",
    ]:
        gate.not_contains(test_py, token, "TASK-220 gate script")

    gate.require(gate.checks >= 120, f"TASK-220 gate must keep at least 120 checks, got {gate.checks}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
