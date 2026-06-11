# TASK-271 ShellAccounting Excel/VBA Import MVP Local Service E2E Acceptance

TASK-271 adds an end-to-end acceptance test for the current Excel/VBA import
MVP using production components composed in a local test harness. It does not
add product functionality or change UI behavior.

## Scope

- Load a sanitized in-memory Excel/VBA export sample.
- Execute preview through `ShellAccountingPresenter`.
- Route preview through the ShellServices DataService client port adapter, a
  real `DataServiceClient`, and a local DataService action host.
- Execute `accounting.excel_vba_import.readonly_preview` and verify the
  Presenter reaches an accepted preview state.
- Execute accepted preview persistence through the Presenter and
  `accounting.excel_vba_import.persist_manual_entry`.
- Persist only to a temporary SQLite database initialized by the test.
- Verify exact `trade_log`, `cash_adjustment`, and `audit_log` row growth.
- Verify the existing post-write readback refresh path is attempted and
  succeeds with sanitized Presenter state.
- Verify duplicate import does not grow rows and idempotency conflict does not
  grow rows or trigger refresh.

## Boundaries

TASK-271 must not modify apps, QML, startup wiring, DataServiceApi action
implementation, DataServiceClient production code, DataAccess repositories,
migrations, AccountingEngine production code, historical fixtures, broker,
network, credentials, endpoints, TradeDraft, strategy, real order placement, or
automatic trading.

The test may initialize and write a temporary SQLite database only. It must not
touch a production database path. The local service host must be in-process or
local socket only and must be closed by the test.

## Evidence

The CTest emits sanitized JSON evidence proving:

- local service host and real DataServiceClient were used.
- preview and persist were initiated through the Presenter.
- preview was accepted.
- persistence called the DataService write action.
- temp `trade_log`, `cash_adjustment`, and `audit_log` rows were written.
- post-persist readback refresh was attempted and succeeded.
- duplicate import and idempotency conflict produced no row growth.
- no residual process, broker order, network access, credential access,
  endpoint access, or automatic trading occurred.
