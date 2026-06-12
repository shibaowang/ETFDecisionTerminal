# EPIC-281 Dashboard MVP Full Delivery

EPIC-281 delivers the ShellAccounting Dashboard MVP as a single local manual
trial surface. The page gathers existing capabilities into one read-only-first
workflow and keeps every write or draft operation behind explicit user
confirmation.

## Goals

- Summarize current system status and sanitized issue codes.
- Provide Excel/VBA import entry points for pasted JSON/TXT and local sanitized
  JSON/TXT export preview.
- Persist only an ACCEPTED Excel/VBA preview through the existing authorized
  manual trade/cash/audit path.
- Show post-write readback refresh status and persisted row counts.
- Display portfolio replay read-only cash, position, realized PnL, and
  unrealized PnL summaries.
- Display manual market data refresh results: current price, historical high,
  drawdown, premium/discount, and provider state.
- Display strategy recommendation action, reason, tier, quantity, amount, and
  issue codes.
- Provide a manual TradeDraft creation flow from BUY/SELL recommendation.
- Provide OTCMap multi-leg preview and internal draft creation flow.
- Make all draft output visibly internal draft output, not orders.

## Out Of Scope

- Direct `.xlsx` import.
- Broker integration.
- Credentials.
- Private endpoints.
- Live auto-refresh or background polling.
- Actual order submission.
- Automatic trading.
- TradeDraft execution into `trade_log`.
- Cash-adjustment execution from draft.
- Production DB destructive operations.

## Safety Boundary

Dashboard QML calls only the existing `ShellAccountingPresenter` methods. It
does not directly access DataServiceClient, SQLite, network APIs,
AccountingEngine, StrategyEngine, MarketEngine, broker systems, credentials, or
endpoints. Excel/VBA import persistence is the only user-triggered persistent
write path in this Dashboard, and it remains limited to the already authorized
accepted-preview manual trade/cash/audit flow.
