# Excel/VBA Import MVP Local Runbook

Use this runbook to verify the local MVP build before trial use. The MVP input
format is a sanitized JSON/TXT export; direct `.xlsx` import is not supported.

## Build

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
```

## Smoke Tests

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_release_readiness --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance --output-on-failure
```

## Full Regression

```powershell
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Manual Trial

1. Start the local shell build.
2. Open the ShellAccounting read-only page.
3. Use the sample payload or choose a local sanitized `.json` / `.txt` export.
4. Run Preview and verify `ACCEPTED`.
5. Confirm and persist only if the preview is accepted.
6. Verify persisted row counts and post-write refresh status.

The runbook does not authorize production database mutation outside the
existing DataService persistence path.
