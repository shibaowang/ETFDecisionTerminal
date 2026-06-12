# EPIC-282 Local Trial Release Candidate Runbook

This runbook is for Windows PowerShell. It uses repo-local paths and does not
require administrator rights, registry writes, external dependency downloads, or
live market connectivity.

## 1. Enter Workspace

```powershell
cd D:\ETFDecisionTerminal
git switch main
git pull --ff-only origin main
```

## 2. Configure And Build

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
```

## 3. Create Local Trial Workspace

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\New-ETFDTLocalTrialWorkspace.ps1
```

Default paths:

- trial root: `.demo\local_trial_rc`
- demo DB: `.demo\local_trial_rc\data\etfdt_local_trial.sqlite`
- samples: `.demo\local_trial_rc\samples`
- logs: `.demo\local_trial_rc\logs`

## 4. Initialize Demo DB

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\New-ETFDTLocalTrialWorkspace.ps1 -InitializeDatabase
```

The command uses the explicit demo DB path above. Do not point it at a
production DB path.

## 5. Start DataService

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\Start-ETFDTLocalDataService.ps1
```

Default socket name: `ETFDTLocalTrialRc`.

## 6. Start Shell

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\Start-ETFDTLocalShell.ps1
```

Open the ShellAccounting Dashboard page.

## 7. Run Dashboard Flows

1. Load `samples\local_trial\EPIC282_local_trial_excel_vba_import.json`.
2. Run Excel/VBA import preview and verify `ACCEPTED`.
3. Confirm accepted preview persistence and verify row counts.
4. Run portfolio replay with `EPIC282_local_trial_portfolio_replay.json`.
5. Run market fixture refresh with
   `EPIC282_local_trial_market_data_fixture.json`.
6. Run strategy recommendation BUY and SELL samples.
7. Preview and create internal TradeDraft BUY / SELL drafts with explicit
   confirmation.
8. Preview and create internal OTCMap multi-leg BUY / SELL drafts with explicit
   confirmation.
9. Verify duplicate draft and idempotency conflict samples do not grow rows.
10. Verify provider-disabled sample shows a safe disabled state.

## 8. Run Scripted Smoke

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\Invoke-ETFDTLocalTrialSmoke.ps1
```

## 9. Cleanup

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\Clear-ETFDTLocalTrialWorkspace.ps1
```

Use `-PreserveLogs` when evidence needs to be retained.
