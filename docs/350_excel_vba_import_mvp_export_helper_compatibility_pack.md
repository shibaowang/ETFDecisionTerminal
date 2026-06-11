# EPIC-274 Excel/VBA Import MVP Export Helper Compatibility Pack

EPIC-274 provides a compatibility pack for users who already maintain
Excel/VBA workbooks and need a local-only path into the Excel/VBA Import MVP.
The pack exports workbook data into the sanitized JSON/TXT contract that the
existing MVP preview and persistence flow already accepts.

## Deliverables

- VBA helper:
  `tools/excel_vba_export/ExcelVbaImportMvpExportHelper.bas`
- Helper README:
  `tools/excel_vba_export/ExcelVbaImportMvpExportHelper_README.md`
- Mapping guide:
  `docs/351_excel_vba_import_mvp_export_helper_mapping.md`
- User guide:
  `docs/352_excel_vba_import_mvp_export_helper_user_guide.md`
- Test plan:
  `docs/353_excel_vba_import_mvp_export_helper_test_plan.md`
- Sanitized expected sample:
  `samples/excel_vba_import/TASK274_export_helper_expected_sample.json`
- Smoke CTest:
  `shell_accounting_excel_vba_import_mvp_export_helper_compatibility`

## Scope

The helper reads only the current workbook sheets named `InitialCash` and
`TradeLog`. It writes a user-selected `.json` or `.txt` export file whose root
object uses:

```json
{
  "schemaVersion": "excel-vba-export/v1",
  "source": "sanitized-excel-vba-export",
  "sheets": []
}
```

Each exported sheet includes `name`, `sheetName`, `headers`, and `rows`.

## Boundaries

- Excel 2010-compatible VBA only.
- Sanitized JSON/TXT export only.
- Direct `.xlsx` import is not supported.
- The helper does not write production database files.
- The helper does not access broker, network, credentials, or endpoint
  integrations.
- The helper does not place real orders or enable automatic trading.
- The helper must fail closed when required sheets or headers are missing, when
  `TradeLog` has no data rows, or when the selected file cannot be written.

## Sample Data Rule

Any sample/default labels in this pack use only:

- `DEMO_ACCOUNT`
- `DEMO_PORTFOLIO`
- `DEMO_ETF`
- `SANITIZED_SAMPLE`

No real user account, real transaction stream, production file path, or broker
identifier belongs in the helper, docs, or sample export.
