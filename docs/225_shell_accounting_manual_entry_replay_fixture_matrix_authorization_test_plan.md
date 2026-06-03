# ShellAccounting Manual Entry Replay Fixture Matrix Authorization Test Plan

## Document Purpose

TASK-214 defines a gate-only / fixture-matrix-authorization-only test plan for
future ShellAccounting manual entry replay fixtures. The scope is static docs
and static gate checks only. This plan does not implement replay, does not
create replay fixture files, does not call AccountingEngine replay, does not
write SQLite, does not write audit or ledger rows, does not implement
backup/export/restore, and does not add broker, network, credential, endpoint,
real order placement, or automatic trading behavior.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/224 exists | Verify `docs/224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md` exists. |
| docs/225 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-214 and docs/224 / docs/225. |
| Gate-only scope | Verify TASK-214 authorizes fixture matrix design only and does not authorize replay implementation or fixture files. |
| Fixture matrix purpose | Verify the matrix is a future replay input/output acceptance contract inherited from TASK-213. |
| Fixture naming policy | Verify `MRF001` to `MRF0xx` stable fixture ids and required fixture fields are documented. |
| Fixture source fact policy | Verify source facts are persisted manual facts only and exclude QML raw payload, broker payload, raw SQL, credentials, endpoint, and backup/export payload. |
| Baseline success fixture matrix | Verify MRF001 through MRF006 design cases cover empty facts, BUY, SELL, Deposit, Withdrawal, and combined daily-use baseline. |
| Ordering and idempotency fixture matrix | Verify same timestamp, execution group order, duplicate requestId/idempotencyKey, out-of-order rows, created_at vs occurred_at, stable tie-breaker, and SQLite unsorted order policies. |
| Position safety fixture matrix | Verify SELL exceeds position, negative position, zero/invalid quantity, missing instrument, account/portfolio isolation, and holding cost policy coverage. |
| Cash safety fixture matrix | Verify Withdrawal exceeds cash, negative cash, insufficient BUY cash, fee/tax cash impact, missing currency, multi-currency, and Deposit/Withdrawal linkage coverage. |
| PnL and market dependency fixture matrix | Verify realized/unrealized PnL, market value, stale price, no fabricated PnL, no fabricated market value, and portfolio PnL safe partial policy coverage. |
| Multi-scope fixture matrix | Verify multi-account, multi-portfolio, multi-instrument, aggregate scope, contamination, and same-symbol identity policies. |
| Unsupported and failure fixture matrix | Verify malformed facts, unsupported fact type, missing scope, duplicate ambiguity, correction/void, broker facts, and backup/export/restored facts are covered. |
| Expected output policy | Verify position, holding cost, cash, PnL, unavailable fields, issues, blocking flags, metadata, trace ids, and privacy-safe text are documented. |
| Privacy and sanitization policy | Verify no raw SQL, raw payload, credentials, endpoint, broker payload, exception stack, full trade_log payload, or full cash_adjustment payload. |
| Test implementation boundary | Verify TASK-214 tests are static only and do not execute replay, write SQLite, call AccountingEngine replay, or create fixture JSON / seed DB. |
| Formal conclusion | Verify TASK-215 manual entry replay fixture files authorization gate is the recommended next task. |
| Production drift | Verify production code, QML, Presenter, Controller, ShellServices, DataServiceActions, repositories, migrations, AccountingEngine, StrategyEngine, MarketEngine, broker, network, credentials, endpoint, real order, and automatic trading are unchanged. |
| TASK-213 regression | Verify TASK-213 replay policy authorization files and CTest remain present. |
| TASK-212 regression | Verify TASK-212 replay audit ledger adequacy review files remain present. |
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
- No replay fixture file scan.
- No seed DB or runtime fixture data scan.
- No readback semantic change scan.
- No audit / ledger write scan.
- No snapshot write scan.
- No backup/export/restore implementation scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- Retained TASK-213 / TASK-212 / TASK-211 / TASK-209 / TASK-207 / TASK-204 / TASK-198 / TASK-196 / TASK-192 regression probes.
- Retained broker and real broker gate probes.
- Retained transport 50-repeat regression probe.

## Go / No-Go Checklist

Go only if:

- docs/224 and docs/225 exist and are indexed by README, docs/README, and
  docs/12.
- TASK-214 CTest is registered.
- docs/224 covers fixture purpose, naming, source fact, baseline, ordering,
  position, cash, PnL, multi-scope, failure, expected output, privacy, and test
  boundary.
- docs/224 explicitly states TASK-214 authorizes fixture matrix design only.
- docs/224 explicitly states TASK-214 does not authorize replay implementation.
- docs/224 explicitly states TASK-214 does not authorize creation of replay
  fixture files.
- Production code, QML, Presenter, Controller, ShellServices,
  DataServiceActions, repositories, migrations, AccountingEngine,
  StrategyEngine, MarketEngine, broker, network, credentials, endpoint, real
  order placement, and automatic trading are unchanged.
- TASK-213 / TASK-212 gates remain present.
- TASK-211 / TASK-209 / TASK-207 / TASK-204 / TASK-198 / TASK-196 / TASK-192
  regressions remain present.
- Broker / real broker gates remain present.
- `transport_local_socket_echo` 50-repeat regression passes.

No-Go if:

- Replay implementation appears.
- AccountingEngine replay call appears.
- Replay fixture JSON / seed DB / runtime fixture files appear.
- Runtime SQL / SQLite read/write appears.
- DataServiceActions runtime behavior changes.
- Repository / migration changes appear.
- QML / Presenter / Controller / ShellServices production drift appears.
- Audit / ledger write appears.
- Backup/export/restore implementation appears.
- Broker / network / credentials / endpoint appears.
- Real order placement or broker order id appears.
- Automatic trading appears.
- Fabricated PnL or fabricated market value appears.
