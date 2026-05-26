# Milestone - v0.2 ReadOnly Business Pages

## Milestone Name

v0.2 ReadOnly Business Pages

## Positioning

This milestone builds on `v0.1 ReadOnly Shell Demo` and closes the first group of formal read-only business page prototypes.

The scope is account / portfolio and instrument / strategy read-only pages, plus read-only table readability, interaction, and manual acceptance coverage.

This is not a completed trading system.

## Completed Capabilities

- v0.1 ReadOnly Shell Demo baseline loop.
- 账户与组合 read-only page.
- 标的与策略 read-only page.
- `ReadOnlyDataPage` preview page.
- Read-only connection presets.
- Read-only state flow: `refreshState`, error state, refresh throttle, and busy guard.
- Shared read-only table components.
- 筛选.
- 排序.
- 列显示开关.
- Table density switching.
- Explicit empty states.
- Explicit error states.
- Read-only status badges.
- Manual acceptance checklist.
- UI readability checklist.

## Verified Pages

- Read-only data preview page.
- 账户与组合.
- 标的与策略.

## Verified Flow

- `ETFDataService --serve-readonly`.
- `ETFDecisionShell --diagnostics-mock`.
- Connect.
- Refresh All.
- Account list displays the default account.
- Portfolio list displays the default portfolio.
- Instrument list displays `CASH`.
- Empty strategy list does not crash.
- Empty OTC result does not crash.
- 筛选 / 排序 / 列显示 / 密度 controls are usable.

## Explicitly Not Supported

- 账户编辑.
- 组合编辑.
- 标的编辑.
- 策略编辑.
- OTC 通道编辑.
- 入账.
- 出入金.
- 现金校准.
- 交易.
- 成交确认.
- 不写 TradeLog.
- TradeDraft lifecycle.
- 策略计算.
- 策略执行.
- 行情接口.
- Excel import.
- 不支持自动交易.
- Broker API.

QML still must not directly access SQLite and must not directly call `DataServiceClient`.

Filtering, sorting, column visibility, and density settings only change UI presentation. They do not change backend data and do not write configuration.

## Quality Gate

- CMake configure passes.
- Build passes.
- Full `ctest` passes.
- `transport_local_socket_echo` passes 50 repeated runs.
- Read-only business pages manual acceptance has passed.
- `git status` is clean.

## Risks And Notes

- This remains a development-stage read-only prototype.
- Demo data volume is very small and does not represent production-scale performance.
- Position page is not implemented yet because it needs accounting replay and snapshot boundaries.
- TradeLog writing is not authorized.
- Strategy calculation is not authorized.
- Market feed integration is not connected.
- UI display settings are not persisted.
- `runtime/` is developer output and must not be committed.

## Recommended Next Stage

- Continue adding more read-only business pages.
- Define accounting replay and snapshot boundaries before building a position page.
- Do not enter TradeLog writing yet.
- Do not enter automatic trading yet.
- Do not enter strategy execution yet.
- Stabilize read-only page data models and DTOs before adding write flows.

## Suggested Tag After Merge

Do not create the tag in this task. After PR merge and local validation, the user may run:

```powershell
git tag -a v0.2.0-readonly-business-pages -m "v0.2 ReadOnly Business Pages"
git push origin v0.2.0-readonly-business-pages
```
