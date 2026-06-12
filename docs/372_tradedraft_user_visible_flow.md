# EPIC-278 TradeDraft User-Visible Flow

EPIC-278 exposes a production C++ Presenter contract for turning the latest accepted recommendation into an internal TradeDraft. It does not wire QML controls in this task.

The intended user-visible flow is:

1. User previews a sanitized StrategyEngine recommendation.
2. Presenter stores the accepted recommendation payload.
3. User requests a TradeDraft preview.
4. Presenter shows that explicit confirmation is required.
5. User explicitly confirms.
6. Presenter calls the ShellServices / Controller / DataService path.
7. DataService creates a TradeDraft and one draft leg.
8. DataService returns sanitized status, summary, and issue codes.
9. Presenter fetches the read-only TradeDraft summary.

Visible state includes:

- TradeDraft status.
- TradeDraft id.
- Side.
- Instrument code.
- Quantity.
- Amount.
- Net cash impact.
- Issue codes.
- Duplicate and idempotency conflict state.
- Explicit confirmation required state.

The user-facing wording must remain clear that TradeDraft is not an order. No broker order is submitted, no network endpoint is called, and no automatic trading is enabled.

No Import, Confirm order, broker order, or automatic trading control is added by EPIC-278.
