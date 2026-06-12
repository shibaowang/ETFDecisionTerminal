# EPIC-279 OTCMap A/C Multi-Channel VBA Parity Matrix

The sample matrix under `samples/otcmap_multichannel/` documents the EPIC-279
VBA parity contract.

## Positive Samples

- `EPIC279_buy_single_channel_A.json`
- `EPIC279_buy_split_A_C_by_limit.json`
- `EPIC279_buy_cash_capped_multi_channel.json`
- `EPIC279_buy_skip_disabled_channel.json`
- `EPIC279_buy_skip_below_min_amount.json`
- `EPIC279_sell_c_class_first.json`
- `EPIC279_sell_split_c_then_a.json`
- `EPIC279_sell_base_protection_pass.json`
- `EPIC279_sell_partial_due_daily_limit.json`
- `EPIC279_otc_generic_recommendation_to_multileg_draft.json`
- `EPIC279_duplicate_same_key_same_digest.json`
- `EPIC279_decimal_nav_quantity.json`

## Negative Samples

- `EPIC279_negative_no_enabled_channels.json`
- `EPIC279_negative_no_buy_supported_channels.json`
- `EPIC279_negative_no_sell_supported_channels.json`
- `EPIC279_negative_invalid_nav.json`
- `EPIC279_negative_invalid_limit.json`
- `EPIC279_negative_invalid_cash.json`
- `EPIC279_negative_no_holding_to_sell.json`
- `EPIC279_negative_base_protection_blocks_sell.json`
- `EPIC279_negative_below_min_amount.json`
- `EPIC279_negative_missing_strategy_code.json`
- `EPIC279_negative_sensitive_token.json`
- `EPIC279_negative_broker_order_token.json`
- `EPIC279_negative_credential_endpoint_token.json`
- `EPIC279_negative_automatic_trading_token.json`
- `EPIC279_negative_idempotency_conflict.json`

## Assertions

Positive samples must produce exact leg order, fund class, side, amount,
quantity, NAV, priority, source, and net cash impact. Negative samples must
fail closed with sanitized issue codes and no database writes.
All samples are draft-only: they must not write `trade_log`,
`cash_adjustment`, persistent read models, broker state, endpoint state, or
automatic trading state.

The matrix verifies BUY split by channel limits, cash-capped BUY, disabled
channel skipping, below-minimum skipping, SELL C-class-first ordering, SELL
split C then A, base protection pass/block behavior, daily limit partial SELL,
and decimal NAV quantities.
