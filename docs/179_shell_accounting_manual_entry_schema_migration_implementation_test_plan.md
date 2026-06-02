# ShellAccounting Manual Entry Schema Migration Implementation Test Plan

## Document Purpose

This document defines the TASK-190 test plan for the manual entry schema
migration implementation. It verifies that the task adds exactly one independent
DDL-only migration and does not expand into repository, DataService write, UI,
broker, or trading implementation.

## Test Matrix

### Migration File

- `migrations/002_shell_accounting_manual_entry_schema.sql` exists.
- `migrations/001_initial_schema.sql` remains unmodified by TASK-190.
- No more than one TASK-190 migration file exists.
- The migration file name is stable and independent.
- The migration is DDL-only.
- The migration contains no `INSERT`, `UPDATE`, `DELETE`, or `REPLACE`.
- The migration contains no `DROP TABLE`.
- The migration contains no destructive table rename.

### Table Columns

- `trade_log` receives `request_id`.
- `trade_log` receives `idempotency_key`.
- `trade_log` receives `occurred_at_utc`.
- `trade_log` receives `tax_cents`.
- `trade_log` receives `source_memo_sanitized`.
- `trade_log` receives `cash_adjustment_uid`.
- `cash_adjustment` receives `request_id`.
- `cash_adjustment` receives `idempotency_key`.
- `cash_adjustment` receives `occurred_at_utc`.
- `cash_adjustment` receives `source_memo_sanitized`.
- `cash_adjustment` receives `trade_log_uid`.
- `audit_log` receives `request_id`.
- `audit_log` receives `idempotency_key`.
- `audit_log` receives `payload_classification`.
- `audit_log` receives `redaction_status`.

### Indexes

- `idx_trade_log_manual_request_id` exists.
- `ux_trade_log_manual_idempotency_key` exists.
- `idx_trade_log_cash_adjustment_uid` exists.
- `idx_cash_adjustment_manual_request_id` exists.
- `ux_cash_adjustment_manual_idempotency_key` exists.
- `idx_cash_adjustment_trade_log_uid` exists.
- `idx_audit_log_manual_request_id` exists.
- `idx_audit_log_manual_idempotency_key` exists.

### SQLite Application Probe

- A test database can execute `001_initial_schema.sql`.
- The same test database can execute `002_shell_accounting_manual_entry_schema.sql`.
- `PRAGMA table_info` verifies all new columns.
- `sqlite_master` verifies all indexes.

### Boundary Regression

- DataServiceActions source, header, and registrar remain unchanged.
- TASK-178 validation production code remains unchanged.
- TASK-185 repository scaffold remains disabled and unchanged.
- No manual entry repository implementation is added.
- No runtime SQL / SQLite write path is added.
- No runtime `trade_log` / cash facts / audit / ledger write path is added.
- Production QML / startup / Presenter / Controller remain unchanged.
- AccountingEngine replay remains unchanged.
- StrategyEngine and MarketEngine remain unchanged.
- No TradeDraft or suggestion implementation is added.
- No broker SDK, network, credentials, endpoint, real order id, real order, or
  automatic trading is added.
- Existing broker safety gates remain retained.

### Regression

- TASK-189 schema implementation authorization tests pass.
- TASK-188 schema gap authorization tests pass.
- TASK-187 schema adequacy review tests pass.
- TASK-186 repository implementation authorization tests pass.
- TASK-185 repository scaffold tests pass.
- TASK-182 validation wiring tests pass.
- TASK-178 validation scaffold tests pass.
- Broker disabled, broker order, and real broker gates pass.
- Full CTest passes.
- transport local socket echo 50 repeat passes.

## Required Probes

- Static migration scan.
- Forbidden DML token scan.
- Forbidden destructive DDL scan.
- SQLite execution probe.
- `PRAGMA table_info` column probe.
- `sqlite_master` index probe.
- No production source drift scan.
- No repository implementation scan.
- No runtime SQL / SQLite write scan.
- No broker / network / credentials / endpoint / real order scan.
- Broker gate retention probe.

## Go / No-Go Checklist

- [ ] TASK-189 follow-up commit is present in main.
- [ ] `migrations/002_shell_accounting_manual_entry_schema.sql` is present.
- [ ] `migrations/001_initial_schema.sql` is unchanged.
- [ ] The migration is DDL-only.
- [ ] The migration executes after `001_initial_schema.sql` in a test database.
- [ ] New columns are verified through `PRAGMA table_info`.
- [ ] New indexes are verified through `sqlite_master`.
- [ ] No repository implementation is added.
- [ ] No DataService write implementation is added.
- [ ] No production QML or startup change is added.
- [ ] No broker SDK, network, credentials, endpoint, real order, or automatic
  trading is added.
- [ ] Full CTest passes.
- [ ] transport local socket echo 50 repeat passes.

## TASK-191 Follow-Up Gate

TASK-191 adds the post-migration repository implementation authorization gate.
The TASK-191 tests must keep the TASK-190 migration semantics intact, prove that
`migrations/001_initial_schema.sql` and
`migrations/002_shell_accounting_manual_entry_schema.sql` are not modified, and
continue to block repository implementation, DataService write implementation,
runtime SQL, SQLite writes, QML/startup changes, broker SDKs, network,
credentials, real order placement, and automatic trading.
