# Local Trial Scripts

These helpers create, start, smoke-check, and clear a repo-local release
candidate workspace.

Default root:

```text
.demo/local_trial_rc
```

Use:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\New-ETFDTLocalTrialWorkspace.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\Start-ETFDTLocalDataService.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\Start-ETFDTLocalShell.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\Invoke-ETFDTLocalTrialSmoke.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\Clear-ETFDTLocalTrialWorkspace.ps1
```

The helpers use explicit local paths, require no elevated rights, perform no
external fetches, and can be removed with the cleanup helper.
