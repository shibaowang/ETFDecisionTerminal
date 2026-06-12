# EPIC-279 OTCMap A/C Multi-Channel Draft Engine Full Delivery

EPIC-279 migrates the OTCMap A/C multi-channel fund split logic into the
production module path. The module turns sanitized in-memory OTCMap payloads
into internal multi-leg TradeDraft previews and, when explicitly confirmed,
persists one `trade_draft` with N `trade_draft_leg` rows plus a sanitized
`audit_log` event.

## VBA Parity Scope

The delivered scope covers:

- OTCMap channel mapping for `strategyCode`.
- A and C fund classes.
- Channel `enabled`, BUY support, SELL support, priority, daily limits, minimum
  buy/sell amounts, NAV, and available holding quantity.
- BUY allocation by priority, daily buy limit, minimum amount, and cash cap.
- SELL allocation with C-class-first ordering, daily sell limit, holding
  quantity, and 20 percent base position protection.
- Decimal NAV quantity calculation using the documented MVP rounding rule.
- Duplicate same idempotency key plus same digest with no row growth.
- Same idempotency key plus different digest as a sanitized conflict.
- Sanitized diagnostics for invalid input and unsafe tokens.

## Boundaries

OTCMap output is a draft generation path, not order execution. EPIC-279 does
not write `trade_log` or `cash_adjustment`, does not mutate positions, cash, or
PnL, does not refresh market data, does not fetch historical highs, does not
access broker, network, credentials, or endpoints, does not place real orders,
and does not enable automatic trading.

The engine is pure in-memory. Persistence is owned by
`ShellAccountingTradeDraftRepository::createTradeDraftWithLegs` and is limited
to `trade_draft`, `trade_draft_leg`, and `audit_log` inside one transaction.

## Delivery

- Engine: `StrategyEngine/OtcMapAcMultiChannelDraftEngine`.
- Read-only preview action: `accounting.otcmap_multichannel.readonly_preview`.
- Write action: `accounting.tradedraft.create_otcmap_multichannel`.
- Client adapters: `DataServiceClient` typed OTCMap preview/create methods.
- Shell contract: ShellServices port, adapter, controller, and Presenter
  mapping for preview and explicit draft creation.
- Samples: `samples/otcmap_multichannel/`.
- CTest: `otcmap_ac_multichannel_draft_engine_full_delivery`.

