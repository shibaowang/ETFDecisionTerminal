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

## TASK-086 SQLite Read-only No-write Harness Skeleton

TASK-086 adds a test-only no-write harness skeleton under
`tests/AccountingNoWrite`. It is not production code and does not implement a
real SQLite facts query.

The harness includes:

- a protected table row-count helper for `trade_log`,
  `trade_execution_group`, `trade_draft`, `cash_snapshot`,
  `position_snapshot`, `portfolio_summary`, and `audit_log`;
- a no-write assertion helper that captures counts before and after a callback;
- a forbidden SQL scanner for `INSERT`, `UPDATE`, `DELETE`, `CREATE`, `DROP`,
  `ALTER`, `REPLACE`, `UPSERT`, `VACUUM`, `PRAGMA writable_schema`,
  `BEGIN IMMEDIATE`, and `BEGIN EXCLUSIVE`;
- CTest coverage for `accounting_forbidden_sql_scanner` and
  `accounting_no_write_harness`.

Future DataService accounting actions and SQLite facts query tasks must reuse
or explicitly mirror this skeleton before any real read-only implementation is
accepted. The skeleton does not read facts, does not call AccountingEngine, and
does not change `position.list` guard behavior.

## TASK-087 Trade Facts Reader No-write Coverage

The DataAccess trade facts reader is covered by the TASK-086 no-write harness.
Tests capture protected table row counts before and after reader calls and
verify that `trade_log`, `trade_execution_group`, `trade_draft`,
`cash_snapshot`, `position_snapshot`, `portfolio_summary`, and `audit_log`
remain unchanged.

The reader SQL is also scanned with the forbidden SQL scanner. The scanner must
continue to reject write keywords before future facts query expansion.

## TASK-088 Cash Facts Query No-write Boundary

Future cash facts query work must pass the no-write harness before it can be
used by any DataService accounting action. It must protect at least
`trade_log`, `trade_execution_group`, `trade_draft`, `cash_snapshot`,
`position_snapshot`, `portfolio_summary`, and `audit_log`.

The cash facts source boundary is documented in
`docs/41_cash_facts_source_boundary.md`, and the decision checklist is in
`docs/42_cash_facts_query_decision.md`. Source unavailable paths, invalid
request paths, and schema gap paths must also be no-write. A future query must
not write `cash_snapshot` or `portfolio_summary`, and must not derive cash facts
from either table.

## TASK-089 cash.summary Guard No-write Coverage

The `cash.summary` guard has no-write table count coverage. The guard test
checks that calling `cash.summary` leaves these protected tables unchanged:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- `audit_log`

The guard also confirms that `cash.summary` does not read cash facts, does not
read `cash_snapshot` or `portfolio_summary`, does not access SQLite facts
queries, and does not call AccountingEngine. The real `cash.summary`
implementation must extend these no-write tests before it can read facts or map
real cash DTOs.

## TASK-090 portfolio.pnl.summary Guard No-write Coverage

The `portfolio.pnl.summary` guard has no-write table count coverage. The guard
test checks that calling `portfolio.pnl.summary` leaves these protected tables
unchanged:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- `audit_log`

The guard also confirms that `portfolio.pnl.summary` does not read `trade_log`,
cash facts, market price facts, `cash_snapshot`, `position_snapshot`, or
`portfolio_summary`, does not access SQLite facts queries, and does not call
AccountingEngine. The real `portfolio.pnl.summary` implementation must extend
these no-write tests before it can read facts or map real PnL DTOs.

## TASK-091 base_position.summary Guard No-write Coverage

The `base_position.summary` guard has no-write table count coverage. The guard
test checks that calling `base_position.summary` leaves these protected tables
unchanged:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- `audit_log`

The guard also confirms that `base_position.summary` does not read `trade_log`,
`position_snapshot`, `cash_snapshot`, or `portfolio_summary`, does not access
SQLite facts queries, does not call AccountingEngine, does not generate trade
suggestions, and does not generate TradeDraft rows. The real
`base_position.summary` implementation must extend these no-write tests before
it can read facts or map real base position DTOs, and it must continue to
protect `trade_draft`, `position_snapshot`, and `portfolio_summary` from writes.

## TASK-092 sniper_pool.summary Guard No-write Coverage

The `sniper_pool.summary` guard has no-write table count coverage. The guard
test checks that calling `sniper_pool.summary` leaves these protected tables
unchanged:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- `audit_log`

The guard also confirms that `sniper_pool.summary` does not read `trade_log`,
`position_snapshot`, `cash_snapshot`, or `portfolio_summary`, does not access
SQLite facts queries, does not call AccountingEngine, does not calculate T1-T6
tiers, does not generate trade suggestions, and does not generate TradeDraft
rows. The real `sniper_pool.summary` implementation must extend these no-write
tests before it can read facts or map real sniper pool DTOs, and it must
continue to protect `trade_draft`, `position_snapshot`, and `portfolio_summary`
from writes.

## TASK-093 Guard Suite No-write Milestone

TASK-093 records the DataService accounting guard suite no-write milestone in
`docs/43_dataservice_accounting_guard_suite_milestone.md` and the readiness
gate in `docs/44_dataservice_accounting_guard_no_write_readiness_review.md`.

The guard suite has stage-level no-write coverage for:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`
- `base_position.summary`
- `sniper_pool.summary`

These tests protect `trade_log`, `trade_execution_group`, `trade_draft`,
`cash_snapshot`, `position_snapshot`, `portfolio_summary`, and `audit_log`.
Future real action implementations must extend no-write tests for their real
SQLite, DataAccess, AccountingEngine mapping, and failure paths before the
guard payload can be replaced.

## TASK-094 position.list Real Implementation No-write Gate

The future real `position.list` implementation boundary is documented in
`docs/45_position_list_real_implementation_boundary.md`, with readiness checks
in `docs/46_position_list_real_implementation_readiness_checklist.md`.

The current guard no-write test is not enough for real implementation. A real
`position.list` action must extend no-write table count tests for request
validation, DataAccess facts query use, AccountingEngine mapping, domain
failure, invalid request, and replay unavailable paths before the guard can be
changed.
