# ShellAccounting Manual Entry Replay Negative Fixture Scaffold Files Test Plan

## Document Purpose

This test plan defines the TASK-222 validation for creating test-only manual
entry replay negative fixture scaffold files. The validation proves the new
negative fixture directory and JSON files are controlled scaffold metadata only
and do not introduce replay, runtime data loading, production code, database
write, broker, network, credential, endpoint, real order, or automatic trading
capability.

## Test Matrix

- docs/240 exists.
- docs/241 exists.
- README references TASK-222 and docs/240 / docs/241.
- docs/README references docs/240 / docs/241.
- docs/12 references TASK-222 and docs/240 / docs/241.
- TASK-222 CTest is registered.
- `tests/fixtures/manual_entry_replay_negative/` exists.
- The negative fixture directory contains only `negative_fixtures_index.json`
  and the ten authorized `NEG_MRF*.json` files.
- The index is valid JSON.
- The index uses schema version
  `manual-entry-replay-negative-fixtures-index/v1`.
- The index marks `scaffoldOnly=true`, `runtimeUse=false`,
  `productionUse=false`, `replayExecuted=false`, and
  `containsSyntheticDataOnly=true`.
- The index exactly references ten `NEG_MRF*.json` files.
- Every `NEG_MRF*.json` file is valid JSON.
- Every `NEG_MRF*.json` file uses schema version
  `manual-entry-replay-negative-fixture-scaffold/v1`.
- Every `NEG_MRF*.json` file has a unique `negativeFixtureId`.
- Every file name and `negativeFixtureId` are aligned.
- Every expected issue code matches the TASK-222 mapping.
- Every source positive fixture id exists in the TASK-217 positive fixture
  index.
- Every scaffold file marks `scaffoldOnly=true`, `runtimeUse=false`,
  `productionUse=false`, `replayExecuted=false`, and
  `containsSyntheticDataOnly=true`.
- Every scaffold file marks `metadata.task=TASK-222`,
  `metadata.notReplayInput=true`, `metadata.notProductionInput=true`, and
  `metadata.notRuntimeDataSource=true`.
- Existing positive fixture JSON files are not modified.
- TASK-219 static validator is not modified.
- No negative validator / parser / loader / reader / replay implementation is
  added.
- No AccountingEngine replay call is added.
- No runtime SQL / SQLite read/write is added.
- No audit / ledger / snapshot write is added.
- No production code, QML, DataServiceActions, repositories, migrations,
  broker, network, credentials, endpoint, real order, or automatic trading path
  changes.

## Required Probes

- Static source scan.
- Documentation registration scan.
- CTest registration scan.
- Changed-path allowlist scan.
- Untracked-file detection scan.
- Negative fixture directory exact-file-set probe.
- Negative fixture index JSON probe.
- Negative fixture JSON schema probe.
- Negative fixture id uniqueness probe.
- Negative fixture file/id alignment probe.
- Expected issue code mapping probe.
- Positive fixture index reference probe.
- Scaffold-only flag probe.
- Runtime / production / replay disabled flag probe.
- Synthetic-only privacy probe.
- DB / WAL / SHM file absence probe.
- Production path absence probe.
- Raw SQL absence probe.
- Credential and endpoint absence probe.
- Broker payload and real order id absence probe.
- Existing positive fixture JSON unchanged probe.
- TASK-219 static validator unchanged probe.
- No negative validator / parser / loader / reader implementation scan.
- No replay implementation scan.
- No AccountingEngine replay call scan.
- No runtime SQL / SQLite read/write scan.
- No audit / ledger / snapshot write scan.
- No production code / QML / DataServiceActions / repository / migration scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- TASK-221 negative fixture scaffold authorization regression.
- TASK-220 negative fixture authorization regression.
- TASK-219 static validator regression.
- TASK-217 positive fixture scaffold regression.
- Manual entry readback/runtime regressions.
- Broker and real broker gate regressions.
- transport_local_socket_echo 50-repeat regression.

## Go / No-Go Checklist

Go only if:

- docs/240 and docs/241 exist and are indexed by README, docs/README, and docs/12.
- TASK-222 CTest is registered.
- The negative fixture directory exists and contains only 11 JSON files.
- The index exactly references 10 `NEG_MRF*.json` files.
- All 10 `NEG_MRF*.json` files are valid JSON.
- Every JSON file has `scaffoldOnly=true`, `runtimeUse=false`,
  `productionUse=false`, `replayExecuted=false`, and
  `containsSyntheticDataOnly=true`.
- Expected issue code mapping is correct.
- Every `sourcePositiveFixtureId` comes from the TASK-217 positive fixture
  index.
- Existing positive fixture JSON is not modified.
- TASK-219 static validator is not modified.
- No negative validator / parser / loader / reader / replay implementation is
  present.
- No AccountingEngine replay call is present.
- No production code / QML / DataServiceActions / repositories / migrations /
  broker path changes are present.

No-Go if:

- Any unauthorized JSON, DB, WAL, SHM, SQLite, runtime output, parser, loader,
  reader, or replay output file appears in the negative fixture directory.
- Any scaffold file is marked for runtime, production, or replay use.
- Any scaffold file embeds raw payloads, executable SQL, credentials, endpoint
  values, broker payloads, real order ids, or production paths.
- Any positive fixture JSON file is modified.
- TASK-219 static validator is modified.
- Any negative validator, parser, loader, reader, replay, AccountingEngine
  replay call, runtime SQL / SQLite read/write, audit / ledger / snapshot write,
  production code, QML, DataServiceActions, repository, migration, broker,
  network, credential, endpoint, real order, or automatic trading change appears.
- Any required CTest pattern matches zero tests.
