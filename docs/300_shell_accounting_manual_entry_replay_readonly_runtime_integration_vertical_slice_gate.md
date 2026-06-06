# TASK-252 ShellAccounting Manual Entry Replay Read-Only Runtime Integration Vertical Slice Gate

## Purpose

TASK-252 adds the first test-only read-only runtime integration vertical slice for ShellAccounting manual entry replay. The slice proves that synthetic manual entry replay facts can pass through a controlled test-only runtime adapter, call the AccountingEngine read-only replay API, and produce an in-memory ShellAccounting projection summary.

This task is not a production runtime integration. It does not authorize production writes, production bridge wiring, read model refresh, broker activity, network access, credential access, endpoints, real order placement, or automatic trading.

## Relationship To TASK-251

TASK-251 authorized future AccountingEngine runtime integration policy only. It did not call AccountingEngine replay and did not implement a runtime integration path.

TASK-252 stays inside the TASK-251 boundary but opens a minimum executable test-only vertical slice:

manual entry replay facts -> test-only read-only runtime adapter -> AccountingEngine read-only replay call -> in-memory position / cash / PnL summary -> deterministic CTest assertions.

## Authorization Scope

TASK-252 authorizes only:

- a test-only CTest under `tests/`;
- inline synthetic manual entry replay facts or TASK-252 test-only input;
- a test-only read-only adapter that calls AccountingEngine public read-only replay;
- stdout JSON evidence and in-memory projection summary assertions;
- failure-mode checks for missing input, malformed input, unsupported scenario, replay issue returned, sanitized error output, and no write on failure.

TASK-252 does not authorize production QML, startup, Presenter, Controller, DataServiceActions, repository, migration, or AccountingEngine implementation changes.

## Test-Only Runtime Integration Path

The runtime path is test-only. The adapter must be located under `tests/` and must be excluded from production startup and production service wiring.

The CTest must report:

- `runtimeIntegrationPath=test-only`;
- `replayExecuted=true` only for the test-only vertical slice evidence;
- `accountingEngineCalled=true` only for the test-only adapter;
- `productionBridgeImplemented=false`;
- `productionRuntimeIntegrationImplemented=false`.

## Read-Only Replay Contract

The test-only adapter may call `AccountingReplayEngine::replayReadOnly` or the public `replayReadOnly` free function. It must not call any write API, direct SQL API, DataServiceActions runtime action, repository write, broker adapter, strategy engine, or production UI path.

The result must be converted to an in-memory projection summary only. Persistent position, cash, PnL, snapshot, audit, ledger, and trade tables remain untouched.

## Evidence JSON Contract

The evidence JSON must include:

- `task=TASK-252`;
- `mode=test-only-readonly-runtime-integration-vertical-slice`;
- `runtimeIntegrationPath=test-only`;
- `replayExecuted=true`;
- `accountingEngineCalled=true`;
- `productionWrite=false`;
- `sqliteProductionWrite=false`;
- `auditWritten=false`;
- `ledgerWritten=false`;
- `snapshotWritten=false`;
- `tradeLogWritten=false`;
- `readModelPersistentWrite=false`;
- `tradeDraftGenerated=false`;
- `tradeSuggestionGenerated=false`;
- `strategyExecuted=false`;
- `brokerOrderSubmitted=false`;
- `networkAccess=false`;
- `credentialAccess=false`;
- `endpointAccess=false`;
- `automaticTrading=false`;
- `qmlDirectAccountingEngineCall=false`;
- `presenterDirectAccountingEngineCall=false`;
- `controllerDirectAccountingEngineCall=false`;
- `startupDirectAccountingEngineCall=false`;
- `positionSummaryAvailable=true`;
- `cashSummaryAvailable=true`;
- `pnlSummaryAvailable=true`;
- `issuesSanitized=true`.

## Failure Modes

The CTest must cover:

- missing input;
- malformed input;
- unsupported scenario;
- replay issue returned;
- sanitized error output;
- no write on failure.

Failure output must remain sanitized. It must not expose raw SQL, raw trade log payload, raw replay payload, internal exception stacks, broker payload, credentials, endpoints, or real order identifiers.

## Production Boundary

TASK-252 must not modify:

- `apps/`;
- production QML;
- startup;
- Presenter / Controller production wiring;
- `libs/DataServiceApi`;
- production repositories;
- `migrations/`;
- `libs/AccountingEngine/`.

QML, Presenter, Controller, and startup must not directly call AccountingEngine.

## Write Boundary

The vertical slice must report and enforce:

- `productionWrite=false`;
- `sqliteProductionWrite=false`;
- `auditWritten=false`;
- `ledgerWritten=false`;
- `snapshotWritten=false`;
- `tradeLogWritten=false`;
- `readModelPersistentWrite=false`.

No runtime SQL / SQLite read/write action is authorized by this task.

## Broker And Network Boundary

The vertical slice must report and enforce:

- `strategyExecuted=false`;
- `brokerOrderSubmitted=false`;
- `networkAccess=false`;
- `credentialAccess=false`;
- `endpointAccess=false`;
- `automaticTrading=false`.

No broker SDK, network client, credentials, endpoint, real order placement, or automatic trading path is authorized.

## Formal Conclusion

TASK-252 is a test-only read-only runtime integration vertical slice.

TASK-252 does not implement production runtime integration.

TASK-252 does not authorize production write.

TASK-252 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

## Recommended Next Task

The recommended next task is TASK-253 ShellAccounting manual entry replay fixture-backed VBA parity read-only vertical slice, unless the owner chooses another closeout or regression hardening task first.
