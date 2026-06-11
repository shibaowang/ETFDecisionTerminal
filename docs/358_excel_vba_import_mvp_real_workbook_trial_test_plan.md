# Excel/VBA Import MVP Real Workbook Trial Test Plan

EPIC-275 verifies the sanitized JSON/TXT real-workbook trial path.

## CTest

`shell_accounting_excel_vba_import_mvp_real_workbook_trial_hardening`

## Coverage

- Verifies EPIC-275 docs and indexes.
- Verifies the compatibility matrix includes positive and negative categories.
- Verifies all EPIC-275 samples are valid JSON objects with
  `schemaVersion=excel-vba-export/v1`, `source=sanitized-excel-vba-export`, and
  sheets containing `name`, `sheetName`, `headers`, and `rows`.
- Verifies positive samples are accepted by the read-only parser.
- Verifies negative samples either fail closed in preview or remain explicitly
  not persistable because they contain market price / FX facts.
- Verifies diagnostics are sanitized and do not echo raw payloads.
- Verifies the helper supports whitespace-tolerant headers, trailing blank row
  skipping, JSON-safe non-ASCII memo escaping, explicit `.json`/`.txt` output,
  and fail-closed missing sheet/header behavior.
- Verifies helper source contains no broker, network, credential, endpoint, or
  automatic-trading capability.
- Verifies local-service E2E acceptance for:
  - single initial cash + single BUY
  - initial cash + BUY + partial SELL
  - multi-trade + multi-cash
- Verifies duplicate import and idempotency conflict do not grow rows.
- Verifies Direct `.xlsx` import is not supported.
- Verifies production databases, broker, network, credentials, endpoints, real
  order placement, and automatic trading are not used.

## Required Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_real_workbook_trial_hardening --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_export_helper_compatibility --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_demo_workspace --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_release_readiness --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```
