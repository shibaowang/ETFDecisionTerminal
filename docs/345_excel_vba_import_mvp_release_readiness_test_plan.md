# EPIC-272 Excel/VBA Import MVP Release Readiness Test Plan

## Targeted Smoke

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_release_readiness --output-on-failure
```

The smoke test must verify:

- sanitized sample export JSON/TXT contract exists, with the checked sample
  stored as JSON.
- sample export JSON is accepted by the existing read-only parser.
- local service E2E acceptance is registered.
- QML panel contains preview, file load, persist, row count, refresh status, and
  diagnostics object names.
- QML does not directly reference DataServiceClient, SQLite, AccountingEngine,
  broker, network, endpoint, credentials, or automaticTrading from the MVP
  panel.
- docs, user guide, runbook, sample format, known limits, and release checklist
  are indexed.
- known limits state direct `.xlsx` import is not supported.
- release checklist states broker and automatic trading are outside MVP scope.

## Required Regression

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Evidence

The smoke test emits JSON evidence with `task=EPIC-272` and explicit false
values for production DB touch, broker order, network, credential, endpoint, and
automatic trading.
