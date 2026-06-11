# ShellAccounting Portfolio Replay User-visible Summary

EPIC-276 exposes a Presenter-level read-only portfolio replay summary that can
be used by a future UI without allowing QML to calculate accounting values.

## Presenter State

The Presenter state includes:

- portfolio replay busy flag
- last portfolio replay status
- last portfolio replay issue
- last portfolio replay summary
- issue codes
- position count
- cash summary count
- realized PnL
- unrealized PnL

All fields are mapped from DataService / ShellServices results. QML must not
directly call DataServiceClient, SQLite, DataServiceApi, AccountingEngine,
broker, network, credentials, or endpoints.

## User-visible Semantics

Successful replay can show position, cash, and PnL summaries. Partial replay
can show missing market price / unavailable unrealized PnL as sanitized
read-only state. Rejected replay must show sanitized issue codes without raw
payloads, raw paths, credentials, broker order details, stack traces, endpoints,
or automatic trading language.

## Boundary

EPIC-276 does not wire a new QML panel. It only prepares production read-only
Presenter state and CTest coverage for future UI consumption. It does not add
production writes, broker calls, network access, real orders, or automatic
trading.
