# ShellAccounting Real Data Adapter Implementation

## Document Purpose

TASK-135 implements the first ShellAccounting real read-only adapter boundary.
It wires the production presenter/controller path to the existing
`ShellAccountingDataServiceAdapter`, `ShellAccountingDataServiceClientPortAdapter`,
and `DataServiceClient` protocol boundary. This is read-only action transport
only: not a write path, not TradeDraft, not trade suggestion, not strategy
execution, and not broker order flow.

## TASK-135 Scope

TASK-135 authorizes only a read-only adapter/data action chain for
ShellAccounting. The production QML shell remains read-only and unavailable-safe.
QML still does not call `DataServiceClient` directly, the presenter still does
not access SQLite, and the presenter still does not call AccountingEngine.

## Implemented Read-only Action Names

The authorized action list is fixed to:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`
- `base_position.summary`
- `sniper_pool.summary`

No write-enabled action is introduced. The following remain forbidden:
`trade_log write`, `trade_execution_group write`, `trade_draft write`,
`cash_snapshot write`, `position_snapshot write`, `portfolio_summary write`,
`audit_log write`, `data.audit.append`, `confirmTrade`, `manualEntry`,
`cashAdjustment`, `createTradeDraft`, `brokerOrder`, and `strategyExecute`.

## DataService Action Contract

The existing DataService read-only guard actions remain the contract boundary.
Responses are structured as status / data / issues payloads with
`readOnly=true` and `writeEnabled=false`. Until SQLite facts query and
AccountingEngine replay are separately authorized, these actions may return
structured unavailable / not implemented state through the DataService
read-only protocol boundary rather than fake success data.

## Adapter Classes

The adapter chain is:

- `ShellAccountingPresenter`
- `ShellAccountingReadOnlyController`
- `ShellAccountingDataServiceAdapter`
- `ShellAccountingDataServiceClientPortAdapter`
- `DataServiceClient`
- DataService read-only action protocol boundary

`ShellAccountingDataServiceAdapter` owns request shaping and maps client-port
responses into `ShellAccountingServiceResult`. The concrete port adapter is the
only ShellAccounting layer that may include and call `DataServiceClient`.

## Request / Response Summary

Requests carry only the read-only accounting action name, optional account /
portfolio fields, requested output hints, and timeout. Responses carry status,
read-only flags, data quality, row presence, issues, warnings, errors, and
adapter error flags. QML sees only presenter/controller state and issue models,
not raw transport payload.

## Error Mapping

Unavailable service, transport error, protocol error, malformed payload, timeout,
and stale data must map to issue state. There is no silent success on failure.
Adapter tests cover fake success mapping, unavailable mapping, transport error
mapping, protocol error mapping, timeout mapping, and no raw payload exposure.

## Timeout Mapping

Timeout maps to unavailable/error issue state through the adapter boundary. It
must not generate fake holdings, fake cash, fake PnL, TradeDraft, or trading
suggestions.

## Unavailable / Stale State Mapping

Unavailable and not implemented payloads remain visible as explicit
Unavailable/issue state in the presenter. Stale data must be explicit if added
by a future DataService implementation.

## Privacy / No Raw Payload Policy

- no raw SQL in logs.
- no full trade_log payload in QML.
- no sensitive raw payload in issue text.
- no raw transport payload exposed to QML.
- privacy mode respected.

## No Write Path Evidence

TASK-135 does not write database tables. It does not add write repositories,
write service actions, `data.audit.append`, snapshot writes, TradeDraft writes,
or trading actions. Static gates scan the production ShellAccounting path for
write/trade tokens after integration.

## No SQLite Direct Access Evidence

The presenter, production QML, startup composition, and ShellAccounting adapter
path do not directly access SQLite, DataAccess repositories, or
`AccountingTradeFactReader`. Future SQLite facts query work must be inside a
separately authorized DataService/DataAccess task.

## No AccountingEngine Direct Access Evidence

The presenter, production QML, startup composition, and ShellAccounting adapter
path do not directly call AccountingEngine, AccountingReplayEngine, replay, or
`replayFromFacts`. Future AccountingEngine replay integration must be a separate
task.

## Production Wiring Location

The production wiring is centralized in `apps/ETFDecisionShell/src/main.cpp`.
It creates the `DataServiceClient`, the concrete ShellAccounting client port,
the `ShellAccountingDataServiceAdapter`, and sets that adapter once on the
existing `ShellAccountingReadOnlyController`. The QML context still exposes only
`accountingPresenter`.

## Rollback / Disable Real Adapter Strategy

The rollback plan is limited to removing the DataServiceClient include, the concrete
client port construction, the `ShellAccountingDataServiceAdapter` construction,
and the single `setServiceAdapter` call from startup, plus TASK-135 tests/docs.
Fallback keeps presenter unavailable. Fallback must not fake data, must not write DB,
must not generate TradeDraft, must not show trading UI, and must not bypass DataService
with SQLite direct reads.
Fallback must not bypass DataService with SQLite direct reads.

## Next Stage

Connecting a real SQLite facts query, AccountingEngine replay, snapshot
generation, TradeDraft, trade suggestion, strategy execution, or broker order
flow requires a separate explicit TASK with its own gates.
