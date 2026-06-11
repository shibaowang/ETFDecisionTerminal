# EPIC-273 Excel/VBA Import MVP Demo Workspace

The demo workspace gives a local, explicit trial area for the Excel/VBA import
MVP. It is only for sanitized JSON/TXT export files. Direct `.xlsx` import is
not supported.

## Default Layout

```text
.demo/excel_vba_import_mvp/
  data/
    excel_vba_import_mvp_demo.sqlite
  samples/
    TASK272_sanitized_excel_vba_import_mvp_sample.json
  logs/
  trial_commands.ps1
  README.txt
```

The demo DB path is explicit:

```text
.demo/excel_vba_import_mvp/data/excel_vba_import_mvp_demo.sqlite
```

Do not point the demo workflow at `data/ETFDecision.db` or any production
database path.

## Bootstrap

Create the repo-local demo workspace:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/excel_vba_import_mvp/New-ExcelVbaImportMvpDemoWorkspace.ps1
```

Create the workspace and initialize the explicit demo SQLite database:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/excel_vba_import_mvp/New-ExcelVbaImportMvpDemoWorkspace.ps1 -InitializeDatabase
```

The bootstrap helper copies only sanitized sample exports and writes local trial
command hints. It does not start broker integrations, access network endpoints,
read credentials, place real orders, or enable automatic trading.

## Trial Boundaries

- Sanitized JSON/TXT export only.
- Direct `.xlsx` import is not supported.
- Demo DB must be repo-local, temporary, or explicitly supplied by the tester.
- Production DB paths are out of scope.
- Broker, network, credentials, endpoint, real order placement, and automatic
  trading are out of scope.
- The demo workspace is a local trial path, not an automated trading system.
