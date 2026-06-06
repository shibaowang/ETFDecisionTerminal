# TASK-254 ShellAccounting Manual Entry Replay Fixture Parity Matrix Test Plan

## Test Objective

This plan verifies that TASK-254 loads a dedicated test-only fixture matrix, parses input facts, calls AccountingEngine read-only replay for each fixture, builds in-memory projection summaries, and proves exact parity with expected fixture output or sanitized issue evidence.

The test must not become production runtime integration and must not write production data.

## Required CTest

`shell_accounting_manual_entry_replay_fixture_parity_matrix_readonly_vertical_slice`

The CTest must execute the TASK-254 helper under `tests/`, load every TASK-254 fixture JSON, call AccountingEngine read-only replay, compare expected and actual projection fields, compare sanitized issue scenarios, and emit sanitized stdout JSON evidence.

## Fixture Matrix Checks

The matrix must include at least four TASK-254-only fixtures:

- `buy_only`: initial cash plus one BUY;
- `buy_partial_sell`: initial cash plus BUY and partial SELL;
- `cash_adjustment`: initial cash plus cash in or cash out, with explicit sanitized unsupported replay evidence if the current engine does not support the scenario;
- `unsupported_or_issue`: explicit replay issue evidence with sanitized output.

TASK-254 fixtures must be local to `tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/` and must not modify historical replay fixture JSON.

## Runtime Evidence Checks

The stdout JSON must include:

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
- all no-write, no-broker, no-network, no-credential, no-endpoint, and no-automatic-trading flags set to `false`.

## Per-Fixture Parity Checks

For successful fixtures, the CTest must compare:

- cash summary;
- all position summaries;
- portfolio PnL summary;
- replay status.

For issue fixtures, the CTest must compare sanitized replay status and issue code / level / blocking / field / source id only. It must not emit raw issue messages or raw input payload.

## No-Write Checks

The CTest must prove:

- `productionWrite=false`;
- `sqliteProductionWrite=false`;
- `auditWritten=false`;
- `ledgerWritten=false`;
- `snapshotWritten=false`;
- `tradeLogWritten=false`;
- `readModelPersistentWrite=false`;
- `tradeDraftGenerated=false`.

## No Trading Checks

The CTest must prove:

- `strategyExecuted=false`;
- `brokerOrderSubmitted=false`;
- `networkAccess=false`;
- `credentialAccess=false`;
- `endpointAccess=false`;
- `automaticTrading=false`.

## Production Boundary Checks

TASK-254 must not modify:

- `apps/`;
- `libs/AccountingEngine/`;
- `libs/DataServiceApi/`;
- `libs/DataAccess/`;
- `migrations/`;
- production QML / startup / Presenter / Controller wiring;
- historical fixture JSON;
- broker / network / credentials / endpoint code.

## Required Commands

- `cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64`;
- `cmake --build build`;
- `ctest --test-dir build -R shell_accounting_manual_entry_replay_fixture_parity_matrix_readonly_vertical_slice --output-on-failure`;
- `ctest --test-dir build --output-on-failure`;
- `ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure`;
- `git diff --check`;
- `git status --short`.

## Formal Conclusion

TASK-254 passes only when every fixture in the test-only parity matrix matches expected projection or sanitized issue evidence, with no production write, no broker, no network, and no automatic trading.
