# ReadOnlyDataPage Manual Acceptance

## Purpose

Validate the real read-only path between `ETFDataService --serve-readonly` and the ETFDecisionShell `只读数据预览` page.

This is a developer demo only. It uses a temporary demo SQLite database, read-only DataService actions, and the Shell read-only page. It does not verify trading, accounting, strategy, market data, TradeDraft, Excel import, broker integration, or automation.

## Preconditions

- The repository has been built successfully.
- Qt 6.9.3 for MSVC 2022 is installed and available at `C:\Qt\6.9.3\msvc2022_64`, or pass a different `-QtRoot`.
- Current `main` has passed automated tests.
- No real trading account data is required.
- The demo database path is `runtime/dev/readonly_demo/ETFDecision.db` by default.

## Start The Demo

From the repository root:

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1
```

Useful options:

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1 -SkipBuild
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1 -ForceRecreateDb
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1 -KeepDataService
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1 -SocketName ETFDataServiceReadonly
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1 -NoShell
```

The script will:

1. Create `runtime/dev/readonly_demo`.
2. Run `ETFDataService --init-db` with `migrations/001_initial_schema.sql`.
3. Run `ETFDataService --check-db`.
4. Start `ETFDataService --serve-readonly --db runtime/dev/readonly_demo/ETFDecision.db --socket-name ETFDataServiceReadonly`.
5. Start `ETFDecisionShell --diagnostics-mock` unless `-NoShell` is set.
6. Record the DataService pid at `runtime/dev/readonly_demo/ETFDataServiceReadonly.pid` unless disabled.

`runtime/` is ignored by Git. The demo database, pid file, SQLite WAL/SHM files, and logs are developer runtime outputs and must not be committed.

Option notes:

- `-ForceRecreateDb` removes the old demo database, WAL, and SHM files before initialization.
- `-KeepDataService` leaves the read-only DataService process running after the script exits.
- `-NoShell` starts only the read-only DataService host and waits for Enter.
- `-SkipBuild` skips the CMake configure/build step and expects existing executables under `build`.

## Manual Steps

1. Confirm the Shell shows Mock / development status.
2. Click the left navigation item `只读数据预览`.
3. Select the `DataService read-only service` preset.
4. Confirm `socketName` is `ETFDataServiceReadonly`.
5. Click `Connect`.
6. Click `Refresh All`.
7. Confirm the page updates without showing an error.

## Expected Results

- Connection state becomes connected / success.
- `healthy` is true.
- The account list contains the default account.
- The portfolio list contains the default portfolio.
- The instrument list contains `CASH`.
- The strategy list may be empty, but the page must not crash.
- No write controls are present.
- No trade entry, manual accounting, trade confirmation, or TradeLog write flow is present.
- No `data.audit.append` call is made.
- `trade_log`, `trade_execution_group`, `trade_draft`, `position_snapshot`, `cash_snapshot`, and `portfolio_summary` are not written by this demo.

## Troubleshooting

- Qt DLLs missing: confirm `-QtRoot` points to a Qt 6.9.3 MSVC 2022 installation.
- `ETFDataService.exe` not found: build the repository first or run without `-SkipBuild`.
- `ETFDecisionShell.exe` not found: build the repository first or run with `-NoShell` for DataService-only debugging.
- Database check fails: delete `runtime/dev/readonly_demo` and rerun the script.
- Connection fails: confirm the preset socketName matches the `-SocketName` used by the script.
- Refresh All is throttled: wait at least one second and click again.
- Local firewall normally does not affect Qt local sockets.

## Cleanup

- Close ETFDecisionShell.
- The script stops the DataService process unless `-KeepDataService` was passed.
- The script only stops the DataService process it started and recorded.
- To stop a kept process through the pid file:

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/stop_readonly_demo.ps1
```

- `stop_readonly_demo.ps1` does not stop every `ETFDataService` process unless `-ForceAll` is explicitly passed.
- You may delete `runtime/dev/readonly_demo` after testing.
- Do not treat files under `runtime/` as real portfolio data.

## Prohibited During This Demo

- Do not use real trading account data.
- 不得调用 `data.audit.append`.
- Do not run any write action.
- Do not manually modify the demo database file.
- Do not treat demo data as real portfolio data.
