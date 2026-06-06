# TASK-253 ShellAccounting Manual Entry Replay Fixture-Backed VBA Parity Test Plan

## Test Objective

This plan verifies that TASK-253 loads a dedicated test-only fixture, parses input facts, calls AccountingEngine read-only replay, builds an in-memory projection summary, and proves exact parity with expected fixture output.

The test must not become production runtime integration and must not write production data.

## Required CTest

`shell_accounting_manual_entry_replay_fixture_backed_vba_parity_readonly_vertical_slice`

The CTest must execute the TASK-253 helper under `tests/`, load the TASK-253 fixture JSON, call AccountingEngine read-only replay, compare expected and actual projection fields, and emit sanitized stdout JSON evidence.

## Fixture Checks

The fixture must include:

- initial cash;
- one BUY;
- one partial SELL or an explicit replay issue scenario;
- expected cash summary;
- expected position summary;
- expected PnL and data-quality status.

TASK-253 uses a partial SELL fixture so parity must pass without falling back to unsupported-scenario evidence.

## Runtime Evidence Checks

The stdout JSON must include:

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
- all no-write, no-broker, no-network, no-credential, no-endpoint, and no-automatic-trading flags set to `false`.

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

TASK-253 must not modify:

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
- `ctest --test-dir build -R shell_accounting_manual_entry_replay_fixture_backed_vba_parity_readonly_vertical_slice --output-on-failure`;
- `ctest --test-dir build --output-on-failure`;
- `ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure`;
- `git diff --check`;
- `git status --short`.

## Formal Conclusion

TASK-253 passes only when the fixture-backed read-only replay projection exactly matches expected cash, position, and PnL output, with no production write, no broker, no network, and no automatic trading.
