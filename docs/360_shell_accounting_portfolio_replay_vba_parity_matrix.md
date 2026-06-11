# ShellAccounting Portfolio Replay VBA Parity Matrix

EPIC-276 defines a read-only VBA parity matrix for ShellAccounting replay.

## Positive Samples

The positive sample set covers:

- buy only
- buy then partial sell
- multi-buy average cost
- sell all position
- cash in / cash out
- multi-instrument replay
- zero fee
- decimal quantity / price / fee
- ETF and OTC split representation
- unrealized PnL when market price is provided

Each positive sample must exactly match expected cash, position, realized PnL,
unrealized PnL, fee, and market value projections.

## Negative Samples

The negative sample set covers:

- sell over position
- duplicate fact id
- invalid quantity
- invalid price
- unsupported trade side
- unsupported cash action
- missing instrument
- currency mismatch
- sensitive token rejection
- broker order token rejection

Each negative sample must fail closed with sanitized issue codes. Negative
samples must not produce cash summaries, positions, writes, broker calls,
network access, endpoint access, credentials access, real order placement, or
automatic trading.

## Data Quality

Missing market prices produce a safe partial read-only projection. Market value
and unrealized PnL are unavailable unless a sanitized market price fact is
provided. No QML calculation is part of this matrix.
