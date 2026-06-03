# ShellAccounting Manual Entry MVP Runtime E2E Acceptance

## Document Purpose

TASK-204 implements runtime E2E acceptance tests for the ShellAccounting manual
entry MVP chain authorized by TASK-203. This task adds tests and documentation
only; it does not add a production feature and does not modify production code.
TASK-204 scope is tests and documentation only.

The acceptance scope is the existing local assistant decision MVP chain:

`QML -> Presenter -> Controller -> ShellServices adapter -> DataService action -> DataAccess repository`

TASK-204 verifies the implemented chain with temporary SQLite databases and the
existing DataService action boundary. It does not modify production QML,
startup, Presenter, Controller, ShellServices adapter / port code,
DataServiceActions, DataServiceActionRegistrar, DataAccess repositories,
validation production code, migrations, schema files, AccountingEngine replay,
StrategyEngine, MarketEngine, broker code, network, credentials, endpoints, real
order placement, or automatic trading.

TASK-205 adds a later readback / replay adequacy review gate for these manual
entry facts. TASK-205 remains review-only and does not implement readback,
replay, daily-use page mapping, audit, ledger, broker, or automatic trading
behavior.

## Runtime E2E Fixture Boundary

The TASK-204 runtime tests use test-only temporary SQLite databases. Each test
database applies:

- `migrations/001_initial_schema.sql`
- `migrations/002_shell_accounting_manual_entry_schema.sql`

The tests seed only synthetic account, portfolio, and instrument fixture rows.
They do not use real user data, do not perform network calls, and do not require
a live broker, external endpoint, or credentials.

## Manual Transaction Acceptance

The manual transaction runtime acceptance verifies that a valid manual BUY
payload submitted through the DataService action boundary writes:

- `trade_execution_group`
- `trade_log`

The same path verifies that manual transaction writes do not write
`cash_adjustment`, `audit_log`, ledger rows, broker state, or any real order id.
Duplicate `idempotencyKey` submission must return an idempotent result and must
not duplicate `trade_log` or `trade_execution_group`. Invalid payloads must be
rejected before repository write and must leave all manual entry write tables
unchanged.
Invalid payloads must be rejected without a write.

Full SELL fixture expansion may be handled by a later TASK if broader scenario
coverage is required. The TASK-204 minimum runtime E2E chain covers BUY.

## Manual Cash Movement Acceptance

The manual cash movement runtime acceptance verifies that a valid Deposit
payload submitted through the DataService action boundary dual-writes:

- `trade_log`
- `cash_adjustment`

The tests verify that `cash_adjustment.trade_log_id` references `trade_log.id`.
Duplicate `idempotencyKey` submission must return an idempotent result and must
not duplicate `trade_log` or `cash_adjustment`. Invalid payloads must be
rejected before repository write and must leave all manual entry write tables
unchanged.

Full Withdrawal fixture expansion may be handled by a later TASK if broader
scenario coverage is required. The TASK-204 minimum runtime E2E chain covers
Deposit.

## Response And Privacy Acceptance

Runtime acceptance verifies safe DataService responses:

- `databaseWritten`, `tradeLogWritten`, and `cashAdjustmentWritten` reflect the
  expected write result.
- manual transaction responses report `cashAdjustmentWritten=false`.
- manual cash movement responses report `cashAdjustmentWritten=true`.
- duplicate submissions expose safe idempotent / duplicate status.
- invalid submissions expose safe validation issues.
- responses do not expose raw SQL.
- responses do not expose raw payload.
- responses do not expose credentials, endpoint values, or internal stack trace.
- responses do not expose real broker order ids.

No silent success is allowed.

## Post-Write Readback Boundary

TASK-204 verifies the existing TASK-202 post-write readback refresh boundary.
The acceptance checks confirm that post-write refresh status fields exist in the
QML / Presenter boundary and that refresh action names remain DataService
read-boundary actions:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`

TASK-204 does not add new read model refresh behavior and does not connect
AccountingEngine replay beyond existing read boundaries.

## No Direct UI DB Access

TASK-204 acceptance keeps UI and Presenter boundaries intact:

- QML only calls Presenter.
- QML does not call DataServiceClient directly.
- QML does not access SQLite.
- QML does not import or call DataAccess.
- Presenter / Controller do not include DataAccess repository headers.
- Presenter / Controller do not execute SQL.
- Presenter / Controller do not trigger AccountingEngine replay.

## Forbidden Behavior

TASK-204 does not authorize:

- production QML changes.
- production startup changes.
- Presenter or Controller behavior changes.
- ShellServices adapter / port changes.
- DataServiceActions or DataServiceActionRegistrar changes.
- DataAccess repository changes.
- changes to `migrations/001_initial_schema.sql`.
- changes to `migrations/002_shell_accounting_manual_entry_schema.sql`.
- new migrations or schema files.
- new production SQL.
- AccountingEngine replay integration.
- audit_log or ledger writes.
- TradeDraft or suggestion behavior.
- broker SDK integration.
- network calls.
- credentials or endpoint access.
- real order placement.
- real broker order id storage.
- automatic trading.
- deleting, weakening, or bypassing broker / real broker / no-network /
  no-credentials / no-order-placement gates.

## Future Task Split

Future audit write policy, ledger policy, broader SELL / Withdrawal fixture
expansion, read model refresh behavior beyond TASK-202, broker capability, real
order placement, strategy execution, or automatic trading must be separately
authorized. Broker sandbox new capability development remains paused in this
phase, and existing broker gates remain required.
