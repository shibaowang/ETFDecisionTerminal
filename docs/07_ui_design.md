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
