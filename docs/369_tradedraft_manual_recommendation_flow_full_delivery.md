# EPIC-278 TradeDraft Manual Recommendation Flow Full Delivery

EPIC-278 connects the EPIC-277 StrategyEngine recommendation result to a manual TradeDraft flow. A user-confirmed BUY or SELL recommendation can become an internal TradeDraft record with one draft leg and a sanitized audit event.

TradeDraft is not an order. It is not sent to a broker, it does not call a broker endpoint, it does not execute a strategy, and it does not enable automatic trading.

The implemented chain is:

Strategy recommendation payload -> StrategyEngine recommendation -> TradeDraft conversion -> DataService write action `accounting.tradedraft.create_from_recommendation` -> DataAccess TradeDraft repository -> `trade_draft` / `trade_draft_leg` / `audit_log` rows -> read-only summary action `accounting.tradedraft.readonly_summary` -> DataServiceClient -> ShellServices / Presenter mapping.

Scope delivered:

- StrategyEngine converts accepted BUY / SELL recommendations into TradeDraft requests.
- HOLD, WAIT, rejected recommendation, base-protected SELL, missing confirmation, missing idempotency key, zero quantity, and missing amount fail closed.
- DataService registers the create-from-recommendation write action and read-only summary action.
- DataServiceClient exposes typed create and summary adapters.
- ShellServices, Controller, and Presenter expose a manual TradeDraft preview / create contract without QML wiring.
- Duplicate same idempotency key and same digest returns a duplicate result without row growth.
- Same idempotency key and different digest returns a sanitized idempotency conflict without row growth.

Persistence boundary:

- EPIC-278 writes only `trade_draft`, `trade_draft_leg`, and a sanitized `audit_log` event.
- EPIC-278 does not write `trade_log`, `cash_adjustment`, ledger rows, snapshots, read models, cash summaries, position summaries, or PnL.
- EPIC-278 does not change cash, positions, or PnL.
- EPIC-278 does not modify production QML or startup wiring.
- EPIC-278 does not modify broker, network, credential, endpoint, real order placement, or automatic trading code.

The full-delivery CTest is `tradedraft_manual_recommendation_flow_full_delivery`.
