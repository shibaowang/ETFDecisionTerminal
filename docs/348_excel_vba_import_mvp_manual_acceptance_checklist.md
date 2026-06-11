# Excel/VBA Import MVP Manual Acceptance Checklist

Use this checklist for a local trial of sanitized Excel/VBA JSON/TXT export
preview and persistence. Direct `.xlsx` import is not supported.

## Setup

- Build the repo with the documented Qt root.
- Create the demo workspace with
  `scripts/excel_vba_import_mvp/New-ExcelVbaImportMvpDemoWorkspace.ps1`.
- Initialize the explicit demo DB:
  `.demo/excel_vba_import_mvp/data/excel_vba_import_mvp_demo.sqlite`.
- Start local DataService with `--serve-dev-audit`, the demo DB path, and a
  local socket name.
- Start the Shell and open the ShellAccounting page.

## Preview

- Load copied sample JSON:
  `.demo/excel_vba_import_mvp/samples/TASK272_sanitized_excel_vba_import_mvp_sample.json`.
- Click Preview.
- Confirm the preview state is `ACCEPTED`.
- Confirm diagnostic codes are sanitized.
- Confirm trade and cash fact counts are visible.
- Confirm no raw path, raw payload, SQL, credential, endpoint, or stack trace is
  displayed.

## Persist Accepted Preview

- Check the explicit persist confirmation control.
- Click the persist/import accepted preview control.
- Confirm persisted row counts are visible:
  - `trade_log` rows written.
  - `cash_adjustment` rows written.
- Confirm post-write readback refresh status is visible and sanitized.
- Confirm no TradeDraft, broker, order, strategy, or automatic trading control is
  shown.

## Duplicate Import

- Run the same accepted import again with the same idempotency key.
- Confirm duplicate import is reported as sanitized state.
- Confirm row counts do not grow.
- Confirm the UI does not claim new rows were written.

## Trial Boundaries

- Sanitized JSON/TXT export only.
- Direct `.xlsx` import is not supported.
- Production DB paths are out of scope.
- Broker, network, credentials, endpoint, real order placement, and automatic
  trading are out of scope.
