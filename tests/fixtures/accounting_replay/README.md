# Accounting Replay Fixtures

These JSON files are static accounting replay fixture samples for future tests.
They are not database rows, do not write SQLite, do not call services, do not
drive trading, and do not generate TradeDraft.

`fixtures_index.json` lists FX001-FX013. Each fixture contains input facts,
expected read-only DTO-shaped outputs, expected issues, blocking status, and
notes. The validator checks structure only; it does not calculate cash,
positions, PnL, or accounting correctness.
