# Production Accounting Replay Readiness Review

## Purpose

This document is the readiness checklist before production accounting replay
work starts.

It prevents moving the test-only fixture engine into production and prevents
connecting DataService or SQLite before module boundaries are explicit.

Detailed architecture boundary: `33_production_accounting_replay_architecture.md`.

AccountingEngine candidate design: `34_accounting_engine_module_candidate.md`.

## Production Replay Preconditions

- Data contracts are stable.
- FX001-FX013 fixture coverage is complete.
- The result skeleton is stable.
- The assertion skeleton is stable.
- Read-only and write boundaries are explicit.
- The `trade_log` fact ledger boundary is explicit.
- Snapshot data is explicitly derived data, not a fact source.
- QML does not calculate accounting fields.

## Required Architecture Questions

- Where does the production replay engine live?
- Should a new `libs/AccountingEngine` module be added?
- May `AccountingEngine` depend on DataAccess?
- Does DataServiceApi only call AccountingEngine?
- Do ShellServices consume DTOs only?
- Does QML bind ViewModels only?
- Is replay read-only?
- Is snapshot generation a separate write task?
- Is TradeLog writing a separate write task?
- How are market price and FX rate sources isolated?

## No Direct Migration From Test-only Engine

`AccountingReplayMinimalEngine` must not be moved directly into `libs`.

Code under `tests/AccountingFixtures` must not become production
implementation. Production code may reuse the fixture contract, expected
outputs, and test approach, but it must redesign module boundaries, error
handling, input sources, performance constraints, and audit behavior.

## Recommended Minimal Production Boundary

- First production stage is read-only.
- Read only from `trade_log` and cash-flow facts.
- Write no tables.
- Generate no snapshots.
- Connect no QML.
- Connect no automatic trading.
- Connect no broker API.
- Connect no TradeDraft lifecycle.
- Execute no strategy.

## DataService Action Readiness

`accounting.replay.preview` is still a guard action.

`position.list`, `cash.summary`, `portfolio.pnl.summary`,
`base_position.summary`, and `sniper_pool.summary` are not implemented.

Any real DataService read-only action must be authorized in a separate task. It
must define transaction and read-only boundaries, include table-count no-write
tests, and specify failure behavior.

## Snapshot Readiness

Snapshot writes are not part of this stage.

Snapshot generation needs separate design. A snapshot must not rewrite
`trade_log` or replace the fact ledger. Stale snapshot detection already has
test-only fixture coverage. Production snapshots need source range,
`calculationVersion`, and `generatedAt`.

## Risks

- Cost basis rules may become more complex than the fixture path.
- Multi-account aggregation can mix account or portfolio boundaries.
- Multi-currency FX rates can cause incorrect aggregation.
- Missing market prices can produce fabricated valuation.
- Stale snapshots can be treated as facts by mistake.
- QML can accidentally recalculate accounting fields.
- DataService actions can cross into writes.
- Test-only code can be misused as production code.
- TradeDraft can be generated too early.
- Automatic trading can be connected before accounting is ready.

## Go / No-Go Checklist

- [ ] Confirmed no direct migration of the test-only engine.
- [ ] Confirmed production module boundary.
- [ ] Confirmed DataService read-only action scope.
- [ ] Confirmed SQLite access boundary.
- [ ] Confirmed no writes.
- [ ] Confirmed no snapshot writes.
- [ ] Confirmed no TradeLog writes.
- [ ] Confirmed no TradeDraft.
- [ ] Confirmed no QML accounting calculation.
- [ ] Confirmed FX001-FX013 tests remain.
- [ ] Confirmed transport stability tests remain.

## Suggested Next Tasks

- TASK-069: Production accounting replay architecture boundary docs.
- TASK-070: AccountingEngine module skeleton without replay logic.
- TASK-071: Production replay DTO parser boundary.
- TASK-072: Read-only replay engine FX001 only.
- TASK-073: DataService position.list contract guard.

These are recommendations only. This document does not implement them.
