# EPIC-283 Local Trial RC Bug Bash Fix Pack

Run date: 2026-06-12

Base commit: `ba3e0a77270c631985ad6d4437e525865b6df4ff`

Branch: `feature/epic-283-local-trial-rc-bug-bash-fix-pack`

## Commands Executed

- `git status --short`: exit 0, clean before branch creation.
- `git switch main`: exit 0.
- `git pull --ff-only origin main`: exit 0, EPIC-282 present on main.
- `git switch -c feature/epic-283-local-trial-rc-bug-bash-fix-pack`: exit 0.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\New-ETFDTLocalTrialWorkspace.ps1 -Force`: exit 0.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\New-ETFDTLocalTrialWorkspace.ps1 -InitializeDatabase -Force`: exit 0.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\Invoke-ETFDTLocalTrialSmoke.ps1`: exit 0.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts\local_trial\Clear-ETFDTLocalTrialWorkspace.ps1`: exit 0 when no local service was running.
- DataService cleanup probe: reproduced a P1 cleanup issue when `ETFDataService.exe` was running and the demo DB was locked.

## Observed Issues

P0 open issues: 0

P1 open issues: 0

- P1-001: `Clear-ETFDTLocalTrialWorkspace.ps1` removed the trial root without first stopping the repo-local DataService process recorded in `logs\dataservice.pid`. When DataService was running, cleanup failed because `etfdt_local_trial.sqlite` was still in use.

P2 open issues: 0

## Fix Applied

- `Clear-ETFDTLocalTrialWorkspace.ps1` now reads `dataservice.pid` and `shell.pid`, validates that the pid belongs to an allowed process under the repository tree, and stops it before deleting the workspace.
- `Start-ETFDTLocalShell.ps1` now writes `logs\shell.pid`, matching the existing DataService pid evidence pattern.
- `local_trial_release_candidate_full_delivery` now starts the local DataService in a repo-local trial workspace and verifies cleanup removes the workspace and stops the service process.
- Added this bug bash log and the EPIC-283 evidence log.

## Files Changed

- `README.md`
- `docs/README.md`
- `docs/12_codex_prompt_template.md`
- `docs/390_local_trial_release_candidate_runbook.md`
- `docs/393_local_trial_release_candidate_cleanup_rollback.md`
- `docs/395_local_trial_release_candidate_test_plan.md`
- `docs/396_local_trial_rc_bug_bash_fix_pack.md`
- `docs/397_local_trial_rc_evidence_log.md`
- `scripts/local_trial/Clear-ETFDTLocalTrialWorkspace.ps1`
- `scripts/local_trial/Start-ETFDTLocalShell.ps1`
- `tests/LocalTrialReleaseCandidateFullDelivery/local_trial_release_candidate_full_delivery.cpp`

## Validation Result

Final validation is recorded in `docs/397_local_trial_rc_evidence_log.md`.

## Known Deferred Items

- Manual visual Shell walkthrough remains a human acceptance step. The Codex run verified scripts, cleanup behavior, static dashboard wiring, local service E2E, and CTest coverage.
- Direct `.xlsx` import remains out of scope. The MVP supports sanitized JSON/TXT export samples only.

## Scope Boundary

No scope expansion beyond RC bug bash was made. No migrations, DataAccess write semantic changes, production DB changes, broker, credential, private endpoint, real order placement, automatic trading, live provider default enablement, installer behavior, service installation, registry writes, or external downloads were added.
