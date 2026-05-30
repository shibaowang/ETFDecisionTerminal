# ShellAccounting DataService Read-only Facts Query Implementation

## Document Purpose

TASK-137 implements the ShellAccounting DataService read-only facts query
boundary that was gated by TASK-136. The implementation is limited to
DataService-internal SELECT-only facts / summary queries. It does not authorize
database writes, snapshot rebuild, AccountingEngine replay, TradeDraft, trade
suggestion, strategy execution, broker order, production QML changes, or schema
changes.

TASK-137 records read-only actions, no write SQL, and no AccountingEngine replay
as hard implementation boundaries.

## TASK-137 Scope

TASK-137 implements the authorized read-only actions:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`
- `base_position.summary`
- `sniper_pool.summary`

The actions continue to run through the existing DataService action / protocol
boundary. Shell / Presenter / QML still do not access SQLite, DataAccess, or
AccountingEngine directly.

## DataService Query Boundary

The query implementation lives in DataService/DataAccess code and uses the
`ShellAccountingReadOnlyFactsQuery` DataAccess boundary. DataService action
handlers call this boundary and map results to structured ShellAccounting
payloads with `implemented=true`, `readOnly=true`, `writeEnabled=false`,
`replayExecuted=false`, `snapshotRebuilt=false`, and
`accountingEngineCalled=false`.

## DataAccess / SQLite Read-only Boundary

The DataAccess query wrapper exposes SELECT-only query strings for existing
facts or derived read-only caches:

- `position_snapshot`
- `cash_snapshot`
- `portfolio_summary`
- `base_position_allocation`
- `trade_tier_allocation`

The implementation does not rebuild snapshots and does not recalculate or write
derived summary rows. Reading `position_snapshot`, `cash_snapshot`, or
`portfolio_summary` is read-only consumption of existing data.

## No Write SQL

The facts query implementation does not contain write SQL. It must not use
`INSERT`, `UPDATE`, `DELETE`, `REPLACE`, `DROP`, `ALTER`, `CREATE TABLE`, write
transactions, write repositories, or `data.audit.append`. It does not write
`trade_log`, `trade_execution_group`, `trade_draft`, `cash_snapshot`,
`position_snapshot`, `portfolio_summary`, or `audit_log`.

## No Snapshot Rebuild

TASK-137 does not authorize snapshot rebuild. Missing, empty, stale, or
inconsistent data must map to structured empty / unavailable / issue state. The
implementation must not silently rebuild snapshots or summaries.

## No AccountingEngine Replay

TASK-137 does not authorize `AccountingReplayEngine`, `replayFromFacts`, replay
rebuild, correction, reversal, or accounting recalculation. If replay is needed,
it must be gated and implemented in a separate task.

## No Trade Capability

The implementation does not generate TradeDraft, does not generate trade
suggestions, does not execute strategy, does not submit broker orders, and does
not add trading UI. The read-only actions are not trading actions.

## Error Mapping

The DataService action layer maps:

- missing data to structured empty / issue state.
- empty ledger or empty derived cache to `EMPTY` with a non-blocking issue.
- query error to `QUERY_ERROR` with a blocking issue.
- protocol error to existing protocol error response handling.
- timeout / unavailable transport to the existing DataServiceClient adapter
  unavailable / issue mapping.
- inconsistent facts to issue state in a SAFE_READONLY-compatible response.

There is no silent success and no fake data fallback.

## Privacy Policy

Payloads must not expose raw SQL, full `trade_log` payloads, sensitive raw facts,
or internal exception stacks to QML. Query error messages are generic and include
privacy flags such as `rawSqlExposed=false` and
`rawTradeLogPayloadExposed=false`.

## Production Wiring

The existing ShellAccounting read-only adapter continues to call DataService via
the authorized DataServiceClient / protocol boundary. Production QML and
production startup are unchanged by TASK-137.

## Rollback / Disable Strategy

Rollback is limited to disabling or reverting the DataService read-only facts
query calls and returning the presenter to unavailable / empty state. Fallback
must keep presenter unavailable when query data cannot be trusted. Fallback must
not fake data, must not write DB, must not generate TradeDraft, must not bypass
DataService with shell-side SQLite reads, and must not call AccountingEngine
replay.

Operationally, rollback can disable facts query and keep presenter unavailable
without changing production QML. The fallback must not fake data, must not write
DB, must not generate TradeDraft, and must not call AccountingEngine replay.

## Tests

TASK-137 adds `shell_accounting_dataservice_facts_query_implementation` tests
covering the implementation marker, all five success mappings, empty data,
missing data, query error mapping, no write SQL, no snapshot rebuild, no
AccountingEngine replay, privacy, and rollback readiness. TASK-136 gate tests
are evolved so the old no-query check now allows only the authorized read-only
query implementation.

## Not Authorized

The next phases still require separate authorization for AccountingEngine
replay, snapshot rebuild, richer stale-data detection, TradeDraft, trade
suggestions, strategy execution, broker order, and any write-enabled action.

## TASK-138 Replay Snapshot Rebuild Gate

TASK-138 adds the replay snapshot rebuild gate after this implementation. The
TASK-137 facts query remains a read-only fallback. AccountingEngine replay,
snapshot rebuild, snapshot write, TradeDraft generation, trade suggestions,
strategy execution, broker order, and trading UI are still not implemented or
authorized.

## TASK-139 Read-only Replay Implementation

TASK-139 adds an explicit DataService-internal read-only replay calculation
path. This document remains the fallback record for TASK-137 SELECT-only facts
query. Snapshot rebuild/write remains unimplemented, and Shell/QML/Presenter
still do not directly access AccountingEngine, SQLite, or DataAccess.
