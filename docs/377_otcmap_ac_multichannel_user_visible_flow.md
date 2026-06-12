# EPIC-279 OTCMap A/C Multi-Channel User-visible Flow

OTCMap creates internal draft proposals for review. It does not submit orders.

## Flow

1. User or service provides a sanitized OTCMap in-memory JSON payload.
2. Preview calls `accounting.otcmap_multichannel.readonly_preview`.
3. The engine returns leg count, totals, leg details, and sanitized issue codes.
4. The user explicitly confirms that this is an internal draft, not an order.
5. Create calls `accounting.tradedraft.create_otcmap_multichannel`.
6. DataAccess writes one `trade_draft`, N `trade_draft_leg` rows, and one
   sanitized `audit_log` row in one transaction.
7. Readback returns the multi-leg draft summary.

## Visible Statuses

- `DRAFT_ELIGIBLE`: preview can become a draft after explicit confirmation.
- `REJECTED`: payload or channel matrix failed closed.
- `DRAFT_CREATED`: internal TradeDraft was persisted.
- `DUPLICATE`: same idempotency key and digest already exist.
- `IDEMPOTENCY_CONFLICT`: same key was reused with different digest.

## Out of Scope

No broker submit, no real order placement, no automatic trading, no
`trade_log`, no `cash_adjustment`, no position/cash/PnL mutation, no market
data refresh, and no historical high fetch are part of this flow.

