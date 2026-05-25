# 系统架构设计

## ETFDiag diagnostic tool boundary

- TASK-018 adds `ETFDiag` as a local diagnostic command line tool.
- `ETFDiag` consumes the Watchdog manifest status JSON report and prints a
  human-readable summary.
- `ETFDiag` does not start services, connect Local Socket, access SQLite, or
  depend on DataAccess.
- `ETFDiag` validates required JSON fields and field types before producing a
  success exit code.
- The tool is a prototype data consumer for a future Shell diagnostics page;
  TASK-018 does not implement QML or UI.

## Diagnostics library boundary

- TASK-019 adds `ETFDiagnostics` as a shared model and parser library for
  Watchdog manifest status reports.
- `ETFDiagnostics` owns the report DTOs, issue DTOs, Watchdog JSON parser, and
  diagnostic summary generation.
- `ETFDiag` and a future Shell diagnostics page should reuse this library
  instead of duplicating JSON parsing.
- `ETFDiagnostics` does not start services, connect Local Socket, access SQLite,
  depend on DataAccess/DataServiceApi/Transport/ServiceHost, or implement
  business logic.

## Shell diagnostics facade boundary

- TASK-020 adds `ETFShellCore` as the C++ boundary between future Shell UI and
  `ETFDiagnostics`.
- `ShellDiagnosticFacade` loads reports through Diagnostics APIs and converts
  them into `ShellDiagnosticViewModel` rows suitable for UI binding.
- Shell UI must not directly access SQLite and must not parse raw Watchdog JSON.
- TASK-020 does not add QML, start services, connect sockets, or implement
  strategy/trading/business logic.
- TASK-021 keeps diagnostics filtering, sorting, aggregation, and report file
  refresh detection in the C++ ShellCore layer.
- Future QML should bind the processed `ShellDiagnosticViewModel`; it should not
  duplicate sorting/filtering logic by parsing Watchdog JSON directly.
- TASK-022 adds `ShellDiagnosticPresenter` between future UI and
  `ShellDiagnosticFacade`.
- The presenter owns current filter, sort, ViewModel, aggregate, refresh state,
  and error state; it still does not access SQLite, start services, or connect
  sockets.
- Mock report builders are only for UI prototypes and tests, not real service
  state.
- TASK-023 adds Qt Core model adapters on top of the presenter:
  `ShellDiagnosticServiceListModel`, `ShellDiagnosticIssueListModel`,
  `ShellDiagnosticSummaryObject`, and `ShellDiagnosticQtAdapter`.
- Future QML should bind these model / object surfaces instead of parsing JSON.
- The Qt adapter remains ShellCore-only and does not access SQLite, start
  services, connect sockets, or depend on DataAccess / DataServiceApi /
  Transport / ServiceHost.
- TASK-024 introduces the first minimal `ETFDecisionShell` Qt Quick page:
  `DiagnosticsMockPage.qml`.
- The page binds only to `ShellDiagnosticQtAdapter`,
  `ShellDiagnosticServiceListModel`, `ShellDiagnosticIssueListModel`, and
  `ShellDiagnosticSummaryObject`.
- This mock page does not connect to Watchdog / DataService, does not access
  SQLite, and does not start services.
- TASK-025 adds the mock shell frame: top status bar, left navigation, content
  host, right info panel, and bottom log panel.
- Only Diagnostics Center is connected to mock `ShellDiagnosticQtAdapter` data.
  Other routes are placeholder pages and do not implement business behavior.
- Shell QML continues to avoid direct SQLite access and direct DataService /
  Watchdog connections.

## 当前状态

v0.1 草案。

## 后续补充方向

补充进程边界、启动依赖、故障恢复策略和本地服务通信细节。

## 技术路线

- 操作系统：Windows
- 技术底座：Qt / C++
- 界面路线：Qt Quick / QML + C++
- 架构方式：本地受控多进程
- 进程通信：Qt Local Socket + JSON
- 数据库：SQLite + WAL
- 构建系统：CMake
- 项目组织：Monorepo

## 进程划分

### ETFDecisionShell.exe

QML 主界面。只负责展示和交互，不写策略逻辑，不直接写数据库。

### ETFWatchdog.exe

负责启动服务、心跳检测、异常重启、崩溃日志。

### ETFDataService.exe

SQLite 唯一写入口。负责账务写入、数据迁移、备份恢复、Repository / DAO 层、数据库事务。

### ETFMarketService.exe

负责行情采集、多源降级、行情缓存、数据状态标记。

### ETFStrategyService.exe

负责底仓保护、狙击资金池、六档策略、TradeDraft、资金分配计划、场内/场外替代判断。

### ETFAlertService.exe

负责预警去重、桌面通知、PushPlus 推送。

## 服务启动顺序

1. ETFDataService
2. ETFMarketService
3. ETFStrategyService
4. ETFAlertService
5. ETFDecisionShell 连接并展示状态

## 核心架构原则

- QML 只做界面。
- C++ 服务层负责业务。
- DataService 是唯一数据库写入口。
- TradeLog 是唯一事实账本。
- position_snapshot、cash_snapshot、portfolio_summary 都是派生缓存。
- 服务异常恢复后，从 SQLite 重建业务状态。

## Transport 层边界

- Transport 位于各本地服务进程之间，当前实现为 Qt Local Socket + JSON。
- Transport 只负责连接管理、协议帧边界、消息发送和接收。
- Transport 不解释业务 action，不做策略计算，不做账务重演。
- Transport 不访问 SQLite，不依赖 DataAccess，不暴露 Repository。
- ETFDecisionShell、ETFMarketService、ETFStrategyService、ETFAlertService 等后续只能通过协议请求 ETFDataService，不能直接读写 SQLite。
- TASK-006 只提供传输层骨架和 echo 测试，不启动真实服务通信拓扑。

## ServiceRuntime 层边界

- ServiceRuntime 位于 Transport 和具体服务业务模块之间。
- ServiceRuntime 只负责校验结构化 MessageEnvelope、按 action 路由到 handler，并生成 ProtocolResponse。
- 未注册 action 必须返回 `E1004_INVALID_ACTION`。
- envelope 校验失败必须返回对应协议错误码。
- handler 异常必须转换为 `E9000_SERVICE_ERROR`，不能向外层泄漏异常。
- TASK-007 只提供 `system.ping` 和 `system.health` 内置 action，不实现 DataService Repository socket API。
- ServiceRuntime 不访问 SQLite，不依赖 DataAccess，不做策略计算、账务重演或 TradeDraft 生命周期处理。

## ServiceHost 层边界

- ServiceHost 位于 Transport 与 ServiceRuntime 之间。
- ServiceHost 负责接收本地 socket 消息、解析受控 MessageEnvelope JSON、调用 ActionDispatcher，并把 ProtocolResponse JSON 写回传输层。
- TASK-008 只提供 DemoServiceHost，用于端到端验证 `system.ping` 和 `system.health`。
- DemoServiceHost 不访问 SQLite，不依赖 DataAccess，不暴露 Repository，也不启动真实 DataService 业务 API。
- DemoServiceHost 不做策略计算、账务重演、TradeDraft 生命周期处理、行情接入或自动交易。
- 未注册 action 必须返回 `E1004_INVALID_ACTION`；无效 JSON 或无效 envelope 必须返回协议错误且不能导致服务崩溃。

## DataServiceApi 层边界

- DataServiceApi 位于 ETFDataService 进程内部，负责把 DataAccess 只读 Repository 封装成白名单 `data.*` action handler。
- TASK-009 只注册 `data.health`、`data.summary`、`data.accounts.list`、`data.portfolios.list`、`data.instruments.list`、`data.strategies.list` 和 `data.otc.list`。
- DataServiceApi 可以依赖 DataAccess，但 Transport 和 ServiceHost 仍不得依赖 DataAccess。
- 当前所有 `data.*` action 都是只读查询，不允许 INSERT / UPDATE / DELETE，不写 `trade_log`，不写派生快照表。
- 写入 action 后续必须单独设计事务、审计、TradeLog 事实账本规则和验收测试，不能复用只读 handler 绕过边界。

## DataServiceApi 写入白名单

- TASK-011 增加 DataServiceApi 写入 action 白名单，默认所有 `data.*` action 仍视为只读。
- 当前唯一允许写入 action 是 `data.audit.append`，并且只能写 `audit_log`。
- `--serve-readonly` 不注册任何写入 action；`--serve-dev-audit` 才会注册 `data.audit.append`。
- 写入 action 必须显式注册，不允许通配符放开 `data.*` 写入，也不允许根据 action 名动态拼接 SQL。
- ServiceHost 和 Transport 仍不得依赖 DataAccess；只有 ETFDataService 进程内的 DataServiceApi handler 可以通过受控 Repository 写入。
- 本阶段不提供 TradeLog、TradeDraft、成交组、资金池、grid_cycle 或派生快照表的写入 action。

## DataServiceClient 层边界

- TASK-012 新增 DataServiceClient，位于客户端 / 服务调用方侧。
- DataServiceClient 只通过 Transport + Protocol 调用 ETFDataService，不直接访问 SQLite。
- DataServiceClient 不依赖 DataAccess，也不依赖 DataServiceApi；测试可以同时链接服务端组件来搭建端到端环境。
- DataServiceClient 不实现业务判断，不新增服务端 action，不扩大写入白名单。
- `appendAuditDemo` 仅封装现有 `data.audit.append` 请求，不代表业务入账能力。
- 未知 action 必须由服务端返回 `E1004_INVALID_ACTION`；客户端不能把未知 action 自动当成有效请求。

## Watchdog 层边界

- TASK-013 新增 Watchdog 服务进程管理骨架。
- Watchdog 使用 `QProcess` 启动、停止和查询本地服务进程状态。
- Watchdog 通过 DataServiceClient 调用 DataService 的 `system.ping` 和 `data.health` 做健康检查。
- Watchdog 不直接访问 SQLite，不依赖 DataAccess，不读写 Repository。
- Watchdog 不新增服务端 action，不实现策略、行情、交易、账务重演或 TradeDraft 生命周期。
- 第一版不做 Windows Service 安装，不写注册表，不实现后台常驻管理。
- TASK-014 新增 Watchdog 服务清单配置加载和校验。
- Watchdog 通过 `config/services.local.example.json` 这类服务清单描述本地服务进程、启动参数、socket 名和超时。
- 服务清单错误必须显式返回错误，不得静默忽略，也不得在配置不合法时尝试启动服务。
- 当前启用状态只支持 `ETFDataService`；其他未来服务可以作为 `enabled=false` 占位项读取并产生 warning。
- `autoRestart` 当前只是配置字段，不执行自动重启。
- TASK-015 支持 Watchdog 从服务清单启动指定的 `ETFDataService`。
- 配置驱动启动后，Watchdog 仍只通过 `DataServiceClient` 执行 `system.ping` 和 `data.health`。
- 配置驱动命令结束前必须停止子进程；失败路径也必须清理已启动进程。
- 第一版仍不做 Windows Service，也不做自动重启。
- TASK-016 新增服务清单状态报告和 dry-run 启动检查。
- dry-run 只检查配置、路径、enabled 状态、socketName 和健康检查条件，不启动服务、不连接 socket、不访问数据库。
- manifest status 会汇总 total / enabled / disabled / error / warning，并标出每个服务是否 canStart。
- TASK-017 新增 JSON 状态报告导出和批量 dry-run。
- JSON 报告用于 Shell 或诊断工具读取，必须保持可解析和字段稳定。
- `dry-run-all` 只检查配置和启动条件，不启动服务、不连接 socket、不访问数据库。
## Shell Navigation Metadata

- TASK-026 adds `ShellPageRegistry`, `ShellNavigationModel`, and `ShellNavigationController` in ShellCore.
- Shell page keys, titles, categories, QML component names, and placeholder state are centralized in `ShellPageRegistry`.
- QML `SideNavigation` binds `ShellNavigationModel` instead of owning the full navigation list.
- `ContentHost` uses `ShellNavigationController` metadata to load the diagnostics mock page or a placeholder page.
- Shell does not directly access SQLite, connect DataService, start Watchdog, or start DataService.
- The current ETFDecisionShell remains a mock / placeholder UI.

## Shell Page Status Mock Models

- TASK-027 adds ShellCore page status, page info, and mock log models.
- `ShellStatusController` provides `ShellPageStatusModel`, `ShellPageInfoObject`, and `ShellMockLogModel` to QML.
- `TopStatusBar`, `RightInfoPanel`, and `BottomLogPanel` bind these C++ models instead of static text.
- Current status is still mock / placeholder only; Shell does not access SQLite, connect DataService, or start services.

## Shell Mock Metrics

- TASK-028 adds page-level mock metrics and action hints in ShellCore.
- `ShellMockMetricsProvider` returns metrics / hints for the current Shell page key.
- `ShellStatusController` exposes `ShellPageMetricModel` and `ShellPageActionHintModel` for QML binding.
- `DashboardPlaceholderPage` and `RightInfoPanel` display these C++ model rows.
- Shell metrics are mock-only and do not access SQLite, connect services, start processes, or implement business logic.

## Shell Read-Only Data Facade

- TASK-029 adds `ShellReadOnlyDataFacade` in `libs/ShellServices`.
- The facade is the Shell-side C++ boundary for future read-only DataService data.
- It calls DataService through `DataServiceClient`; it does not access SQLite directly.
- It does not depend on DataAccess, DataServiceApi, ServiceHost, or Watchdog.
- It exposes only whitelisted read methods and does not allow arbitrary action forwarding.
- QML does not call this facade yet; future UI integration must pass through ViewModel / adapter layers.

## Shell Read-Only Data ViewModels

- TASK-030 adds ShellServices view models and Qt Core list models above `ShellReadOnlyDataFacade`.
- `ShellReadOnlyDataController` loads read-only health, summary, accounts, portfolios, instruments, and strategies through the facade.
- The controller updates `ShellDataConnectionObject`, `ShellReadOnlyDataViewModel`, and account / portfolio / instrument / strategy list models for future UI binding.
- Shell still does not directly access SQLite and does not depend on DataAccess, DataServiceApi, ServiceHost, or Watchdog.
- QML is not connected to these models in TASK-030; future UI tasks must explicitly authorize real data binding.

## Shell Read-Only Data Page Prototype

- TASK-031 connects one QML prototype page to `ShellReadOnlyDataController`.
- `ReadOnlyDataPage.qml` binds only ShellServices controller / models and does not call `DataServiceClient` directly.
- Shell still does not directly access SQLite; all read-only data flows through DataService socket actions.
- The page is a development preview and exposes no edit, write, trade, audit append, or arbitrary action capability.
- Future business pages must keep this Controller / ViewModel boundary instead of binding directly to protocol clients.

## Shell Read-Only State Flow

- TASK-032 keeps read-only connection, refresh, throttle, and error state in `ShellReadOnlyDataController`.
- QML binds `refreshState`, `isBusy`, `canRefresh`, timestamp fields, counters, and `errorState`; it does not implement throttle logic itself.
- `Refresh All` is rejected while busy and throttled by a minimum interval in C++ before any DataService request is sent.
- Failed refresh attempts preserve previously loaded successful models so the UI does not replace real data with fake or empty data.
- ShellServices still only calls whitelisted read-only DataService actions through `ShellReadOnlyDataFacade`.
- ShellServices and QML still do not access SQLite, call `data.audit.append`, expose arbitrary actions, or implement business logic.

## Shell Read-Only Connection Presets

- TASK-033 adds runtime-only connection presets in ShellServices for the read-only data page.
- `ShellReadOnlyConnectionPresetModel` exposes `readonly_default`, `audit_dev`, and `custom` presets to QML through `ShellReadOnlyDataController`.
- Presets are UI guidance only: they do not start DataService, call Watchdog, connect automatically, or persist configuration.
- The audit development preset only fills a socket name and command hint; it does not call `data.audit.append` or any write action.
- Shell still does not directly access SQLite and QML still does not call `DataServiceClient` directly.

## Account Portfolio Read-Only Page

- TASK-037 upgrades the `account_portfolio` shell route from placeholder to `AccountPortfolioReadOnlyPage`.
- The page binds ShellServices read-only models through `ShellReadOnlyDataController`; QML does not call `DataServiceClient` directly.
- Account and portfolio data still flow through DataService read-only actions and the ShellServices facade / controller boundary.
- The page is explicitly read-only and exposes no edit, deposit, withdrawal, accounting, trade, audit append, or write controls.
- Shell still does not directly access SQLite, DataAccess, DataServiceApi, ServiceHost, or Watchdog for this page.
