# TASK-253 ShellAccounting Manual Entry Replay Fixture-Backed VBA Parity Read-Only Vertical Slice

## Purpose

TASK-253 advances TASK-252 from inline synthetic facts to a TASK-253-specific fixture-backed VBA parity read-only vertical slice. The slice loads test-only fixture facts, calls AccountingEngine read-only replay, builds an in-memory projection summary, compares that projection to expected fixture output, and emits sanitized stdout JSON evidence.

This task is not production runtime integration. It does not authorize production writes, persistent read models, fixture history rewrites, broker activity, network access, credential access, endpoints, real order placement, or automatic trading.

## Relationship To TASK-252

TASK-252 proved that inline synthetic facts can pass through a test-only adapter and call AccountingEngine read-only replay. TASK-253 keeps that read-only boundary but replaces inline facts with a dedicated TASK-253 fixture and expected projection assertions.

TASK-253 does not modify TASK-252 behavior, production code, AccountingEngine code, DataServiceActions, repositories, migrations, QML, startup, Presenter, or Controller wiring.

## Fixture-Backed Slice

The authorized path is:

test-only fixture input facts -> test-only fixture loader / parser -> AccountingEngine read-only replay -> in-memory projection summary -> expected projection parity assertions -> sanitized stdout JSON evidence.

The TASK-253 fixture covers:

- initial cash;
- one BUY;
- one partial SELL;
- expected cash summary;
- expected position summary;
- expected PnL and data-quality status.

The fixture is local to `tests/ShellAccountingManualEntryReplayFixtureBackedVbaParityReadOnlyVerticalSlice/fixtures/` and does not modify historical replay fixtures.

## Read-Only Replay Contract

The CTest may call `AccountingReplayEngine::replayReadOnly` only from the TASK-253 test-only helper. The result is converted to an in-memory summary and compared against fixture expected projection fields.

The helper must not call write APIs, SQLite, DataServiceActions, repositories, QML, startup wiring, broker adapters, strategy execution, network, credentials, endpoints, or order placement paths.

## Evidence JSON Contract

The CTest stdout evidence must include:

- `task=TASK-253`;
- `mode=test-only-fixture-backed-vba-parity-readonly-vertical-slice`;
- `fixtureLoaded=true`;
- `expectedProjectionLoaded=true`;
- `replayExecuted=true`;
- `accountingEngineCalled=true`;
- `parityPassed=true`;
- `cashSummaryMatched=true`;
- `positionSummaryMatched=true`;
- `pnlSummaryMatched=true`;
- `fixtureMutated=false`;
- `productionWrite=false`;
- `sqliteProductionWrite=false`;
- `auditWritten=false`;
- `ledgerWritten=false`;
- `snapshotWritten=false`;
- `tradeLogWritten=false`;
- `readModelPersistentWrite=false`;
- `tradeDraftGenerated=false`;
- `strategyExecuted=false`;
- `brokerOrderSubmitted=false`;
- `networkAccess=false`;
- `credentialAccess=false`;
- `endpointAccess=false`;
- `automaticTrading=false`.

## Production Boundary

TASK-253 must not modify:

- `apps/`;
- `libs/AccountingEngine/`;
- `libs/DataServiceApi/`;
- `libs/DataAccess/`;
- `migrations/`;
- production QML;
- production startup;
- production Presenter / Controller wiring;
- historical fixture JSON;
- broker / network / credentials / endpoint code.

## Write Boundary

TASK-253 output is limited to in-memory projection summary and stdout JSON evidence. It must not write SQLite, audit, ledger, snapshot, trade log, persistent read model, or TradeDraft data.

## Formal Conclusion

TASK-253 is a test-only fixture-backed VBA parity read-only vertical slice. It proves that a dedicated fixture can drive AccountingEngine read-only replay and match expected projection output exactly.

Future production runtime integration, read model refresh, production parser / loader / reader work, broker integration, strategy execution, real order placement, or automatic trading must be separately authorized.
