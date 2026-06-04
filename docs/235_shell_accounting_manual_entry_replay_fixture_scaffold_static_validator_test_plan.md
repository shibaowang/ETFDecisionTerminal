# ShellAccounting Manual Entry Replay Fixture Scaffold Static Validator Test Plan

## Document Purpose

This test plan defines TASK-219 validation for the test-only ShellAccounting
manual entry replay fixture scaffold static validator.

TASK-219 is static-validator-only. It validates fixture scaffold files and does
not implement replay, does not call AccountingEngine replay, does not import
production modules, does not modify fixture JSON files, does not add runtime
SQL / SQLite read/write, does not write audit / ledger / snapshot rows, and
does not connect broker / network / credentials / endpoint or place real
orders.

## Test Matrix

- CTest `shell_accounting_manual_entry_replay_fixture_static_validator` is registered.
- Fixture directory exists.
- Expected exact file set exists.
- Missing fixture file fails closed.
- Extra JSON fixture file fails closed.
- DB / SQLite / WAL / SHM file presence fails closed.
- Runtime parser / loader / reader / replay runner output fails closed.
- `fixtures_index.json` parses as JSON.
- MRF001 through MRF006 parse as JSON.
- Index `schemaVersion` is `manual-entry-replay-fixtures.scaffold.v1`.
- Index `fixtureSet` is `manual_entry_replay_scaffold`.
- Index `generatedByTask` is `TASK-217`.
- Index `runtimeUse=false`.
- Index `productionUse=false`.
- Index `replayExecuted=false`.
- Index `containsSyntheticDataOnly=true`.
- Index fixture ids are unique.
- Index fixture ids are exactly MRF001 through MRF006.
- Index fixture file references are exact.
- Index `expectedStatus` is `SCAFFOLD_ONLY`.
- Index `privacyStatus` is `SYNTHETIC_ONLY`.
- Index `blockingExpected` is boolean.
- Each fixture has schemaVersion `manual-entry-replay-fixture.scaffold.v1`.
- Each fixture id matches the file prefix.
- Each fixture has non-empty title, purpose, and category.
- Each fixture has `scaffoldOnly=true`.
- Each fixture has `runtimeUse=false`.
- Each fixture has `productionUse=false`.
- Each fixture has `replayExecuted=false`.
- Each fixture has `containsSyntheticDataOnly=true`.
- Each fixture has sourceFacts, expectedOutputs, expectedIssues, blockingExpected, privacyExpectations, and metadata.
- Expected outputs remain `SCAFFOLD_ONLY`, `UNAVAILABLE_EXPECTED`, or `TODO_PLACEHOLDER`.
- No concrete replay-calculated PnL is accepted.
- No concrete market value is accepted.
- No concrete cash or position replay output is required.
- Source facts remain synthetic placeholders.
- Privacy expectations include `SYNTHETIC_ONLY` and `SANITIZED_MEMO`.
- Forbidden raw SQL token scan is active.
- Forbidden SQL write token scan is active.
- Forbidden credentials token scan is active.
- Forbidden endpoint token scan is active.
- Forbidden broker payload token scan is active.
- Forbidden real order id token scan is active.
- Forbidden SQLite runtime marker scan is active.
- Forbidden raw trade_log / cash_adjustment payload scan is active.
- Forbidden internal stack-trace marker scan is active.
- Validator emits sanitized issue codes only.
- Validator does not output raw fixture payload.
- Validator does not import production modules.
- Validator does not import sqlite3.
- Validator does not call AccountingEngine replay.
- Validator does not write files.
- Validator does not write SQLite.
- Validator does not compute position / cash / PnL / market value.

## Required Probes

- Static source scan.
- CTest registration scan.
- Exact fixture file set probe.
- Extra JSON file probe.
- DB / SQLite / WAL / SHM file probe.
- Parser / loader / reader / replay runner file probe.
- Index JSON parse probe.
- Fixture JSON parse probe.
- Index scaffold metadata probe.
- Fixture scaffold metadata probe.
- Unique fixture id probe.
- Exact fixture reference probe.
- Placeholder expected-output probe.
- Synthetic source fact probe.
- Privacy marker probe.
- Forbidden raw SQL scan.
- Forbidden SQL write token scan.
- Forbidden credential scan.
- Forbidden endpoint scan.
- Forbidden broker payload scan.
- Forbidden real order id scan.
- Forbidden SQLite runtime marker scan.
- Forbidden raw trade_log / cash_adjustment payload scan.
- Forbidden stack-trace marker scan.
- Sanitized diagnostics probe.
- No production module import scan.
- No AccountingEngine replay call scan.
- No sqlite3 import scan.
- No runtime SQL / SQLite read/write scan.
- No audit / ledger / snapshot write scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- TASK-218 authorization gate regression.
- TASK-217 fixture scaffold regression.
- TASK-216 through TASK-212 replay policy regressions.
- Manual entry readback/runtime regressions.
- Broker and real broker gate regressions.
- transport_local_socket_echo 50-repeat regression.

## Go / No-Go Checklist

Go only if:

- docs/234 and docs/235 exist and are indexed.
- The TASK-219 CTest is registered.
- The static validator reads only the exact test-only fixture scaffold files.
- The static validator validates the expected file set, index metadata, fixture metadata, placeholder output semantics, synthetic privacy markers, and forbidden tokens.
- The static validator emits only sanitized issue codes and short safe context.
- The static validator does not import production modules.
- The static validator does not import sqlite3.
- The static validator does not call AccountingEngine replay.
- The static validator does not modify fixture JSON files.
- The static validator does not write runtime SQL / SQLite.
- The static validator does not write audit / ledger / snapshot rows.
- The static validator does not connect broker / network / credentials / endpoint.
- The static validator does not place real orders or enable automatic trading.
- TASK-218, TASK-217, TASK-216, TASK-215, TASK-214, TASK-213, and TASK-212 gates remain green.
- Manual entry readback/runtime regressions remain green.
- Broker and real broker gates remain green.

No-Go if:

- Any fixture JSON file is modified.
- Any production module is imported by the validator.
- Any replay execution or AccountingEngine replay call appears.
- Any runtime SQL / SQLite read/write appears.
- Any audit / ledger / snapshot write appears.
- Any backup/export/restore implementation appears.
- Any broker / network / credentials / endpoint integration appears.
- Any real order placement, real broker order id, or automatic trading appears.
- An old gate blocks TASK-219 exact paths and no separate authorization is provided.
