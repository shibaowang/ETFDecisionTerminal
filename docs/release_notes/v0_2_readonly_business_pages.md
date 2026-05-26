# Release Notes - v0.2 ReadOnly Business Pages

Date: TBD

## Highlights

- Account & Portfolio read-only page.
- Instrument & Strategy read-only page.
- Read-only table components.
- Filter / sort / column visibility / density controls.
- Manual acceptance checklist.
- UI readability checklist.

## Added

### Shell QML Pages

- Account and portfolio read-only business page.
- Instrument and strategy read-only business page.
- Read-only data preview workflow.

### ReadOnly QML Components

- `ReadOnlyTable`.
- `ReadOnlyFilterBar`.
- `ReadOnlyColumnChooser`.
- `ReadOnlyDensityToggle`.
- `ReadOnlyStatusBadge`.
- `ReadOnlyEmptyState`.
- `ReadOnlySectionHeader`.
- `ReadOnlyFieldLabel`.

### ShellServices Readonly Models / Proxies

- Read-only data controller bindings for account, portfolio, instrument, strategy, and OTC models.
- Read-only proxy model filtering and sorting.
- Runtime-only column visibility and density display state.

### DevDocs Acceptance Checks

- Static documentation checks for read-only business page acceptance assets.

### Manual Acceptance Docs

- Manual acceptance checklist for read-only business pages.
- UI readability checklist for future read-only page work.

## Changed

- `account_portfolio` route is no longer a plain placeholder.
- `strategy` route now opens the instrument / strategy read-only page.
- Read-only pages use shared table components.
- README and docs index were updated.

## Not Included

- Writing to `trade_log`.
- TradeDraft lifecycle.
- Strategy calculation.
- Market feed.
- Excel import.
- Broker API.
- Auto trading.
- Account editing.
- Portfolio editing.
- Instrument editing.
- Strategy editing.
- OTC channel editing.
- Any write action.

## Validation

- Full CTest passed.
- `transport_local_socket_echo` repeat passed.
- Manual acceptance passed.

## Suggested Tag After Merge

Do not create this tag in this task.

Suggested tag:

```powershell
git tag -a v0.2.0-readonly-business-pages -m "v0.2 ReadOnly Business Pages"
git push origin v0.2.0-readonly-business-pages
```
