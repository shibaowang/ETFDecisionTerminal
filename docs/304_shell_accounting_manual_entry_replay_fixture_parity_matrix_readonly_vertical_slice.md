# TASK-254 ShellAccounting Manual Entry Replay Fixture Parity Matrix Read-Only Vertical Slice

## Purpose

TASK-254 extends TASK-253 from one fixture-backed VBA parity slice to a minimal fixture parity matrix. The matrix loads TASK-254-only fixture JSON, parses input facts through a shared test-only loader, calls AccountingEngine read-only replay, builds in-memory projection summaries, compares each fixture to expected parity output, and emits sanitized stdout JSON evidence.

This task is not a pure authorization gate and is not production runtime integration. It does not authorize production writes, persistent read models, fixture history rewrites, broker activity, network access, credential access, endpoints, real order placement, or automatic trading.

## Relationship To TASK-253

TASK-253 proved that one dedicated fixture with initial cash, BUY, and partial SELL can drive AccountingEngine read-only replay and match expected projection output exactly. TASK-254 keeps the same read-only boundary and expands the evidence into a small fixture matrix.

TASK-254 does not modify TASK-253 behavior, production code, AccountingEngine code, DataServiceActions, repositories, migrations, QML, startup, Presenter, or Controller wiring.

## Fixture Matrix Slice

The authorized path is:

TASK-254 test-only fixtures -> shared test-only fixture matrix loader -> AccountingEngine read-only replay -> in-memory projection summaries -> per-fixture expected parity assertions -> sanitized stdout JSON evidence.

The TASK-254 fixture matrix covers:

- `buy_only`: initial cash plus one BUY;
- `buy_partial_sell`: initial cash plus BUY and partial SELL;
- `cash_adjustment`: initial cash plus a cash adjustment fact, represented as an expected sanitized replay issue while AccountingEngine cash adjustment replay remains unsupported;
- `unsupported_or_issue`: an explicit SELL-exceeds-position issue scenario with sanitized evidence.

The fixtures are local to `tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/` and do not modify historical replay fixtures.

## Read-Only Replay Contract

The CTest may call `AccountingReplayEngine::replayReadOnly` only from the TASK-254 test-only helper. The helper converts replay results to in-memory summaries and compares the actual cash, position, PnL, replay status, and sanitized issue output to fixture expectations.

The helper must not call write APIs, SQLite, DataServiceActions, repositories, QML, startup wiring, broker adapters, strategy execution, network, credentials, endpoints, or order placement paths.

## Evidence JSON Contract

The CTest stdout evidence must include:

- `task=TASK-254`;
- `mode=test-only-fixture-parity-matrix-readonly-vertical-slice`;
- `fixtureMatrixLoaded=true`;
- `fixtureCount>=4`;
- `replayExecuted=true`;
- `accountingEngineCalled=true`;
- `allParityPassed=true`;
- `cashSummaryMatched=true`;
- `positionSummaryMatched=true`;
- `pnlSummaryMatched=true`;
- `issueScenarioSanitized=true`;
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

Per-fixture evidence may include sanitized issue code, level, blocking, field, and source id, but must not include raw payloads, raw SQL, credentials, endpoints, broker payloads, exception stacks, or unsanitized issue text.

## Production Boundary

TASK-254 must not modify:

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

TASK-254 output is limited to in-memory projection summaries and stdout JSON evidence. It must not write SQLite, audit, ledger, snapshot, trade log, persistent read model, or TradeDraft data.

## Future Work

Future Excel/VBA sample import, production runtime integration, read model refresh, production parser / loader / reader work, broker integration, strategy execution, real order placement, or automatic trading must be separately authorized.

## Formal Conclusion

TASK-254 is a test-only fixture parity matrix read-only vertical slice. It proves that multiple dedicated fixtures can drive AccountingEngine read-only replay and match expected projection or sanitized issue output without production writes or trading side effects.
