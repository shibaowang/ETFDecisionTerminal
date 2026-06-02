#!/usr/bin/env python3

import argparse
import re
import subprocess
from pathlib import Path


ALLOWED_DIFF_PREFIXES = {
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/178_shell_accounting_manual_entry_schema_migration_implementation.md",
    "docs/179_shell_accounting_manual_entry_schema_migration_implementation_test_plan.md",
    "docs/180_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_gate.md",
    "docs/181_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_test_plan.md",
    "docs/182_shell_accounting_manual_transaction_repository_write_implementation.md",
    "docs/183_shell_accounting_manual_transaction_repository_write_implementation_test_plan.md",
    "docs/184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md",
    "docs/185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md",
    "libs/DataAccess/CMakeLists.txt",
    "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
    "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
    "tests/CMakeLists.txt",
    "tests/DevDocs/test_readonly_demo_acceptance.py",
    "tests/ShellAccountingManualEntryDataServiceActionAuthorizationGate/manual_entry_dataservice_action_authorization_gate.cpp",
    "tests/ShellAccountingManualEntryDataServiceActionImplementationAuthorizationGate/manual_entry_dataservice_action_implementation_authorization_gate.cpp",
    "tests/ShellAccountingManualEntryDataServiceActionScaffold/manual_entry_dataservice_action_scaffold.cpp",
    "tests/ShellAccountingManualEntryPersistenceAuthorizationGate/manual_entry_persistence_authorization_gate.cpp",
    "tests/ShellAccountingManualEntryRepositoryScaffoldAuthorizationGate/manual_entry_repository_scaffold_authorization_gate.cpp",
    "tests/ShellAccountingManualTransactionCashMovementMvpAuthorizationGate/manual_transaction_cash_movement_mvp_authorization_gate.cpp",
    "tests/ShellAccountingManualTransactionCashMovementValidationScaffold/manual_transaction_cash_movement_validation_scaffold.cpp",
}

ALLOWED_DIFF_DIRS = {
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/",
    "tests/ShellAccountingManualEntrySchemaMigrationImplementation/",
    "tests/ShellAccountingManualEntryRepositoryImplementationAuthorizationGate/",
    "tests/ShellAccountingManualEntryRepositoryScaffold/",
    "tests/ShellAccountingManualEntryDataServiceActionValidationWiring/",
    "tests/ShellAccountingManualTransactionRepositoryWriteImplementation/",
}

TASK190_MIGRATION_TOKENS = [
    "ALTER TABLE trade_log ADD COLUMN request_id TEXT",
    "ALTER TABLE trade_log ADD COLUMN idempotency_key TEXT",
    "ALTER TABLE trade_log ADD COLUMN occurred_at_utc TEXT",
    "ALTER TABLE trade_log ADD COLUMN tax_cents INTEGER NOT NULL DEFAULT 0",
    "ALTER TABLE trade_log ADD COLUMN source_memo_sanitized TEXT",
    "ALTER TABLE trade_log ADD COLUMN cash_adjustment_uid TEXT",
    "ALTER TABLE cash_adjustment ADD COLUMN request_id TEXT",
    "ALTER TABLE cash_adjustment ADD COLUMN idempotency_key TEXT",
    "ALTER TABLE cash_adjustment ADD COLUMN occurred_at_utc TEXT",
    "ALTER TABLE cash_adjustment ADD COLUMN source_memo_sanitized TEXT",
    "ALTER TABLE cash_adjustment ADD COLUMN trade_log_uid TEXT",
    "ALTER TABLE audit_log ADD COLUMN request_id TEXT",
    "ALTER TABLE audit_log ADD COLUMN idempotency_key TEXT",
    "ALTER TABLE audit_log ADD COLUMN payload_classification TEXT",
    "ALTER TABLE audit_log ADD COLUMN redaction_status TEXT",
    "idx_trade_log_manual_request_id",
    "ux_trade_log_manual_idempotency_key",
    "idx_cash_adjustment_manual_request_id",
    "ux_cash_adjustment_manual_idempotency_key",
    "idx_audit_log_manual_request_id",
    "idx_audit_log_manual_idempotency_key",
]


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def require_contains(text: str, token: str, context: str) -> None:
    require(token in text, f"{context} missing `{token}`")


def changed_paths(root: Path) -> set[str]:
    completed = subprocess.run(
        ["git", "diff", "--name-only", "main"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def assert_allowed_diff_only(root: Path) -> None:
    unexpected = []
    for path in changed_paths(root):
        if path in ALLOWED_DIFF_PREFIXES:
            continue
        if any(path.startswith(prefix) for prefix in ALLOWED_DIFF_DIRS):
            continue
        unexpected.append(path)
    require(not unexpected, "unexpected TASK-191 changed paths: " + ", ".join(sorted(unexpected)))


def assert_path_not_changed(root: Path, relative_path: str) -> None:
    path = relative_path.replace("\\", "/")
    require(path not in changed_paths(root), f"{relative_path} must not be changed by TASK-191")


def files_under(root: Path, suffixes=None) -> list[Path]:
    if not root.exists():
        return []
    files = [path for path in root.rglob("*") if path.is_file()]
    if suffixes:
        files = [path for path in files if path.suffix in suffixes]
    return files


def repo_text(root: Path, paths: list[Path]) -> str:
    return "\n".join(read(path) for path in paths if path.exists())


class Harness:
    def __init__(self, root: Path):
        self.root = root

    def doc_path(self, number: int) -> Path:
        names = {
            178: "178_shell_accounting_manual_entry_schema_migration_implementation.md",
            179: "179_shell_accounting_manual_entry_schema_migration_implementation_test_plan.md",
            180: "180_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_gate.md",
            181: "181_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_test_plan.md",
        }
        return self.root / "docs" / names[number]

    def doc(self, number: int) -> str:
        return read(self.doc_path(number))

    @property
    def migration001(self) -> Path:
        return self.root / "migrations" / "001_initial_schema.sql"

    @property
    def migration002(self) -> Path:
        return self.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql"

    @property
    def data_service_actions_cpp(self) -> Path:
        return self.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp"

    @property
    def data_service_actions_h(self) -> Path:
        return self.root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" / "DataServiceActions.h"

    @property
    def registrar(self) -> Path:
        return self.root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp"

    @property
    def scaffold_header(self) -> Path:
        return self.root / "libs" / "DataAccess" / "include" / "DataAccess" / "ShellAccountingManualEntryRepositoryScaffold.h"

    @property
    def scaffold_source(self) -> Path:
        return self.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualEntryRepositoryScaffold.cpp"


def test_gate(h: Harness) -> None:
    assert_allowed_diff_only(h.root)
    require(h.doc_path(180).exists(), "docs/180 exists")
    require(h.doc_path(181).exists(), "docs/181 exists")
    require(h.migration001.exists(), "001 migration exists")
    require(h.migration002.exists(), "002 migration exists")


def test_docs(h: Harness) -> None:
    doc180 = h.doc(180)
    for token in [
        "TASK-191",
        "repository implementation post-migration authorization gate only",
        "TASK-190 completed the manual entry schema migration implementation",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
        "does not implement a repository",
        "does not modify DataAccess production code",
        "does not modify the TASK-185 repository scaffold",
        "does not modify DataServiceActions",
        "does not write SQLite",
        "does not execute runtime SQL",
        "does not write runtime `trade_log` / `cash_adjustment` / `audit_log`",
        "does not add persistent id responses",
        "Future repository implementation must be a separate TASK",
        "DataService action write implementation must remain a separate TASK",
        "DataAccess repository boundary",
        "must not scatter SQL in DataService action handlers",
        "Broker sandbox new capability development remains paused",
    ]:
        require_contains(doc180, token, "docs/180")
    doc181 = h.doc(181)
    for token in [
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "post-migration authorization",
        "Only TASK-192 manual transaction repository implementation is added",
        "No runtime SQL / SQLite write is added",
        "No broker SDK, network, credentials, endpoint, real order, or automatic",
    ]:
        require_contains(doc181, token, "docs/181")


def test_docs_index_prompt(h: Harness) -> None:
    readme = read(h.root / "README.md")
    docs_index = read(h.root / "docs" / "README.md")
    prompt = read(h.root / "docs" / "12_codex_prompt_template.md")
    for token, context in [
        ("docs/180_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_gate.md", "README"),
        ("docs/181_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_test_plan.md", "README"),
        ("180_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_gate.md", "docs/README"),
        ("181_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_test_plan.md", "docs/README"),
        ("TASK-191", "docs/12"),
        ("docs/180 and docs/181", "docs/12"),
    ]:
        text = readme if context == "README" else docs_index if context == "docs/README" else prompt
        require_contains(text, token, context)


def test_task190_migration_exists(h: Harness) -> None:
    require(h.migration002.exists(), "TASK-190 migration exists")
    require_contains(h.doc(180), "TASK-190", "docs/180")


def test_authorization_only(h: Harness) -> None:
    require_contains(h.doc(180), "gate only", "docs/180")
    require_contains(h.doc(181), "authorization gate", "docs/181")


def test_no_repository_current_scope(h: Harness) -> None:
    require_contains(h.doc(180), "TASK-192 implements manual transaction repository write", "docs/180")
    require_contains(h.doc(181), "Only TASK-192 manual transaction repository implementation is added", "docs/181")


def test_no_migration_modification(h: Harness) -> None:
    require_contains(h.doc(180), "does not modify `migrations/001_initial_schema.sql`", "docs/180")
    require_contains(h.doc(180), "does not modify `migrations/002_shell_accounting_manual_entry_schema.sql`", "docs/180")


def test_future_repository_separate_task(h: Harness) -> None:
    require_contains(h.doc(180), "Future repository implementation must be a separate TASK", "docs/180")


def test_future_dataservice_write_separate_task(h: Harness) -> None:
    require_contains(h.doc(180), "DataService action write implementation must remain a separate TASK", "docs/180")


def test_dataaccess_boundary(h: Harness) -> None:
    require_contains(h.doc(180), "DataAccess repository boundary", "docs/180")


def test_no_scattered_sql(h: Harness) -> None:
    require_contains(h.doc(180), "must not scatter SQL in DataService action handlers", "docs/180")


def test_manual_transaction_mapping(h: Harness) -> None:
    doc = h.doc(180)
    for token in [
        "trade_log.request_id",
        "trade_log.idempotency_key",
        "trade_log.occurred_at_utc",
        "trade_log.tax_cents",
        "trade_log.source_memo_sanitized",
        "trade_log.cash_adjustment_uid",
        "trade_log.quantity_1e6",
        "trade_log.price_1e6",
        "trade_log.amount_cents",
        "trade_log.fee_cents",
        "trade_log.net_cash_impact_cents",
        "trade_log.action_type",
        "trade_log.trade_source",
        "trade_log.manual_entry",
    ]:
        require_contains(doc, token, "docs/180 manual transaction mapping")


def test_manual_cash_mapping(h: Harness) -> None:
    doc = h.doc(180)
    for token in [
        "cash_adjustment.request_id",
        "cash_adjustment.idempotency_key",
        "cash_adjustment.occurred_at_utc",
        "cash_adjustment.source_memo_sanitized",
        "cash_adjustment.trade_log_uid",
        "cash_adjustment.amount_cents",
        "cash_adjustment.adjustment_type",
        "trade_log.request_id",
        "trade_log.idempotency_key",
        "trade_log.occurred_at_utc",
        "trade_log.cash_adjustment_uid",
        "trade_log.action_type",
        "trade_log.trade_source",
        "trade_log.manual_entry",
    ]:
        require_contains(doc, token, "docs/180 manual cash mapping")


def test_audit_mapping(h: Harness) -> None:
    doc = h.doc(180)
    for token in [
        "audit_log.request_id",
        "audit_log.idempotency_key",
        "audit_log.payload_classification",
        "audit_log.redaction_status",
        "sanitized payload shape",
        "audit failure policy",
        "transaction boundary between business facts and audit facts",
    ]:
        require_contains(doc, token, "docs/180 audit mapping")


def test_transaction_idempotency_policy(h: Harness) -> None:
    doc = h.doc(180)
    for token in [
        "transaction boundary for manual transaction write",
        "transaction boundary for manual cash movement write",
        "rollback / failure atomicity",
        "idempotency behavior on duplicate idempotency_key",
        "request_id trace behavior",
        "unique index conflict handling",
        "safe error mapping without leaking raw SQL or raw payload",
        "no partial facts on failure",
    ]:
        require_contains(doc, token, "docs/180 transaction policy")


def test_initial_schema_unmodified(h: Harness) -> None:
    assert_path_not_changed(h.root, "migrations/001_initial_schema.sql")


def test_task190_migration_unmodified(h: Harness) -> None:
    assert_path_not_changed(h.root, "migrations/002_shell_accounting_manual_entry_schema.sql")


def test_no_new_migration(h: Harness) -> None:
    migrations = sorted(path.name for path in (h.root / "migrations").glob("*.sql"))
    require("001_initial_schema.sql" in migrations, "001 migration exists")
    require("002_shell_accounting_manual_entry_schema.sql" in migrations, "002 migration exists")
    require(len([name for name in migrations if name.startswith("003")]) == 0, "TASK-191 must not add 003 migration")


def test_no_schema_file(h: Harness) -> None:
    schema_files = [path for path in h.root.rglob("*") if path.is_file() and "schema" in path.name.lower() and path.suffix in {".ddl", ".schema"}]
    require(not schema_files, "TASK-191 must not add schema side files")


def test_task190_columns_indexes_retained(h: Harness) -> None:
    sql = read(h.migration002)
    for token in TASK190_MIGRATION_TOKENS:
        require_contains(sql, token, "TASK-190 migration")


def test_dataserviceactions_cpp_unmodified(h: Harness) -> None:
    assert_path_not_changed(h.root, "libs/DataServiceApi/src/DataServiceActions.cpp")


def test_dataserviceactions_h_unmodified(h: Harness) -> None:
    assert_path_not_changed(h.root, "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h")


def test_registrar_unmodified(h: Harness) -> None:
    assert_path_not_changed(h.root, "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp")


def test_validation_code_unmodified(h: Harness) -> None:
    assert_path_not_changed(h.root, "libs/DataServiceApi/include/DataServiceApi/ShellAccountingManualTransactionCashMovementValidation.h")
    assert_path_not_changed(h.root, "libs/DataServiceApi/src/ShellAccountingManualTransactionCashMovementValidation.cpp")


def test_scaffold_unmodified(h: Harness) -> None:
    assert_path_not_changed(h.root, "libs/DataAccess/include/DataAccess/ShellAccountingManualEntryRepositoryScaffold.h")
    assert_path_not_changed(h.root, "libs/DataAccess/src/ShellAccountingManualEntryRepositoryScaffold.cpp")


def test_scaffold_disabled(h: Harness) -> None:
    header = read(h.scaffold_header)
    source = read(h.scaffold_source)
    for token in [
        "bool success = false",
        "bool writeImplemented = false",
        "bool databaseWritten = false",
        "bool tradeLogWritten = false",
        "bool cashFactsWritten = false",
        "bool auditWritten = false",
        "bool ledgerWritten = false",
    ]:
        require_contains(header, token, "TASK-185 scaffold header")
    require_contains(source, "disabledResult()", "TASK-185 scaffold source")


def test_task182_validation_only(h: Harness) -> None:
    actions = read(h.data_service_actions_cpp)
    require_contains(actions, "writeImplemented", "DataServiceActions validation wiring")
    require_contains(actions, "manualEntryEnabled", "DataServiceActions validation wiring")


def test_no_repository_implementation(h: Harness) -> None:
    header = h.root / "libs" / "DataAccess" / "include" / "DataAccess" / "ShellAccountingManualTransactionRepository.h"
    source = h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualTransactionRepository.cpp"
    require(header.exists(), "TASK-192 manual transaction repository header exists")
    require(source.exists(), "TASK-192 manual transaction repository source exists")
    text = read(header) + "\n" + read(source)
    require_contains(text, "ShellAccountingManualTransactionRepository", "TASK-192 repository")
    require("ManualCashMovementWriteRepository" not in text, "manual cash movement repository implementation must not exist")


def test_no_runtime_sql(h: Harness) -> None:
    forbidden = subprocess.run(
        [
            "git",
            "diff",
            "main",
            "--",
            "libs/DataServiceApi",
            "apps",
            "libs/ShellServices",
            "libs/ShellCore",
        ],
        cwd=h.root,
        check=True,
        capture_output=True,
        text=True,
    ).stdout
    require(re.search(r"\b(INSERT|UPDATE|DELETE|REPLACE)\b", forbidden, re.IGNORECASE) is None, "TASK-192 must not add DataService/Shell runtime DML")
    repository_source = read(h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualTransactionRepository.cpp")
    require_contains(repository_source, "INSERT INTO trade_execution_group", "TASK-192 DataAccess DML scope")
    require_contains(repository_source, "INSERT INTO trade_log", "TASK-192 DataAccess DML scope")
    require("INSERT INTO cash_adjustment" not in repository_source, "TASK-192 must not add manual cash movement write")
    require("INSERT INTO audit_log" not in repository_source, "TASK-192 must not add audit write")


def test_no_sqlite_write(h: Harness) -> None:
    diff = subprocess.run(["git", "diff", "main", "--", "libs/DataServiceApi", "apps", "libs/ShellServices", "libs/ShellCore"], cwd=h.root, check=True, capture_output=True, text=True).stdout
    for token in ["sqlite3_exec", "SQLite::Statement", "prepareStatement", "executeWrite"]:
        require(token not in diff, f"TASK-191 must not add SQLite write token {token}")


def test_no_trade_log_write(h: Harness) -> None:
    diff = subprocess.run(["git", "diff", "main", "--", "libs/DataServiceApi", "apps", "libs/ShellServices", "libs/ShellCore"], cwd=h.root, check=True, capture_output=True, text=True).stdout
    require("trade_log" not in diff, "TASK-192 must not add DataService/Shell trade_log references")


def test_no_cash_adjustment_write(h: Harness) -> None:
    diff = subprocess.run(["git", "diff", "main", "--", "libs", "apps"], cwd=h.root, check=True, capture_output=True, text=True).stdout
    require("INSERT INTO cash_adjustment" not in diff, "TASK-192 must not add cash_adjustment writes")
    forbidden = subprocess.run(["git", "diff", "main", "--", "libs/DataServiceApi", "apps", "libs/ShellServices", "libs/ShellCore"], cwd=h.root, check=True, capture_output=True, text=True).stdout
    require("cash_adjustment" not in forbidden, "TASK-192 must not add DataService/Shell cash_adjustment references")


def test_no_audit_ledger_write(h: Harness) -> None:
    diff = subprocess.run(["git", "diff", "main", "--", "libs", "apps"], cwd=h.root, check=True, capture_output=True, text=True).stdout
    for token in ["INSERT INTO audit_log", "ledgerId", "auditLogId"]:
        require(token not in diff, f"TASK-191 must not add runtime audit/ledger token {token}")


def test_production_qml_unmodified(h: Harness) -> None:
    for path in changed_paths(h.root):
        require(not path.startswith("apps/ETFDecisionShell/qml/"), "production QML must not change")


def test_startup_unmodified(h: Harness) -> None:
    assert_path_not_changed(h.root, "apps/ETFDecisionShell/src/main.cpp")


def test_presenter_controller_unmodified(h: Harness) -> None:
    for path in changed_paths(h.root):
        lower = path.lower()
        require("presenter" not in lower and "controller" not in lower, "Presenter / Controller must not change")


def test_accountingengine_replay_unmodified(h: Harness) -> None:
    for path in changed_paths(h.root):
        require(not path.startswith("libs/AccountingEngine/"), "AccountingEngine must not change")


def test_strategy_market_unmodified(h: Harness) -> None:
    for path in changed_paths(h.root):
        require(not path.startswith("libs/StrategyEngine/"), "StrategyEngine must not change")
        require(not path.startswith("libs/MarketEngine/"), "MarketEngine must not change")


def test_no_tradedraft_suggestion(h: Harness) -> None:
    diff = subprocess.run(["git", "diff", "main", "--", "libs", "apps"], cwd=h.root, check=True, capture_output=True, text=True).stdout
    added_lines = "\n".join(line for line in diff.splitlines() if line.startswith("+") and not line.startswith("+++"))
    for token in ["TradeDraft", "trade suggestion", "strategy recommendation"]:
        require(token not in added_lines, f"TASK-192 must not add {token}")


def test_no_broker_sdk(h: Harness) -> None:
    diff = subprocess.run(["git", "diff", "main", "--", "libs", "apps"], cwd=h.root, check=True, capture_output=True, text=True).stdout
    for token in ["broker SDK", "BrokerSdk", "IBKR", "Futu", "Tiger"]:
        require(token not in diff, f"TASK-191 must not add broker SDK token {token}")


def test_no_network_endpoint(h: Harness) -> None:
    diff = subprocess.run(["git", "diff", "main", "--", "libs", "apps"], cwd=h.root, check=True, capture_output=True, text=True).stdout
    for token in ["http://", "https://", "endpoint configuration", "network call"]:
        require(token.lower() not in diff.lower(), f"TASK-191 must not add network token {token}")


def test_no_credentials(h: Harness) -> None:
    diff = subprocess.run(["git", "diff", "main", "--", "libs", "apps"], cwd=h.root, check=True, capture_output=True, text=True).stdout
    for token in ["credential store", "secret store", "password value", "apiKey", "api_key"]:
        require(token.lower() not in diff.lower(), f"TASK-191 must not add credential token {token}")


def test_no_real_order(h: Harness) -> None:
    diff = subprocess.run(["git", "diff", "main", "--", "libs", "apps"], cwd=h.root, check=True, capture_output=True, text=True).stdout
    for token in ["placeOrder", "brokerOrderId", "real order", "order placement"]:
        require(token.lower() not in diff.lower(), f"TASK-191 must not add real order token {token}")


def test_no_automatic_trading(h: Harness) -> None:
    diff = subprocess.run(["git", "diff", "main", "--", "libs", "apps"], cwd=h.root, check=True, capture_output=True, text=True).stdout
    for token in ["automatic trading", "autoTrade", "strategyExecute"]:
        require(token.lower() not in diff.lower(), f"TASK-191 must not add automatic trading token {token}")


def test_task190_still_valid(h: Harness) -> None:
    cmake = read(h.root / "tests" / "ShellAccountingManualEntrySchemaMigrationImplementation" / "CMakeLists.txt")
    require_contains(cmake, "shell_accounting_manual_entry_schema_migration_implementation_gate", "TASK-190 CMake")


def test_task189_still_valid(h: Harness) -> None:
    cmake = read(h.root / "tests" / "ShellAccountingManualEntrySchemaImplementationAuthorizationGate" / "CMakeLists.txt")
    require_contains(cmake, "shell_accounting_manual_entry_schema_implementation_authorization_gate", "TASK-189 CMake")


def test_task188_still_valid(h: Harness) -> None:
    cmake = read(h.root / "tests" / "ShellAccountingManualEntrySchemaGapAuthorizationGate" / "CMakeLists.txt")
    require_contains(cmake, "shell_accounting_manual_entry_schema_gap_authorization_gate", "TASK-188 CMake")


def test_task187_still_valid(h: Harness) -> None:
    cmake = read(h.root / "tests" / "ShellAccountingManualEntrySchemaAdequacyReviewGate" / "CMakeLists.txt")
    require_contains(cmake, "shell_accounting_manual_entry_schema_adequacy_review_gate", "TASK-187 CMake")


def test_task186_still_valid(h: Harness) -> None:
    cmake = read(h.root / "tests" / "ShellAccountingManualEntryRepositoryImplementationAuthorizationGate" / "CMakeLists.txt")
    require_contains(cmake, "shell_accounting_manual_entry_repository_implementation_authorization_gate", "TASK-186 CMake")


def test_task185_still_valid(h: Harness) -> None:
    cmake = read(h.root / "tests" / "ShellAccountingManualEntryRepositoryScaffold" / "CMakeLists.txt")
    require_contains(cmake, "shell_accounting_manual_entry_repository_scaffold", "TASK-185 CMake")


def test_task182_still_valid(h: Harness) -> None:
    cmake = read(h.root / "tests" / "ShellAccountingManualEntryDataServiceActionValidationWiring" / "CMakeLists.txt")
    require_contains(cmake, "shell_accounting_manual_entry_dataservice_action_validation_wiring", "TASK-182 CMake")


def test_task177_still_valid(h: Harness) -> None:
    cmake = read(h.root / "tests" / "ShellAccountingManualTransactionCashMovementMvpAuthorizationGate" / "CMakeLists.txt")
    require_contains(cmake, "shell_accounting_manual_transaction_cash_movement_mvp_authorization", "TASK-177 CMake")


def test_broker_gates_retained(h: Harness) -> None:
    tests_cmake = read(h.root / "tests" / "CMakeLists.txt")
    for token in [
        "ShellAccountingBrokerAdapterDisabled",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementationGate",
    ]:
        require_contains(tests_cmake, token, "tests/CMakeLists broker gate")


CASES = {
    name.removeprefix("test_"): value
    for name, value in list(globals().items())
    if name.startswith("test_") and callable(value)
}


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    parser.add_argument("--case", required=True, choices=sorted(CASES))
    args = parser.parse_args()
    harness = Harness(Path(args.source_root))
    CASES[args.case](harness)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
