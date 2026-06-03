# ShellAccounting Manual Entry Replay Policy Authorization Test Plan

## Document Purpose

TASK-213 defines a policy-authorization-only test plan for future
ShellAccounting manual entry replay. The test scope is static documentation and
gate coverage only. It does not implement replay, does not call
AccountingEngine replay, does not write a database, does not write audit or
ledger rows, does not implement backup/export/restore, and does not add broker,
network, credential, endpoint, real order placement, or automatic trading
behavior.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/222 | Verify `docs/222_shell_accounting_manual_entry_replay_policy_authorization_gate.md` exists. |
| docs/223 | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-213 and docs/222 / docs/223. |
| Gate-only scope | Verify TASK-213 authorizes replay policy only and does not authorize replay implementation. |
| Replay input policy | Verify future replay input must come only from authorized persisted manual entry facts, not UI raw payloads or strategy/broker output. |
| Replay ordering policy | Verify deterministic occurred-at plus source-id ordering, duplicate handling, same-timestamp tie-breakers, and isolation policy are documented. |
| Position replay policy | Verify BUY, SELL, cost basis, sell-exceeds-position, negative position, fee/tax, and scope policies are documented. |
| Cash replay policy | Verify Deposit, Withdrawal, BUY/SELL cash impact, insufficient cash, negative cash, and multi-currency safe handling policies are documented. |
| PnL replay policy | Verify realized PnL, unrealized PnL, market price dependency, stale/missing price, and no fabricated PnL policies are documented. |
| Replay output policy | Verify structured position/cash/PnL outputs, safe unavailable markers, sanitized issue codes, and no write side effects are documented. |
| Unsupported/safety | Verify unsupported states fail closed or return sanitized issue codes with no silent success. |
| Privacy/sanitization | Verify no raw SQL, raw payloads, credentials, endpoints, broker payloads, real broker order ids, or exception stacks. |
| Integration boundary | Verify future replay must be separately authorized and stay inside DataService / AccountingEngine boundaries. |
| Out-of-scope | Verify no production code, QML, startup, Presenter/Controller, ShellServices, DataServiceActions, DataAccess, migrations, audit/ledger, backup/export/restore, broker, strategy, real order, or automatic trading drift. |
| Formal conclusion | Verify TASK-214 manual entry replay fixture matrix authorization gate is the recommended next task. |
| Production drift | Verify changed paths are limited to the TASK-213 docs, static gate, test registration, and documentation indexes. |
| TASK-212 regression | Verify TASK-212 replay audit ledger adequacy review files remain present. |
| TASK-211 regression | Verify SELL / Withdrawal daily-use runtime acceptance tests remain present. |
| TASK-209 regression | Verify BUY + Deposit daily-use runtime acceptance tests remain present. |
| TASK-207 regression | Verify readback mapping implementation tests remain present. |
| TASK-204 regression | Verify MVP runtime E2E acceptance tests remain present. |
| TASK-198 regression | Verify DataService write wiring implementation tests remain present. |
| TASK-196 regression | Verify manual cash movement repository dual-write implementation tests remain present. |
| TASK-192 regression | Verify manual transaction repository write implementation tests remain present. |
| Broker/real broker gates | Verify broker disabled, broker order, real broker authorization, and real broker implementation gate dirs remain present. |
| transport 50-repeat | Verify `transport_local_socket_echo` remains registered for repeated transport regression. |

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
- No replay fixture implementation scan.
- No readback semantic change scan.
- No audit / ledger write scan.
- No backup/export/restore implementation scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- Retained TASK-212 / TASK-211 / TASK-209 / TASK-207 / TASK-204 / TASK-198 / TASK-196 / TASK-192 regression probes.
- Retained broker and real broker gate probes.
- Retained transport 50-repeat regression probe.

## Go / No-Go Checklist

Go only if:

- docs/222 and docs/223 exist and are indexed.
- TASK-213 CTest is registered.
- docs/222 contains Replay Input Policy.
- docs/222 contains Replay Ordering Policy.
- docs/222 contains Position Replay Policy.
- docs/222 contains Cash Replay Policy.
- docs/222 contains PnL Replay Policy.
- docs/222 contains Replay Output Policy.
- docs/222 contains Unsupported And Safety Policy.
- docs/222 contains Privacy And Sanitization Policy.
- docs/222 contains Integration Boundary.
- docs/222 contains Out-of-Scope Boundaries.
- docs/222 contains Formal Authorization Conclusion And Next Task.
- TASK-213 authorizes replay policy only.
- TASK-213 does not authorize replay implementation.
- TASK-213 does not authorize audit / ledger writes.
- TASK-213 does not authorize backup/export/restore.
- TASK-213 does not authorize broker, network, credentials, endpoint, real
  order placement, or automatic trading.
- Production code, QML, startup, Presenter, Controller, ShellServices,
  DataServiceActions, DataServiceActionRegistrar, DataAccess repositories,
  migrations, AccountingEngine, StrategyEngine, MarketEngine, broker, network,
  credentials, endpoint, real order placement, and automatic trading behavior
  are unchanged.
- TASK-212 / TASK-211 / TASK-209 / TASK-207 / TASK-204 / TASK-198 / TASK-196 /
  TASK-192 regression tests remain present.
- Broker and real broker gates remain present.
- `transport_local_socket_echo` remains registered.

No-Go if any production implementation change, database write, replay
implementation, audit / ledger write, backup/export/restore implementation,
broker capability, network call, credential handling, endpoint configuration,
real order placement, automatic trading behavior, schema drift, migration
drift, or old-gate production-boundary drift appears.
