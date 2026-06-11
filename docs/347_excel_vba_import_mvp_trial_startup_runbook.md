# Excel/VBA Import MVP Trial Startup Runbook

This runbook starts the Excel/VBA import MVP against an explicit local demo
workspace. It supports sanitized JSON/TXT exports only. Direct `.xlsx` import is
not supported.

## Build

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
```

## Create Demo Workspace

```powershell
powershell -ExecutionPolicy Bypass -File scripts/excel_vba_import_mvp/New-ExcelVbaImportMvpDemoWorkspace.ps1 -InitializeDatabase
```

The default demo DB path is:

```text
.demo/excel_vba_import_mvp/data/excel_vba_import_mvp_demo.sqlite
```

Do not use `data/ETFDecision.db` for the trial.

## Start Local DataService

Use the explicit demo DB path and a local socket name:

```powershell
$DemoDb = ".demo\excel_vba_import_mvp\data\excel_vba_import_mvp_demo.sqlite"
$SocketName = "ETFDataServiceExcelVbaImportMvpDemo"
build\apps\ETFDataService\Debug\ETFDataService.exe --serve-dev-audit --db $DemoDb --socket-name $SocketName
```

`--serve-dev-audit` registers the existing local read-only preview and accepted
preview persistence actions needed by the MVP. It does not authorize broker,
network, credentials, endpoint, real order placement, or automatic trading.

## Start Shell

In a second terminal:

```powershell
build\apps\ETFDecisionShell\Debug\ETFDecisionShell.exe --diagnostics-mock
```

Open the ShellAccounting page and use the copied sample:

```text
.demo/excel_vba_import_mvp/samples/TASK272_sanitized_excel_vba_import_mvp_sample.json
```

## Smoke Verification

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_demo_workspace --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_release_readiness --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance --output-on-failure
```

The MVP trial path is local only. It does not read credentials, contact
endpoints, submit broker orders, or perform automatic trading.
