# ShellAccounting Manual Entry Replay Audit Ledger Adequacy Review Gate

## Document Purpose

TASK-212 is a replay / audit / ledger / backup-export adequacy review gate for
ShellAccounting manual entry facts. It is review-gate-only and docs/tests only.
This task does not implement new functionality, does not modify production
code, and does not modify DataServiceActions, DataServiceActionRegistrar,
repositories, migrations, QML, startup, Presenter, Controller, ShellServices
adapter / port code, AccountingEngine production code, StrategyEngine,
MarketEngine, broker SDKs, network, credentials, endpoints, real order
placement, real broker order ids, or automatic trading.

TASK-212 does not implement AccountingEngine replay, does not write
audit / ledger rows, does not implement backup/export/restore, does not add
runtime SQL / SQLite read/write, does not add read model refresh, and does not
connect broker / network / credentials / endpoint capability. First phase
remains no real broker, no automatic trading, and no real order placement.
Broker sandbox new capability remains paused.

TASK-212 does not modify production code.

## Static Boundary Tokens

This review gate fixes the following exact boundaries for static checks:

- TASK-212 does not modify DataServiceActions.
- TASK-212 does not modify DataServiceActionRegistrar.
- TASK-212 does not implement AccountingEngine replay.
- TASK-212 does not write audit / ledger rows.
- TASK-212 does not implement backup/export/restore.
- TASK-212 does not add runtime SQL / SQLite read/write.
- Replay is not ready for implementation until replay input/output policy is separately authorized.
- Audit / ledger are not ready for implementation until policy is separately authorized.
- Backup/export/restore requires separate acceptance.

## Current Manual Entry Coverage

The manual entry local assistant decision MVP now has the following covered
chain:

- TASK-192 implemented DataAccess-only manual transaction repository write for
  manual BUY / SELL facts into `trade_execution_group` and `trade_log`.
- TASK-196 implemented DataAccess-only manual cash movement repository
  dual-write for Deposit / Withdrawal facts into `trade_log` and
  `cash_adjustment`.
- TASK-198 implemented DataService runtime write wiring for
  `accounting.manual_transaction.create` and
  `accounting.manual_cash_movement.create` through validation and repository
  boundaries.
- TASK-200 implemented minimum QML / Presenter manual entry UI wiring through
  Presenter / Controller / ShellServices / DataService boundaries.
- TASK-202 implemented post-write DataService read-boundary refresh.
- TASK-204 implemented manual entry MVP runtime E2E acceptance with temporary
  SQLite fixtures.
- TASK-207 implemented DataService readback mapping for `position.list`,
  `cash.summary`, and safe partial `portfolio.pnl.summary`.
- TASK-209 implemented BUY + Deposit daily-use runtime acceptance.
- TASK-211 implemented SELL / Withdrawal daily-use runtime acceptance.

Current manual BUY / SELL / Deposit / Withdrawal local writes and daily-use
readback acceptance are covered at the local readback acceptance level. Replay,
audit / ledger production policy, and backup / export / restore acceptance are
not implemented by TASK-212.

## Replay Adequacy Review

Manual entry facts are partially adequate for a future AccountingEngine replay
policy, but not ready for replay implementation in this task.

- BUY / SELL quantity flow: persisted manual transaction facts can represent
  BUY quantity increases and SELL quantity reductions through `trade_log`.
  Future replay must define partial sell semantics, sell exceeds position
  policy, and duplicate handling before implementation.
- Deposit / Withdrawal cash flow: persisted manual cash movement facts can
  represent Deposit inflow and Withdrawal outflow through `trade_log` plus
  `cash_adjustment`. Future replay must define insufficient cash and negative
  cash policy before implementation.
- fee / tax treatment: manual transaction facts include fee and tax fields, but
  replay policy must define whether fees and taxes affect holding cost,
  realized PnL, cash impact, or all of those outputs.
- realized PnL readiness: realized PnL is not ready. SELL facts exist, but
  realized PnL requires cost basis policy, lot handling, fee / tax allocation,
  and correction / void policy.
- unrealized PnL readiness: unrealized PnL is not ready without market price
  dependency policy and stale price behavior.
- market price dependency: market price input is separate future work. Current
  readback acceptance uses safe unavailable output and must not fabricate market
  value or unrealized PnL.
- cost basis: current readback can expose holding cost for tested fixtures, but
  replay cost basis policy must be separately authorized.
- sell exceeds position: current daily-use acceptance requires no silent
  success. Replay policy must decide fail-closed, negative position visibility,
  or explicit unsupported issue behavior.
- negative cash / insufficient cash: current daily-use acceptance requires no
  silent success. Replay policy must decide fail-closed, negative cash
  visibility, or explicit unsupported issue behavior.
- multi-account / multi-portfolio / multi-instrument: schema and repositories
  carry account, portfolio, and instrument fields. Future replay fixtures must
  prove isolation and aggregation across accounts, portfolios, and instruments.
- idempotency and duplicate handling: requestId and idempotencyKey exist in the
  write chain. Replay input must avoid duplicate fact consumption and define
  duplicate correction behavior.
- replay input DTO: future replay input DTO mapping must consume persisted
  manual facts from repository/read boundaries, not UI raw payloads.
- replay output mapping: future replay output mapping must define how replay
  results flow to readback and snapshot outputs without changing TASK-207
  readback semantics until separately authorized.
- no replay side effects in current implementation: TASK-212 does not invoke
  AccountingEngine replay and does not add replay side effects.

## Audit Adequacy Review

Manual entry facts are not yet adequate for production audit implementation.
The chain has useful trace fields, but audit policy must be separately
authorized.

- write request trace: requestId and idempotencyKey are available as candidates
  for future audit event correlation.
- actor / user confirmation source: TASK-200 UI wiring creates a path for user
  initiated manual entry, but actor identity and user confirmation source policy
  are not yet formalized.
- requestId / idempotencyKey: future audit payloads must include sanitized
  requestId and idempotencyKey or explicit unavailable markers.
- sanitized sourceMemo: manual entry validation and repository writes already
  avoid sensitive memo exposure in tests. Future audit must store only
  sanitized memo, not raw payload.
- before / after facts: not ready. Future audit must define whether before and
  after facts are represented as counts, hashes, summary deltas, or sanitized
  field snapshots.
- validation issues: future audit should record sanitized validation issue
  codes, never raw SQL, raw payload, credentials, endpoints, or stack traces.
- repository result: future audit must map repository result status,
  idempotent result, and safe issue codes.
- post-write readback status: future audit must decide whether post-write
  readback status is recorded, and how readback failure affects the audit
  event.
- sensitive data redaction: audit payload must redact sourceMemo, raw SQL, raw
  manual entry payload, credentials, endpoint values, broker payloads, and
  internal exception stacks.
- no raw SQL / raw payload exposure: future audit must statically prove no raw
  SQL / raw payload exposure.
- audit failure behavior: not ready. Future audit policy must define whether a
  failed audit rolls back the source write, blocks success, or records a safe
  unavailable issue.
- audit storage boundary: not ready. Future audit storage must be authorized in
  a separate TASK and must not become a generic write escape hatch.

## Ledger Adequacy Review

Manual entry facts are not yet adequate for production ledger policy. The
current chain writes factual rows, but ledger semantics need a separate policy.

- manual transaction ledger facts: manual BUY / SELL facts exist in
  `trade_execution_group` and `trade_log`.
- cash movement ledger facts: Deposit / Withdrawal facts exist through
  `trade_log` and `cash_adjustment` linkage.
- trade_log source-of-truth relationship: `trade_log` is the current factual
  source for manual transaction and manual cash movement facts, but future
  ledger policy must decide how derived ledger rows, if any, relate to it.
- cash_adjustment linkage: `cash_adjustment` links cash movement detail back to
  `trade_log`; future ledger policy must define link immutability and safe
  export behavior.
- position / cash / pnl derived facts: position, cash, and PnL are derived
  readback facts today. Future ledger policy must not silently treat derived
  values as independent source facts.
- double-entry policy: not ready. Double-entry policy requires account mapping,
  debit/credit convention, cash vs security legs, and correction semantics.
- rollback / idempotency semantics: repository write tests cover transaction
  rollback and duplicate idempotency, but ledger policy must define immutable
  event replay, duplicate prevention, and safe retry behavior.
- ledger immutability policy: not ready. Future implementation must define
  append-only behavior, correction records, or explicit void records.
- correction / void policy: not ready. Future correction / void policy must be
  separately authorized before ledger implementation.
- reconciliation policy: not ready. Future reconciliation policy must define how
  manual facts reconcile with readback, replay, backup, and any later broker
  facts.

## Backup Export Restore Adequacy Review

Backup / export / restore acceptance is not ready for implementation in
TASK-212.

- migration 001 / 002 coverage: migrations 001 / 002 cover the current manual
  entry persistence schema used by tests, but backup/export/restore acceptance
  must separately verify schema version handling.
- manual entries export scope: future export must define whether manual
  transaction and manual cash movement rows are exported together, and how
  request trace is included.
- trade_log / trade_execution_group / cash_adjustment export scope: future
  export must include or explicitly exclude `trade_log`, `trade_execution_group`,
  and `cash_adjustment` with a rationale.
- idempotency and request trace export scope: future export must preserve
  requestId and idempotencyKey when needed for restore replay safety.
- sanitized memo export policy: future export must include only sanitized memo
  fields and must not include raw manual entry payloads.
- schema version metadata: future export must include schema version metadata
  for migrations 001 / 002 compatibility and later migrations.
- restore replay safety: future restore must prove replay safety, duplicate
  safety, and no fabricated replay output after restore.
- no credentials / endpoint / broker payload export: export must never include
  credentials, endpoint values, broker payloads, raw SQL, internal exception
  stacks, or real broker order ids.
- offline local-only packaging boundary: backup/export/restore must remain
  offline local-only unless a future task explicitly authorizes packaging,
  network, endpoint, or credential behavior.

## Gap Matrix

| Area | Status | Gap |
| --- | --- | --- |
| replay gap | Not ready | Replay input DTO, output mapping, realized PnL, market price dependency, cost basis, sell exceeds position, and negative cash policy need separate authorization. |
| audit gap | Not ready | Audit payload, actor / user confirmation, before / after facts, audit failure behavior, and audit storage boundary need separate authorization. |
| ledger gap | Not ready | Double-entry policy, immutability, correction / void, reconciliation, and ledger source-of-truth relationship need separate authorization. |
| backup/export gap | Not ready | Export scope, schema version metadata, sanitized memo export, and local packaging boundary need separate acceptance. |
| restore gap | Not ready | Restore replay safety, duplicate handling after restore, and no fabricated readback after restore need separate acceptance. |
| UI explainability gap | Partially ready | UI can show readback status, but future replay/audit/ledger states need explicit explainability policy. |
| fixture/test gap | Partially ready | BUY, SELL, Deposit, and Withdrawal daily-use acceptance exists; replay, audit, ledger, restore, multi-account, and market price fixtures remain future work. |
| policy decision gap | Not ready | Replay, audit, ledger, backup/export/restore priority and rollout order need owner decision. |

## Formal Conclusion And Next Task

Manual entry daily-use local MVP is ready at readback acceptance level. TASK-209
and TASK-211 cover BUY / SELL / Deposit / Withdrawal daily-use runtime
acceptance through existing DataService action boundaries and temporary SQLite
fixtures.

What is ready for future TASK:

- Manual entry persisted facts can be reviewed as candidate replay inputs.
- Readback mappings for `position.list`, `cash.summary`, and safe partial
  `portfolio.pnl.summary` can be used as acceptance baselines.
- Request trace and idempotency fields can be reviewed as future audit inputs.
- migrations 001 / 002 can be reviewed as candidate backup/export schema scope.

What is not ready:

- Replay is not ready for implementation until replay input/output policy is
  separately authorized.
- Audit / ledger are not ready for implementation until policy is separately
  authorized.
- Backup/export/restore requires separate acceptance.
- Broker, network, credentials, endpoint, real order placement, and automatic
  trading remain unauthorized.

Which area must be implemented first:

- Replay policy should be authorized before replay implementation, audit
  production write, ledger production policy, or backup/export/restore
  implementation because replay input/output rules define how manual facts
  become reproducible accounting state.

Recommended next task: TASK-213 manual entry replay policy authorization gate,
unless the project owner prefers audit or backup first with an explicit
rationale. Future audit, ledger, backup/export, packaging, broker, strategy,
and automatic trading work must remain separate TASKs unless explicitly chosen.
