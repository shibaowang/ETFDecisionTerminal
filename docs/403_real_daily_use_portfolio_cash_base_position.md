# EPIC-289 Real Daily Use Portfolio, Cash, and Base Position

## Read Model Sources

The dashboard reads daily-use facts from the explicit local DB path:

```text
.local/daily_use/etfdt_daily_use.sqlite
```

Holdings must come from manual/imported `trade_log` replay. Remaining cash must
come from replayed cash impact and cash adjustments. EPIC-289-FIX includes the
`cash_adjustment` path in the cash calculation when the schema exposes an amount
column; when only a `trade_log` linkage exists, the snapshot uses the linked cash
effect and reports the missing amount column explicitly. The dashboard must not
use sample holdings, sample cash, or mock market values as real daily-use data.

## Visible Portfolio Fields

Required daily-use fields:

- current holdings by account / portfolio / instrument
- remaining cash
- total assets when market data is available
- total market value when market data is available
- floating PnL when market data is available
- ETF current price
- ETF historical high
- ETF drawdown from high
- index current point
- index historical high
- index drawdown from high

When market prices are unavailable, market value and floating PnL must be shown
as unavailable, not fabricated.

When market prices are available, `totalMarketValueText`, `totalAssetsText`, and
`floatingPnlText` must be concrete values instead of
`CALCULABLE_WITH_MARKET_DATA`.

## Base Position

If the base position target config is missing, show:

```text
缺少底仓目标配置，无法计算底仓完成度。
```

Evidence must include:

```json
{"basePositionTargetMissing": true}
```

If the target is available through the daily-use base-position allocation table,
completion is calculated from real target and allocated amounts. If the target
is unavailable, the missing-config prompt remains explicit.

ETF market instruments are derived from real holdings. ETF-to-index data is used
only when an explicit mapping is present in configuration or imported payload;
missing mappings are surfaced as a sanitized issue and are not silently mapped to
`000300`.

This portfolio/cash/base-position readback never enables broker access, real
order placement, or automatic trading.
