# ShellAccounting Manual Entry Replay Fixture Files Authorization Test Plan

## Document Purpose

TASK-215 defines a gate-only / fixture-files-authorization-only test plan for
future ShellAccounting manual entry replay fixture files. The scope is static
documentation and static gate checks only. This plan does not create replay
fixture files, does not create replay fixture JSON, does not create seed DB or
runtime fixture data, does not implement replay, does not call AccountingEngine
replay, does not write SQLite, does not write audit or ledger rows, does not
write snapshots, does not implement backup/export/restore, and does not add
broker, network, credential, endpoint, real order placement, or automatic
trading behavior.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/226 exists | Verify `docs/226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md` exists. |
| docs/227 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-215 and docs/226 / docs/227. |
| Gate-only scope | Verify TASK-215 authorizes fixture files contract only and does not authorize fixture file creation or replay implementation. |
| Fixture files authorization purpose | Verify the contract inherits TASK-214 and defines future fixture files as test-only acceptance material. |
| Future fixture directory policy | Verify `tests/fixtures/manual_entry_replay/` is documented as a future candidate and is not created in TASK-215. |
| Future fixture file naming policy | Verify `MRF001_empty_manual_facts.json` through `MRF006_daily_use_combined_baseline.json` are documented as future candidates only. |
| Future fixture index policy | Verify `fixtures_index.json` is documented as a future candidate only and must validate ids and privacy status. |
| Future fixture JSON schema policy | Verify future schema fields, sourceFacts fields, expectedOutputs fields, and expectedIssues fields are documented. |
| Future fixture content policy | Verify future content uses persisted manual facts candidates only and excludes QML raw payload, DataService raw request body, broker payload, raw SQL, credentials, endpoint, and backup/export payload. |
| Future fixture case coverage policy | Verify baseline, ordering, idempotency, position, cash, PnL, multi-scope, unsupported, and failure coverage inherited from TASK-214. |
| Privacy and redaction policy | Verify raw SQL, raw payload, credentials, endpoint, broker payload, exception stack, full trade_log payload, full cash_adjustment payload, real account data, and real order data are forbidden. |
| Validation tool boundary | Verify TASK-215 does not create a parser, validator, JSON fixture reader, or replay runner. |
| Repository and runtime boundary | Verify future fixture files must not be read by production code or change DataServiceActions, repositories, migrations, AccountingEngine replay, audit / ledger, snapshot, backup/export/restore, broker, network, credentials, endpoint, real order, or automatic trading behavior. |
| Out-of-scope boundaries | Verify no fixture files, replay, parser, validator, production drift, runtime SQL, audit / ledger, snapshot, broker, or automatic trading is authorized. |
| Formal conclusion | Verify TASK-216 manual entry replay fixture files scaffold authorization gate is the recommended next task. |
| No fixture file creation | Verify `tests/fixtures/manual_entry_replay/`, fixture JSON files, seed DB files, and runtime fixture data are not added. |
| Production drift | Verify production code, QML, Presenter, Controller, ShellServices, DataServiceActions, repositories, migrations, AccountingEngine, StrategyEngine, MarketEngine, broker, network, credentials, endpoint, real order, and automatic trading are unchanged. |
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
- No seed DB or runtime fixture data scan.
- No parser / validator implementation scan.
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
- Retained TASK-214 / TASK-213 / TASK-212 / TASK-211 / TASK-209 / TASK-207 / TASK-204 / TASK-198 / TASK-196 / TASK-192 regression probes.
- Retained broker and real broker gate probes.
- Retained transport 50-repeat regression probe.

## Go / No-Go Checklist

Go only if:

- docs/226 and docs/227 exist and are indexed by README, docs/README, and
  docs/12.
- TASK-215 CTest is registered.
- docs/226 covers fixture directory, naming, index, schema, content, privacy,
  validation, and runtime boundary.
- docs/226 explicitly states TASK-215 authorizes fixture files contract only.
- docs/226 explicitly states TASK-215 does not authorize creation of replay
  fixture files.
- No `tests/fixtures/manual_entry_replay/` directory is added.
- No replay fixture JSON files are added.
- No seed DB or runtime fixture data is added.
- Production code, QML, Presenter, Controller, ShellServices,
  DataServiceActions, repositories, migrations, AccountingEngine,
  StrategyEngine, MarketEngine, broker, network, credentials, endpoint, real
  order placement, and automatic trading are unchanged.
- TASK-214 / TASK-213 / TASK-212 gates remain present.
- TASK-211 / TASK-209 / TASK-207 / TASK-204 / TASK-198 / TASK-196 / TASK-192
  regressions remain present.
- Broker / real broker gates remain present.
- `transport_local_socket_echo` 50-repeat regression passes.

No-Go if:

- Replay implementation appears.
- AccountingEngine replay call appears.
- Replay fixture JSON / seed DB / runtime fixture files appear.
- Fixture parser / validator implementation appears.
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
