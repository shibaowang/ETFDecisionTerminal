# EPIC-277 Strategy Recommendation Engine VBA Parity Matrix

The EPIC-277 parity matrix lives in `samples/strategy_recommendation/` and contains 12 positive samples plus 12 negative fail-closed samples. Each positive fixture includes exact expected status, action code, reason code, tier label, source code, suggested quantity, suggested amount, net cash impact, base-protection status, and cash-limit status.

Positive samples:

- `EPIC277_buy_tier1_etf_direct.json`
- `EPIC277_buy_tier3_cash_capped_etf.json`
- `EPIC277_buy_tier6_cumulative_etf.json`
- `EPIC277_hold_no_signal.json`
- `EPIC277_wait_missing_market_data.json`
- `EPIC277_sell_take_profit_base_safe.json`
- `EPIC277_sell_extreme_premium_base_safe.json`
- `EPIC277_hold_base_protected_sell_blocked.json`
- `EPIC277_otc_replacement_generic_buy.json`
- `EPIC277_zero_fee_quantization.json`
- `EPIC277_decimal_price_quantity.json`
- `EPIC277_existing_position_hold.json`

Negative samples:

- `EPIC277_negative_missing_instrument.json`
- `EPIC277_negative_invalid_price.json`
- `EPIC277_negative_invalid_cash.json`
- `EPIC277_negative_invalid_position_quantity.json`
- `EPIC277_negative_invalid_principal_base.json`
- `EPIC277_negative_unsupported_trade_source.json`
- `EPIC277_negative_sensitive_token.json`
- `EPIC277_negative_broker_order_token.json`
- `EPIC277_negative_unsupervised_execution_token.json`
- `EPIC277_negative_sell_over_position.json`
- `EPIC277_negative_no_cash_for_min_lot.json`
- `EPIC277_negative_invalid_thresholds.json`

The matrix explicitly stays read-only: no TradeDraft, no database writes, no broker, no network, no credentials, no endpoint, and no automatic trading.
