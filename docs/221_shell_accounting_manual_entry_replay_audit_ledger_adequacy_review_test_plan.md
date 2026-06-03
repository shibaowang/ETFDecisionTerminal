# ShellAccounting Manual Entry Replay Audit Ledger Adequacy Review Test Plan

## Document Purpose

TASK-212 defines a review-gate-only test plan for ShellAccounting manual entry
replay / audit / ledger / backup-export adequacy. The tests are static gate,
documentation consistency, and boundary checks only. They do not write a
database, do not run AccountingEngine replay, do not write audit / ledger rows,
do not implement backup/export/restore, do not trigger network access, and do
not depend on real user data.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/220 exists | Verify the replay / audit / ledger adequacy review gate document exists. |
| docs/221 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-212 and docs/220 / docs/221. |
| Review-gate-only scope | Verify docs/220 says review-gate-only, no production code changes, no replay implementation, no audit / ledger implementation, and no backup/export/restore implementation. |
| Current coverage | Verify docs/220 lists TASK-192, TASK-196, TASK-198, TASK-200, TASK-202, TASK-204, TASK-207, TASK-209, and TASK-211. |
| Replay adequacy | Verify docs/220 covers BUY / SELL, Deposit / Withdrawal, fee / tax, realized PnL, unrealized PnL, market price dependency, cost basis, sell exceeds position, negative cash / insufficient cash, replay input DTO, replay output mapping, and no replay side effects. |
| Audit adequacy | Verify docs/220 covers requestId, idempotencyKey, actor / user confirmation, sanitized memo, validation issue, repository result, readback status, sensitive data redaction, audit failure behavior, and audit storage boundary. |
| Ledger adequacy | Verify docs/220 covers manual transaction ledger facts, cash movement ledger facts, trade_log source-of-truth relationship, cash_adjustment linkage, double-entry policy, idempotency, rollback, immutability, correction / void, and reconciliation. |
| Backup/export/restore adequacy | Verify docs/220 covers migration 001 / 002, manual entries export scope, trade_log / trade_execution_group / cash_adjustment export scope, request trace export scope, sanitized memo export policy, schema version metadata, restore replay safety, no credentials / endpoint / broker payload export, and local offline packaging. |
| Gap matrix | Verify docs/220 lists replay gap, audit gap, ledger gap, backup/export gap, restore gap, UI explainability gap, fixture/test gap, and policy decision gap. |
| Formal conclusion | Verify docs/220 says manual entry daily-use local MVP is ready at readback acceptance level, replay is not ready for implementation, audit / ledger are not ready for implementation, backup/export/restore requires separate acceptance, and TASK-213 is recommended. |
| Production drift | Verify TASK-212 does not modify production code, production QML, startup, Presenter / Controller, ShellServices adapter / port, DataServiceActions, repositories, migrations, AccountingEngine, StrategyEngine, MarketEngine, broker code, network, credentials, endpoints, real orders, or automatic trading. |
| Regression | Verify TASK-211, TASK-209, TASK-207, TASK-204, TASK-202, TASK-200, TASK-198, TASK-196, TASK-192, broker, and real broker gates remain present and passing. |

## Required Probes

- Static source scan.
- Documentation token scan.
- Changed-path allowlist scan.
- No production code changed scan.
- No production QML / startup / Presenter / Controller drift scan.
- No ShellServices adapter / port drift scan.
- No DataServiceActions drift scan.
- No DataAccess repository drift scan.
- No migrations drift scan.
- No new migration or schema file scan.
- No runtime SQL / SQLite read/write scan.
- No replay implementation scan.
- No read model refresh implementation scan.
- No audit / ledger write scan.
- No backup/export implementation scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- Retained TASK-211 / TASK-209 / TASK-207 / TASK-204 / TASK-202 / TASK-200 / TASK-198 / TASK-196 / TASK-192 regression probes.
- Retained broker and real broker gate probes.

## Go / No-Go Checklist

Go only if:

- docs/220 and docs/221 exist and are indexed.
- TASK-212 CTest is registered.
- docs/220 contains replay adequacy review.
- docs/220 contains audit adequacy review.
- docs/220 contains ledger adequacy review.
- docs/220 contains backup / export / restore adequacy review.
- docs/220 contains gap matrix.
- docs/220 contains formal conclusion.
- docs/220 recommends TASK-213 unless audit/backup is explicitly chosen with
  rationale.
- production code, QML, startup, Presenter, Controller, ShellServices,
  DataServiceActions, repositories, migrations, AccountingEngine, StrategyEngine,
  MarketEngine, broker, network, credentials, endpoint, real order, and
  automatic trading behavior are unchanged.
- TASK-211 SELL / Withdrawal runtime acceptance remains passing.
- TASK-209 BUY + Deposit runtime acceptance remains passing.
- TASK-207 readback mapping implementation remains passing.
- TASK-204 runtime E2E acceptance remains passing.
- TASK-202 post-write refresh implementation remains passing.
- TASK-200 QML Presenter implementation remains passing.
- TASK-198 DataService write wiring implementation remains passing.
- TASK-196 / TASK-192 repository tests remain passing.
- Broker and real broker gates remain passing.

No-Go if any production implementation change, database write, replay change,
audit / ledger write, backup/export/restore implementation, broker capability,
network call, credential handling, endpoint configuration, real order placement,
automatic trading behavior, schema drift, or migration drift appears.
