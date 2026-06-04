# ShellAccounting Manual Entry Replay Fixture Static Validator Negative Fixtures Authorization Test Plan

## Document Purpose

This test plan defines TASK-220 gate-only validation for future
ShellAccounting manual entry replay fixture static validator negative fixture
coverage.

TASK-220 is negative-fixtures-authorization-only. It does not create negative
fixture files, does not implement negative validator logic, does not implement
parser / loader / reader code, does not run replay, does not call
AccountingEngine replay, does not modify existing fixture JSON, and does not
add runtime SQL / SQLite read/write, audit / ledger / snapshot writes,
backup/export/restore, broker, network, credentials, endpoint, real order
placement, or automatic trading.

## Test Matrix

- docs/236 exists.
- docs/237 exists.
- README references TASK-220 and docs/236 / docs/237.
- docs/README references docs/236 / docs/237.
- docs/12 references TASK-220 and docs/236 / docs/237.
- TASK-220 is documented as gate-only.
- TASK-220 documents no negative fixture creation.
- TASK-220 documents no negative validator implementation.
- TASK-220 documents no parser / loader / reader implementation.
- TASK-220 documents no replay implementation.
- TASK-220 documents no AccountingEngine replay call.
- TASK-220 documents no runtime SQL / SQLite read/write.
- TASK-220 documents no audit / ledger / snapshot writes.
- TASK-220 documents no backup/export/restore.
- TASK-220 documents no broker / network / credentials / endpoint / real order / automatic trading.
- Future negative fixture directory boundary is documented.
- Future negative fixture naming policy is documented.
- NEG_MRF001 through NEG_MRF010 candidate names are documented.
- Future negative fixture schema policy is documented.
- Required schema fields are documented.
- Future negative fixture case matrix is documented.
- Future negative expected issue codes are documented.
- Static validator negative-case boundary is documented.
- Privacy and sanitization boundary is documented.
- No-replay boundary is documented.
- No-runtime-dependency boundary is documented.
- Out-of-scope boundaries are documented.
- Formal next task is TASK-221.
- New CTest `shell_accounting_manual_entry_replay_fixture_negative_fixtures_authorization` is registered.
- Changed-path allowlist is exact.
- Untracked-file detection is active.
- `tests/fixtures/manual_entry_replay_negative/` does not exist.
- No negative fixture JSON files are created.
- No existing `tests/fixtures/manual_entry_replay/*.json` file is modified.
- TASK-219 static validator script is not modified.
- No parser / loader / reader implementation is created.
- No production source is modified.
- apps / libs production code is unchanged.
- migrations are unchanged.
- TASK-219 / TASK-218 / TASK-217 / TASK-216 / TASK-215 / TASK-214 / TASK-213 / TASK-212 gates remain present.
- Broker and real broker gate directories remain present.
- transport_local_socket_echo registration remains present.

## Required Probes

- Static source scan.
- Documentation registration scan.
- CTest registration scan.
- Changed-path allowlist scan.
- Untracked-file detection scan.
- No negative fixture directory probe.
- No negative fixture JSON probe.
- Existing positive fixture JSON unchanged probe.
- TASK-219 validator script unchanged probe.
- No parser / loader / reader implementation scan.
- No replay implementation scan.
- No AccountingEngine replay call scan.
- No runtime SQL / SQLite read/write scan.
- No audit / ledger / snapshot write scan.
- No backup/export/restore scan.
- No production code change scan.
- No QML / startup / Presenter / Controller / ShellServices scan.
- No DataServiceActions / DataServiceActionRegistrar scan.
- No DataAccess repository change scan.
- No migration change scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- NEG_MRF001 through NEG_MRF010 documentation probe.
- Negative schema field documentation probe.
- Negative issue code documentation probe.
- Sanitized diagnostics documentation probe.
- TASK-219 static validator regression.
- TASK-218 static validator authorization regression.
- TASK-217 fixture scaffold regression.
- TASK-216 through TASK-212 replay policy regressions.
- Manual entry readback/runtime regressions.
- Broker and real broker gate regressions.
- transport_local_socket_echo 50-repeat regression.

## Go / No-Go Checklist

Go only if:

- docs/236 and docs/237 exist and are indexed.
- TASK-220 CTest is registered.
- TASK-220 documents negative fixture policy only.
- TASK-220 does not create `tests/fixtures/manual_entry_replay_negative/`.
- TASK-220 does not create negative fixture JSON files.
- TASK-220 does not modify existing positive fixture JSON files.
- TASK-220 does not modify the TASK-219 static validator implementation.
- TASK-220 does not create parser / loader / reader implementation.
- TASK-220 does not implement replay or call AccountingEngine replay.
- TASK-220 does not add runtime SQL / SQLite read/write.
- TASK-220 does not write audit / ledger / snapshot rows.
- TASK-220 does not implement backup/export/restore.
- TASK-220 does not modify production code, QML, startup, Presenter,
  Controller, ShellServices, DataServiceActions, DataAccess, or migrations.
- TASK-220 does not connect broker / network / credentials / endpoint.
- TASK-220 does not place real orders or enable automatic trading.
- TASK-219, TASK-218, TASK-217, TASK-216, TASK-215, TASK-214, TASK-213, and
  TASK-212 regressions remain green.
- Manual entry readback/runtime regressions remain green.
- Broker and real broker gates remain green.

No-Go if:

- Any negative fixture directory or negative fixture JSON file is created.
- Any existing positive fixture JSON file is modified.
- Any parser / loader / reader implementation appears.
- Any replay execution or AccountingEngine replay call appears.
- Any runtime SQL / SQLite read/write appears.
- Any audit / ledger / snapshot write appears.
- Any backup/export/restore implementation appears.
- Any production code, production QML, DataServiceActions, repository, or
  migration path changes.
- Any broker / network / credentials / endpoint integration appears.
- Any real order placement, real broker order id, or automatic trading appears.
- An old gate blocks TASK-220 exact paths and no separate authorization is provided.
