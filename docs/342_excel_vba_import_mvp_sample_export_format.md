# Excel/VBA Import MVP Sample Export Format

The MVP import format is a sanitized JSON object. It may be saved as `.json` or
`.txt`. This is the MVP JSON/TXT export contract. Direct `.xlsx` import is not
supported.

## Required Root Fields

```json
{
  "schemaVersion": "excel-vba-export/v1",
  "source": "sanitized-excel-vba-export",
  "sheets": []
}
```

## Sheet Contract

Each sheet must include `sheetName`, `headers`, and `rows`. The optional `name`
field is accepted by current UI helpers for display compatibility. Release
samples include both `name` and `sheetName`.

```json
{
  "name": "TradeLog",
  "sheetName": "TradeLog",
  "headers": ["ROW_ID", "TRADE_TIME_UTC", "ACCOUNT_CODE"],
  "rows": [["ROW_001", "2026-01-05T09:30:00Z", "DEMO_ACCOUNT"]]
}
```

## Supported MVP Sheets

- `InitialCash`: initial or cash adjustment facts.
- `TradeLog`: manual trade facts.

Market price and FX rate persistence are not part of the current MVP.

## Sanitization Requirements

Do not include raw user account names, production file paths, SQL, credentials,
endpoints, stack traces, broker payloads, real order identifiers, or secrets.

See:
`samples/excel_vba_import/TASK272_sanitized_excel_vba_import_mvp_sample.json`.
