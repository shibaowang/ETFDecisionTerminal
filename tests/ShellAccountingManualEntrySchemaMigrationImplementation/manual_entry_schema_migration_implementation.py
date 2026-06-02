#!/usr/bin/env python3

import argparse
import re
import sqlite3
import tempfile
from pathlib import Path


TRADE_LOG_COLUMNS = {
    "request_id",
    "idempotency_key",
    "occurred_at_utc",
    "tax_cents",
    "source_memo_sanitized",
    "cash_adjustment_uid",
}

CASH_ADJUSTMENT_COLUMNS = {
    "request_id",
    "idempotency_key",
    "occurred_at_utc",
    "source_memo_sanitized",
    "trade_log_uid",
}

AUDIT_LOG_COLUMNS = {
    "request_id",
    "idempotency_key",
    "payload_classification",
    "redaction_status",
}

INDEXES = {
    "idx_trade_log_manual_request_id",
    "ux_trade_log_manual_idempotency_key",
    "idx_trade_log_cash_adjustment_uid",
    "idx_cash_adjustment_manual_request_id",
    "ux_cash_adjustment_manual_idempotency_key",
    "idx_cash_adjustment_trade_log_uid",
    "idx_audit_log_manual_request_id",
    "idx_audit_log_manual_idempotency_key",
}


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def require_contains(text: str, token: str, context: str) -> None:
    require(token in text, f"{context} missing `{token}`")


def require_not_contains(text: str, token: str, context: str) -> None:
    require(token not in text, f"{context} unexpectedly contains `{token}`")


def files_under(root: Path, suffixes=None):
    if not root.exists():
        return []
    files = [path for path in root.rglob("*") if path.is_file()]
    if suffixes:
        files = [path for path in files if path.suffix in suffixes]
    return files


class Harness:
    def __init__(self, root: Path):
        self.root = root

    @property
    def migration001(self) -> Path:
        return self.root / "migrations" / "001_initial_schema.sql"

    @property
    def migration002(self) -> Path:
        return self.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql"

    @property
    def migration_sql(self) -> str:
        return read(self.migration002)

    def doc(self, number: int) -> str:
        names = {
            176: "176_shell_accounting_manual_entry_schema_implementation_authorization_gate.md",
            177: "177_shell_accounting_manual_entry_schema_implementation_authorization_test_plan.md",
            178: "178_shell_accounting_manual_entry_schema_migration_implementation.md",
            179: "179_shell_accounting_manual_entry_schema_migration_implementation_test_plan.md",
        }
        return read(self.root / "docs" / names[number])


def apply_migrations(h: Harness):
    with tempfile.TemporaryDirectory(prefix="task190_schema_") as tmp:
        db_path = Path(tmp) / "schema.sqlite"
        connection = sqlite3.connect(db_path)
        try:
            connection.executescript(read(h.migration001))
            connection.executescript(read(h.migration002))
            yield connection
        finally:
            connection.close()


def table_columns(connection, table_name: str) -> set[str]:
    rows = connection.execute(f"PRAGMA table_info({table_name})").fetchall()
    return {row[1] for row in rows}


def index_names(connection) -> set[str]:
    rows = connection.execute(
        "SELECT name FROM sqlite_master WHERE type = 'index'"
    ).fetchall()
    return {row[0] for row in rows}


def assert_columns(connection, table_name: str, expected: set[str]) -> None:
    columns = table_columns(connection, table_name)
    missing = sorted(expected - columns)
    require(not missing, f"{table_name} missing columns: {', '.join(missing)}")


def assert_indexes(connection, expected: set[str]) -> None:
    indexes = index_names(connection)
    missing = sorted(expected - indexes)
    require(not missing, "missing indexes: " + ", ".join(missing))


def test_gate(h: Harness) -> None:
    require(h.migration001.exists(), "001 migration exists")
    require(h.migration002.exists(), "002 migration exists")
    require((h.root / "docs" / "178_shell_accounting_manual_entry_schema_migration_implementation.md").exists(), "docs/178 exists")
    require((h.root / "docs" / "179_shell_accounting_manual_entry_schema_migration_implementation_test_plan.md").exists(), "docs/179 exists")


def test_docs(h: Harness) -> None:
    doc178 = h.doc(178)
    for token in [
        "TASK-190",
        "manual entry schema migration implementation",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
        "migrations/001_initial_schema.sql` unchanged",
        "schema-only",
        "does not implement a manual entry repository",
        "does not add a DataService write implementation",
        "does not execute runtime SQL",
        "does not write SQLite at runtime",
        "does not modify production QML or startup",
        "Broker sandbox new capability development remains paused",
        "no-real-order",
        "no-network",
        "no-credentials",
        "no-order-placement",
        "forward-fix migration",
    ]:
        require_contains(doc178, token, "docs/178")
    doc179 = h.doc(179)
    for token in [
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "DDL-only",
        "PRAGMA table_info",
        "sqlite_master",
        "No repository implementation is added",
        "No DataService write implementation is added",
        "No broker SDK, network, credentials, endpoint, real order, or automatic",
    ]:
        require_contains(doc179, token, "docs/179")


def test_docs_index_prompt(h: Harness) -> None:
    readme = read(h.root / "README.md")
    docs_index = read(h.root / "docs" / "README.md")
    prompt = read(h.root / "docs" / "12_codex_prompt_template.md")
    require_contains(readme, "docs/178_shell_accounting_manual_entry_schema_migration_implementation.md", "README")
    require_contains(readme, "docs/179_shell_accounting_manual_entry_schema_migration_implementation_test_plan.md", "README")
    require_contains(docs_index, "178_shell_accounting_manual_entry_schema_migration_implementation.md", "docs/README")
    require_contains(docs_index, "179_shell_accounting_manual_entry_schema_migration_implementation_test_plan.md", "docs/README")
    require_contains(prompt, "TASK-190", "docs/12")
    require_contains(prompt, "docs/178 and docs/179", "docs/12")


def test_migration_file_exists(h: Harness) -> None:
    require(h.migration002.exists(), "migration 002 must exist")


def test_initial_schema_unmodified(h: Harness) -> None:
    initial = read(h.migration001)
    for token in ["TASK-190", "002_shell_accounting_manual_entry_schema", "idempotency_key", "tax_cents"]:
        require_not_contains(initial, token, "initial schema")


def test_single_migration_file(h: Harness) -> None:
    matches = [path.name for path in (h.root / "migrations").glob("*manual_entry_schema*.sql")]
    require(matches == ["002_shell_accounting_manual_entry_schema.sql"], "only TASK-190 manual entry schema migration is allowed")


def test_stable_filename(h: Harness) -> None:
    require(h.migration002.name == "002_shell_accounting_manual_entry_schema.sql", "migration filename is stable")


def test_ddl_only(h: Harness) -> None:
    sql = h.migration_sql.upper()
    require("ALTER TABLE" in sql, "migration must alter existing tables")
    require("CREATE INDEX" in sql, "migration must create indexes")


def test_no_dml(h: Harness) -> None:
    sql = h.migration_sql
    for token in ["INSERT", "UPDATE", "DELETE", "REPLACE"]:
        require(re.search(rf"\b{token}\b", sql, re.IGNORECASE) is None, f"migration must not contain {token}")


def test_no_drop_table(h: Harness) -> None:
    require(re.search(r"\bDROP\s+TABLE\b", h.migration_sql, re.IGNORECASE) is None, "migration must not drop tables")


def test_no_destructive_rename(h: Harness) -> None:
    require(re.search(r"\bRENAME\s+TO\b", h.migration_sql, re.IGNORECASE) is None, "migration must not rename tables")


def test_trade_log_alter(h: Harness) -> None:
    require_contains(h.migration_sql, "ALTER TABLE trade_log", "migration")


def test_cash_adjustment_alter(h: Harness) -> None:
    require_contains(h.migration_sql, "ALTER TABLE cash_adjustment", "migration")


def test_audit_log_alter(h: Harness) -> None:
    require_contains(h.migration_sql, "ALTER TABLE audit_log", "migration")


def test_trade_log_columns(h: Harness) -> None:
    for column in TRADE_LOG_COLUMNS:
        require_contains(h.migration_sql, column, "trade_log migration columns")


def test_cash_adjustment_columns(h: Harness) -> None:
    for column in CASH_ADJUSTMENT_COLUMNS:
        require_contains(h.migration_sql, column, "cash_adjustment migration columns")


def test_audit_log_columns(h: Harness) -> None:
    for column in AUDIT_LOG_COLUMNS:
        require_contains(h.migration_sql, column, "audit_log migration columns")


def test_trade_log_indexes(h: Harness) -> None:
    for index in ["idx_trade_log_manual_request_id", "ux_trade_log_manual_idempotency_key", "idx_trade_log_cash_adjustment_uid"]:
        require_contains(h.migration_sql, index, "trade_log indexes")


def test_cash_adjustment_indexes(h: Harness) -> None:
    for index in ["idx_cash_adjustment_manual_request_id", "ux_cash_adjustment_manual_idempotency_key", "idx_cash_adjustment_trade_log_uid"]:
        require_contains(h.migration_sql, index, "cash_adjustment indexes")


def test_audit_log_indexes(h: Harness) -> None:
    for index in ["idx_audit_log_manual_request_id", "idx_audit_log_manual_idempotency_key"]:
        require_contains(h.migration_sql, index, "audit_log indexes")


def test_sqlite_apply_001_002(h: Harness) -> None:
    for connection in apply_migrations(h):
        require("trade_log" in {row[0] for row in connection.execute("SELECT name FROM sqlite_master WHERE type='table'")}, "trade_log table exists after migrations")


def test_pragma_trade_log_columns(h: Harness) -> None:
    for connection in apply_migrations(h):
        assert_columns(connection, "trade_log", TRADE_LOG_COLUMNS)


def test_pragma_cash_adjustment_columns(h: Harness) -> None:
    for connection in apply_migrations(h):
        assert_columns(connection, "cash_adjustment", CASH_ADJUSTMENT_COLUMNS)


def test_pragma_audit_log_columns(h: Harness) -> None:
    for connection in apply_migrations(h):
        assert_columns(connection, "audit_log", AUDIT_LOG_COLUMNS)


def test_sqlite_master_indexes(h: Harness) -> None:
    for connection in apply_migrations(h):
        assert_indexes(connection, INDEXES)


def exact_file_has_no_task190(h: Harness, relative: str) -> None:
    text = read(h.root / relative)
    require_not_contains(text, "TASK-190", relative)
    require_not_contains(text, "manual_entry_schema_migration", relative)


def test_dataserviceactions_cpp_unmodified(h: Harness) -> None:
    exact_file_has_no_task190(h, "libs/DataServiceApi/src/DataServiceActions.cpp")


def test_dataserviceactions_h_unmodified(h: Harness) -> None:
    exact_file_has_no_task190(h, "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h")


def test_registrar_unmodified(h: Harness) -> None:
    exact_file_has_no_task190(h, "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp")


def test_validation_code_unmodified(h: Harness) -> None:
    exact_file_has_no_task190(h, "libs/DataServiceApi/include/DataServiceApi/ShellAccountingManualTransactionCashMovementValidation.h")
    exact_file_has_no_task190(h, "libs/DataServiceApi/src/ShellAccountingManualTransactionCashMovementValidation.cpp")


def test_scaffold_unmodified(h: Harness) -> None:
    exact_file_has_no_task190(h, "libs/DataAccess/include/DataAccess/ShellAccountingManualEntryRepositoryScaffold.h")
    exact_file_has_no_task190(h, "libs/DataAccess/src/ShellAccountingManualEntryRepositoryScaffold.cpp")


def test_scaffold_disabled(h: Harness) -> None:
    source = read(h.root / "libs/DataAccess/include/DataAccess/ShellAccountingManualEntryRepositoryScaffold.h")
    for token in ['writeImplemented = false', 'databaseWritten = false', 'tradeLogWritten = false', 'cashFactsWritten = false', 'auditWritten = false', 'ledgerWritten = false']:
        require_contains(source, token, "TASK-185 scaffold disabled policy")


def test_no_repository_implementation(h: Harness) -> None:
    data_access = "\n".join(read(path) for path in files_under(h.root / "libs" / "DataAccess", {".h", ".cpp"}))
    for token in [
        "ManualEntryImplementationRepository",
        "ManualEntryWriteRepository",
        "ManualTransactionWriteRepository",
        "ManualCashMovementWriteRepository",
        "insertManualTransaction",
        "insertCashMovement",
    ]:
        require_not_contains(data_access, token, "manual entry repository implementation scan")


def test_no_runtime_sql_write(h: Harness) -> None:
    scaffold = read(h.root / "libs/DataAccess/src/ShellAccountingManualEntryRepositoryScaffold.cpp")
    for token in ["INSERT ", "UPDATE ", "DELETE ", "REPLACE ", "sqlite3_exec", "executeNonQuery", "beginTransaction", "commit"]:
        require_not_contains(scaffold, token, "manual entry scaffold runtime SQL write scan")


def test_no_runtime_trade_cash_audit_ledger_write(h: Harness) -> None:
    scaffold = read(h.root / "libs/DataAccess/src/ShellAccountingManualEntryRepositoryScaffold.cpp")
    for token in ["trade_log", "cash_adjustment", "audit_log", "ledger"]:
        require_not_contains(scaffold, token, "manual entry scaffold runtime ledger write scan")


def require_no_task190_under(h: Harness, relative: str, context: str) -> None:
    for path in files_under(h.root / relative, {".h", ".cpp", ".qml"}):
        text = read(path)
        require_not_contains(text, "TASK-190", context)
        require_not_contains(text, "manualEntrySchemaMigration", context)


def test_production_qml_unmodified(h: Harness) -> None:
    require_no_task190_under(h, "apps/ETFDecisionShell/qml", "production QML")


def test_startup_unmodified(h: Harness) -> None:
    exact_file_has_no_task190(h, "apps/ETFDecisionShell/src/main.cpp")


def test_presenter_controller_unmodified(h: Harness) -> None:
    require_no_task190_under(h, "libs/ShellServices", "ShellServices")
    require_no_task190_under(h, "libs/ShellCore", "ShellCore")


def test_accountingengine_replay_unmodified(h: Harness) -> None:
    require_no_task190_under(h, "libs/AccountingEngine", "AccountingEngine")


def test_strategy_market_unmodified(h: Harness) -> None:
    if (h.root / "libs" / "StrategyEngine").exists():
        require_no_task190_under(h, "libs/StrategyEngine", "StrategyEngine")
    if (h.root / "libs" / "MarketEngine").exists():
        require_no_task190_under(h, "libs/MarketEngine", "MarketEngine")


def test_no_tradedraft_suggestion(h: Harness) -> None:
    doc178 = h.doc(178)
    require_contains(doc178, "does not add broker SDKs", "docs/178")
    exact_file_has_no_task190(h, "libs/DataServiceApi/src/DataServiceActions.cpp")


def test_no_broker_sdk(h: Harness) -> None:
    for path in files_under(h.root / "libs", {".h", ".cpp"}):
        if "Broker" in path.name:
            require_not_contains(read(path), "TASK-190", "broker source scan")


def test_no_network_endpoint(h: Harness) -> None:
    doc178 = h.doc(178)
    require_contains(doc178, "network calls", "docs/178")
    require_contains(doc178, "endpoints", "docs/178")


def test_no_credentials(h: Harness) -> None:
    doc178 = h.doc(178)
    require_contains(doc178, "credentials", "docs/178")


def test_no_real_order(h: Harness) -> None:
    doc178 = h.doc(178)
    require_contains(doc178, "real orders", "docs/178")


def test_no_automatic_trading(h: Harness) -> None:
    doc178 = h.doc(178)
    require_contains(doc178, "automatic trading", "docs/178")


def test_task189_evolved(h: Harness) -> None:
    for number in [176, 177]:
        require_contains(h.doc(number), "TASK-190", f"docs/{number}")
        require_contains(h.doc(number), "002_shell_accounting_manual_entry_schema.sql", f"docs/{number}")


def test_prior_gates_retained(h: Harness) -> None:
    tests_cmake = read(h.root / "tests" / "CMakeLists.txt")
    for token in [
        "ShellAccountingManualEntrySchemaImplementationAuthorizationGate",
        "ShellAccountingManualEntrySchemaGapAuthorizationGate",
        "ShellAccountingManualEntrySchemaAdequacyReviewGate",
        "ShellAccountingManualEntryRepositoryImplementationAuthorizationGate",
        "ShellAccountingManualEntryRepositoryScaffold",
        "ShellAccountingManualEntryDataServiceActionValidationWiring",
        "ShellAccountingManualTransactionCashMovementValidationScaffold",
    ]:
        require_contains(tests_cmake, token, "tests/CMake")


def test_broker_gates_retained(h: Harness) -> None:
    tests_cmake = read(h.root / "tests" / "CMakeLists.txt")
    for token in [
        "ShellAccountingBrokerAdapterDisabledWiring",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementationGate",
    ]:
        require_contains(tests_cmake, token, "broker gate retention")


CASES = {
    name[5:]: func
    for name, func in list(globals().items())
    if name.startswith("test_") and callable(func)
}


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True, type=Path)
    parser.add_argument("--case", required=True)
    args = parser.parse_args()

    h = Harness(args.source_root)
    case = CASES.get(args.case)
    if case is None:
        raise SystemExit(f"unknown case: {args.case}")
    case(h)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
