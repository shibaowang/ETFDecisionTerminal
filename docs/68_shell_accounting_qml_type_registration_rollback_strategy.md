# ShellAccounting QML Type Registration Rollback Strategy

## Document Purpose

This document defines the rollback / disable strategy for future
ShellAccounting QML type registration. It does not implement rollback and does
not register any QML type.

## Rollback Triggers

- App startup failure.
- QML module import failure.
- Forbidden type exposure.
- Forbidden method exposure.
- QML static gate failure.
- Smoke test failure.
- Privacy display failure.
- No-trade gate failure.
- Unexpected write path.
- Issue hidden or Unavailable misclassified.

## Fallback Behavior

- Disable accounting QML binding.
- Show Unavailable.
- Do not show accounting UI when unavailable display is not ready.
- Do not fake data.
- Do not directly call `DataServiceClient`.
- Do not access SQLite.
- Do not write to any database table.
- Do not generate `TradeDraft`.
- Do not generate trade suggestions.

## Verification After Rollback

- App launches.
- QML static gate passes.
- Full CTest passes.
- No forbidden tokens are present.
- No write action is present.
- `git status` is clean.

## Manual Rollback Steps

A future implementation task may use these steps as a rollback template:

- Revert the registration commit.
- Disable the registration feature flag.
- Remove the module import.
- Rerun static gate.
- Rerun full tests.

This task does not execute these steps. The future implementation task must
document its actual rollback steps.
