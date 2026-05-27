# AccountingEngine Module Candidate

## Purpose

This document evaluates whether a future `libs/AccountingEngine` module should
be added.

It is a candidate design only. It does not implement code and does not create a
new production library.

## Recommended Responsibilities

Future AccountingEngine candidate responsibilities:

- Replay facts.
- Validate accounting consistency.
- Generate read-only DTOs.
- Report accounting issues.
- Support fixture-backed tests.
- Keep deterministic behavior.
- Avoid UI dependencies.
- Avoid service transport dependencies.

## Non-responsibilities

AccountingEngine should not be responsible for:

- SQLite connection management.
- DataService action registration.
- QML ViewModel construction.
- TradeLog writes.
- Snapshot writes.
- TradeDraft generation.
- Broker API integration.
- Auto trading.
- Market data fetching.
- FX rate fetching.

## Input DTO Draft

- `TradeFactDto`.
- `CashFactDto`.
- `MarketPriceFactDto`.
- `FxRateFactDto`.
- `ReplayRequestDto`.

## Output DTO Draft

- `PositionListResponse`.
- `CashSummaryDto`.
- `PortfolioPnlDto`.
- `BasePositionDto`.
- `SniperPoolDto`.
- `AccountingIssueDto`.
- `AccountingReplayMetadata`.

## Dependency Strategy

The preferred design is that AccountingEngine does not directly depend on DataAccess
and does not directly depend on SQLite.

The preferred flow is for DataServiceApi or an application layer to provide
facts to AccountingEngine. If a future task allows AccountingEngine to depend on
DataAccess, that task must explicitly authorize the dependency and explain why
the boundary is necessary.

AccountingEngine must not depend on QML or Qt Quick.

## Test Strategy

FX001-FX013 fixture tests must be ported or reused for production engine
coverage.

Production engine core algorithm tests should not depend on SQLite. SQLite
integration tests must be separate.

Before any DataService action is added, no-write table-count tests are required.

## Migration Strategy

The test-only `AccountingReplayMinimalEngine` must not be directly migrated to
production.

Production work may reuse the fixture contract, expected outputs, and assertion
ideas. The production engine should be reimplemented with a clean architecture
boundary.

Each fixture implementation must remain incremental and must preserve previous
fixture tests.

## TASK-070 Candidate Skeleton Created

TASK-070 creates the `libs/AccountingEngine` skeleton as a production module
candidate, but it does not implement replay.

Current capability declarations:

- `productionReady=false`.
- `replayImplemented=false`.
- `readOnlyReplaySupported=false`.
- `snapshotWriteEnabled=false`.
- `tradeLogWriteEnabled=false`.
- `dataServiceActionImplemented=false`.
- `fixtureReplayAvailableInProduction=false`.

Current forbidden boundaries include:

- No DataAccess dependency.
- No DataServiceApi dependency.
- No DataServiceClient dependency.
- No ServiceHost dependency.
- No Watchdog dependency.
- No Qt Quick / QML dependency.
- No test-only AccountingFixtures dependency.
- No TradeLog writes.
- No snapshot writes.
- No portfolio summary writes.

Any future replay logic, DataAccess dependency, DataService action, or write
capability must be authorized by a separate task.
