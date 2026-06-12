# Dashboard MVP User Flow

## Flow A: Import Old Workbook Export

1. User opens the Dashboard.
2. User loads a sanitized JSON/TXT Excel/VBA export.
3. User previews the export.
4. Preview shows ACCEPTED or REJECTED.
5. User explicitly confirms persistence for an ACCEPTED preview.
6. Persist writes only the authorized trade/cash/audit path.
7. UI shows persisted row counts.
8. UI shows post-write readback refresh status.

## Flow B: Portfolio Replay

1. User clicks the read-only replay action.
2. UI shows cash, position, realized PnL, and unrealized PnL summary fields.
3. UI shows sanitized issue codes.
4. No DB write occurs.

## Flow C: Market Data Refresh

1. User clicks manual market refresh.
2. Default provider state is fixture or disabled.
3. UI shows current price, historical high, drawdown, and premium/discount.
4. UI shows provider disabled, stale, partial, or OK states.
5. QML performs no direct network access.
6. No background polling is started.

## Flow D: Strategy Recommendation

1. User runs a read-only recommendation.
2. UI shows BUY, SELL, HOLD, WAIT, or REJECTED.
3. UI shows reason, tier, source, quantity, and amount.
4. No draft is created automatically.

## Flow E: TradeDraft

1. BUY/SELL recommendation is visible.
2. UI shows draft preview from the latest eligible recommendation.
3. User checks explicit confirmation: "This is only a draft, not an order."
4. User creates an internal TradeDraft.
5. UI shows draft id, status, duplicate, or conflict state.
6. No broker, order submission, `trade_log`, or `cash_adjustment` write occurs.

## Flow F: OTCMap

1. User loads or uses the sanitized OTCMap sample payload.
2. UI previews multi-leg A/C split.
3. UI shows leg count, total amount, C-first sell or buy split status.
4. User explicitly confirms internal draft creation.
5. UI shows multi-leg draft summary.
6. No broker, order submission, `trade_log`, or `cash_adjustment` write occurs.
