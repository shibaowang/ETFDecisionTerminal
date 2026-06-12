# EPIC-282 Local Trial Cleanup And Rollback

## Stop Local Processes

Close the Shell window when doing manual cleanup. The cleanup script also reads
`.demo\local_trial_rc\logs\dataservice.pid` and
`.demo\local_trial_rc\logs\shell.pid`; when either pid points at the repository
local trial executable, cleanup stops it before removing the workspace. It
refuses to stop unrelated system processes.

## Remove Workspace

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\Clear-ETFDTLocalTrialWorkspace.ps1
```

Use `-PreserveLogs` before deleting the demo DB when logs are needed for release
candidate evidence.

## Preserve Evidence

Copy `.demo/local_trial_rc/logs` to an explicit evidence folder before cleanup.
Do not copy a production DB path into the trial package.

## Delete Demo DB

The cleanup script removes `.demo/local_trial_rc/data/etfdt_local_trial.sqlite`
when logs are not preserved. This is the only trial DB path used by default.

## Re-run Clean Smoke

After cleanup:

```powershell
ctest --test-dir build -R local_trial_release_candidate_full_delivery --output-on-failure
```

## Branch Rollback

If a local branch needs rollback before merge:

```powershell
git switch main
git branch -D feature/epic-282-local-trial-release-candidate-final-acceptance
```

After PR merge, delete the feature branch locally and remotely according to the
normal repository process.

## Final Safety Checks

- No production DB touched.
- No hidden service or listener remains.
- No trial files remain outside the explicit trial root.
