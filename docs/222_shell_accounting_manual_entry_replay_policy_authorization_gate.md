# ShellAccounting Manual Entry Replay Policy Authorization Gate

## Document Purpose

TASK-213 authorizes replay policy only.
TASK-213 does not authorize replay implementation.
TASK-213 does not authorize audit / ledger writes.
TASK-213 does not authorize backup/export/restore.
TASK-213 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

This gate defines the policy that future manual entry replay work must satisfy
before any implementation task may call AccountingEngine replay, build replay
fixtures, write derived outputs, or connect replay results to readback,
snapshot, audit, ledger, backup, export, restore, broker, or strategy flows.

TASK-213 is policy-authorization-only and docs/tests only. It does not modify
production code, production QML, startup, Presenter, Controller, ShellServices
adapter / port code, DataServiceActions, DataServiceActionRegistrar, DataAccess
repositories, migrations, AccountingEngine, StrategyEngine, MarketEngine,
broker SDKs, network, credentials, endpoints, real order placement, real broker
order ids, or automatic trading.

TASK-213 does not modify production code.

## Static Boundary Tokens

- TASK-213 does not modify production QML.
- TASK-213 does not modify startup.
- TASK-213 does not modify Presenter.
- TASK-213 does not modify Controller.
- TASK-213 does not modify ShellServices adapter / port code.
- TASK-213 does not modify DataServiceActions.
- TASK-213 does not modify DataServiceActionRegistrar.
- TASK-213 does not modify DataAccess repositories.
- TASK-213 does not modify migrations.
- TASK-213 does not implement AccountingEngine replay.
- TASK-213 does not write audit / ledger rows.
- TASK-213 does not implement backup/export/restore.
- TASK-213 does not add runtime SQL / SQLite read/write.

## Replay Input Policy

Future replay implementation must consume only authorized, persisted manual
entry facts:

- Manual BUY / SELL facts written through the authorized manual transaction
  repository and DataService action path.
- Manual Deposit / Withdrawal facts written through the authorized manual cash
  movement repository and DataService action path.
- Request trace fields, including requestId and idempotencyKey, as replay
  duplicate-control inputs.
- Sanitized memo fields only when explicitly needed for issue reporting.

Future replay must not consume QML raw payloads, UI form payloads, unvalidated
DataService request bodies, strategy output, broker payloads, raw SQL, backup
payloads, export payloads, credentials, endpoints, or synthetic facts that were
not produced by the authorized manual entry write chain.

Replay input DTOs must keep account id, portfolio id, instrument id / symbol,
side, quantity, price, fee, tax, currency, movement type, occurred-at time,
requestId, idempotencyKey, and source row ids as structured fields. Stringly
typed or generic raw JSON escape hatches are not authorized by TASK-213.

## Replay Ordering Policy

Future replay must define deterministic ordering before implementation:

- Facts must be ordered by occurred-at time and stable source row id.
- Duplicate idempotencyKey or duplicate requestId handling must be deterministic.
- same-timestamp facts must have a stable tie-breaker.
- multi-account, multi-portfolio, and multi-instrument facts must be isolated
  before aggregation.
- Correction, void, stale fact, and out-of-order fact behavior must be explicit
  or safely unsupported.

Replay must not silently reorder facts, skip facts, merge unrelated facts, or
fabricate missing ordering fields.

## Position Replay Policy

Future position replay must define how manual BUY and SELL facts affect
positions before implementation:

- BUY may increase quantity and holding cost only according to an authorized
  cost basis policy.
- SELL may reduce quantity only according to an authorized lot / cost basis
  policy.
- SELL exceeds position must fail closed or return a safe unsupported issue.
- Negative positions are not authorized unless a future task explicitly
  authorizes them.
- Fee and tax treatment must define whether fees and taxes affect holding cost,
  realized PnL, cash impact, or all of those outputs.
- Instrument identity must remain stable across account and portfolio scopes.

TASK-213 does not authorize position replay implementation and does not change
TASK-207 readback mapping semantics.

## Cash Replay Policy

Future cash replay must define how manual transactions and manual cash
movements affect cash before implementation:

- Deposit increases cash according to currency and principal base policy.
- Withdrawal decreases cash according to currency and principal base policy.
- BUY cash outflow and SELL cash inflow must be explicitly defined, including
  fee / tax cash impact.
- Insufficient cash and negative cash must fail closed or return a safe
  unsupported issue unless a future task explicitly authorizes negative cash.
- Multi-currency behavior must remain unsupported or safely unavailable until
  explicitly authorized.

TASK-213 does not authorize cash replay implementation and does not add runtime
SQL / SQLite read/write behavior.

## PnL Replay Policy

Future PnL replay must define realized and unrealized PnL before
implementation:

- Realized PnL requires cost basis, lot handling, fee / tax allocation, partial
  SELL behavior, and correction / void policy.
- Unrealized PnL requires market price dependency policy, stale price behavior,
  missing price behavior, and currency conversion policy.
- PnL must not be fabricated when market prices, cost basis, or replay inputs
  are unavailable.
- Safe unavailable output is required for partial or unsupported PnL states.

TASK-213 does not authorize PnL replay implementation, market price integration,
derived table refresh, or AccountingEngine replay calls.

## Replay Output Policy

Future replay output must be structured, deterministic, and safe:

- Position outputs must include account / portfolio / instrument scope,
  quantity, holding cost, and safe unavailable markers when needed.
- Cash outputs must include account / portfolio / currency scope, cash balance,
  principal base policy status, and safe unavailable markers when needed.
- PnL outputs must include realized PnL, unrealized PnL, total PnL, market price
  status, and safe unavailable markers when needed.
- Output issue codes must be sanitized and must not expose raw SQL, raw manual
  entry payloads, full trade_log payloads, credentials, endpoints, broker
  payloads, or internal exception stacks.

Replay output must not write audit_log, ledger rows, snapshot tables,
trade_log, trade_execution_group, cash_adjustment, trade_draft, backup files, or
export files in TASK-213.

## Unsupported And Safety Policy

Future replay implementation must fail safely for unsupported states:

- Missing source facts.
- Duplicate source facts.
- stale or out-of-order facts.
- SELL exceeds position.
- Withdrawal exceeds cash or negative cash.
- Missing market price.
- Missing or unsupported currency.
- Missing account / portfolio / instrument mapping.
- Invalid quantity, price, fee, tax, side, or movement type.
- Unsupported correction / void behavior.

No silent success is allowed. Unsupported states must return explicit sanitized
issue codes, safe unavailable markers, or fail-closed results.

## Privacy And Sanitization Policy

Replay policy must preserve privacy and response sanitization:

- No raw SQL.
- No raw manual entry payload.
- No full trade_log payload.
- No full cash_adjustment payload.
- No full replay input payload in error text.
- No credentials.
- No endpoint values.
- No broker payloads.
- No internal exception stacks.
- No real broker order ids.

Future replay diagnostics must use sanitized issue codes and limited source
identifiers only.

## Integration Boundary

Future replay implementation must be separately authorized and must remain
inside the DataService / AccountingEngine boundary chosen by that future task.
The implementation must not be triggered directly by QML, Presenter,
Controller, ShellServices adapter / port code, or arbitrary DataServiceClient
payloads.

Future integration must preserve existing readback behavior until a future task
explicitly authorizes readback mapping changes. It must not modify TradeDraft,
broker order, strategy execution, backup/export/restore, audit write, ledger
write, production QML, startup, schema, or migrations as part of TASK-213.

## Out-of-Scope Boundaries

TASK-213 does not authorize:

- AccountingEngine replay implementation.
- Replay fixture matrix implementation.
- Runtime SQL / SQLite read/write.
- Derived table refresh.
- Snapshot rebuild or snapshot write changes.
- audit_log writes.
- Ledger writes.
- Backup/export/restore.
- Production QML changes.
- Startup changes.
- Presenter / Controller / ShellServices production changes.
- DataServiceActions or DataServiceActionRegistrar changes.
- DataAccess repository changes.
- migrations/001_initial_schema.sql changes.
- migrations/002_shell_accounting_manual_entry_schema.sql changes.
- TradeDraft, suggestion, strategy, broker, network, credentials, endpoint, real
  order placement, real broker order ids, or automatic trading changes.

## Formal Authorization Conclusion And Next Task

Manual entry replay is policy-authorized only. Manual BUY / SELL and Deposit /
Withdrawal facts may be treated as future replay candidate inputs, but
implementation remains unauthorized until the next explicit task.

TASK-213 authorizes replay policy only.
TASK-213 does not authorize replay implementation.
TASK-213 does not authorize audit / ledger writes.
TASK-213 does not authorize backup/export/restore.
TASK-213 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-214 manual entry replay fixture matrix authorization gate.
