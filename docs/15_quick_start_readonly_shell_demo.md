# Quick Start - v0.1 ReadOnly Shell Demo

## Environment

- Windows.
- Visual Studio Build Tools / MSVC.
- CMake.
- Qt `6.9.3` `msvc2022_64`.
- Qt Quick / QML components installed.

Default Qt path used by this project stage:

```powershell
C:\Qt\6.9.3\msvc2022_64
```

## Build

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
```

## Test

```powershell
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## Start The Demo

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1 -ForceRecreateDb
```

The script initializes a demo database under `runtime/dev/readonly_demo`, starts `ETFDataService --serve-readonly`, and opens `ETFDecisionShell --diagnostics-mock`.

## Manual Acceptance

Follow the full checklist in [13_manual_acceptance_readonly_shell.md](13_manual_acceptance_readonly_shell.md).

High-level steps:

1. Open the Shell page `只读数据预览`.
2. Select the `DataService 只读服务` preset.
3. Confirm `socketName` is `ETFDataServiceReadonly`.
4. Click `Connect`.
5. Click `Refresh All`.
6. Confirm the default account, default portfolio, and `CASH` instrument are visible.

## Cleanup

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/stop_readonly_demo.ps1
Remove-Item -Recurse -Force runtime -ErrorAction SilentlyContinue
```

`runtime/` is ignored by Git and contains only developer runtime outputs for this demo.

## Common Issues

- Qt DLLs are missing: verify `C:\Qt\6.9.3\msvc2022_64\bin` exists or pass a different `-QtRoot` to the demo script.
- `ETFDataService.exe` is missing: run the build commands first.
- `ETFDecisionShell.exe` is missing: run the build commands first.
- Connect fails: confirm the selected preset socketName matches the running DataService socket.
- Refresh All is throttled: wait at least one second and retry.
- `runtime/` appears in `git status`: confirm `.gitignore` includes `runtime/`, or remove the folder manually.
- GitHub push fails: retry `git push -u origin <branch>` after confirming network and credentials.

## Scope Boundary

This quick start is for a read-only local demo. It does not support automatic trading, broker integration, real account edits, TradeLog writes, strategy calculation, market data feeds, or Excel import.
