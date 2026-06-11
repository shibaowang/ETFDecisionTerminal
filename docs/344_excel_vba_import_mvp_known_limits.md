# Excel/VBA Import MVP Known Limits

- Only sanitized JSON/TXT export files are supported.
- Direct `.xlsx` import is not supported.
- Production file loading is limited to the existing local JSON/TXT loader.
- Market price and FX rate persistence are not supported.
- Duplicate import and idempotency conflict behavior is intentionally
  conservative.
- Errors are sanitized; raw payloads, full local paths, credentials, endpoints,
  SQL, and stack traces must not be displayed.
- Broker order placement is not part of this MVP.
- Network access, external endpoints, credentials, real order placement, and
  automatic trading are not part of this MVP.
- TradeDraft generation and strategy execution are not part of this MVP.

Future direct spreadsheet import, broader format support, and production
release packaging require separate authorization.
