# ShellAccounting Manual Entry Replay Fixture Matrix Authorization Gate

## Fixture Matrix Purpose

TASK-214 is gate-only / fixture-matrix-authorization-only.
TASK-214 does not implement replay.
TASK-214 does not create runtime fixture data / JSON fixture files.
TASK-214 does not modify production code.
TASK-214 does not modify DataServiceActions.
TASK-214 does not modify DataServiceActionRegistrar.
TASK-214 does not modify repositories.
TASK-214 does not modify migrations.
TASK-214 does not modify QML / startup / Presenter / Controller / ShellServices adapter / port.
TASK-214 does not call AccountingEngine replay.
TASK-214 does not add runtime SQL / SQLite read/write.
TASK-214 does not write audit / ledger.
TASK-214 does not write snapshot.
TASK-214 does not implement backup/export/restore.
TASK-214 does not connect broker / network / credentials / endpoint.
TASK-214 does not place real orders.
TASK-214 does not enable automatic trading.
Broker sandbox new capability remains paused.

The replay fixture matrix is a future input / output acceptance contract for
manual entry replay implementation. TASK-214 authorizes fixture matrix design
only. It does not implement replay, create actual JSON fixture files, create a
seed database, or write runtime fixtures. The matrix must inherit the TASK-213
manual entry replay policy and must cover manual BUY, SELL, Deposit, and
Withdrawal facts before a future implementation task can create fixtures.

## Fixture Naming Policy

Future manual replay fixtures must use stable ids from `MRF001` to `MRF0xx`.
The ids are manual replay fixture numbers and must not reuse meanings after a
case is assigned. Each future fixture must document:

- id.
- title.
- purpose.
- source facts.
- expected position output.
- expected cash output.
- expected PnL output.
- expected issues.
- blocking / non-blocking status.
- privacy expectation.

Fixture ids must not depend on test execution order. Fixture ids must not
depend on QML display order.

## Fixture Source Fact Policy

Future fixture source facts may come only from persisted manual facts
candidates. Manual transaction facts come from `trade_execution_group` plus
`trade_log`. Manual cash movement facts come from `trade_log` plus
`cash_adjustment`.

Future fixtures must not use QML raw payload, broker payload, real order id,
credentials, endpoint, raw SQL, or backup/export payload. Source facts must
include account, portfolio, instrument, and currency scope when applicable.

## Baseline Success Fixture Matrix

The future baseline success matrix must define at least these design cases:

- `MRF001` empty manual facts -> empty / unavailable-safe output.
- `MRF002` single BUY -> position quantity increase + cash outflow candidate.
- `MRF003` BUY + Deposit -> position + cash baseline.
- `MRF004` BUY + SELL partial reduction -> reduced position candidate.
- `MRF005` Deposit + Withdrawal -> cash movement baseline.
- `MRF006` BUY + SELL + Deposit + Withdrawal combined daily-use baseline.

Each fixture listed here is design authorization only. TASK-214 does not
implement any fixture or create fixture files.

## Ordering And Idempotency Fixture Matrix

The future ordering and idempotency matrix must define at least:

- same timestamp deterministic ordering.
- same execution group deterministic leg order.
- duplicate requestId / idempotencyKey no double consumption.
- out-of-order source rows deterministic replay.
- created_at vs occurred_at ordering decision candidate.
- stable uid / id tie-breaker candidate.
- SQLite unsorted return order must not affect replay.

## Position Safety Fixture Matrix

The future position safety matrix must define at least:

- SELL exceeds position -> fail-closed or blocking issue.
- negative position not silently accepted.
- zero quantity unsupported.
- invalid quantity unsupported.
- missing instrument unsupported.
- same instrument across multiple accounts remains isolated.
- same instrument across multiple portfolios remains isolated.
- holding cost candidate remains unavailable or explicitly policy-bound if cost
  basis unsupported.

## Cash Safety Fixture Matrix

The future cash safety matrix must define at least:

- Withdrawal exceeds cash -> fail-closed or blocking issue.
- negative cash not silently accepted.
- BUY cash outflow with insufficient cash -> explicit unsupported / blocking
  issue.
- fee / tax cash impact requires policy.
- missing currency unsupported.
- multi-currency unsupported or unavailable-safe.
- Deposit / Withdrawal cash adjustment linkage must be deterministic.

## PnL And Market Dependency Fixture Matrix

The future PnL and market dependency matrix must define at least:

- realized PnL unavailable until cost basis / lot policy exists.
- unrealized PnL unavailable until market price policy exists.
- market value unavailable without market price.
- no fabricated PnL.
- no fabricated market value.
- stale market price behavior requires future authorization.
- `portfolio.pnl.summary` safe partial output remains unavailable-safe.

## Multi-Scope Fixture Matrix

The future multi-scope matrix must define at least:

- multi-account isolation.
- multi-portfolio isolation.
- multi-instrument isolation.
- aggregate output requires explicit scope policy.
- cross-account contamination must be blocking.
- cross-portfolio contamination must be blocking.
- same symbol / different instrument identity must be deterministic.

## Unsupported And Failure Fixture Matrix

The future unsupported and failure matrix must define at least:

- malformed persisted fact.
- unsupported fact type.
- missing account.
- missing portfolio.
- missing instrument.
- missing currency.
- duplicate fact ambiguity.
- correction / void facts unsupported.
- broker facts unsupported.
- backup/export/restored facts unsupported until separately authorized.

## Expected Output Policy

Future fixture expected outputs must document:

- position quantity candidate.
- holding cost candidate.
- cash balance candidate.
- safe partial PnL candidate.
- unavailable fields.
- issue list.
- blocking issue flag.
- calculation metadata.
- source fact trace ids.
- privacy-safe display text.

TASK-214 does not change TASK-207 readback semantics. TASK-214 does not change
TASK-209 / TASK-211 daily-use acceptance semantics. TASK-214 does not write
snapshot. TASK-214 does not refresh read model. TASK-214 does not call replay.

## Privacy And Sanitization Policy

Future fixture expected errors and metadata must not expose:

- raw SQL.
- raw payload.
- credentials.
- endpoint.
- broker payload.
- internal exception stack.
- full `trade_log` payload.
- full `cash_adjustment` payload.

Only sanitized issue code, sanitized memo, and safe metadata are allowed.
Fixture expected errors must use sanitized issue codes.

## Test Implementation Boundary

TASK-214 tests may only perform documentation token checks, path allowlist
checks, and no-production-drift static checks. TASK-214 tests must not execute
replay, write SQLite, call AccountingEngine replay, introduce network / broker
or credentials, or create fixture JSON / seed DB files.

Future TASK-215 is the first task that may authorize creation of fixture files.

## Out-of-Scope Boundaries

TASK-214 does not authorize:

- replay implementation.
- AccountingEngine replay calls.
- replay fixture JSON files.
- seed DB files.
- runtime fixture data.
- runtime SQL / SQLite read/write.
- production code changes.
- QML / startup / Presenter / Controller / ShellServices production changes.
- DataServiceActions or DataServiceActionRegistrar changes.
- repository changes.
- migration changes.
- audit / ledger writes.
- snapshot writes.
- read model refresh.
- backup/export/restore implementation.
- broker / network / credentials / endpoint integration.
- real broker order id generation.
- real order placement.
- automatic trading.

## Formal Authorization Conclusion And Next Task

TASK-214 authorizes replay fixture matrix design only.
TASK-214 does not authorize replay implementation.
TASK-214 does not authorize creation of replay fixture files.
TASK-214 does not authorize AccountingEngine replay calls.
TASK-214 does not authorize runtime SQL / SQLite read/write.
TASK-214 does not authorize audit / ledger writes.
TASK-214 does not authorize backup/export/restore.
TASK-214 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-215 manual entry replay fixture files authorization gate.
