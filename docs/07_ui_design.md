# 界面设计总纲

## Diagnostics page data source

- A future Shell diagnostics page can reuse the Watchdog manifest status JSON
  fields: service counts, issue counts, per-service health and startability
  fields, and issue arrays.
- TASK-018 only adds `ETFDiag` as a command line consumer of that report.
- TASK-018 does not implement QML, a Shell page, or any UI workflow.
- TASK-019 adds Diagnostics DTOs and parser logic for reuse by future Shell
  diagnostics views.
- Future QML should consume structured Diagnostics DTOs exposed by C++ instead
  of parsing raw Watchdog JSON in QML.
- TASK-020 introduces `ShellDiagnosticViewModel`,
  `ShellDiagnosticServiceRow`, and `ShellDiagnosticIssueRow` as the intended UI
  binding shape.
- A future QML page should bind to ShellCore-provided ViewModel fields and must
  not access SQLite or parse complex diagnostic JSON directly.
- TASK-020 does not implement QML.
- TASK-021 prepares future diagnostics UI interactions in C++: status filters,
  severity sorting, service / issue search, aggregate counters, and report file
  refresh detection.
- Current task still does not implement QML or any visual page.
- TASK-022 introduces `ShellDiagnosticPresenter` as the intended C++ object a
  future diagnostics page should bind to.
- Mock diagnostics reports may be used for UI prototypes, but the UI must label
  them as mock / non-real data.
- QML still must not parse raw Watchdog JSON or access SQLite.
- TASK-023 adds the future diagnostics page data sources:
  `ShellDiagnosticServiceListModel` for the service list,
  `ShellDiagnosticIssueListModel` for the selected service issues, and
  `ShellDiagnosticSummaryObject` for the top summary.
- Current task still does not create QML files. Future QML should bind
  `ShellDiagnosticQtAdapter` rather than calling Diagnostics or parsing JSON.
- TASK-024 adds a minimal diagnostics mock page. The page displays Summary,
  Service List, and Issue List sections backed by ShellCore models.
- The page includes visible mock labeling and load buttons for healthy, warning,
  error, and mixed mock data.
- QML remains presentation-only: filtering calls C++ adapter methods, QML does
  not parse JSON, and QML does not access databases or services.
- TASK-025 introduces the terminal shell layout: top status bar, side
  navigation, content host, right info panel, and bottom log / alert panel.
- The navigation contains dashboard, market, account / portfolio, positions,
  strategy, TradeDraft, confirmation, TradeLog, cash plan, OTC A/C channel,
  alerts, diagnostics, and settings.
- Diagnostics is the only mock-data page; all other modules are clearly marked
  placeholder mock pages.

## 当前状态

v0.1 草案。

## 后续补充方向

补充桌面端信息架构、关键工作流、状态展示和 QML 边界约束。

## 主界面结构

采用东方财富式终端布局：

- 顶部：账户状态 / 行情状态 / 服务状态 / 时间
- 左侧：功能导航
- 中间：主工作区
- 右侧：策略详情 / 委托明细 / 风险提示
- 底部：日志 / 预警 / 服务消息

## 第一版模块

- 首页驾驶舱
- 行情监控
- 账户与组合
- 持仓明细
- 策略中心
- TradeDraft 交易建议
- 成交确认 / 手工入账
- TradeLog 账本
- 资金分配计划
- OTCMap A/C 类通道
- 预警中心
- 日志中心
- Excel 导入向导
- 系统设置

## 首页驾驶舱指标

- 账户总资产
- 本金基准
- 剩余现金
- 现金比例
- 战略底仓完成度
- 狙击资金池
- 持仓盈亏
- 当日盈亏
- active grid_cycle 数量
- active TradeDraft 数量
- 底仓破损数量
- 预算重算建议数量
- 行情状态
- 服务状态
- 今日预警
- 今日成交入账

## 持仓展示

采用双层结构：

- 策略汇总层
- 实际代码明细层

例如：

```text
159509 策略总持仓
    ├─ 159509 场内 ETF
    ├─ 017091 A类
    └─ 017092 C类
```

## 图表范围

第一版只做轻量图表：

- 资产曲线
- 现金比例曲线
- 本金基准变化
- 单标的持仓成本 vs 市值
- grid_cycle 周期盈亏图
- 底仓完成度图
- TradeDraft 信号时间轴

不做复杂 K 线系统。
## TASK-026 Navigation Model

- Navigation items come from C++ `ShellPageRegistry`.
- `SideNavigation.qml` binds `ShellNavigationModel` and no longer hardcodes the full navigation list.
- `ContentHost.qml` loads pages from `currentPageKey` / `currentPageQmlComponent`.
- Only Diagnostics Center uses mock data; all other modules remain placeholder pages.
- QML does not parse complex JSON, access SQLite, connect services, or contain business logic.

## TASK-027 Page Status Binding

- `TopStatusBar` displays page title, module status, data mode, and connection status from `ShellPageInfoObject`.
- `RightInfoPanel` displays current page detail text and placeholder flags from `ShellPageInfoObject`.
- `BottomLogPanel` displays mock log rows from `ShellMockLogModel`.
- QML does not infer business state; it only binds C++ model properties and rows.

## TASK-028 Mock Metrics

- Dashboard placeholder pages display mock metric cards from `ShellPageMetricModel`.
- `RightInfoPanel` displays the current page's mock metric summary and non-executable action hints.
- Action hints are presentation-only; they do not execute commands or call services.
- QML must not compute business metrics directly; metrics and hints come from ShellCore C++ models.
- Current metrics are explicitly mock / placeholder data and do not access SQLite.

## TASK-029 Read-Only Data Boundary

- Future real read-only data must enter the Shell through `ShellReadOnlyDataFacade` and later ViewModel adapters.
- QML must not call `DataServiceClient` directly.
- QML must not connect sockets or parse DataService responses directly.
- TASK-029 does not connect QML to real data; the visible Shell UI remains mock / placeholder.

## TASK-030 Read-Only Data ViewModels

- Future account, portfolio, instrument, and strategy pages should bind ShellServices read-only list models instead of calling `DataServiceClient` directly.
- `ShellReadOnlyDataController` is the C++ presenter boundary for loading health, summary, and list data through `ShellReadOnlyDataFacade`.
- QML is still not connected to real DataService data in TASK-030.
- QML must not connect sockets, call write actions, or parse DataService protocol payloads directly.

## TASK-031 Read-Only Data Prototype Page

- `ReadOnlyDataPage.qml` is a development-only read-only preview page.
- It displays connection state, health / summary counts, accounts, portfolios, instruments, and strategies.
- QML binds `ShellReadOnlyDataController` and the ShellServices list models; it does not call `DataServiceClient` directly.
- The page must show connection errors instead of faking successful data.
- The page provides no edit, trade, accounting, strategy, audit append, or write controls.

## TASK-032 Read-Only Data State Flow

- The read-only data page displays controller-owned `refreshState`, busy / refreshing state, last success time, and error text.
- `Refresh All` is enabled from `canRefresh`; QML does not implement its own duplicate-click or throttle rules.
- The controller exposes `IDLE`, `CONNECTING`, `REFRESHING`, `SUCCESS`, and `FAILED` states for direct UI binding.
- Error state must remain visible for failed connects, disconnected refreshes, local validation failures, and throttled refreshes.
- The page still does not edit data, call write actions, access SQLite, or directly call `DataServiceClient`.

## TASK-033 Read-Only Connection Guide

- The read-only data page now shows runtime connection presets and a command hint before connecting.
- Users can choose the recommended read-only DataService socket, the audit development socket, or a custom socket name.
- Presets are clearly development guidance and do not auto-start services or save configuration.
- The Connect button uses controller-owned `selectedSocketName`; empty custom socket names are rejected by C++ state.
- The page remains read-only and exposes no edit, trade, accounting, audit append, or write controls.

## TASK-037 Account Portfolio Read-Only Page

- `账户与组合` is the first business navigation page promoted from placeholder to a read-only prototype.
- The page displays account and portfolio lists from ShellServices models only.
- Visible account fields include name, account type, broker, base currency, active state, and initial cash.
- Visible portfolio fields include name, base position ratio, and active state.
- The page must keep edit, delete, deposit, withdrawal, accounting, confirmation, and trade controls out of scope.
- Errors and disconnected state must remain visible; QML must not fake successful data or call `DataServiceClient` directly.

## TASK-038 Instrument Strategy Read-Only Page

- `标的与策略` is a read-only business page prototype for instruments, strategies, and OTC channels.
- Visible instrument fields include code, name, instrument type, market, currency, and enabled state.
- Visible strategy fields include strategy code, name, and enabled state.
- Visible OTC fields include strategy code, actual code, fund class, enabled state, daily limit, priority, and minimum buy amount.
- The page must not include edit, delete, strategy execution, TradeDraft generation, trade, or accounting controls.
- Empty strategy lists and empty OTC channel results must be visible and must not crash the page.

## TASK-039 Read-Only Table Components

- Read-only business pages should use shared table components for headers, rows, badges, and empty states.
- `ReadOnlyTable` provides the visual table shell and table header area.
- `ReadOnlyStatusBadge` displays stable state labels such as `ACTIVE`, `ENABLED`, `DISABLED`, `READONLY`, `OK`, `WARNING`, and `ERROR`.
- `ReadOnlyEmptyState` makes no-data states explicit instead of leaving blank panels.
- `ReadOnlySectionHeader` and `ReadOnlyFieldLabel` standardize section titles and status fields.
- These components must remain display-only and must not contain write buttons, service calls, JSON parsing, or business calculations.

## TASK-040 Read-Only Filtering And Sorting

- Read-only tables support a shared filter bar, sortable headers, stable column widths, aligned numeric columns, and explicit empty states.
- Search and status filters are user input only; filtering and sorting happen through ShellServices proxy models.
- Filter / sort operations do not refresh services, mutate source models, write SQLite, or call write actions.
- QML pages forward search and sort requests to `ShellReadOnlyDataController` and bind filtered models for display.
- Read-only pages must keep edit, delete, accounting, trade, strategy execution, and TradeDraft generation controls out of scope.

## TASK-041 Read-Only Column Visibility And Density

- Read-only tables support runtime column visibility controls for account, portfolio, instrument, strategy, and OTC lists.
- Required identity columns remain visible and cannot be hidden; optional columns can be toggled without changing source model data.
- Table density supports `compact`, `normal`, and `comfortable`, changing only row height, padding, and text size.
- Column visibility and density settings are not persisted and must not write files, SQLite, registry, or service configuration.
- These controls are visual-only; they must not call services, connect sockets, execute actions, or make read-only pages look trade-capable.
## TASK-044 Future Position Page Boundary

- A future position page must bind ShellServices ViewModel / Model objects for position, cash, PnL, base position, and sniper-pool data.
- QML must not calculate cost,收益, position quantity, cash balance, principal, base-position completion, or sniper-pool tiers.
- QML must not directly access SQLite or call `DataServiceClient`.
- The current task does not implement a position page, position replay, cash replay, PnL calculation, snapshot generation, or TradeLog writes.
- Position and accounting UI work must follow [Position Accounting Boundary](20_position_accounting_boundary.md) and the [Position Read-Only Data Contract Draft](21_position_readonly_data_contract_draft.md).

## TASK-099 ShellServices Accounting Controller Contract

- Accounting UI must bind ShellServices ViewModel / Model objects documented in
  [ShellServices Accounting Controller Contract](49_shellservices_accounting_controller_contract.md)
  and [Shell Accounting ViewModel State Contract](50_shell_accounting_viewmodel_state_contract.md).
- Unavailable / warning / error states must remain visible; QML must not hide
  guard or domain issues.
- Read-only accounting pages must not expose trading buttons, TradeDraft
  generation, strategy execution, or write actions.
- Privacy mode masks display text only; it must not mutate source data or make
  QML calculate accounting values.

## TASK-100 Shell Accounting State Scaffolding

- Accounting UI states must distinguish Empty / Unavailable / Warning / Error /
  Stale.
- Privacy mode masks display text only; QML must not calculate masked text.
- Read-only accounting pages must not show trading buttons, TradeDraft actions,
  broker order actions, or strategy execution controls.
- TASK-100 scaffolding is test-only and does not connect QML to accounting
  actions.

## TASK-101 ShellAccountingReadOnlyController Skeleton

- Accounting controller skeleton exists, but QML is still not wired.
- Future UI must still bind through ShellServices only in a separately
  authorized task.
- Skeleton state / issue / privacy behavior does not permit QML accounting
  calculation.

## TASK-102 Shell Accounting Fake Client Boundary Tests

- Shell accounting fake/client tests do not imply QML integration.
- Future UI must still bind through ShellServices only.
- Fake payload state, warning, error, stale, privacy, and no-trade coverage is
  a controller boundary test, not a page implementation.

## TASK-103 Shell Accounting Service Adapter Boundary

- ShellServices service adapter boundary docs do not imply QML integration.
- Future UI must still bind ViewModel / Model objects only.
- QML must not call DataServiceClient, ShellAccountingDataServiceAdapter,
  SQLite, DataAccess, or AccountingEngine directly.
- Adapter integration remains a ShellServices boundary concern, not a QML page
  shortcut.

## TASK-105 Shell Accounting Fake Adapter Injection

- Fake adapter injection tests do not imply QML integration.
- Future UI must still bind through ShellServices ViewModel / Model objects
  only.
- Controller refresh state mapping is a ShellServices boundary; QML must not
  call DataServiceClient or calculate accounting values.
- Read-only accounting UI must still avoid TradeDraft, trade suggestion,
  strategy, and broker-order controls.

## TASK-113 Shell Accounting ViewModel / Model Boundary

- Future accounting UI must bind ShellServices ViewModel / Model objects:
  `ShellAccountingStatusObject`, `ShellAccountingIssueListModel`, and
  `ShellPositionListModel`.
- QML must not call DataServiceClient, ShellAccountingDataServiceAdapter,
  SQLite, DataAccess, or AccountingEngine directly.
- QML must not calculate accounting, cost, PnL, base-position sellable amount,
  sniper-pool remaining amount, privacy masks, or issue state.
- Read-only accounting UI must not show buy / sell buttons, TradeDraft actions,
  broker order actions, strategy execution controls, or write-enabled flows.
- TASK-113 does not wire QML and does not register QML types.

## TASK-114 Shell Accounting QML Binding Readiness

- The QML binding readiness plan is defined in
  [Shell Accounting QML Binding Readiness Plan](58_shell_accounting_qml_binding_readiness_plan.md).
- Future accounting UI must bind ShellServices ViewModel / Model objects only.
- QML must not directly call DataServiceClient, access SQLite, call
  AccountingEngine, or calculate accounting values.
- Read-only accounting UI must explicitly distinguish Empty, Unavailable,
  Warning, Error, and Stale.
- Read-only accounting UI must not display trading buttons, TradeDraft actions,
  trade suggestion actions, strategy execution controls, or broker order
  controls.
- TASK-114 does not modify QML.

## TASK-115 ShellAccountingPresenter Skeleton

- The ShellAccountingPresenter skeleton exists, but QML is still not wired to
  it and no QML type is registered.
- Future UI must continue to bind only ShellServices read-only presentation
  objects.
- Future UI must preserve Empty vs Unavailable and visible Warning / Error /
  Stale states.
- Future UI must use ShellServices display text for privacy and must not
  calculate masks or accounting values in QML.
- Future UI must avoid TradeDraft, trade suggestion, strategy execution, and
  broker order controls unless a separate task explicitly authorizes a new
  boundary.

## TASK-117 Presenter All Guard Refresh

- The presenter now has read-only refresh wiring for all five accounting guard
  actions, but QML remains unwired.
- Future UI must still bind only ShellServices presenter / ViewModel / Model
  objects and must not call DataServiceClient, SQLite, AccountingEngine, or
  adapter classes directly.
- The five current guard states are `Unavailable`; UI must not present them as
  normal Empty data.
- Read-only accounting UI still must not display buy / sell buttons,
  TradeDraft actions, trade suggestion actions, strategy execution controls,
  or broker order controls.

## TASK-118 ShellAccounting QML Smoke Readiness

- The QML binding smoke plan is defined in
  [ShellAccounting QML Binding Smoke Plan](59_shell_accounting_qml_binding_smoke_plan.md).
- The QML static gate is defined in
  [ShellAccounting QML Static Gate](60_shell_accounting_qml_static_gate.md).
- Future accounting UI must distinguish Empty, Unavailable, Warning, Error,
  and Stale.
- Future accounting UI must remain read-only and must not display buy / sell,
  TradeDraft, strategy execution, broker order, or write-enabled controls.
- QML still must not directly call DataServiceClient, access SQLite or
  DataAccess, call AccountingEngine, or calculate accounting values.
- TASK-118 does not modify QML.
## TASK-119 ShellAccounting QML Static Gate CTest

The ShellAccounting QML static gate is now automated by CTest. Future read-only
accounting UI must pass the gate before merge: QML must not directly call
DataServiceClient, access SQLite / DataAccess, call AccountingEngine, expose
write actions, or show accounting-scope trading buttons.

The gate preserves the UI contract that Empty is not Unavailable, warning /
error / stale states remain visible, privacy display is controlled by
ShellServices, and read-only accounting UI does not display buy, sell,
TradeDraft, strategy, or broker-order controls.

TASK-119 does not modify QML, register QML types, or implement accounting QML
pages.

## TASK-120 ShellAccounting Read-only UI Readiness

The v0.6 ShellAccounting read-only UI readiness milestone is documented in
`docs/61_shell_accounting_readonly_ui_readiness_milestone.md`, with next phase
review in `docs/62_shell_accounting_next_phase_review.md`.

This milestone does not wire QML. Future accounting UI binding must remain
read-only, must pass the TASK-119 QML static gate, must keep Empty distinct from
Unavailable, and must not display trading buttons, TradeDraft, strategy, or
broker-order controls.

## TASK-121 ShellAccounting QML Binding Smoke Scaffold

The QML binding smoke scaffold is now available as a test-only readiness gate.
Future accounting UI should pass the scaffold before QML binding: only
ShellServices read-only objects may be bound, all UI states must be represented,
guard payloads must show Unavailable, issues and privacy rules must remain
visible, and no trading controls may be displayed.

## TASK-122 ShellAccounting QML Smoke CTest

Test-only ShellAccounting QML smoke CTests now verify the first runtime binding
expectations with inline QML. The smoke tests cover object availability, state
display, guard Unavailable display, issue visibility, privacy display, no-trade
UI tokens, and static gate regression.

Production UI binding is still not implemented. Future accounting UI must still
be read-only, must not show trading buttons, must keep Empty distinct from
Unavailable, and must pass TASK-119 / TASK-121 / TASK-122 before any QML file is
modified.

## TASK-123 Production QML Binding Boundary

Production QML binding boundary is now defined in docs/63, with the first-stage
checklist in docs/64. The first production stage can only be a read-only
placeholder / Unavailable / issue / privacy smoke. It must not show real
accounting data, trading buttons, TradeDraft, broker order, strategy execution,
or write actions.

## TASK-124 QML Type Registration Boundary

QML type registration boundary is now defined in docs/65, with the registration
test plan in docs/66. Type registration is not UI page binding. The first stage
must remain read-only and must not expose DataServiceClient, SQLite,
AccountingEngine, write methods, TradeDraft, trade suggestion, broker order, or
strategy execution to QML.

## TASK-125 QML Type Registration Scaffold

QML type registration scaffold tests now fix the first-stage registration
allowlist, denylist, module boundary, and no-production-change gate. Production
UI remains unwired; the scaffold is not QML type registration and is not a QML
page implementation.

## TASK-126 QML Type Registration Implementation Gate

QML type registration implementation gate and rollback strategy are now defined
in docs/67 and docs/68. Production UI remains unwired. Future registration must
stay read-only, preserve rollback / disable behavior, and must not expose
DataServiceClient, SQLite, AccountingEngine, write methods, TradeDraft, trade
suggestion, broker order, or strategy execution.

## TASK-127 QML Type Registration Implementation

The first-stage ShellAccounting QML type registration now exists as a
centralized ShellServices helper. It registers only read-only, uncreatable
allowlist types in `ETFDecisionTerminal.ShellAccounting` 1.0. UI page binding is
still not implemented: production QML must not import the module until a
separate authorized task, and the future first UI stage remains read-only
placeholder / unavailable / issue / privacy smoke only.

## TASK-128 / TASK-129 Startup Registration UI Boundary

The startup registration gate is now defined, and TASK-129 wires production app
startup to call `registerShellAccountingQmlTypes` once from the centralized QML
type registration location. Production QML still does not import
`ETFDecisionTerminal.ShellAccounting`, and no ShellAccounting UI page is wired.
Future UI work must pass the startup registration gate and remain read-only: no
trading buttons, no write action, no `TradeDraft`, no strategy execution, no
broker order, and no real accounting data until separately authorized.

## TASK-130 Production QML Binding UI Gate

The production QML binding gate is now defined. Production UI is still not
wired: no production QML import, page/component, `accountingPresenter` binding,
or ShellAccounting presenter context exposure exists yet. Future ShellAccounting
UI must be read-only, Unavailable-safe, disabled-state aware, and must not show
trading buttons, write actions, TradeDraft generation, trade suggestions,
strategy execution, broker order controls, or real accounting data until a
separate task explicitly authorizes that scope.

## TASK-131 Production QML Binding Shell

The ShellAccounting production QML shell is now wired as a read-only,
Unavailable-safe page. It imports `ETFDecisionTerminal.ShellAccounting` only in
`ShellAccountingReadOnlyPage.qml`, declares a nullable presenter property, and
shows disabled state when no presenter lifecycle is provided. It still does not
connect real accounting data, write paths, TradeDraft generation, trade
suggestions, broker behavior, strategy execution, or trading UI.

## TASK-132 Presenter Lifecycle Gate

The ShellAccounting presenter lifecycle gate is now defined. The production UI
shell still supports a null / Unavailable presenter and remains read-only. No
presenter is created, no context property is exposed, no real data is connected,
and no trading UI or write path is enabled.

## TASK-133 Presenter Lifecycle Implementation

The ShellAccounting UI shell now receives a production-owned
`accountingPresenter` context object. The page still renders an
Unavailable-safe, read-only state because real accounting data is not connected.
No fake rows, trade buttons, write actions, TradeDraft generation, broker order,
or strategy execution UI is introduced.

## TASK-134 Real Data Adapter Gate

The real data adapter gate is now defined. The UI remains read-only and
Unavailable-safe until a separate real adapter implementation task is
authorized. Future real data must map unavailable/error/stale states explicitly
and must not add trade UI, write actions, TradeDraft generation, broker order,
strategy execution, direct SQLite reads, or direct AccountingEngine calls.

## TASK-135 Real Read-only Adapter

The ShellAccounting UI can now reflect read-only adapter state through the
production presenter path. It remains unavailable-safe and must not show trade
UI, write actions, TradeDraft generation, trade suggestions, broker order, or
strategy execution controls.

## TASK-136 DataService Facts Query Gate

The DataService facts query gate is added. The ShellAccounting UI remains
read-only and unavailable-safe until a separately authorized DataService facts
query implementation exists. The UI still has no trade UI.

## TASK-137 DataService Facts Query Implementation

ShellAccounting read-only data can now flow from DataService SELECT-only facts
queries through the existing presenter path. The UI remains read-only and must
not add trade UI, write actions, TradeDraft generation, trade suggestions,
broker order, strategy execution controls, direct SQLite reads, or
AccountingEngine replay.

## TASK-138 Replay Snapshot Rebuild Gate

The replay / snapshot rebuild gate is added. The UI remains read-only; replay
and rebuild are not connected to UI. The UI must not trigger snapshot rebuild,
snapshot writes, TradeDraft generation, trade suggestions, broker order,
strategy execution, or trading buttons.

## TASK-139 Read-only Replay Implementation

ShellAccounting read-only replay is now available inside the DataService
boundary. The UI remains read-only, does not directly trigger replay, does not
trigger snapshot rebuild/write, and still has no trading buttons. Shell/QML do
not call AccountingEngine, SQLite, or DataAccess directly.
