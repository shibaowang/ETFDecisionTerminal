# TASK-270 ShellAccounting Excel/VBA Import Persist Post-Write Readback Refresh

TASK-270 connects the already authorized accepted Excel/VBA import persistence
success path to the existing ShellAccounting post-write readback refresh path.
It does not add a new write action, does not change the DataService write
action implementation, and does not change DataAccess repositories.

## Scope

- After a successful accepted preview persistence result, the Presenter invokes
  the existing read-only post-write refresh path.
- The refresh reads existing ShellAccounting readback actions and maps their
  sanitized state back to Presenter and QML-visible status.
- QML displays persisted `trade_log` row count, persisted `cash_adjustment`
  row count, post-persist refresh status, refresh summary, and sanitized issue.
- Duplicate idempotent imports do not claim new rows and do not trigger a
  misleading refresh.
- Idempotency conflicts fail closed and do not trigger readback refresh.
- Refresh failure does not roll back already successful persistence; it is
  surfaced as a sanitized post-persist refresh issue.

## Boundaries

TASK-270 may only use the existing read-only refresh path. It must not modify
the DataService write action, DataAccess repositories, migrations,
DataServiceClient, or AccountingEngine production code.

QML must not directly access DataServiceClient, DataServiceApi, SQLite, or
AccountingEngine. The UI must not add broker, network, credentials, endpoint,
TradeDraft, strategy, real order placement, or automatic trading behavior.

The refresh path remains read-only: no SQLite write, audit write, ledger write,
snapshot write, trade_log write, or persistent read model write is performed by
the refresh itself.

## Evidence

The TASK-270 CTest verifies:

- persist success triggers readback refresh.
- post-persist refresh succeeds through existing read-only calls.
- persisted trade and cash row counts are visible.
- duplicate import does not claim new rows.
- idempotency conflict does not refresh.
- refresh failure is sanitized and does not roll back persistence.
- QML has no direct SQLite / DataServiceClient / AccountingEngine access.
