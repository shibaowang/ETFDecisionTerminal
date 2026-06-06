# TASK-264 ShellAccounting Excel/VBA Import Preview To Manual Entry Persistence Authorization Gate Test Plan

## Test Objective

Prove TASK-264 is an authorization gate only. The test must verify that the
future Excel/VBA import persistence boundary is documented around accepted
preview state, DataService-only writes, transaction rollback, idempotency,
duplicate import handling, sanitized errors, and no broker / no strategy /
no TradeDraft / no automatic trading.

## Required CTest

`shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_gate`

## Documentation Checks

The gate must verify that docs/324 and docs/325 define:

- `ACCEPTED` preview is required before future persistence.
- non-accepted states are forbidden from write entry.
- write entry must be a dedicated DataService action.
- QML, Presenter, Controller, and ShellServices must not directly write.
- future persistence must transact manual transaction facts or `trade_log`,
  `audit_log`, and an idempotency record / import batch marker together.
- rollback is required for any partial failure.
- idempotency key strategy is required.
- duplicate import policy is required.
- sanitized error mapping is required.
- minimal accepted-preview-to-persistence request DTO is defined.
- broker, strategy, TradeDraft, real order placement, and automatic trading are
  forbidden.

## Changed Path Checks

The gate must fail if TASK-264 modifies:

- `apps/`
- startup / `main.cpp`
- `libs/DataServiceApi/` production implementation
- `libs/DataServiceClient/`
- `libs/ShellServices/` production C++
- `libs/DataAccess/`
- `libs/AccountingEngine/`
- `migrations/`
- historical fixture JSON

The only permitted TASK-264 paths are the docs/index/test files and exact old
gate allowlist repairs needed to keep authorization gates self-consistent.

## Forbidden Implementation Checks

The gate must fail if TASK-264 introduces:

- SQLite write implementation.
- `audit_log`, ledger, snapshot, `trade_log`, or persistent read model write.
- Import / Confirm / Persist UI controls.
- TradeDraft generation.
- strategy execution.
- broker, network, credentials, endpoint, real order placement, or automatic
  trading.

## Evidence JSON

The test must emit sanitized evidence containing:

- `task=TASK-264`
- `persistenceAuthorizationGateCreated=true`
- `acceptedPreviewRequired=true`
- `dataserviceOnlyWriteBoundaryDefined=true`
- `qmlDirectWriteForbidden=true`
- `presenterDirectWriteForbidden=true`
- `transactionBoundaryDefined=true`
- `auditLogRequired=true`
- `tradeLogWriteFutureOnly=true`
- `idempotencyRequired=true`
- `rollbackRequired=true`
- `duplicateImportPolicyDefined=true`
- `sanitizedErrorMappingDefined=true`
- `implementationAdded=false`
- `importButtonAdded=false`
- `productionWrite=false`
- `sqliteProductionWrite=false`
- `auditWritten=false`
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
ctest --test-dir build -R shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_gate --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Formal Conclusion

TASK-264 authorizes only the future persistence boundary. It does not implement
DataService write persistence, does not add Import UI, does not write SQLite,
does not write audit or trade rows, does not generate TradeDrafts, and does not
add broker, network, real order placement, or automatic trading capability.

