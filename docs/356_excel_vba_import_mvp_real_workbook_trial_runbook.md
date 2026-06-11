# Excel/VBA Import MVP Real Workbook Trial Runbook

This EPIC-275 runbook is for a local, sanitized trial of the Excel/VBA Import MVP. It is
not a broker workflow. Direct `.xlsx` import is not supported.

## Before The Trial

1. Back up the workbook.
2. Work on a copy.
3. Replace real account, portfolio, instrument, broker, endpoint, credential,
   and free-form user identifiers with sanitized labels.
4. Import `tools/excel_vba_export/ExcelVbaImportMvpExportHelper.bas` into the
   workbook.
5. Confirm the workbook has `InitialCash` and `TradeLog` sheets.
6. Confirm row 1 contains the required headers documented in
   `docs/355_excel_vba_import_mvp_compatibility_matrix.md`.

## Export

1. Run `ExportMvpJsonTxt`.
2. Choose an explicit local `.json` or `.txt` target path.
3. Do not use a production database directory as the export target.
4. Confirm the export root contains:
   - `schemaVersion = excel-vba-export/v1`
   - `source = sanitized-excel-vba-export`
   - `sheets`

## Preview And Persist

1. Create or select a demo workspace using the EPIC-273 runbook.
2. Start the local DataService and Shell as documented by the demo runbook.
3. Open the ShellAccounting page.
4. Load the sanitized JSON/TXT export.
5. Run Preview.
6. Continue only if the preview status is `ACCEPTED`.
7. Review trade/cash fact counts and diagnostics.
8. Explicitly confirm the accepted preview.
9. Persist the accepted preview.
10. Verify persisted row counts:
    - `trade_log` rows written equals trade facts plus cash facts.
    - `cash_adjustment` rows written equals cash facts.
    - `audit_log` rows written equals one import audit marker.
11. Verify post-write readback refresh status is visible and sanitized.
12. Re-run the same import to verify duplicate import does not increase rows.

## Negative Diagnostics

If preview is `REJECTED`, do not persist. Fix the workbook copy and export
again. Diagnostics must not expose raw paths, raw payloads, SQL snippets,
credentials, endpoints, stack traces, broker payloads, real orders, or
automatic-trading language.

## Boundaries

- Supported: sanitized JSON/TXT export only.
- Not supported: direct `.xlsx` import.
- Not supported: broker integration, real order placement, automatic trading.
- Not supported: market price / FX persistence.
- The trial must use a local demo or temporary database, never a production DB.
