# AccountingEngine Next Phase Boundary Review

## Document Purpose

After the AccountingEngine skeleton scenario coverage milestone, the next phase
must not jump directly into DataService or SQLite integration. The project must
first choose a boundary route and keep one boundary in scope at a time.

This review documents the safe options before production replay grows beyond
DTO-only skeleton behavior.

## Optional Routes

### Route A: Continue AccountingEngine pure DTO scenarios

- basePosition production scenario
- sniperPool production scenario
- stale snapshot production boundary
- more controlled error cases

This keeps AccountingEngine isolated from DataAccess and DataService. It is the
lowest integration risk route.

### Route B: DataService read-only action contract

- `position.list` contract
- `cash.summary` contract
- `portfolio.pnl.summary` contract
- no-write guarantee
- `ProtocolResponse` shape

This route defines action names, request DTOs, response DTOs, guard behavior,
and error shape before implementation.

### Route C: SQLite read-only facts query boundary

- `trade_log` read-only query
- cash facts query
- market price facts query
- no write tests
- table count tests

This route defines how production facts can be read without enabling writes or
embedding replay business logic in DataAccess.

### Route D: market price / FX rate boundary

- no network by default
- injectable facts
- no fake valuation
- `supportsMarketPrice` / `supportsFxRate` capability transition

This route should separate externally sourced market data from replay math and
must not fabricate prices or exchange rates.

## Recommended Order

1. TASK-083: DataService read-only accounting action contract docs.
2. TASK-084: SQLite read-only facts query boundary docs.
3. TASK-085: DataService action guard for `position.list`.
4. TASK-086: No-write integration test harness.

It is not recommended to directly implement a DataService action. It is not
recommended to directly make AccountingEngine depend on DataAccess. It is not
recommended to write snapshots before the read-only contracts and no-write
guards exist.

If the project continues DTO-only coverage first, TASK-083 can instead be an
AccountingEngine replay skeleton milestone validation / docs task, followed by
another explicitly authorized DTO-only scenario.

## Hard Boundaries To Keep

- no writes
- no snapshot writes
- no TradeLog writes
- no TradeDraft
- no QML calculation
- no broker
- no auto trading
- no test-only code migration
- no DataAccess dependency unless authorized
- no DataService action unless authorized

## Go / No-Go Checklist

- [ ] PR has been merged to main.
- [ ] main full test suite has passed.
- [ ] transport 50-repeat test has passed.
- [ ] `replayImplemented=false` has been confirmed.
- [ ] `writeEnabled=false` has been confirmed.
- [ ] DataService action is not integrated.
- [ ] SQLite is not integrated.
- [ ] snapshot has not been written.
- [ ] TradeLog has not been written.
- [ ] QML does not calculate accounting.
- [ ] Next task boundary has been separately authorized.

## TASK-083 Route B Contract Documents

Route B now has DataService read-only accounting action contract docs in
`docs/37_dataservice_readonly_accounting_action_contracts.md`.

The future no-write proof plan is in
`docs/38_dataservice_accounting_no_write_test_plan.md`.

Implementation still requires separate authorization. The contracts do not
implement DataService actions, do not connect SQLite, do not call
AccountingEngine, and do not enable writes.

## TASK-084 Route C Boundary Documents

Route C now has SQLite read-only facts query boundary docs in
`docs/39_sqlite_readonly_facts_query_boundary.md`.

Accounting facts source mapping is documented in
`docs/40_accounting_facts_source_mapping.md`.

Implementation still requires separate authorization. The boundary documents do
not add DataAccess repositories, do not implement SQLite queries, do not add
DataService actions, and do not enable writes.

## TASK-093 DataService Guard Suite Milestone

The DataService accounting guard suite milestone is documented in
`docs/43_dataservice_accounting_guard_suite_milestone.md`, with no-write
readiness in
`docs/44_dataservice_accounting_guard_no_write_readiness_review.md`.

The DataService guard suite is complete for the current milestone, but
DataService and AccountingEngine integration has not started. SQLite facts
query is not connected to DataService, QML is not connected to accounting
actions, and the next stage must be separately authorized one boundary at a
time.
