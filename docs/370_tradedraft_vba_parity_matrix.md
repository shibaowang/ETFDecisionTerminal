# EPIC-278 TradeDraft VBA Parity Matrix

The EPIC-278 TradeDraft matrix lives in `samples/trade_draft/`. It contains eight positive samples and twelve fail-closed negative samples.

Positive samples:

- `EPIC278_buy_tier1_to_draft.json`
- `EPIC278_buy_cash_capped_to_draft.json`
- `EPIC278_sell_take_profit_to_draft.json`
- `EPIC278_sell_extreme_premium_to_draft.json`
- `EPIC278_otc_generic_buy_to_draft.json`
- `EPIC278_zero_fee_to_draft.json`
- `EPIC278_decimal_quantity_to_draft.json`
- `EPIC278_duplicate_same_key_same_digest.json`

Negative samples:

- `EPIC278_negative_hold_not_eligible.json`
- `EPIC278_negative_wait_not_eligible.json`
- `EPIC278_negative_rejected_recommendation.json`
- `EPIC278_negative_base_protected_sell_blocked.json`
- `EPIC278_negative_missing_confirmation.json`
- `EPIC278_negative_missing_idempotency_key.json`
- `EPIC278_negative_idempotency_conflict.json`
- `EPIC278_negative_broker_order_token.json`
- `EPIC278_negative_credential_token.json`
- `EPIC278_negative_automatic_trading_token.json`
- `EPIC278_negative_zero_quantity.json`
- `EPIC278_negative_missing_amount.json`

Every sample is sanitized. No sample contains real accounts, real broker data, credentials, endpoints, or live order instructions.

Matrix expectations:

- BUY recommendations can create TradeDrafts after explicit user confirmation.
- SELL recommendations can create TradeDrafts only when base-position protection allows the SELL.
- HOLD and WAIT recommendations are not eligible for TradeDraft creation.
- Rejected recommendations are not eligible for TradeDraft creation.
- Broker, credential, endpoint, and automatic-trading token samples fail closed.
- Duplicate same key / same digest does not grow `trade_draft`, `trade_draft_leg`, or `audit_log`.
- Same key / different digest returns a sanitized conflict.

TradeDraft is not an order, and this matrix does not permit broker submission, network access, credential access, endpoint access, real order placement, or automatic trading.
