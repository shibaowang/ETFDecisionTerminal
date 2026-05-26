# Accounting Replay Incremental Implementation Plan

## Status

FX001-FX013 test-only minimal engine coverage is complete.

- FX001 completed.
- FX002 completed.
- FX003 completed.
- FX004 completed.
- FX005 completed.
- FX006 completed.
- FX007 completed.
- FX008 completed.
- FX009 completed.
- FX010 completed.
- FX011 completed.
- FX012 completed.
- FX013 completed.

This means `tests/AccountingFixtures` has fixture-backed test-only minimal
coverage. It does not mean production accounting replay is complete.

## Boundary

The test-only minimal engine must stay in `tests/AccountingFixtures`.

Do not directly migrate `AccountingReplayMinimalEngine`, fixture helper code, or
test-only assertion code into production libraries. Production replay must be
designed as a separate architecture boundary.

## Next Step

The next step is readiness review and production architecture boundary design,
not a direct DataService action.

Recommended next work:

- Production accounting replay architecture boundary documentation.
- Production replay engine placement decision.
- DataService read-only action boundary.
- SQLite read-only source boundary.
- Snapshot write boundary, still requiring separate authorization.
- TradeLog write boundary, still requiring separate authorization.

## Constraints

- No production replay without a separate task.
- No DataService action without a separate task.
- No snapshot writes without a separate task.
- No TradeLog writes without a separate task.
- No QML accounting calculation.
- No direct `DataServiceClient` calls from QML.
- No TradeDraft generation.
- No automatic trading.
- No broker API.
