# EPIC-283 Local Trial RC Evidence Log

Run date: 2026-06-12

Branch: `feature/epic-283-local-trial-rc-bug-bash-fix-pack`

Base commit: `ba3e0a77270c631985ad6d4437e525865b6df4ff`

## Local Trial Script Evidence

- Workspace create: passed, `.demo\local_trial_rc` created with explicit demo DB, samples, logs, and `trial_paths.json`.
- Demo DB init: passed, `etfdt_local_trial.sqlite` health check passed.
- Scripted smoke: passed.
- Cleanup without running service: passed.
- Cleanup with running DataService after fix: covered by `local_trial_release_candidate_full_delivery`; expected behavior is service pid stopped and workspace removed.

## Validation Evidence

- Build configure: passed.
- Build compile: passed.
- Targeted RC CTest `local_trial_release_candidate_full_delivery`: passed.
- Dashboard CTest `dashboard_mvp_full_delivery`: passed.
- All requested regression CTests: passed.
- Full CTest: passed.
- Transport repeat `transport_local_socket_echo` 50 runs: passed.
- `git diff --check`: passed.
- Final `git status --short`: clean after commit.

## Evidence JSON

```json
{
  "task": "EPIC-283",
  "localTrialRcBugBashCompleted": true,
  "bugBashLogCreated": true,
  "evidenceLogCreated": true,
  "p0IssuesOpen": 0,
  "p1IssuesOpen": 0,
  "localTrialScriptsValidated": true,
  "workspaceCreateCleanupValidated": true,
  "sampleManifestValidated": true,
  "dashboardRegressionPassed": true,
  "localTrialRcRegressionPassed": true,
  "fullCTestPassed": true,
  "transportRepeat50Passed": true,
  "productionDbTouched": false,
  "externalDownloads": false,
  "adminRequired": false,
  "testNetworkAccess": false,
  "liveProviderDisabledByDefault": true,
  "brokerOrderSubmitted": false,
  "credentialAccess": false,
  "endpointAccess": false,
  "realOrderPlacement": false,
  "automaticTrading": false
}
```

## Boundary Evidence

- productionDbTouched=false
- externalDownloads=false
- adminRequired=false
- testNetworkAccess=false
- liveProviderDisabledByDefault=true
- brokerOrderSubmitted=false
- credentialAccess=false
- endpointAccess=false
- realOrderPlacement=false
- automaticTrading=false
- migrationChanged=false
- scopeExpansion=false

## EPIC-285 Localization Follow-up

EPIC-285 adds a trial-feedback fix on top of the local trial RC evidence:

- User feedback addressed: Dashboard UI English text was not readable enough
  for Chinese local trial users.
- Visible Dashboard labels, module titles, action buttons, diagnostics labels,
  and safety warnings are localized to Chinese.
- Stable technical identifiers remain English where they are contracts:
  `objectName`, action names, JSON schema keys, CTest names, enum values, and
  file names.
- Draft warning evidence must include:
  `这是内部草案，不是订单，不会提交券商。`
- Market provider warning evidence must include:
  `默认使用样例数据/禁用行情源，不会自动联网。`
- No business logic, persistence semantics, database migrations, broker /
  credential / endpoint access, real order placement, or automatic trading is
  added by this localization follow-up.
