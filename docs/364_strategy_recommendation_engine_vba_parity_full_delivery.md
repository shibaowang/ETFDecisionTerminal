# EPIC-277 Strategy Recommendation Engine VBA Parity Full Delivery

EPIC-277 delivers a production StrategyEngine read-only recommendation module for the VBA strategy recommendation behavior. The module evaluates sanitized in-memory strategy rows and returns a deterministic recommendation summary. It does not persist, create TradeDrafts, submit broker orders, read credentials, access endpoints, or perform automatic trading.

The parity scope covers strategy row input, current ETF price / OTC NAV, index drawdown, premium, return rate, target buy grid amount, cumulative tier remaining amount, buy recommendation, cash-capped buy fallback, sell recommendation, extreme-premium sell, take-profit sell, base-position protection, ETF lot quantization, OTC generic replacement recommendation, and fail-closed issue behavior.

Out of scope for EPIC-277:

- TradeDraft persistence.
- trade_log, cash_adjustment, audit_log, ledger, snapshot, or read model writes.
- OTCMap A/C multi-channel legs, deferred to EPIC-279.
- live market refresh and historical high auto-fetch, deferred to EPIC-280.
- broker, network, credential, endpoint, real order placement, or automatic trading.

The read-only chain is:

Strategy recommendation payload -> StrategyEngine read-only recommendation -> DataService read-only action `strategy.recommendation.readonly_summary` -> DataServiceClient typed adapter -> ShellServices / Presenter mapping.

The CTest for this delivery is `strategy_recommendation_engine_vba_parity_full_delivery`.
