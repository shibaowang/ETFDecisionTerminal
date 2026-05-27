# DataService Accounting No-write Test Plan

## Document Purpose

This document defines a no-write test plan for future DataService accounting
read-only actions. It does not implement tests, actions, SQLite integration, or
AccountingEngine integration.

## Protected Tables

Future accounting read-only action tests must protect:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- `audit_log`
- `system_log`, if the project treats it as business-observable state
- `error_log`, if the project treats it as business-observable state

## Table Count Test Strategy

- Capture row counts before the action.
- Execute the action.
- Capture row counts after the action.
- Every forbidden table count must remain unchanged.
- Missing tables must be interpreted against the schema and migration under
  test; a missing optional table should not create a false failure.
- Temporary database tests are preferred so failures cannot affect local data.

## SQLite Read-only Connection Strategy

- Use SQLite read-only open mode if the production path supports it.
- Do not use `BEGIN IMMEDIATE`.
- Do not use `BEGIN EXCLUSIVE`.
- Do not execute `INSERT`.
- Do not execute `UPDATE`.
- Do not execute `DELETE`.
- Do not execute `CREATE`.
- Do not execute `DROP`.
- Do not execute `ALTER`.
- Do not use `PRAGMA writable_schema`.
- Do not run `VACUUM` or migrations.

## SQL Scanning Strategy

Future action implementation must be statically scanned for obvious write
keywords:

- `INSERT`
- `UPDATE`
- `DELETE`
- `CREATE`
- `DROP`
- `ALTER`
- `REPLACE`
- `UPSERT`
- `VACUUM`

`SELECT` is allowed. Documentation strings, error messages, comments, and test
fixtures can create false positives; those cases require manual review instead
of automatic approval.

## Integration Test Matrix

Every future accounting action must cover the same no-write matrix.

`position.list`:

- valid request
- invalid request
- replay unavailable
- missing market price
- multi currency unsupported
- no-write row counts
- transport stable

`cash.summary`:

- valid request
- invalid request
- replay unavailable
- missing market price
- multi currency unsupported
- no-write row counts
- transport stable

`portfolio.pnl.summary`:

- valid request
- invalid request
- replay unavailable
- missing market price
- multi currency unsupported
- no-write row counts
- transport stable

`base_position.summary`:

- valid request
- invalid request
- replay unavailable
- missing market price
- multi currency unsupported
- no-write row counts
- transport stable

`sniper_pool.summary`:

- valid request
- invalid request
- replay unavailable
- missing market price
- multi currency unsupported
- no-write row counts
- transport stable

## Failure Handling

- A domain error response must still be no-write.
- An invalid request response must still be no-write.
- A replay unavailable response must still be no-write.
- Panic, exception, timeout, and handler failure paths must still be no-write.
- Cleanup code must not compensate by writing snapshots, TradeLog rows, or
  TradeDraft rows.

## Automation Checklist

- CTest names should use explicit action and guarantee names, for example
  `dataservice_accounting_position_list_no_write`.
- Test database setup should use a temporary database copied or initialized from
  migrations.
- Row count helpers should read counts before and after the action.
- Forbidden write scanners should inspect action implementation and SQL strings.
- Transport stability must keep using:

```powershell
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## TASK-084 SQLite Facts Query Extension

SQLite read-only facts query implementation must use the same no-write row
count tests before it can be accepted.

Future facts query implementation must include SQL forbidden keyword scans for
`INSERT`, `UPDATE`, `DELETE`, `CREATE`, `DROP`, `ALTER`, `REPLACE`, `UPSERT`,
and `VACUUM`.

The SQLite facts query boundary is documented in
`docs/39_sqlite_readonly_facts_query_boundary.md`.

Accounting facts source mapping is documented in
`docs/40_accounting_facts_source_mapping.md`.

## TASK-085 position.list Guard No-write Coverage

The `position.list` guard has no-write table count coverage. The guard test
checks that calling `position.list` leaves these protected tables unchanged:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- `audit_log`

The real `position.list` implementation must extend these no-write tests before
it can read facts or map real positions.
