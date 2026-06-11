# Excel/VBA Import MVP Export Helper User Guide

This guide explains how to use the Excel/VBA export helper with an existing
workbook. It creates a sanitized JSON/TXT export file for the current MVP.
Direct `.xlsx` import is not supported.

## Install The Helper

1. Open the workbook in Excel 2010 or newer.
2. Open the VBA editor.
3. Import `tools/excel_vba_export/ExcelVbaImportMvpExportHelper.bas`.
4. Ensure the workbook contains `InitialCash` and `TradeLog` sheets.
5. Ensure the required headers are present in row 1.

## Export

1. Review workbook data and replace real identifiers with sanitized labels.
2. Run `ExportMvpJsonTxt`.
3. Select an explicit local `.json` or `.txt` output path.
4. Save the export.
5. Load the saved file in the ETFDecisionTerminal Excel/VBA Import MVP preview
   panel.
6. Confirm the preview reaches `ACCEPTED` before any manual persistence step.

## Fail-Closed Behavior

The helper stops with a clear message when:

- `InitialCash` is missing.
- `TradeLog` is missing.
- a required header is missing.
- `TradeLog` has no data rows.
- the selected output file cannot be written.

Errors must stay sanitized: no raw local path, raw payload, SQL text,
credential, endpoint, or stack trace should be copied into issue reports.

## MVP Boundary

The helper is local-only and export-only. It does not read production
databases, does not use broker integrations, does not access network
endpoints, does not place real orders, and does not enable automatic trading.
