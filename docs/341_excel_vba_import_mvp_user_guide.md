# Excel/VBA Import MVP User Guide

This guide is for local trial use of the Excel/VBA import MVP.

## What You Can Do

1. Export sanitized Excel/VBA data as JSON or TXT using the documented contract.
2. Open the ShellAccounting read-only page.
3. Paste the sanitized payload or select a local `.json` / `.txt` export file.
4. Run Preview.
5. Review status, summary, diagnostic codes, and fact counts.
6. If the preview is `ACCEPTED`, explicitly confirm persistence.
7. Click the persist control to write manual trade/cash entries through the
   existing DataService/DataAccess path.
8. Review persisted row counts and post-write refresh status.

## Safe Trial Rules

- Only sanitized JSON/TXT export is supported.
- Direct `.xlsx` import is not supported.
- File paths are shown as basename only.
- Errors are sanitized and must not expose raw payload, credentials, endpoints,
  SQL, stack traces, or full local paths.
- Duplicate imports must not create new rows.
- Idempotency conflicts must not write rows or trigger misleading refresh.

## Status Meanings

- `READY`: no preview has been run.
- `FILE_SELECTED`: a local file basename is selected.
- `PREVIEWING`: preview is in progress.
- `ACCEPTED`: the preview is eligible for explicit persistence.
- `REJECTED`: the payload was parsed but cannot be accepted.
- `INPUT_ERROR`: the payload or local file input is invalid.
- `PERSISTED`: accepted preview was persisted as manual entries.
- `DUPLICATE`: same idempotency key and digest were already persisted.
- `CONFLICT`: same idempotency key with a different digest was rejected.

## Current MVP Boundary

The MVP does not place broker orders, execute strategy, create TradeDrafts,
connect to external endpoints, or perform automatic trading.
