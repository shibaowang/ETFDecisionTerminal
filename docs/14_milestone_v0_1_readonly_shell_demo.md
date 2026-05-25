# v0.1 ReadOnly Shell Demo

## Positioning

`v0.1 ReadOnly Shell Demo` is a local desktop read-only demonstration loop. Its purpose is to validate the engineering skeleton and the basic path between the DataService read-only service, DataServiceClient, Watchdog / diagnostic tooling, ShellServices, and the ETFDecisionShell QML read-only data page.

This milestone is not a completed trading system. It is a controlled developer milestone for validating the local read-only runtime.

## Completed Capabilities

- CMake multi-target project structure.
- SQLite initial schema and migration.
- CoreDomain fixed-point and domain types.
- Protocol message envelope, response, service names, and error codes.
- DataAccess SQLite connection, migration runner, read-only repositories, transactions, and audit repository boundary.
- ETFDataService database initialization and database health checks.
- ETFDataService read-only socket actions.
- DataServiceClient wrapper for read-only protocol calls.
- Transport, ServiceRuntime, and ServiceHost.
- Watchdog service manifest loading, dry-run, status report, and JSON export.
- ETFDiag diagnostic report consumer.
- ShellCore diagnostics, navigation, page status, mock metrics, and Qt model adapters.
- ShellServices read-only facade, controller, list models, connection state flow, refresh guard, and connection presets.
- ETFDecisionShell QML mock framework and read-only data preview page.
- ReadOnly demo script under `tools/dev/run_readonly_demo.ps1`.
- Manual acceptance document under `docs/13_manual_acceptance_readonly_shell.md`.

## Explicitly Not Supported

- 不支持自动交易.
- 不支持券商接口.
- 不支持真实下单.
- 不支持成交确认.
- 不支持手工入账.
- 不写 TradeLog.
- 不支持 TradeDraft 生命周期.
- 不支持策略计算.
- 不支持行情接口.
- 不支持 Excel 导入.
- 不支持真实账户编辑.
- 不支持真实组合编辑.
- QML 不直接访问数据库.
- QML 不直接调用 DataServiceClient.

## Verified Path

- `ETFDataService --init-db`
- `ETFDataService --check-db`
- `ETFDataService --serve-readonly`
- `ETFDecisionShell --diagnostics-mock`
- ReadOnlyDataPage `Connect`
- ReadOnlyDataPage `Refresh All`
- Default account display.
- Default portfolio display.
- `CASH` instrument display.

## Quality Gates

- CMake configure passes.
- Build passes.
- Full `ctest --test-dir build --output-on-failure` passes.
- `transport_local_socket_echo` passes 50 repeated runs.
- Manual read-only demo acceptance passes.
- `git status` is clean after runtime outputs are ignored or removed.

## Risks And Notes

- SQLite may still use the amalgamation fallback in local environments.
- QML remains in a mock / prototype phase.
- The read-only data page is a development prototype.
- Real business pages are not connected yet.
- `runtime/` is a developer runtime output directory and must not be committed.

## Next Stage Recommendations

- Define account, portfolio, position, and strategy page ViewModels before connecting more real read-only pages.
- Keep account / portfolio UI work read-only until a dedicated write-boundary task authorizes editing, accounting, or trading.
- Keep TradeLog writing out of scope until a dedicated write-boundary task authorizes it.
- Keep automatic trading out of scope.
- Keep real market data integration out of scope until the service boundary is specified.
- Split ReadOnly data display pages before adding editable business workflows.
