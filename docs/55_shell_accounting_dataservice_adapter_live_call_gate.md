# ShellAccountingDataServiceAdapter Live-call Gate

## Document Purpose

This document defines the gate for a future task that may switch
`ShellAccountingDataServiceAdapter` from the current not-connected skeleton to
real DataServiceClient wrapper calls.

This document does not implement live calls. It does not call DataServiceClient,
does not modify the adapter skeleton, does not wire QML, does not access
SQLite, does not call AccountingEngine, and does not write database tables. It
is a review gate for a separately authorized live-call implementation task.

## Current Status

- `ShellAccountingDataServiceAdapter` skeleton exists.
- All current fetch methods return
  `SHELL_ACCOUNTING_DATASERVICE_ADAPTER_NOT_CONNECTED`.
- ShellAccountingDataServiceAdapter spy/wrapper scaffolding exists.
- DataServiceClient accounting guard wrappers exist.
- DataService accounting guard suite exists.
- Real DataServiceClient integration has not started.
- QML is not wired to accounting actions.
- DataService actions remain read-only guard actions.
- Real `position.list` is not implemented.

## Gate Summary

Passing this gate only means a future live-call implementation task may be
prepared. It does not authorize implementation in the current task.

A future live-call task must be separately authorized. It may only start from
read-only guard wrappers, must not wire QML, must not access SQLite, must not
call AccountingEngine, must not write database tables, and must not generate
TradeDraft or trade suggestions.

## Read-only Allowlist

The first live-call implementation may only call:

- `DataServiceClient::positionList`
- `DataServiceClient::cashSummary`
- `DataServiceClient::portfolioPnlSummary`
- `DataServiceClient::basePositionSummary`
- `DataServiceClient::sniperPoolSummary`

These five wrappers are the only allowed first live-call targets. They are
currently guard wrappers. Future real accounting actions must still preserve
`readOnly=true` and `writeEnabled=false`.

## Forbidden Call Denylist

The live adapter must not call:

- `data.audit.append`
- trade write actions
- `trade_execution_group` write paths
- `trade_draft` create / update / send paths
- `cash_snapshot` write paths
- `position_snapshot` write paths
- `portfolio_summary` write paths
- strategy execution actions
- broker order actions
- any write-enabled action
- any direct QML call
- any direct SQLite access
- any direct AccountingEngine call from the adapter

## Request Mapping Gate

Future live adapter request mapping must convert
`ShellAccountingServiceRequest` into a JSON object.

Required preserved fields:

- `accountId`
- `portfolioId`
- `sourceFromTime`
- `sourceToTime`
- `includeIssues`
- `includeDebugMetadata`
- `calculationVersion`
- `requestedOutputs`
- `timeoutMs`, used only for timeout behavior

The request payload must not add `writeEnabled=true`, TradeDraft fields, trade
suggestion fields, strategy execution fields, or broker order fields.

## Response Mapping Gate

Future response mapping from DataService ProtocolResponse / payload to
`ShellAccountingServiceResult` must preserve:

- `ProtocolResponse.success -> protocolSuccess`
- `payload.implemented -> implemented`
- `payload.readOnly -> readOnly`
- `payload.writeEnabled -> writeEnabled`
- `payload.status -> payloadStatus`
- `payload.dataQualityStatus -> dataQualityStatus`
- `payload.issues -> issues`
- `payload.warnings -> warnings`
- `payload.errors -> errors`
- raw payload

Unknown fields must not crash mapping. `implemented=false` is a domain
unavailable result, not a protocol failure. `*_NOT_AVAILABLE` statuses must be
preserved as domain unavailable payload statuses.

## Error Mapping Gate

The live adapter must define controlled behavior for:

- malformed JSON
- `ProtocolResponse.success=false`
- timeout
- transport failure
- server unavailable
- `implemented=false`
- `*_NOT_AVAILABLE`
- `MARKET_PRICE_MISSING`
- `MULTI_CURRENCY_UNSUPPORTED`
- `FX_RATE_MISSING`
- `REPLAY_NOT_AVAILABLE`
- unknown issue

Protocol error and domain issue must remain distinct. Warning issues must not
be swallowed. Blocking issues must remain visible. Timeout, transport failure,
and malformed response paths must not trigger writes.

## No-write / No-trade Gate

- The adapter itself must not access a database.
- The adapter may only call allowlisted read-only wrappers.
- The adapter must not call `data.audit.append`.
- The adapter must not call any write action.
- The adapter must not generate TradeDraft.
- The adapter must not generate trade suggestions.
- The adapter must not execute strategy.
- The adapter must not submit broker orders.
- The adapter must not trigger snapshot writes.
- The adapter must not write `audit_log` unless separately authorized.
- All tests must preserve no-write / no-trade behavior.

## Controller Integration Gate

- Controller integration must continue through `ShellAccountingServiceAdapter`.
- `ShellAccountingReadOnlyController` must not directly depend on
  DataServiceClient.
- Controller live integration must call the adapter, not wrappers directly.
- State mapping must continue to preserve:
  - `implemented=false -> Unavailable`
  - warning -> `Warning`
  - error -> `Error`
  - stale -> `Stale`
- Controller integration must not wire QML.
- Controller integration must not generate trade suggestions.

## Rollback / Fallback Gate

The future live-call implementation must retain a safe fallback to the
not-connected skeleton behavior or an equivalent unavailable result.

Fallback requirements:

- missing DataServiceClient dependency returns adapter not connected
- unavailable transport returns transport error / unavailable
- fallback must not return fabricated data
- fallback must not write database tables
- fallback must not generate TradeDraft or trade suggestions

## Future Live-call PR Requirements

A future live-call PR must state:

- which wrappers are enabled
- whether each wrapper is read-only
- that QML remains unwired
- that SQLite remains unaccessed by the adapter
- that AccountingEngine remains uncalled by the adapter
- no-write / no-trade test results
- protocol / domain / timeout / transport mapping test results
- rollback behavior
- full `ctest` result
- `transport_local_socket_echo` 50-repeat result

## TASK-110 Live-call Skeleton Status

TASK-110 satisfies part of this gate with a test-first skeleton. The adapter now
has an internal abstract client port and can exercise method mapping, request
mapping, response mapping, error mapping, and no-write / no-trade behavior with
a test-only spy port.

This does not enable real DataServiceClient calls. The concrete DataServiceClient
port is still absent, QML remains unwired, SQLite remains unaccessed, and
AccountingEngine remains uncalled. The default no-port behavior remains the safe
not-connected / unavailable fallback.

## TASK-111 Concrete Guard Wrapper Port Status

TASK-111 adds the concrete `ShellAccountingDataServiceClientPortAdapter`. This
is the only ShellServices production class that may include and hold the real
`DataServiceClient`.

The concrete port is limited to the first live-call scope:

- `DataServiceClient::positionList`
- `DataServiceClient::cashSummary`
- `DataServiceClient::portfolioPnlSummary`
- `DataServiceClient::basePositionSummary`
- `DataServiceClient::sniperPoolSummary`

`ShellAccountingDataServiceAdapter` and `ShellAccountingReadOnlyController`
still do not directly include `DataServiceClient`. The concrete port preserves
request payload / timeout forwarding, `ProtocolResponse` mapping, guard
`implemented=false` payloads, protocol failure, timeout, transport error,
malformed response, domain issues, no-write flags, and no-trade flags.

QML remains unwired. SQLite, AccountingEngine, DataAccess, write actions,
TradeDraft generation, trade suggestions, strategy execution, and broker orders
remain forbidden.
