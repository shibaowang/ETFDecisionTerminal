# ShellAccounting Manual Entry Replay Negative Fixtures Scaffold Authorization Test Plan

## Document Purpose

This test plan defines the TASK-221 authorization-gate-only validation for the
future manual entry replay negative fixtures scaffold.

TASK-221 does not create negative fixture files, does not create the negative
fixture directory, does not implement negative validator logic, does not
implement parser / loader / reader code, does not run replay, does not call
AccountingEngine replay, does not modify existing fixture JSON, and does not
modify production code, DataServiceActions, repositories, migrations, QML,
broker, network, credentials, or endpoint code.

## Test Matrix

- docs/238 exists.
- docs/239 exists.
- README references TASK-221 and docs/238 / docs/239.
- docs/README references docs/238 / docs/239.
- docs/12 references TASK-221 and docs/238 / docs/239.
- TASK-221 CTest is registered.
- TASK-221 is documented as authorization-gate-only.
- TASK-221 documents relationship to TASK-220.
- TASK-221 documents future directory contract.
- TASK-221 documents future index contract.
- TASK-221 documents future negative fixture file set.
- TASK-221 documents future metadata contract.
- TASK-221 documents future mutation categories.
- TASK-221 documents future issue code mapping.
- TASK-221 documents privacy and sanitization boundary.
- TASK-221 documents no replay.
- TASK-221 documents no runtime dependency.
- TASK-221 documents formal next task as TASK-222.
- `tests/fixtures/manual_entry_replay_negative/` is not created.
- `negative_fixtures_index.json` is not created.
- No `NEG_MRF*.json` file is created.
- Existing positive fixture JSON files are not modified.
- TASK-219 static validator is not modified.
- No negative validator / parser / loader / reader / replay implementation is created.
- No AccountingEngine replay call is added.
- No runtime SQL / SQLite read/write is added.
- No audit / ledger / snapshot write is added.
- No production code, QML, DataServiceActions, repositories, migrations, broker,
  network, credentials, endpoint, real order, or automatic trading path changes.

## Required Probes

- Static source scan.
- Documentation registration scan.
- CTest registration scan.
- Changed-path allowlist scan.
- Untracked-file detection scan.
- No negative fixture directory probe.
- No negative fixture JSON probe.
- Existing positive fixture JSON unchanged probe.
- TASK-219 static validator unchanged probe.
- Future directory contract probe.
- Future index contract probe.
- Future file set documentation probe.
- Future metadata contract documentation probe.
- Future mutation category documentation probe.
- Future issue code mapping documentation probe.
- Privacy and sanitization documentation probe.
- No negative validator / parser / loader / reader implementation scan.
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
- TASK-220 negative fixture authorization regression.
- TASK-219 static validator regression.
- TASK-218 static validator authorization regression.
- TASK-217 fixture scaffold regression.
- TASK-216 through TASK-212 replay policy regressions.
- Manual entry readback/runtime regressions.
- Broker and real broker gate regressions.
- transport_local_socket_echo 50-repeat regression.

## Go / No-Go Checklist

Go only if:

- docs/238 and docs/239 exist and are indexed by README, docs/README, and docs/12.
- TASK-221 CTest is registered.
- docs/238 covers future directory, index, file set, metadata, mutation
  categories, issue code mapping, privacy, no replay, and no runtime dependency.
- TASK-221 does not create `tests/fixtures/manual_entry_replay_negative/`.
- TASK-221 does not create `NEG_MRF*.json`.
- TASK-221 does not modify `tests/fixtures/manual_entry_replay/*.json`.
- TASK-221 does not modify the TASK-219 static validator.
- TASK-221 does not implement negative validator / parser / loader / reader / replay.
- TASK-221 does not call AccountingEngine replay.
- TASK-221 does not modify production code / QML / DataServiceActions /
  repositories / migrations / broker.
- TASK-220, TASK-219, TASK-218, TASK-217, TASK-216, TASK-215, TASK-214,
  TASK-213, and TASK-212 regressions remain green.
- Broker and real broker gates remain green.

No-Go if:

- Any negative fixture directory or negative fixture JSON file is created.
- Any positive fixture JSON file is modified.
- Any negative validator, parser, loader, reader, or replay implementation appears.
- Any AccountingEngine replay call appears.
- Any runtime SQL / SQLite read/write appears.
- Any audit / ledger / snapshot write appears.
- Any production code, QML, DataServiceActions, repository, or migration path changes.
- Any broker / network / credentials / endpoint integration appears.
- Any real order placement, real broker order id, or automatic trading appears.
- Any required CTest pattern matches zero tests.
