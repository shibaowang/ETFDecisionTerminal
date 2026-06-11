# Excel VBA Import MVP Export Helper

`ExcelVbaImportMvpExportHelper.bas` is a local workbook-side export helper for
Excel 2010-compatible VBA. It reads only the current workbook `InitialCash` and
`TradeLog` sheets and writes a user-selected `.json` or `.txt` file that follows
the MVP sanitized JSON/TXT contract.

Direct `.xlsx` import is not supported. The supported path is:

```text
Excel/VBA workbook -> sanitized JSON/TXT export -> ETFDecisionTerminal MVP preview
```

## Required Sheets

- `InitialCash`
- `TradeLog`

Required columns are documented in
`docs/351_excel_vba_import_mvp_export_helper_mapping.md`.

Header lookup trims harmless leading/trailing whitespace. Blank trailing rows
are skipped. Non-ASCII memo text, including Chinese memo text, is emitted using
JSON-safe `\uXXXX` escapes so the export remains safe as a JSON/TXT payload.

## Safety Boundary

The helper is export-only. It does not use network access, credentials,
endpoints, broker integrations, real order placement, or automatic trading. It
does not write production database files.

Use sanitized sample labels such as `DEMO_ACCOUNT`, `DEMO_PORTFOLIO`,
`DEMO_ETF`, and `SANITIZED_SAMPLE`.
