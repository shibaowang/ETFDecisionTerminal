# EPIC-277 Strategy Recommendation Engine User Visible Summary

The user-visible recommendation summary is read-only. It may show recommendation status, action, source, reason, tier, suggested quantity, suggested amount, net cash impact, and issue codes.

The summary is not a TradeDraft and is not an order ticket. It does not persist anything, does not refresh market data, does not fetch historical highs, does not connect to broker systems, does not read credentials or endpoints, and does not perform automatic trading.

Expected states:

- `BUY`: a buy tier is reached and the minimum quantity or OTC amount can be satisfied.
- `SELL`: a sell condition is reached and base-position protection allows a sell.
- `HOLD`: a position exists but no sell condition applies, or base protection blocks a sell.
- `WAIT`: required market data is missing or no build-position condition is reached.
- `REJECTED`: input fails closed with sanitized issue codes.

EPIC-277 exposes the summary through DataServiceClient and ShellServices / Presenter mapping. QML may later display the fields, but EPIC-277 does not add write buttons, TradeDraft controls, broker controls, or automatic trading controls.
