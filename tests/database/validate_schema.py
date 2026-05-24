#!/usr/bin/env python3

import argparse
import sqlite3
import sys
import tempfile
from pathlib import Path


REQUIRED_TABLES = (
    "account",
    "portfolio",
    "instrument",
    "strategy",
    "otc_channel",
    "trade_log",
    "trade_draft",
    "trade_execution_group",
    "trade_tier_allocation",
    "base_position_allocation",
    "grid_cycle",
    "cash_allocation_plan_current",
    "audit_log",
    "system_log",
    "error_log",
    "schema_version",
)


def table_exists(connection: sqlite3.Connection, table_name: str) -> bool:
    row = connection.execute(
        "SELECT 1 FROM sqlite_master WHERE type = 'table' AND name = ?",
        (table_name,),
    ).fetchone()
    return row is not None


def schema_version_exists(connection: sqlite3.Connection) -> bool:
    row = connection.execute(
        """
        SELECT 1
        FROM schema_version
        WHERE migration_name LIKE '%001_initial_schema%'
           OR version = 1
        LIMIT 1
        """
    ).fetchone()
    return row is not None


def validate_schema(migration_path: Path) -> None:
    sql = migration_path.read_text(encoding="utf-8")

    with tempfile.TemporaryDirectory(prefix="etfdt_schema_") as temp_dir:
        db_path = Path(temp_dir) / "schema_test.sqlite"
        connection = sqlite3.connect(db_path)
        try:
            connection.executescript(sql)

            missing_tables = [
                table_name
                for table_name in REQUIRED_TABLES
                if not table_exists(connection, table_name)
            ]
            if missing_tables:
                raise AssertionError(
                    "Missing required tables: " + ", ".join(missing_tables)
                )

            if not schema_version_exists(connection):
                raise AssertionError(
                    "schema_version does not contain migration 001_initial_schema"
                )
        finally:
            connection.close()


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Validate the initial SQLite schema migration."
    )
    parser.add_argument(
        "--migration",
        required=True,
        type=Path,
        help="Path to migrations/001_initial_schema.sql",
    )
    args = parser.parse_args()

    try:
        validate_schema(args.migration)
    except Exception as exc:
        print(f"Schema validation failed: {exc}", file=sys.stderr)
        return 1

    print("Schema validation passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
