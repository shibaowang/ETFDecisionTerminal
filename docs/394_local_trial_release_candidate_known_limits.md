# EPIC-282 Known Limits

- Direct `.xlsx` import is not supported.
- The MVP supports sanitized JSON/TXT export files only.
- Live market provider access is disabled / deferred by default.
- Broker integration is not part of this release candidate.
- Real order placement is not part of this release candidate.
- Automatic trading is not part of this release candidate.
- TradeDraft is an internal draft, not an order.
- OTCMap multi-leg draft is not execution.
- PushPlus / alert integration is not included.
- Installer packaging is not included unless separately authorized later.
- This RC runbook targets Windows local trial usage.
- Production DB usage is not part of the local trial.
- Paid or private market data is not included.
- No investment outcome is guaranteed.
- Dashboard user-facing labels are localized to Chinese for trial users, but
  stable technical identifiers remain English where they are part of a contract:
  QML `objectName`, CTest names, JSON schema keys, action names, enum values,
  and file names.
- `Accepted`, `Rejected`, `Partial`, `Stale`, `Duplicate`, and `Idempotency
  conflict` may still appear as raw status codes in evidence or diagnostics;
  the visible Dashboard labels explain them in Chinese.
- Internal TradeDraft / OTCMap outputs remain drafts only. The Chinese UI text
  must say they are not orders and will not be submitted to a broker.
- Market data remains fixture/disabled by default. The Chinese UI text must say
  it uses sample data or a disabled provider and will not automatically connect
  to the network.
