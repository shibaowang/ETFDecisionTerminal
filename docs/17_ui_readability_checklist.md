# UI Readability Checklist

## 1. Read-Only Page Design Principles

- Read-only pages present data and state only.
- Read-only pages must visibly mark themselves as ReadOnly.
- Mock / Placeholder / ReadOnly labels must be clear and must not imply real trading capability.
- The page must not hide errors behind empty panels.

## 2. Table Readability Standards

- Tables must have clear section headers.
- Headers must align with row content.
- Numeric or amount-like fields should be aligned consistently.
- Row height must be consistent inside the same table.
- Column widths should support scanning without unnecessary wrapping.
- Empty tables must show `ReadOnlyEmptyState` or equivalent text.

## 3. Status Badge Standards

- Status badges should use stable labels such as `ACTIVE`, `ENABLED`, `DISABLED`, `READONLY`, `OK`, `WARNING`, and `ERROR`.
- Badge color must support quick scanning.
- Badge text must remain readable on normal desktop DPI.
- Badges are display-only and must not trigger actions.

## 4. Empty State Standards

- Empty state must include a title.
- Empty state must explain why the table is empty when possible.
- Empty state must not look like a loading failure unless there is an actual error.
- Empty state must not include add / create buttons on read-only pages.

## 5. Error State Standards

- Connection errors must be visible.
- Refresh errors must be visible.
- Error text must not claim data loaded successfully.
- Prior successful data may remain visible if the controller preserves it, but the error must still be shown.

## 6. 禁止写入按钮 Rules

Read-only pages must not include buttons or menu entries for:

- edit
- delete
- add
- deposit
- withdrawal
- accounting entry
- trade
- order placement
- strategy execution
- TradeDraft generation
- TradeLog writing

## 7. Mock / Placeholder / ReadOnly Marking

- Mock state must be labeled as Mock.
- Placeholder pages must state that real business data is not connected.
- ReadOnly pages must state that editing, accounting entry, trading, and strategy execution are unsupported.
- Labels must not be hidden in tooltips only.

## 8. Column Visibility Standards

- Column chooser state is runtime UI state only.
- Column visibility must not change source model data.
- Required identity columns must remain visible.
- Clearing visible-column state should fall back to the default column set.
- Column visibility must not be persisted unless a later task explicitly authorizes persistence.

## 9. Density Setting Standards

- Supported density values are `compact`, `normal`, and `comfortable`.
- Density only changes row height, padding, and text sizing.
- Density must not change backend data, sorting, filtering, or refresh behavior.
- Density must not be persisted unless a later task explicitly authorizes persistence.

## 10. Filter And Sort Standards

- QML may collect search text and sort clicks.
- Filtering and sorting should flow through C++ Model / ProxyModel / Controller boundaries.
- Filter and sort must not call write actions.
- Filter and sort must not mutate backend data.
- Empty filtered results must show a clear empty state.

## 11. QML 不做业务逻辑

- QML 不做业务逻辑.
- QML must not call `DataServiceClient` directly.
- QML must not access SQLite directly.
- QML must not parse complex protocol payloads directly.
- QML components must remain presentation-only.

## 12. Reuse Requirement For Future Pages

- New read-only business pages should reuse the shared read-only table, status badge, empty state, section header, filter bar, column chooser, and density toggle when applicable.
- Any exception must be documented in the task scope.
- Future write-capable pages require a separate authorized task and service boundary.
