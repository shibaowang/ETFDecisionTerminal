# TASK-262 ShellAccounting Excel/VBA Import Read-Only Local Export JSON File Loader Preview

TASK-262 adds a read-only local export JSON file loader preview vertical slice
for ShellAccounting Excel/VBA import preview. TASK-261 already provides pasted
sanitized in-memory JSON preview in QML. TASK-262 adds a local file picker path
that reads a sanitized local `.json` or `.txt` export file inside ShellServices
C++ and then reuses the existing Presenter preview contract.

## Boundary

The authorized path is:

1. QML FileDialog selects a local JSON/TXT file.
2. QML displays only the selected basename.
3. QML calls `ShellAccountingPresenter.previewExcelVbaImportReadOnlyFromLocalFile(fileUrlOrPath)`.
4. `ShellAccountingExcelVbaImportReadOnlyFileLoader` reads the local file as an
   in-memory payload string.
5. Presenter calls the existing `previewExcelVbaImportReadOnly(payloadJson)` path.
6. QML displays the existing status, summary, diagnostic codes, and fact counts.

The loader is read-only and local-file-only. It rejects `http`, `https`, `qrc`,
resource, endpoint, and other non-local URLs. It accepts only `.json` and `.txt`
extensions, enforces a 1 MB maximum size, requires a JSON object, and rejects
payload keys or values containing `filePath`, `path`, `filename`, raw SQL,
credential, password, secret, endpoint, or network URL tokens.

## Sanitization

QML may hold the selected file URL for the Presenter call, but it must display
only the basename. QML must not read file contents. Loader errors must be
generic and must not expose full local paths, raw payloads, raw SQL, credentials,
endpoints, or stack traces.

## Forbidden Scope

TASK-262 does not import, persist, confirm, or generate any trading workflow.
It does not modify startup wiring, `main.cpp`, DataServiceApi action
implementation, DataServiceClient, DataAccess repositories, migrations,
AccountingEngine production code, or historical fixture JSON.

TASK-262 does not write SQLite, audit, ledger, snapshot, trade_log, or
persistent read model data. It does not call AccountingEngine replay, generate
TradeDrafts, execute strategy, submit broker orders, access networks, read
credentials, read endpoints, place real orders, or enable automatic trading.

## Files

- `apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml`
- `libs/ShellServices/include/ShellServices/ShellAccountingExcelVbaImportReadOnlyFileLoader.h`
- `libs/ShellServices/src/ShellAccountingExcelVbaImportReadOnlyFileLoader.cpp`
- `libs/ShellServices/include/ShellServices/ShellAccountingPresenter.h`
- `libs/ShellServices/src/ShellAccountingPresenter.cpp`
- `tests/ShellAccountingExcelVbaImportReadOnlyLocalExportJsonFileLoaderPreview/`

## Next Step

Future production import persistence, read model refresh, AccountingEngine
runtime replay, TradeDraft generation, broker submission, or automatic trading
must be separate TASKs.
