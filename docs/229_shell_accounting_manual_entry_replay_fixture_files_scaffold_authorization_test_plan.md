# ShellAccounting Manual Entry Replay Fixture Files Scaffold Authorization Test Plan

## Document Purpose

TASK-216 defines a gate-only / scaffold-authorization-only test plan for future
ShellAccounting manual entry replay fixture file scaffold work. The scope is
static documentation and static CTest checks only. This plan does not create
replay fixture files, does not create replay fixture JSON, does not create a
fixture index, does not create seed DB or runtime fixture data, does not
implement a parser or validator, does not implement replay, does not call
AccountingEngine replay, does not write SQLite, does not write audit / ledger
or snapshot rows, does not implement backup/export/restore, and does not add
broker, network, credentials, endpoint, real order placement, or automatic
trading behavior.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/228 exists | Verify `docs/228_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_gate.md` exists. |
| docs/229 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-216 and docs/228 / docs/229. |
| Gate-only scope | Verify TASK-216 authorizes replay fixture files scaffold policy only and does not authorize fixture file creation, JSON creation, parser, validator, or replay implementation. |
| Scaffold authorization purpose | Verify TASK-216 states the current task is gate-only / scaffold-authorization-only and no production behavior is modified. |
| Future scaffold directory boundary | Verify `tests/fixtures/manual_entry_replay/` is documented as a future candidate and is not created in TASK-216. |
| Future scaffold file set boundary | Verify `fixtures_index.json` and MRF001 through MRF006 candidate files are documented as future scaffold candidates only. |
| Future fixture index scaffold boundary | Verify the future index top-level fields and entry candidate fields are documented. |
| Future placeholder content boundary | Verify TODO, synthetic, sanitized, and unavailable placeholders are documented. |
| Future fixture JSON schema boundary | Verify future JSON top-level fields are documented and no parser / validator / replay runner is created. |
| Future static validation boundary | Verify future static validation scope is documented but not implemented. |
| Privacy and synthetic data boundary | Verify future scaffold content must be synthetic, sanitized, and free of raw SQL, raw payload, credentials, endpoint, broker payload, exception stack, real account data, and real order data. |
| No-runtime-dependency boundary | Verify future scaffold files must not change DataServiceActions, repositories, migrations, AccountingEngine replay, QML, Presenter, Controller, ShellServices, broker, network, credentials, endpoint, real order, or automatic trading behavior. |
| Out-of-scope boundaries | Verify no fixture creation, replay, parser, validator, production drift, runtime SQL, audit / ledger, snapshot, backup/export/restore, broker, or automatic trading is authorized. |
| Formal conclusion | Verify TASK-217 manual entry replay fixture files scaffold is the recommended next task. |
| No fixture file creation | Verify `tests/fixtures/manual_entry_replay/`, fixture JSON files, fixture index files, seed DB files, and runtime fixture data are not added. |
| No parser / validator creation | Verify no parser, validator, loader, fixture reader, or replay runner implementation is added. |
| Production drift | Verify production code, QML, Presenter, Controller, ShellServices, DataServiceActions, DataServiceActionRegistrar, repositories, migrations, AccountingEngine, StrategyEngine, MarketEngine, broker, network, credentials, endpoint, real order, and automatic trading are unchanged. |
| TASK-215 regression | Verify TASK-215 replay fixture files authorization files and CTest remain present. |
| TASK-214 regression | Verify TASK-214 replay fixture matrix authorization files and CTest remain present. |
| TASK-213 regression | Verify TASK-213 replay policy authorization files and CTest remain present. |
| TASK-212 regression | Verify TASK-212 replay audit ledger adequacy review files and CTest remain present. |
| TASK-211 regression | Verify SELL / Withdrawal daily-use runtime acceptance tests remain present. |
| TASK-209 regression | Verify BUY + Deposit daily-use runtime acceptance tests remain present. |
| TASK-207 regression | Verify readback mapping implementation tests remain present. |
| TASK-204 regression | Verify MVP runtime E2E acceptance tests remain present. |
| TASK-198 regression | Verify DataService write wiring implementation tests remain present. |
| TASK-196 regression | Verify manual cash movement repository dual-write implementation tests remain present. |
| TASK-192 regression | Verify manual transaction repository write implementation tests remain present. |
| Broker / real broker gates | Verify broker disabled, broker order, real broker authorization, and real broker implementation gates remain present. |
| transport_local_socket_echo 50-repeat regression | Verify `transport_local_socket_echo` remains registered for repeated transport regression. |

## Required Probes

- Static source scan.
- Documentation token scan.
- Changed-path allowlist scan.
- Untracked-file detection scan.
- No fixture directory creation scan.
- No replay fixture JSON scan.
- No fixture index file scan.
- No seed DB or runtime fixture data scan.
- No parser / validator implementation scan.
- No loader / fixture reader / replay runner implementation scan.
- No production code changed scan.
- No production QML / startup / Presenter / Controller drift scan.
- No ShellServices adapter / port drift scan.
- No DataServiceActions drift scan.
- No DataServiceActionRegistrar drift scan.
- No DataAccess repository drift scan.
- No migrations drift scan.
- No new migration or schema file scan.
- No runtime SQL / SQLite read/write scan.
- No AccountingEngine replay implementation scan.
- No AccountingEngine replay call scan.
- No audit / ledger write scan.
- No snapshot write scan.
- No backup/export/restore implementation scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- Retained TASK-215 / TASK-214 / TASK-213 / TASK-212 / TASK-211 / TASK-209 / TASK-207 / TASK-204 / TASK-198 / TASK-196 / TASK-192 regression probes.
- Retained broker and real broker gate probes.
- Retained transport 50-repeat regression probe.

## Go / No-Go Checklist

Go only if:

- docs/228 and docs/229 exist and are indexed by README, docs/README, and
  docs/12.
- TASK-216 CTest is registered.
- docs/228 covers scaffold purpose, directory, file set, fixture index,
  placeholder content, JSON schema, static validation, privacy, runtime
  boundary, out-of-scope boundaries, and formal conclusion.
- docs/228 explicitly states TASK-216 authorizes replay fixture files scaffold
  policy only.
- docs/228 explicitly states TASK-216 does not authorize creation of replay
  fixture files, replay fixture JSON, fixture index files, parser, validator,
  replay implementation, AccountingEngine replay calls, runtime SQL / SQLite
  read/write, audit / ledger writes, snapshot writes, backup/export/restore,
  broker, network, credentials, endpoint, real order placement, or automatic
  trading.
- No `tests/fixtures/manual_entry_replay/` directory is added.
- No replay fixture JSON files are added.
- No `fixtures_index.json` file is added.
- No seed DB or runtime fixture data is added.
- No parser, validator, loader, fixture reader, or replay runner is added.
- Production code, QML, Presenter, Controller, ShellServices,
  DataServiceActions, DataServiceActionRegistrar, repositories, migrations,
  AccountingEngine, StrategyEngine, MarketEngine, broker, network,
  credentials, endpoint, real order placement, and automatic trading are
  unchanged.
- TASK-215 / TASK-214 / TASK-213 / TASK-212 gates remain present.
- TASK-211 / TASK-209 / TASK-207 / TASK-204 / TASK-198 / TASK-196 / TASK-192
  regressions remain present.
- Broker / real broker gates remain present.
- `transport_local_socket_echo` 50-repeat regression passes.

No-Go if:

- Replay fixture files appear.
- Replay fixture JSON appears.
- Fixture index files appear.
- Seed DB / runtime fixture data appears.
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
- Fabricated PnL or fabricated market value appears.
