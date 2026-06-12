# EPIC-278 TradeDraft Contract

TradeDraft is an internal manual draft contract. It captures a user-confirmed recommendation as a reviewable draft and draft leg. TradeDraft is not an order and is not an execution instruction.

Create-from-recommendation request requirements:

- `recommendationPayloadJson` is a sanitized StrategyEngine recommendation input.
- `userConfirmed` must be true.
- `idempotencyKey` must be present.
- `recommendationDigest` must be present.
- `accountId`, `portfolioId`, `strategyId`, and `instrumentId` must reference existing rows.
- The recommendation must be accepted and map to BUY or SELL.
- Quantity and amount must be positive.
- Base-protected SELL must fail closed.

Create result requirements:

- Successful creation returns a draft id, draft uid, draft signature, status, side, instrument code, quantity, amount, net cash impact, and issue codes.
- Duplicate same idempotency key and same digest maps to duplicate without writing new rows.
- Same idempotency key and different digest maps to sanitized idempotency conflict without writing new rows.
- Ineligible recommendations return sanitized no-write results.

Read-only summary requirements:

- `accounting.tradedraft.readonly_summary` returns only TradeDraft summary fields.
- The summary action does not write SQLite, audit, ledger, snapshot, trade_log, cash_adjustment, or read model rows.

Boundary requirements:

- TradeDraft is not an order.
- TradeDraft creation does not write `trade_log` or `cash_adjustment`.
- TradeDraft creation does not change cash, position, PnL, or market data.
- TradeDraft creation does not call AccountingEngine replay.
- TradeDraft creation does not call broker, network, credentials, endpoints, real order placement, or automatic trading.

Future work may convert a reviewed TradeDraft into a manual transaction only through separately authorized confirmation / persistence tasks.
