# EPIC-277 Strategy Recommendation Engine Test Plan

Required CTest: `strategy_recommendation_engine_vba_parity_full_delivery`.

The test verifies:

- 12 positive samples produce exact StrategyEngine recommendations.
- 12 negative samples fail closed with exact sanitized issue codes.
- ETF lot rounding is exact.
- Cash-capped buy fallback is exact.
- Tier 1, tier 3, and tier 6 buy recommendations are exact.
- Take-profit sell and extreme-premium sell are exact.
- Base-position protection blocks unsafe sells.
- OTC replacement returns only a generic read-only recommendation.
- DataService read-only action `strategy.recommendation.readonly_summary` is registered.
- A real DataServiceClient calls the action through a local service host.
- ShellServices / Presenter maps status, action, source, reason, tier, quantity, amount, net cash impact, and issue codes.
- No trade_log, cash_adjustment, or audit_log rows are written in the temp SQLite database.
- No production DB, broker, network, credential, endpoint, real order placement, or automatic trading is touched.

Regression commands must include the EPIC-276 portfolio replay CTest, EPIC-275/274 Excel/VBA import hardening and compatibility tests, EPIC-273 demo workspace, EPIC-272 release readiness, TASK-271 local service E2E, full CTest, and transport echo repeat.
