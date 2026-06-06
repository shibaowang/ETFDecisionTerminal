# TASK-263 ShellAccounting Excel/VBA Import Read-Only Preview Acceptance UX Export Format Contract

TASK-263 adds a read-only acceptance UX and compact export format contract for
ShellAccounting Excel/VBA import preview. TASK-262 already supports pasted
sanitized JSON and local `.json` / `.txt` export file preview. TASK-263 keeps
those two read-only paths and makes the preview state and expected sanitized
export shape clear enough for user acceptance testing.

## Purpose

The goal is a preview-only UI contract:

1. Pasted sanitized JSON or selected local sanitized JSON/TXT file remains the
   input source.
2. QML shows a normalized preview acceptance state.
3. QML shows accepted / rejected / input-error status, fact counts, diagnostic
   codes, and sanitized issue text.
4. QML exposes a compact sample export format contract.
5. QML can load a TASK-263 sanitized sample preview through the existing
   `previewExcelVbaImportReadOnly(payloadJson)` path.

TASK-263 is not import persistence and is not a production file import
workflow. It does not confirm or write anything.

## Acceptance States

The preview panel must expose these states:

- `READY`
- `FILE_SELECTED`
- `PREVIEWING`
- `ACCEPTED`
- `REJECTED`
- `INPUT_ERROR`

`FILE_SELECTED` is a QML acceptance display state derived from a selected local
file basename while the Presenter preview status remains `READY`. The remaining
states come from the existing Presenter preview state contract.

## Export Format Contract

The compact sample contract is:

```json
{
  "schemaVersion": "excel-vba-export/v1",
  "source": "sanitized-excel-vba-export",
  "sheets": [
    {
      "name": "TradeLog",
      "headers": ["date", "account", "portfolio", "instrument", "side", "quantity", "price", "currency"],
      "rows": [
        ["2026-01-05", "DEMO_ACCOUNT", "DEMO_PORTFOLIO", "DEMO_ETF", "BUY", 10, 25.5, "USD"]
      ]
    }
  ]
}
```

The sample is synthetic and sanitized. It is not a real account, real trade,
real broker export, or production file path. The UI may load the sample preview
into memory and call the existing read-only preview path.

## UI Boundary

The QML page may show:

- preview acceptance state and acceptance summary.
- trade, cash, market price, and FX fact counts.
- diagnostic code list.
- sanitized issue text.
- compact export format contract.
- Load Sample Preview control.
- existing pasted JSON preview controls.
- existing local file preview controls.

The QML page must not directly read files, access DataServiceClient, access
DataServiceApi, access SQLite, access AccountingEngine, call network APIs, read
credentials, or read endpoints.

## Forbidden Controls

TASK-263 must not add Import, Confirm, Persist, TradeDraft, strategy, broker,
order placement, real order, or automatic trading controls. It must not add
production file loading outside the TASK-262 local read-only loader path.

## Write Boundary

TASK-263 does not write SQLite, audit, ledger, snapshot, trade_log, cash facts,
manual entry facts, or persistent read model data. It does not refresh read
models, import rows, confirm drafts, or generate TradeDrafts.

## Production Boundary

TASK-263 does not modify startup wiring, `main.cpp`, DataServiceApi action
implementation, DataServiceClient, DataAccess repositories, migrations,
AccountingEngine production code, historical fixture JSON, broker code,
network code, credential code, or endpoint code.

## Evidence

The TASK-263 CTest must emit sanitized JSON evidence proving acceptance UX,
sample payload availability, sample preview path reuse, continued pasted and
local file preview support, no raw path/payload exposure, no import/persist
controls, no writes, no broker, no network, and no automatic trading.

## Next Step

Future import persistence, read model refresh, AccountingEngine runtime replay,
TradeDraft generation, broker submission, or automatic trading must be separate
TASKs with explicit authorization.
