# EPIC-289 Real Daily Use Portfolio, Cash, and Base Position

## Read Model Sources

The dashboard reads daily-use facts from the explicit local DB path:

```text
.local/daily_use/etfdt_daily_use.sqlite
```

Holdings must come from manual/imported `trade_log` replay. Remaining cash must
come from replayed cash impact and cash adjustments. The dashboard must not use
sample holdings, sample cash, or mock market values as real daily-use data.

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

## Base Position

If the base position target config is missing, show:

```text
缺少底仓目标配置，无法计算底仓完成度。
```

Evidence must include:

```json
{"basePositionTargetMissing": true}
```

If the target is available in a future task, completion must be calculated from
real imported holdings and real target config only.

This portfolio/cash/base-position readback never enables broker access, real
order placement, or automatic trading.
