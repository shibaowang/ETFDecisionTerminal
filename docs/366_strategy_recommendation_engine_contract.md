# EPIC-277 Strategy Recommendation Engine Contract

The StrategyEngine contract accepts sanitized in-memory JSON containing an `input` object. The core fields are account, portfolio, instrument, strategy, trade source, current price, OTC NAV, index drawdown, premium, return rate, principal base, holding cost, position quantity, average cost, cash balance, target grid amount, cumulative tier remaining amount, buy thresholds, sell thresholds, base-position ratio, ETF lot size, fee rate, and fixed fee.

The output contract returns status, action code, action label, source code, source label, reason code, reason label, tier label, target amount, suggested quantity, suggested amount, net cash impact, fee, base-protection status, cash-limit status, and sanitized issues.

Buy logic:

- A matching buy tier is selected from configured drawdown thresholds.
- Target amount is bounded by tier target, grid target, and cumulative remaining amount.
- ETF direct quantity is rounded down to the configured lot size, default 100.
- Cash-capped fallback is allowed only when at least one minimum lot can be bought.
- OTC replacement returns one generic read-only recommendation and does not split A/C channel legs.

Sell logic:

- Extreme premium has precedence over take-profit premium and return-threshold sells.
- Sell quantity is bounded by position quantity and base-position protection.
- Remaining cost basis must stay above `principalBase * basePositionRatio` with one-cent tolerance.
- ETF sell quantity is rounded down to lot size.

The engine is deterministic and fail closed for invalid input, sensitive tokens, broker/order tokens, automatic trading tokens, unsupported trade source, invalid thresholds, insufficient cash for minimum ETF lot, and requested sells over position. The engine remains read-only and never writes SQLite, trade_log, cash_adjustment, audit_log, ledger, snapshot, or read models.
