# EPIC-276 ShellAccounting Portfolio Replay VBA Parity Full Delivery

EPIC-276 delivers the ShellAccounting portfolio replay / position / cash / PnL
read-only projection path for sanitized VBA-style replay facts. It moves the
Excel/VBA import MVP from import persistence readiness toward a production
read-only portfolio summary module that can be exercised through samples,
DataService, DataServiceClient, ShellServices, and Presenter state.

## Scope

- Add a production AccountingEngine read-only replay module for sanitized
  trade, cash, and market price facts.
- Add a read-only DataService action:
  `accounting.portfolio_replay.readonly_summary`.
- Add typed DataServiceClient and ShellServices / Presenter readback mapping.
- Add ten positive and ten negative EPIC-276 parity samples under
  `samples/shell_accounting_replay/`.
- Add local-service and Presenter CTest coverage that proves replay evidence
  without touching production databases.

## Boundary

The replay module is read-only. It does not write SQLite, audit_log, ledger,
snapshot, trade_log, read models, or production database paths. It does not
generate TradeDraft, run strategy logic, access broker, network, credentials,
or endpoints, place real orders, or enable automatic trading.

EPIC-276 does not modify migrations, DataAccess write semantics, Excel/VBA
import persistence write semantics, production QML, startup wiring, broker
integration, or any automatic trading behavior.

## Runtime Contract

Input is a sanitized in-memory JSON object using
`schemaVersion=shell-accounting-portfolio-replay/v1`. It may contain:

- `inputFacts.cashFacts`
- `inputFacts.tradeFacts`
- `inputFacts.marketPriceFacts`

The output is a sanitized read-only summary containing:

- replay status and issue codes
- position summaries
- cash summaries
- realized / unrealized PnL
- no-write evidence flags

## Acceptance

The CTest `shell_accounting_portfolio_replay_vba_parity_full_delivery` verifies
the full EPIC-276 scope, including sample parity, negative fail-closed behavior,
DataService registration, real DataServiceClient use, Presenter mapping, and
no production database writes.
