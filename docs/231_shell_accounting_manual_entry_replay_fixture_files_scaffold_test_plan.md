# ShellAccounting Manual Entry Replay Fixture Files Scaffold Test Plan

## Document Purpose

TASK-217 defines the test plan for ShellAccounting manual entry replay fixture
scaffold files. The task creates static test-only JSON scaffold files for
future replay fixture design. It does not implement replay, does not create a
parser or validator, does not create a loader or fixture reader, does not call
AccountingEngine replay, does not add runtime SQL / SQLite read/write, does
not write audit / ledger / snapshot rows, does not implement
backup/export/restore, and does not add broker, network, credentials,
endpoint, real order placement, or automatic trading behavior.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/230 exists | Verify `docs/230_shell_accounting_manual_entry_replay_fixture_files_scaffold.md` exists. |
| docs/231 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-217 and docs/230 / docs/231. |
| Fixture directory exists | Verify `tests/fixtures/manual_entry_replay/` exists. |
| fixtures_index exists | Verify `tests/fixtures/manual_entry_replay/fixtures_index.json` exists. |
| MRF001-MRF006 exist | Verify all six scaffold fixture JSON files exist. |
| JSON parse checks | Verify the index and each fixture are valid JSON. |
| Fixture id uniqueness | Verify fixture ids in the index are unique and match the files. |
| Index references all fixture files | Verify the index references MRF001 through MRF006 by exact relative file name. |
| scaffoldOnly flags | Verify every fixture has `scaffoldOnly=true`. |
| runtimeUse false | Verify the index and every fixture have `runtimeUse=false`. |
| productionUse false | Verify the index and every fixture have `productionUse=false`. |
| replayExecuted false | Verify the index and every fixture have `replayExecuted=false`. |
| synthetic-only privacy flags | Verify the index and every fixture have synthetic-only privacy markers. |
| Forbidden token scan | Verify fixture files do not include raw SQL, raw payload, credentials, endpoint, broker payload, real order id, real broker order id, real account data, or internal stacks. |
| No seed DB / WAL / SHM | Verify no DB, SQLite, WAL, SHM, or runtime fixture data files are added. |
| No parser / validator | Verify TASK-217 adds no parser, validator, loader, fixture reader, or replay runner. |
| No production drift | Verify production code, QML, startup, Presenter, Controller, ShellServices, DataServiceActions, DataServiceActionRegistrar, repositories, migrations, AccountingEngine, StrategyEngine, MarketEngine, broker, network, credentials, and endpoint paths are unchanged. |
| TASK-216 regression | Verify TASK-216 scaffold authorization files and CTest remain present. |
| TASK-215 regression | Verify TASK-215 replay fixture files authorization files and CTest remain present. |
| TASK-214 regression | Verify TASK-214 replay fixture matrix authorization files and CTest remain present. |
| TASK-213 regression | Verify TASK-213 replay policy authorization files and CTest remain present. |
| TASK-212 regression | Verify TASK-212 replay audit ledger adequacy review files and CTest remain present. |
| Broker / real broker gates | Verify broker disabled, broker order, real broker authorization, and real broker implementation gates remain present. |
| transport_local_socket_echo 50-repeat regression | Verify `transport_local_socket_echo` remains registered for repeated transport regression. |

## Required Probes

- Static source scan.
- Documentation token scan.
- Changed-path allowlist scan.
- Untracked-file detection scan.
- Fixture directory presence scan.
- Fixture index presence scan.
- MRF001-MRF006 presence scan.
- JSON parse scan.
- Fixture id uniqueness scan.
- Index-to-file reference scan.
- Scaffold-only flag scan.
- Runtime-use false scan.
- Production-use false scan.
- Replay-executed false scan.
- Synthetic-only privacy scan.
- Forbidden fixture token scan.
- No seed DB / SQLite / WAL / SHM scan.
- No parser / validator implementation scan.
- No loader / fixture reader / replay runner implementation scan.
- No production code changed scan.
- No production QML / startup / Presenter / Controller drift scan.
- No ShellServices adapter / port drift scan.
- No DataServiceActions drift scan.
- No DataServiceActionRegistrar drift scan.
- No DataAccess repository drift scan.
- No migrations drift scan.
- No runtime SQL / SQLite read/write scan.
- No AccountingEngine replay implementation scan.
- No AccountingEngine replay call scan.
- No audit / ledger write scan.
- No snapshot write scan.
- No backup/export/restore implementation scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- TASK-216 / TASK-215 / TASK-214 / TASK-213 / TASK-212 regression probes.
- Broker and real broker gate probes.
- transport_local_socket_echo 50-repeat regression probe.

## Go / No-Go Checklist

Go only if:

- docs/230 and docs/231 exist and are indexed by README, docs/README, and
  docs/12.
- TASK-217 CTest is registered.
- `tests/fixtures/manual_entry_replay/` exists.
- `fixtures_index.json` exists and parses as JSON.
- MRF001 through MRF006 exist and parse as JSON.
- Fixture ids are unique.
- The index references every fixture by exact file name.
- The index and every fixture are scaffold-only, runtimeUse=false,
  productionUse=false, replayExecuted=false, and synthetic-only.
- Fixture files do not include forbidden privacy or broker/order tokens.
- No DB, SQLite, WAL, SHM, seed data, or runtime fixture data files are added.
- No parser, validator, loader, fixture reader, or replay runner is added.
- Production code, QML, startup, Presenter, Controller, ShellServices,
  DataServiceActions, DataServiceActionRegistrar, repositories, migrations,
  AccountingEngine, StrategyEngine, MarketEngine, broker, network,
  credentials, endpoint, real order placement, and automatic trading are
  unchanged.
- TASK-216 / TASK-215 / TASK-214 / TASK-213 / TASK-212 gates remain present.
- Broker / real broker gates remain present.
- `transport_local_socket_echo` 50-repeat regression passes.

No-Go if:

- Fixture JSON cannot be parsed.
- Fixture ids are duplicated.
- The index omits an MRF001-MRF006 fixture or points to a wrong file.
- Any fixture is marked runtimeUse=true, productionUse=true, or
  replayExecuted=true.
- Any fixture is not synthetic-only.
- Seed DB, SQLite, WAL, SHM, or runtime fixture output appears.
- Fixture parser / validator / loader / reader implementation appears.
- Replay implementation appears.
- AccountingEngine replay call appears.
- Runtime SQL / SQLite read/write appears.
- DataServiceActions runtime behavior changes.
- Repository / migration changes appear.
- QML / Presenter / Controller / ShellServices production drift appears.
- Audit / ledger write appears.
- Snapshot write appears.
- Backup/export/restore implementation appears.
- Broker / network / credentials / endpoint appears.
- Real order placement or broker order id appears.
- Automatic trading appears.
