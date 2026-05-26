# Manual Acceptance - ReadOnly Business Pages

## 1. Document Purpose

This checklist validates the v0.2 ReadOnly Business Pages quality gate.

Scope:

- Account and portfolio read-only page.
- Instrument and strategy read-only page.
- Read-only data preview page.
- Read-only data flow and UI presentation only.

Out of scope:

- Trading.
- Data writes.
- Strategy execution.
- TradeDraft generation.
- Accounting entry.

## 2. Prerequisites

- The project has been built successfully.
- Current `main` has passed the full automated test suite.
- Qt `6.9.3` `msvc2022_64` is available.
- DataService is running in read-only mode.
- ETFDecisionShell is running.
- No real account data is required.
- Use a demo SQLite database only.
- `runtime/` is ignored by `.gitignore` and must not be committed.

## 3. Startup Commands

Recommended demo startup:

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1 -ForceRecreateDb
```

Manual startup:

```powershell
build\apps\ETFDataService\Debug\ETFDataService.exe --serve-readonly --db runtime\dev\readonly_demo\ETFDecision.db --socket-name ETFDataServiceReadonly
build\apps\ETFDecisionShell\Debug\ETFDecisionShell.exe --diagnostics-mock
```

## 4. Global Acceptance Items

Check all of the following:

- Shell opens successfully.
- Left navigation is clickable.
- Top status bar shows Mock / development state.
- Page switching does not crash.
- No QML error dialog appears.
- Read-only data preview page connects successfully.
- `Refresh All` succeeds.
- Connection failure displays a visible error.
- `Refresh All` throttle / busy state is visible.
- Pages have no trade, accounting, edit, add, or delete buttons.

## 5. Read-Only Data Preview Page

Acceptance items:

- Default preset is `DataService 只读服务`.
- `socketName` is `ETFDataServiceReadonly`.
- `Connect` succeeds.
- `Refresh All` succeeds.
- `healthy=true`.
- `accounts=1`.
- `portfolios=1`.
- `instruments=1`.
- `strategies=0` is acceptable.
- Error panel can display a connection error.
- `Refresh All` button behaves correctly when busy or throttled.

## 6. 账户与组合 Page

Acceptance items:

- Page title and notice identify the page as account / portfolio read-only.
- Page clearly states it is read-only and does not support edit, accounting entry, or trading.
- Account list displays the default account.
- Portfolio list displays the default portfolio.
- Account fields include at least:
  - `name`
  - `accountType`
  - `brokerName` or a hideable broker column
  - `baseCurrency`
  - `isActive`
  - `initialCashText`
- Portfolio fields include at least:
  - `name`
  - `basePositionRatioText`
  - `isActive`
- 搜索 / 筛选 is usable.
- Active-only filter is usable.
- 表头排序 is usable.
- 列显示 switch is usable.
- Required columns cannot be hidden.
- `compact` / `normal` / `comfortable` 密度 switching is usable.
- Empty state is clear.
- Error state is clear.
- No add, edit, delete, deposit, withdrawal, accounting, or trade button is present.

## 7. 标的与策略 Page

Acceptance items:

- Page title and notice identify the page as instrument / strategy read-only.
- Page clearly states it is read-only and does not support edit, strategy execution, or trading.
- Instrument list displays `CASH`.
- Empty strategy list does not crash the page.
- Empty OTC channel result displays a no-data state.
- Instrument fields include at least:
  - `code`
  - `name`
  - `instrumentType`
  - `marketCode`
  - `currency`
  - `enabled`
- Strategy fields include at least:
  - `strategyCode`
  - `name`
  - `enabled`
- OTC fields include at least:
  - `strategyCode`
  - `actualCode`
  - `fundClass`
  - `enabled`
  - `dailyLimitText`
  - `priority`
  - `minBuyAmountText`
- 搜索 / 筛选 is usable.
- Enabled-only filter is usable.
- 表头排序 is usable.
- 列显示 switch is usable.
- Required columns cannot be hidden.
- 密度 switching is usable.
- No add, edit, delete, strategy execution, trade suggestion generation, or trade button is present.

## 8. UI Readability Acceptance

Check:

- Table headers are clear.
- Row height is consistent.
- Column width is reasonable.
- Status badge is readable.
- Empty state is explicit, not blank.
- Error information is visible.
- ReadOnly marking is visible.
- Page does not appear trade-capable.
- Mock / Placeholder / ReadOnly state is clearly expressed.
- Mixed Chinese / English text does not block basic readability.

## 9. Prohibited Items

The pages must not contain operational controls for:

- 编辑
- 删除
- 新增
- 入账
- 出金
- 入金
- 现金校准
- 交易
- 下单
- 执行策略
- 生成交易建议
- 成交确认
- TradeDraft 生成
- TradeLog 写入

## 10. Database And Write Boundary Acceptance

Required boundary:

- 不得调用 data.audit.append.
- Do not call any write action.
- Do not write `audit_log`.
- 不写 trade_log.
- Do not write `trade_execution_group`.
- Do not write `trade_draft`.
- Do not write `position_snapshot`, `cash_snapshot`, or `portfolio_summary`.
- Do not modify `migrations/001_initial_schema.sql`.
- QML must not directly access SQLite.
- QML must not directly call `DataServiceClient`.
- 筛选 / 排序 / 列显示 / 密度 settings only change UI presentation and do not change backend data.

## 11. Cleanup

Steps:

1. Close ETFDecisionShell.
2. Stop ETFDataService.
3. Run:

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/stop_readonly_demo.ps1
```

Optional cleanup:

```powershell
Remove-Item -Recurse -Force runtime -ErrorAction SilentlyContinue
```

After cleanup:

```powershell
git status
```

Expected: clean working tree. `runtime/` output must not be committed.

## 12. Pass Criteria

Acceptance passes only when:

- All pages open.
- All expected read-only data appears.
- 筛选 / 排序 / 列显示 / 密度 switching are usable.
- No write entry point exists.
- No crash occurs.
- Automated tests pass.
- `git status` is clean.
- No Git tag is created as part of this checklist.

## 13. Failure Record Template

```markdown
- Date:
- Version / commit:
- Page:
- Problem description:
- Reproduction steps:
- Expected result:
- Actual result:
- Screenshot:
- Blocks merge: yes / no
- Fix task id:
```
