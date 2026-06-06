# TASK-262 ShellAccounting Excel/VBA Import Read-Only Local Export JSON File Loader Preview Test Plan

## CTest

`shell_accounting_excel_vba_import_readonly_local_export_json_file_loader_preview`
validates the TASK-262 vertical slice.

## Required Coverage

- `ShellAccountingExcelVbaImportReadOnlyFileLoader` exists in ShellServices.
- Local `.json` and `.txt` files are accepted.
- Non-local URLs such as `https` and `qrc` are rejected.
- Unsupported extensions are rejected.
- Files larger than 1 MB are rejected before preview.
- File content must parse as a JSON object.
- Payloads containing path, filename, raw SQL, credential, endpoint, password,
  secret, or network URL tokens are rejected.
- Rejection issues are sanitized and do not expose raw paths, raw payloads, raw
  SQL, credentials, endpoints, or stack traces.
- Presenter method `previewExcelVbaImportReadOnlyFromLocalFile(fileUrlOrPath)`
  exists.
- Presenter loads the local file through the ShellServices loader and reuses
  `previewExcelVbaImportReadOnly(payloadJson)`.
- QML includes a FileDialog only for local file selection.
- QML displays only basename text for the selected file.
- QML includes Select File, Preview File, and Clear File controls.
- QML does not read file content directly.
- QML does not directly access DataServiceClient, SQLite, DataServiceApi, or
  AccountingEngine.
- No startup, DataServiceApi action implementation, DataServiceClient,
  DataAccess, migration, AccountingEngine production, broker, network,
  credential, or endpoint code is modified.

## Evidence JSON

The CTest must print sanitized evidence with:

- `task=TASK-262`
- `localExportJsonFileLoaderCreated=true`
- `qmlFileDialogWired=true`
- `localFileOnly=true`
- `extensionAllowlistEnforced=true`
- `fileSizeLimitEnforced=true`
- `jsonObjectRequired=true`
- `fileLoadedAsInMemoryPayload=true`
- `existingPreviewPathReused=true`
- `previewStatusVisible=true`
- `diagnosticsMapped=true`
- `replayFactSummaryMapped=true`
- `networkUrlRejected=true`
- `rawPathSanitized=true`
- all write, broker, network, credential, endpoint, real order, and automatic
  trading flags false.

## Required Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_readonly_local_export_json_file_loader_preview --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```
