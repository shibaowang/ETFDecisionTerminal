# Release Notes - v0.1 ReadOnly Shell Demo

Date: TBD

## Highlights

- ReadOnly DataService demo.
- QML Shell mock framework.
- ReadOnlyDataPage for DataService read-only calls.
- Watchdog manifest, dry-run, and status report tooling.
- ETFDiag diagnostic report consumer.
- Manual acceptance script for the read-only Shell demo.

## Added

- CoreDomain: fixed-point and core domain value types.
- Protocol: message envelope, response type, service names, and error codes.
- DataAccess: SQLite connection, migration runner, read-only repositories, transactions, and audit boundary.
- DataService: database initialization, health checks, read-only actions, and the guarded audit development write action from the earlier audit task.
- Transport: frame codec and local socket transport.
- ServiceRuntime: action dispatcher and built-in ping / health actions.
- ServiceHost: action service host for local socket services.
- DataServiceClient: read-only client wrapper and controlled response parsing.
- Watchdog: service process manager, manifest loader, dry-run status, and JSON export.
- ETFDiag: diagnostic report consumer.
- ShellCore: diagnostics facade, presenter, navigation, page status, mock metrics, and Qt model adapters.
- ShellServices: read-only DataService facade, controller, list models, connection state, refresh guard, error state, and connection presets.
- ETFDecisionShell: QML mock shell layout and read-only data preview page.
- tools/dev: read-only demo run and stop scripts.

## Changed

- `runtime/` is ignored as a developer runtime output directory.
- Read-only demo script records pid files and uses scoped cleanup.
- QML Shell layout introduced a dashboard, navigation, read-only data preview, diagnostics mock page, right info panel, and bottom log panel.

## Not Included

- Automatic trading.
- Broker integration.
- Real order placement.
- TradeLog writing.
- TradeDraft lifecycle.
- Trade confirmation.
- Manual accounting entry.
- Strategy calculation.
- Real market data integration.
- Excel import.
- Real account editing.
- Real portfolio editing.
- Windows Service installation.
- Cloud sync or multi-user permission management.

## Validation

- Full CTest pass.
- `transport_local_socket_echo` repeat pass.
- Manual read-only demo pass.

## Suggested Tag After Merge

Do not create this tag before the PR is merged and the local milestone validation has passed.

```powershell
git tag -a v0.1.0-readonly-shell-demo -m "v0.1 ReadOnly Shell Demo"
git push origin v0.1.0-readonly-shell-demo
```
