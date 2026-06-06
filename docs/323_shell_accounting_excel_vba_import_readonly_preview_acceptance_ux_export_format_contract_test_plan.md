# TASK-263 ShellAccounting Excel/VBA Import Read-Only Preview Acceptance UX Export Format Contract Test Plan

## Test Objective

Prove TASK-263 adds only read-only preview acceptance UX and export format
contract coverage for ShellAccounting Excel/VBA import preview. The test must
prove pasted JSON and local file preview remain available, the sample preview
uses the existing preview path, and no import, persistence, replay, broker, or
trading capability is added.

## Required CTest

`shell_accounting_excel_vba_import_readonly_preview_acceptance_ux_export_format_contract`

## QML Checks

The test must verify:

- preview panel still exists.
- pasted JSON preview input and Preview control still exist.
- local FileDialog select, Preview File, Clear File, and basename display still
  exist.
- acceptance state text exists and includes `READY`, `FILE_SELECTED`,
  `PREVIEWING`, `ACCEPTED`, `REJECTED`, and `INPUT_ERROR`.
- acceptance summary text exists.
- fact counts, diagnostic codes, and sanitized issue text remain visible.
- compact export format contract text exists.
- Load Sample Preview exists.
- sample preview calls `previewExcelVbaImportReadOnly(payloadJson)`.

## Sample Contract Checks

The test must verify a TASK-263 sanitized sample exists and uses:

- `schemaVersion`: `excel-vba-export/v1`
- `source`: `sanitized-excel-vba-export`
- `sheets[].name`
- `sheets[].headers`
- `sheets[].rows`

The sample must not contain real account numbers, raw local paths, raw SQL,
credentials, endpoints, broker payloads, real order ids, or stack traces.

## Forbidden UI Checks

The test must fail if the preview panel adds Import, Confirm, Persist,
TradeDraft, strategy, broker order, real order, endpoint, credential, network,
or automatic trading controls.

The test must fail if QML directly reads file contents or directly references
DataServiceClient, DataServiceApi, SQLite, DataAccess, or AccountingEngine.

## No-Write Checks

The test must prove TASK-263 does not write SQLite, audit, ledger, snapshot,
trade_log, persistent read model data, or fixture JSON, and does not generate
TradeDrafts.

## Evidence JSON

The test must emit sanitized evidence containing:

- `task=TASK-263`
- `acceptanceUxCreated=true`
- `exportFormatContractDocumented=true`
- `samplePayloadAvailable=true`
- `samplePreviewUsesExistingPath=true`
- `pastePreviewStillAvailable=true`
- `localFilePreviewStillAvailable=true`
- `acceptedStateVisible=true`
- `rejectedStateVisible=true`
- `inputErrorStateVisible=true`
- `factCountsVisible=true`
- `diagnosticCodesVisible=true`
- `sanitizedIssueVisible=true`
- `rawPathSanitized=true`
- `rawPayloadExposed=false`
- `importButtonAdded=false`
- `confirmButtonAdded=false`
- `persistenceAdded=false`
- `accountingEngineCalled=false`
- `productionWrite=false`
- `sqliteProductionWrite=false`
- `auditWritten=false`
- `ledgerWritten=false`
- `snapshotWritten=false`
- `tradeLogWritten=false`
- `readModelPersistentWrite=false`
- `tradeDraftGenerated=false`
- `strategyExecuted=false`
- `brokerOrderSubmitted=false`
- `networkAccess=false`
- `credentialAccess=false`
- `endpointAccess=false`
- `automaticTrading=false`

## Required Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_readonly_preview_acceptance_ux_export_format_contract --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Formal Conclusion

TASK-263 is a read-only preview acceptance UX and export format contract. It
does not authorize import persistence, read model refresh, AccountingEngine
runtime replay, TradeDraft generation, broker submission, network access, real
order placement, or automatic trading.
