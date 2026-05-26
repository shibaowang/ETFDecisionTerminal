# 跨境 ETF 智能投资决策终端

## TASK-018 ETFDiag diagnostic report consumer

`ETFDiag` is a local diagnostic command line tool that consumes the JSON report
produced by Watchdog. It does not start services, connect Local Socket, access
SQLite, or implement UI/business logic.

## TASK-019 Diagnostics report model library

`ETFDiagnostics` is a Qt Core based shared diagnostics model library. It owns
the Watchdog report DTOs, JSON parsing, validation, and summary generation used
by `ETFDiag` and intended for a future Shell diagnostics page.

The library is deliberately passive: it does not start services, connect Local
Socket, access SQLite, depend on DataAccess/DataServiceApi/Transport/ServiceHost,
or implement trading/strategy/UI logic. `ETFDiag` now only parses CLI arguments,
calls `WatchdogReportParser`, prints `DiagnosticSummary.text`, and returns
`DiagnosticSummary.exitCode`.

## TASK-020 Shell diagnostics facade

`ETFShellCore` adds a C++ Shell diagnostics boundary for future QML use. The
`ShellDiagnosticFacade` loads Watchdog report files or JSON strings through
`ETFDiagnostics`, then converts `DiagnosticReport` / `DiagnosticSummary` into
stable UI-facing DTOs:

- `ShellDiagnosticViewModel`
- `ShellDiagnosticServiceRow`
- `ShellDiagnosticIssueRow`

The facade does not expose raw JSON to UI code, does not access SQLite, does
not start services, and does not connect Local Socket. TASK-020 does not add
QML or UI files.

## TASK-021 Shell diagnostics ViewModel tools

`ShellDiagnosticFacade` now provides C++ ViewModel processing helpers for future
diagnostics UI:

- `ShellDiagnosticFilter` filters disabled, enabled, blocked, issue-bearing,
  minimum severity, and search matches.
- `ShellDiagnosticSort` sorts by service name, severity, canStart, enabled, or
  issue count.
- `ShellDiagnosticAggregate` summarizes visible rows.
- `ShellDiagnosticRefreshState` / `refreshIfChanged` support report file change
  detection and reload without starting services.

The implementation still only consumes Diagnostics output. It does not parse
raw Watchdog JSON outside Diagnostics, access SQLite, start services, connect
Local Socket, or implement QML.

## TASK-022 Shell diagnostics presenter

`ShellDiagnosticPresenter` is a C++ presenter boundary for future Shell
diagnostics UI. It owns current filter, sort, ViewModel, aggregate, refresh
state, source path, and error state, while delegating report parsing and
ViewModel generation to `ShellDiagnosticFacade`.

Mock data entry points are available through `ShellDiagnosticMockReportBuilder`
and presenter helpers:

- `loadMockHealthy()`
- `loadMockWithWarnings()`
- `loadMockWithErrors()`
- `loadMockMixed()`

Mock reports are only for tests and future UI prototypes. They are not real
service state. TASK-022 does not add QML, access SQLite, start services, connect
Local Socket, or add business logic.

## TASK-023 Shell diagnostics Qt model adapter

`ShellDiagnosticQtAdapter` adapts the existing presenter ViewModel into Qt Core
binding surfaces for a future Shell diagnostics page. It provides:

- `ShellDiagnosticServiceListModel` for service rows.
- `ShellDiagnosticIssueListModel` for selected service issues.
- `ShellDiagnosticSummaryObject` for summary counters and text.

The adapter can load mock healthy / warning / error / mixed reports, load report
files, apply filter / sort settings through `ShellDiagnosticPresenter`, select a
service, and refresh file-backed reports. It does not add QML, access SQLite,
start services, connect Local Socket, or add business logic.

## TASK-024 Shell QML diagnostics mock page

`ETFDecisionShell` now has a minimal Qt Quick shell that loads a diagnostics mock
page backed only by `ShellDiagnosticQtAdapter`.

Run it:

```powershell
build\apps\ETFDecisionShell\Debug\ETFDecisionShell.exe --diagnostics-mock
```

The page shows mock summary data, a service list, selected service issues, mock
load buttons, and simple filters. It is explicitly demo data: it does not
connect to Watchdog or DataService, does not access SQLite, and does not start
services. TASK-024 requires Qt Core / Gui / Qml / Quick / QuickControls2 from
the configured Qt installation.

## TASK-025 Shell layout and navigation mock

`ETFDecisionShell` now has a mock terminal frame for future pages:

- `TopStatusBar` for application name, page title, and mock service status.
- `SideNavigation` for module routing.
- `ContentHost` for the current page.
- `RightInfoPanel` for mock contextual information.
- `BottomLogPanel` for mock logs / alerts.

Only Diagnostics Center is backed by mock model data. Dashboard, market,
account/portfolio, positions, strategy, TradeDraft, confirmation, TradeLog, cash
plan, OTC channel, alerts, and settings are placeholders. The shell still does
not connect to real services, access SQLite, start processes, or implement
business logic.

Generate a Watchdog report:

```powershell
build\apps\ETFWatchdog\Debug\ETFWatchdog.exe --manifest-status-json --config config\services.local.example.json --output build\watchdog-status.json
```

Consume the report:

```powershell
build\tools\ETFDiag\Debug\ETFDiag.exe --watchdog-report build\watchdog-status.json
```

Exit code rules:

- Invalid JSON, missing required fields, or wrong field types return non-zero.
- `errorCount > 0` returns non-zero.
- Any enabled service with `canStart=false` returns non-zero.
- Warnings alone return 0 and are printed in the summary.

The validated report fields include top-level `version`, service counts,
`errorCount`, `warningCount`, and `services`; each service must include stable
string, boolean, and `issues` array fields from the Watchdog status report.

Windows 本地桌面端投资决策辅助终端。第一阶段不做自动下单，不接券商真实交易接口；用户在外部平台手工下单，软件负责行情、策略建议、资金池、TradeDraft、成交确认、TradeLog、审计和复盘等辅助能力。

## TASK-001 范围

TASK-001 完成了初始工程骨架：

- Monorepo 目录结构。
- CMake 顶层工程、apps、libs、tests 基础目标。
- 最小可编译服务入口：`ETFDecisionShell`、`ETFWatchdog`、`ETFDataService`、`ETFMarketService`、`ETFStrategyService`、`ETFAlertService`。
- 公共库骨架：`CoreDomain`、`Protocol`、`DataAccess`、`StrategyEngine`、`MarketEngine`、`Quantization`、`Security`。
- 数据库迁移脚本 `migrations/001_initial_schema.sql`。
- CTest 数据库初始化验证，使用 Python 标准库 `sqlite3` 执行迁移并验证核心表。
- docs 文档骨架、`.gitignore`、`.clang-format`。

## TASK-002 范围

TASK-002 在 `libs/CoreDomain` 中补充了全项目共享的基础领域类型：

- 定点类型：`MoneyCents`、`Price1e6`、`Quantity1e6`、`RatioPpm`。
- ID / 代码类型：`Uuid`、`AccountId`、`PortfolioId`、`StrategyCode`、`InstrumentCode`。
- 数据库枚举对应的 C++ enum class 及 `toString` / `fromString`。
- CoreDomain 单元测试 `coredomain_types`。

## TASK-003 范围

TASK-003 在 `libs/Protocol` 中补充了本地服务通信协议基础类型：

- 服务名：`ServiceName`。
- 错误码：`ErrorCode`、`numericCode`、`errorCategory`、`defaultSeverity`。
- 请求信封：`MessageEnvelope`。
- 响应信封：`ProtocolResponse`。
- 最小 JSON 字符串转义和 payload 片段校验。
- Protocol 单元测试 `protocol_messages`。

## TASK-004 范围

TASK-004 在 `libs/DataAccess` 和 `apps/ETFDataService` 中补充了最小数据库基础设施：

- SQLite C API 连接管理：`SQLiteConnection`。
- 数据库配置、状态、错误类型：`DatabaseConfig`、`DatabaseStatus`、`DatabaseError`。
- 初始迁移检查与执行：`MigrationRunner`。
- WAL、foreign_keys、busy_timeout 初始化。
- 只读健康检查。
- `ETFDataService` 命令行数据库初始化和健康检查入口。
- DataAccess / DataService CLI 自动化测试。

## TASK-005 范围

TASK-005 在 `libs/DataAccess` 和 `apps/ETFDataService` 中补充了 DataService 内部只读查询边界：

- 只读 Repository 基础类型、DTO 和 `ReadOnlyRepositoryBase`。
- 账户、组合、标的、策略、OTC 通道、`schema_version` 的基础只读查询。
- `ETFDataService` 只读 CLI 查询入口。
- DataAccess Repository 和 DataService 只读 CLI 自动化测试。

这些 Repository 只允许在 ETFDataService 进程内部使用。其他服务未来只能通过 Protocol 请求 DataService，不能直接读 SQLite，也不能直接链接 Repository 绕过服务边界。

## TASK-006 范围

TASK-006 新增 `libs/Transport`，建立 Qt Local Socket 传输层骨架：

- `FrameCodec`：4 字节大端长度前缀 + UTF-8 JSON payload 的协议帧编码 / 解码。
- `LocalSocketServer`：基于 `QLocalServer` 的本地 socket 服务端封装。
- `LocalSocketClient`：基于 `QLocalSocket` 的本地 socket 客户端封装。
- Transport 测试：协议帧半包、粘包、超限、非 JSON-looking payload，以及 Local Socket echo。

Transport 只负责本地进程通信和消息收发，不解释业务 action，不访问 SQLite，不暴露 DataService Repository，不实现策略、行情、TradeDraft 或账务逻辑。

## TASK-007 范围

TASK-007 新增 `libs/ServiceRuntime`，建立服务端 action 分发骨架：

- `ActionContext`：封装结构化 `MessageEnvelope`、服务名、接收时间、traceId 和可选元数据。
- `ActionDispatcher`：支持注册、注销、查询和分发 action。
- `BuiltinActions`：提供 `system.ping` 和 `system.health` 内置响应。
- ServiceRuntime 测试：注册/注销、未知 action、envelope 校验失败、handler 异常、非法 handler 响应、Ping/Health。

ServiceRuntime 只负责路由和协议错误响应，不访问 SQLite，不依赖 DataAccess，不实现 DataService Repository socket API，也不实现任何业务 action。

## TASK-008 范围

TASK-008 新增 `libs/ServiceHost`，把 Transport 和 ServiceRuntime 串接成最小 demo 服务宿主：

- `DemoServiceHost`：基于 `LocalSocketServer` 接收本地 socket JSON 消息。
- 在 ServiceHost 内部做受控 MessageEnvelope JSON 解析，保留 payload 原始 JSON 片段。
- 将解析后的 MessageEnvelope 交给 `ActionDispatcher`。
- 返回 ProtocolResponse JSON。
- 端到端支持 `system.ping`、`system.health`、未知 action 和 invalid JSON 错误响应。

DemoServiceHost 不访问 SQLite，不依赖 DataAccess，不暴露 DataService Repository，不实现真实 DataService 业务 API。

## TASK-009 范围

TASK-009 新增 `libs/DataServiceApi`，在 ETFDataService 内部接入只读 `data.*` action：

- `data.health`
- `data.summary`
- `data.accounts.list`
- `data.portfolios.list`
- `data.instruments.list`
- `data.strategies.list`
- `data.otc.list`

这些 action 全部通过白名单注册，只调用 DataAccess 只读 Repository 或 `SQLiteConnection::healthCheck`。当前不提供任何写入 action，不写 `trade_log`，不写派生快照表，不实现成交确认、账务重演、策略计算或 TradeDraft 生命周期。

## TASK-010 范围

TASK-010 在 `libs/DataAccess` 中建立写入事务基础设施和审计写入基础：

- `Transaction`：RAII 事务边界，未 commit 时析构自动 rollback。
- `TransactionRunner`：回调成功时 commit，失败或异常时 rollback。
- `AuditLogRepository`：只允许写 `audit_log`，并支持最近审计记录查询。
- `SQLiteConnection`：新增受控参数化 statement helper、事务 begin / commit / rollback。

本任务只写 `audit_log`。不写 `trade_log`，不写 `trade_execution_group`，不写 TradeDraft、grid_cycle、资金池或派生快照表，也不开放 socket 写 action。

## TASK-011 范围

TASK-011 在 `libs/DataServiceApi` 和 `apps/ETFDataService` 中增加第一个受控写入 action：

- `WriteActionPolicy` 明确列出写入白名单，当前唯一允许写入 action 是 `data.audit.append`。
- `data.audit.append` 只写 `audit_log`，必须走 `TransactionRunner` 和 `AuditLogRepository`。
- `ETFDataService --serve-dev-audit` 会注册只读 `data.*` action 和 `data.audit.append`，用于开发期验证审计写入链路。
- `ETFDataService --serve-readonly` 仍然保持只读，不接受 `data.audit.append`。
- 新增 `dataservice_audit_write_socket_actions` 端到端测试，验证成功写入、缺字段失败不写入、未知写入 action 返回 `E1004_INVALID_ACTION`，并确认业务账务表和派生快照表未被写入。

本任务仍不写 `trade_log`、`trade_execution_group`、`trade_draft`、`position_snapshot`、`cash_snapshot` 或 `portfolio_summary`，不实现成交确认、手工入账、账务重演、策略计算或自动交易。

## TASK-012 范围

TASK-012 新增 `libs/DataServiceClient`，为 Shell、Watchdog、StrategyService 等未来调用方提供统一 DataService 客户端封装：

- `DataServiceClient` 基于 `Transport::LocalSocketClient` 发送 `MessageEnvelope` 并接收 `ProtocolResponse`。
- 支持 `connect`、`disconnect`、`sendRaw`、`ping`、`health`、`summary`、`listAccounts`、`listPortfolios`、`listInstruments`、`listStrategies`、`listOtc` 和 `appendAuditDemo`。
- `parseProtocolResponseJson` 只解析本项目受控响应字段，不提供通用 JSON 解析器。
- `appendAuditDemo` 只是调用 TASK-011 已存在的 `data.audit.append`，不新增服务端 action，也不扩大写入白名单。
- 新增 `dataservice_client_end_to_end` 测试，验证客户端调用、错误响应、timeout / 未连接错误和业务表未被写入。

DataServiceClient 不访问 SQLite，不依赖 DataAccess，不依赖 DataServiceApi，不做业务判断，不传入任意 SQL。

## TASK-013 范围

TASK-013 新增 `libs/Watchdog`，建立 Watchdog 服务进程管理骨架：

- `ServiceProcessConfig` 定义本地服务进程配置。
- `ServiceProcessStatus` 记录 running、pid、启动/停止时间、退出码、健康状态和诊断消息。
- `ServiceProcessManager` 使用 `QProcess` 启动、停止、查询本地服务进程。
- Watchdog 通过 `DataServiceClient` 调用 `system.ping` 和 `data.health` 检查 DataService 健康。
- `ETFWatchdog` 增加开发期 CLI：`--demo-start-dataservice`。
- 新增 `watchdog_service_manager` 测试，覆盖服务配置、启动、健康检查、停止、stopAll、不存在 exe 和错误 socket 健康检查。

Watchdog 当前只是开发期进程管理骨架，不做 Windows Service，不访问 SQLite，不依赖 DataAccess，不实现策略、交易或任何业务写入。

## TASK-014 范围

TASK-014 在 `libs/Watchdog` 中新增服务清单配置加载和校验能力：

- `ServiceManifest` 描述清单版本和服务配置列表。
- `ServiceManifestLoader` 支持从 JSON 字符串或文件加载配置。
- `ServiceManifestValidationResult` 返回校验错误和警告。
- `config/services.local.example.json` 提供本地服务清单示例。
- `ETFWatchdog` 新增 `--check-config` 和 `--list-services`。

服务清单当前只支持启用状态的 `ETFDataService`。其他服务可以作为 `enabled=false` 的未来占位项读取，并产生 warning。`autoRestart` 目前只是配置字段，不启用自动重启。Watchdog 仍不访问 SQLite，不依赖 DataAccess，不实现 Windows Service、不写数据库、不实现任何业务交易逻辑。

## TASK-015 范围

TASK-015 在 TASK-014 服务清单基础上增加配置驱动启动能力：

- `ETFWatchdog --show-config-status` 展示清单中每个服务的配置状态，不启动服务。
- `ETFWatchdog --start-service-from-config` 从清单查找指定服务并启动。
- 第一版仅支持启动 `ETFDataService`。
- 启动后通过 `DataServiceClient` 执行 `system.ping` 和 `data.health`。
- 命令结束前会停止服务并清理子进程。

本任务仍不启用 `autoRestart`，不实现 Windows Service，不访问 SQLite，不依赖 DataAccess，不实现任何业务交易逻辑。

## TASK-016 范围

TASK-016 增加服务清单状态汇总和 dry-run 启动检查：

- `ETFWatchdog --manifest-status` 基于服务清单生成状态报告。
- `ETFWatchdog --dry-run-start` 检查指定服务是否具备启动条件。
- dry-run 检查 executablePath、workingDirectory、enabled 状态、socketName 和健康检查条件。
- dry-run 不启动服务，不连接 socket，不访问数据库。
- `autoRestart` 仍只报告为被忽略，不启用自动重启。

## TASK-017 范围

TASK-017 增加 Watchdog 状态报告 JSON 导出和批量 dry-run 检查：

- `ETFWatchdog --manifest-status-json` 输出可解析的 JSON 状态报告。
- `--manifest-status-json --output <path>` 将 JSON 状态报告写入文件。
- `ETFWatchdog --dry-run-all` 对清单内全部服务执行 dry-run 检查。
- JSON 报告包含 `configPath`、`version`、服务计数、error / warning 计数和 `services[]`。
- `services[]` 包含 `serviceName`、`enabled`、`supported`、`executablePath`、`executableExists`、`workingDirectory`、`workingDirectoryExists`、`socketName`、`socketNamePresent`、`autoRestart`、`autoRestartEnabledButIgnored`、`healthCheckSupported`、`canStart` 和 `issues[]`。

`--dry-run-all` 不启动服务，不连接 socket，不访问数据库。`autoRestart` 仍只报告为被忽略，不启用自动重启。

## 构建

环境要求：

- CMake 3.20 或更高版本。
- 支持 C++17 的编译器。
- Python 3，用于当前临时数据库迁移测试。
- SQLite3 开发文件，用于 DataAccess SQLite C API。
- Qt 6 Core + Network，用于 Transport Qt Local Socket。

SQLite3 安装说明：

- Windows / vcpkg：`vcpkg install sqlite3:x64-windows`，并用 vcpkg toolchain 配置 CMake。
- 也可以配置 `-DETFDT_SQLITE3_AMALGAMATION_DIR=<dir>`，目录内需要有 `sqlite3.c` 和 `sqlite3.h`。
- 如果 CMake 找不到 SQLite3 或可用 amalgamation，会在配置阶段给出明确错误。

Qt6 安装说明：

- Windows / aqtinstall 示例：
  `python -m pip install --user aqtinstall`
  `python -m aqt install-qt windows desktop 6.6.3 win64_msvc2019_64 -O D:\Qt --archives qtbase`
- 配置方式可以使用 `-DETFDT_QT6_ROOT=D:\Qt\6.6.3\msvc2019_64`，或把 Qt prefix 加入 `CMAKE_PREFIX_PATH`。
- 如果 CMake 找不到 Qt6 Core / Network，会在 `libs/Transport` 配置阶段给出明确错误。

```powershell
cmake -S . -B build
cmake --build build
```

## 初始化数据库

```powershell
build\apps\ETFDataService\ETFDataService.exe --init-db --db data\ETFDecision.db --migration migrations\001_initial_schema.sql
```

该命令会打开数据库、初始化 PRAGMA、启用 WAL、执行初始迁移，并输出健康检查摘要。已执行初始迁移时会跳过，不重复报错。

## 检查数据库

```powershell
build\apps\ETFDataService\ETFDataService.exe --check-db --db data\ETFDecision.db
```

该命令只做健康检查，不执行账务重演，不自动修账。

## 只读查询

```powershell
build\apps\ETFDataService\ETFDataService.exe --db-summary --db data\ETFDecision.db
build\apps\ETFDataService\ETFDataService.exe --list-accounts --db data\ETFDecision.db
build\apps\ETFDataService\ETFDataService.exe --list-portfolios --db data\ETFDecision.db
build\apps\ETFDataService\ETFDataService.exe --list-instruments --db data\ETFDecision.db
build\apps\ETFDataService\ETFDataService.exe --list-strategies --db data\ETFDecision.db
build\apps\ETFDataService\ETFDataService.exe --list-otc --db data\ETFDecision.db --strategy-code 159509
```

这些命令每次都会先打开数据库并执行健康检查，然后通过 DataService 内部只读 Repository 查询。当前不会写入任何业务表，不会写 `trade_log`，也不会生成或确认 TradeDraft。

## 只读 Local Socket 服务

```powershell
build\apps\ETFDataService\ETFDataService.exe --serve-readonly --db data\ETFDecision.db --socket-name ETFDataServiceReadonly
```

该命令会打开已有数据库、初始化 PRAGMA、启用 WAL、执行健康检查，然后启动开发预览版只读 Local Socket 服务。它不会自动执行 migration，不会写入业务表，只处理 `system.ping`、`system.health` 和白名单 `data.*` 只读 action。

## 开发期审计写入 Local Socket 服务

```powershell
build\apps\ETFDataService\ETFDataService.exe --serve-dev-audit --db data\ETFDecision.db --socket-name ETFDataServiceAuditDev
```

该命令会打开已有数据库、初始化 PRAGMA、启用 WAL、执行健康检查，然后启动开发期审计写入服务。它会注册只读 `data.*` action，并额外注册唯一写入 action `data.audit.append`。该 action 只写 `audit_log`，不是业务交易入账，不写 `trade_log`、TradeDraft、成交组或派生快照表。

## Watchdog 开发期 DataService Demo

```powershell
build\apps\ETFWatchdog\ETFWatchdog.exe --demo-start-dataservice --dataservice-exe build\apps\ETFDataService\Debug\ETFDataService.exe --db data\ETFDecision.db --socket-name ETFDataServiceWatchdogDemo
```

该命令使用 `QProcess` 启动 `ETFDataService --serve-readonly`，通过 `DataServiceClient` 执行 `system.ping` 和 `data.health`，输出状态后停止子进程。它不是 Windows Service，不写注册表，不访问 SQLite，也不实现业务交易逻辑。

## Watchdog 服务清单配置

示例配置位于 `config/services.local.example.json`。配置字段包括：

- `version`：清单版本，必填。
- `services`：服务数组，必填且至少包含一个服务。
- `serviceName`：服务名，当前启用状态只支持 `ETFDataService`。
- `executablePath`：服务可执行文件路径，必填。
- `workingDirectory`：工作目录，可选。
- `arguments`：启动参数数组，可选。
- `socketName`：DataService 健康检查 socket 名，启用的 `ETFDataService` 必填。
- `startupTimeoutMs` / `shutdownTimeoutMs` / `healthTimeoutMs`：超时毫秒数，缺省使用合理默认值，不能为负数。
- `enabled`：缺省为 `true`。
- `autoRestart`：缺省为 `false`，当前只保留字段，不启用自动重启。

检查配置：

```powershell
build\apps\ETFWatchdog\ETFWatchdog.exe --check-config --config config\services.local.example.json
```

列出服务：

```powershell
build\apps\ETFWatchdog\ETFWatchdog.exe --list-services --config config\services.local.example.json
```

配置错误会返回非 0，不会尝试启动服务。`enabled=false` 的服务可被读取，但不会被启动。

展示配置状态：

```powershell
build\apps\ETFWatchdog\ETFWatchdog.exe --show-config-status --config config\services.local.example.json
```

按配置启动指定服务并做健康检查：

```powershell
build\apps\ETFWatchdog\ETFWatchdog.exe --start-service-from-config --config config\services.local.example.json --service ETFDataService
```

`--demo-start-dataservice` 是手工传参 demo；`--start-service-from-config` 是配置驱动 demo。两者都会在命令结束前停止子进程。当前仅支持 `ETFDataService`，不会启动 `enabled=false` 服务，不启用 `autoRestart`。

生成服务清单状态报告：

```powershell
build\apps\ETFWatchdog\ETFWatchdog.exe --manifest-status --config config\services.local.example.json
```

Dry-run 启动检查：

```powershell
build\apps\ETFWatchdog\ETFWatchdog.exe --dry-run-start --config config\services.local.example.json --service ETFDataService
```

`--dry-run-start` 只检查配置和启动条件，不启动真实进程，不连接 Local Socket，不访问数据库。存在 ERROR 时返回非 0；只有 INFO / WARNING 时仍可返回 0。

导出 JSON 状态报告：

```powershell
build\apps\ETFWatchdog\ETFWatchdog.exe --manifest-status-json --config config\services.local.example.json
build\apps\ETFWatchdog\ETFWatchdog.exe --manifest-status-json --config config\services.local.example.json --output build\watchdog-status.json
```

批量 dry-run：

```powershell
build\apps\ETFWatchdog\ETFWatchdog.exe --dry-run-all --config config\services.local.example.json
```

`--dry-run-all` 只检查配置和启动条件，不启动真实进程，不连接 Local Socket，不访问数据库。任一 enabled 服务存在 ERROR 或 `canStart=false` 时返回非 0；disabled 服务不会导致失败，但会输出 disabled / skipped 信息。

## 运行测试

```powershell
ctest --test-dir build --output-on-failure
```

当前测试 `database_initial_schema` 会：

- 创建临时 SQLite 数据库。
- 执行 `migrations/001_initial_schema.sql`。
- 验证核心表存在。
- 验证 `schema_version` 已记录初始迁移。

当前测试 `coredomain_types` 会验证定点类型解析/格式化、基础 ID 校验和枚举双向转换。

当前测试 `protocol_messages` 会验证服务名、错误码、请求/响应信封校验和 JSON 字符串生成。

当前测试 `dataaccess_database_init` 会验证 SQLite 打开、初始迁移、WAL、foreign_keys、schema_version 和核心表。

当前测试 `dataservice_cli_init` / `dataservice_cli_check` 会验证 `ETFDataService` 命令行入口。

当前测试 `dataaccess_readonly_repositories` 会验证账户、组合、CASH 标的、schema_version 和只读 Repository 边界。

当前测试 `dataservice_cli_summary`、`dataservice_cli_list_accounts`、`dataservice_cli_list_portfolios`、`dataservice_cli_list_instruments`、`dataservice_cli_list_strategies`、`dataservice_cli_list_otc_missing` 会验证只读 CLI 查询路径。

当前测试 `transport_frame_codec` 会验证长度前缀协议帧的编码、半包、粘包、超限和 payload 基础校验。

当前测试 `transport_local_socket_echo` 会验证 `LocalSocketServer` / `LocalSocketClient` 能发送和回显 Protocol JSON 字符串。

当前测试 `service_runtime_dispatcher` 会验证 action 分发骨架、协议错误响应和 `system.ping` / `system.health` 内置 action。

当前测试 `service_host_demo_end_to_end` 会启动 DemoServiceHost，通过 LocalSocketClient 发送 `system.ping`、`system.health`、未知 action 和 invalid JSON，并验证 ProtocolResponse。

当前测试 `dataservice_readonly_socket_actions` 会初始化临时数据库，启动只读 DataService action host，通过 LocalSocketClient 验证 `data.health`、`data.summary`、列表查询、`data.otc.list` 缺字段错误、未知 action 和 invalid JSON。

当前测试 `dataaccess_transaction_audit` 会验证 Transaction commit / rollback、TransactionRunner 成功 / 失败路径、AuditLogRepository 输入校验、最近审计查询，并确认 `trade_log` 和派生快照表未被写入。

当前测试 `dataservice_audit_write_socket_actions` 会启动开发期审计写入 action host，通过 LocalSocketClient 验证 `data.audit.append` 成功写入 `audit_log`、缺少 `reason` 或 `entityType` 时返回 `E1002_MISSING_REQUIRED_FIELD` 且不写入、未知写入 action 返回 `E1004_INVALID_ACTION`，并确认 `trade_log`、`trade_execution_group`、`trade_draft`、`position_snapshot`、`cash_snapshot` 和 `portfolio_summary` 仍为空。

当前测试 `dataservice_client_end_to_end` 会启动开发期审计写入 action host，通过 `DataServiceClient` 验证 ping、health、summary、账户/组合/标的/策略/OTC 查询、`appendAuditDemo`、缺字段错误、未知 action、连接不存在 socket 和断开后发送错误，并确认 `trade_log`、`trade_execution_group`、`trade_draft`、`position_snapshot`、`cash_snapshot` 和 `portfolio_summary` 仍为空。

当前测试 `watchdog_service_manager` 会初始化临时数据库，使用 Watchdog 启动 `ETFDataService --serve-readonly`，通过 `DataServiceClient` 检查 ping / data.health，验证 `stopService` / `stopAll` 不遗留进程，并覆盖不存在 exe 和错误 socket 的受控失败。

当前测试 `watchdog_service_manifest` 会验证服务清单 JSON / 文件加载、缺字段、重复服务名、负 timeout、`enabled=false` 占位服务、arguments 转换，以及 `ETFWatchdog --check-config` / `--list-services` CLI。

当前测试 `watchdog_service_manifest` 还会初始化临时数据库，通过 `ETFWatchdog --start-service-from-config` 启动 `ETFDataService --serve-readonly`，验证 ping / data.health 成功、服务结束后 socket 关闭且无包含测试 socketName 的 `ETFDataService` 残留进程，并覆盖 missing service、disabled service、unsupported service、missing executable 和 invalid config。

当前测试 `watchdog_service_manifest` 还会验证 `--manifest-status` 和 `--dry-run-start`：有效配置、缺失 executablePath、缺失 workingDirectory、`autoRestart=true` warning、disabled 服务、缺失 socketName、非 `ETFDataService` 服务和 dry-run 不启动进程。

当前测试 `watchdog_service_manifest` 还会验证 JSON 状态报告、stdout JSON、`--output` 文件写入、无效输出路径、含 ERROR 配置返回非 0、`--dry-run-all` 有效 / 无效 / disabled / autoRestart warning 路径，以及 dry-run-all 不启动进程。

Transport 稳定性重复验证命令：

```powershell
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## TASK-026 Shell Navigation Metadata

- `ShellPageRegistry` now owns the shell page keys, titles, QML component names, categories, and placeholder flags.
- `ShellNavigationModel` exposes the navigation list to QML as a `QAbstractListModel`.
- `ShellNavigationController` owns `currentPageKey`, title, description, component name, and selection methods.
- `SideNavigation.qml` binds the C++ model instead of maintaining a full hardcoded navigation list.
- `ContentHost.qml` uses controller metadata to load the diagnostics mock page or placeholder content.
- The UI remains mock-only: it does not access SQLite, connect sockets, start services, or implement business logic.

Run the Shell mock:

```powershell
build\apps\ETFDecisionShell\Debug\ETFDecisionShell.exe --diagnostics-mock
```

## TASK-027 Shell Page Status Model

- `ShellPageStatusModel` exposes mock status for every registered Shell page.
- `ShellPageInfoObject` exposes the currently selected page status to QML.
- `ShellMockLogModel` provides bottom-panel mock log rows; it does not read or write `system_log`.
- `ShellStatusController` owns page status, current page info, and mock logs.
- `TopStatusBar.qml`, `RightInfoPanel.qml`, and `BottomLogPanel.qml` now bind C++ ShellCore mock models.
- The UI still does not access SQLite, connect sockets, start services, or implement business logic.

## TASK-028 Shell Page Mock Metrics

- `ShellPageMetricModel` exposes page-level mock metric rows to QML.
- `ShellPageActionHintModel` exposes non-executable mock action hints to QML.
- `ShellMockMetricsProvider` supplies mock metrics and hints by Shell page key.
- The dashboard placeholder and right-side information panel now bind ShellCore metrics / hints.
- All TASK-028 metrics are mock / placeholder data; no real service, socket, or SQLite access is performed.

Run the Shell mock:

```powershell
build\apps\ETFDecisionShell\Debug\ETFDecisionShell.exe --diagnostics-mock
```

## TASK-029 Shell Read-Only Data Facade

- `libs/ShellServices` adds `ShellReadOnlyDataFacade` as the future Shell-side C++ boundary for read-only DataService calls.
- The facade uses `DataServiceClient` and exposes only whitelisted read methods: ping, health, summary, accounts, portfolios, instruments, strategies, OTC list, and snapshot loading.
- It does not expose arbitrary action forwarding and does not call `data.audit.append` or any write action.
- It does not access SQLite directly and does not depend on DataAccess, DataServiceApi, ServiceHost, or Watchdog.
- TASK-029 does not connect QML to real data; QML remains mock-only.

## TASK-030 Shell Read-Only Data ViewModels

- `libs/ShellServices` now adds read-only Shell view models and Qt Core list models on top of `ShellReadOnlyDataFacade`.
- `ShellReadOnlyDataController` owns the facade, connection object, summary view model, and account / portfolio / instrument / strategy list models.
- The controller only calls whitelisted read methods and does not expose raw protocol action forwarding.
- It does not call `data.audit.append` or any write action, and it does not access SQLite directly.
- TASK-030 does not connect QML to real data; future QML integration must bind these models through an explicitly authorized task.

## TASK-031 Shell Read-Only Data Page Prototype

- ETFDecisionShell now includes a development-only `ReadOnlyDataPage.qml` under the `readonly_data` navigation key.
- The page binds `ShellReadOnlyDataController` plus account / portfolio / instrument / strategy list models; QML does not call `DataServiceClient` directly.
- The page can connect to a configured local socket, refresh all read-only lists, and disconnect.
- It is read-only: no `data.audit.append`, no write action, no SQLite access from QML, and no business table writes.

Start a read-only DataService for manual testing:

```powershell
build\apps\ETFDataService\Debug\ETFDataService.exe --serve-readonly --db data\ETFDecision.db --socket-name ETFDataServiceReadonly
```

Run the Shell:

```powershell
build\apps\ETFDecisionShell\Debug\ETFDecisionShell.exe --diagnostics-mock
```

## TASK-032 Shell Read-Only State Flow

- `ShellReadOnlyDataController` now exposes connection / refresh / error state for the read-only data page.
- `refreshState` uses stable values: `IDLE`, `CONNECTING`, `REFRESHING`, `SUCCESS`, and `FAILED`.
- `Refresh All` has C++-side duplicate-request and throttle guards; QML only binds `canRefresh`, `isBusy`, and state fields.
- The controller tracks last refresh start / finish / success times plus attempt, success, failure, and throttle counts.
- Failed refreshes keep previously loaded successful list rows; they do not silently replace real data with mock data.
- QML still does not call `DataServiceClient` directly and does not call `data.audit.append` or any write action.

## TASK-033 Shell Read-Only Connection Presets

- `ShellReadOnlyConnectionPresetModel` provides runtime-only connection presets for the read-only data page.
- Built-in presets are `readonly_default`, `audit_dev`, and `custom`.
- `readonly_default` points to `ETFDataServiceReadonly` and shows the `--serve-readonly` command hint.
- `audit_dev` points to `ETFDataServiceAuditDev` as a development socket preset only; it does not call write actions.
- `custom` allows a manual runtime socket name and is not persisted.
- The Shell still does not start DataService, call Watchdog, write config files, access SQLite, or call `data.audit.append`.

Start a read-only DataService manually:

```powershell
build\apps\ETFDataService\Debug\ETFDataService.exe --serve-readonly --db data\ETFDecision.db --socket-name ETFDataServiceReadonly
```

Run the Shell:

```powershell
build\apps\ETFDecisionShell\Debug\ETFDecisionShell.exe --diagnostics-mock
```

## TASK-034 Read-Only Shell Demo Acceptance

- `tools/dev/run_readonly_demo.ps1` starts a developer-only manual acceptance flow for the Shell read-only page.
- The script initializes `runtime/dev/readonly_demo/ETFDecision.db` with the existing migration, checks the database, starts `ETFDataService --serve-readonly`, and opens `ETFDecisionShell --diagnostics-mock`.
- It does not start the audit development service, does not call write actions, does not write trading tables, and does not persist user configuration.
- Manual acceptance steps and expected results are documented in `docs/13_manual_acceptance_readonly_shell.md`.
- The page under test remains read-only: no trade entry, no accounting entry, no TradeLog write flow, and no real account integration.

Run the manual demo:

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1
```

Run the script without launching Shell:

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1 -NoShell
```

## TASK-035 Read-Only Demo Script Hardening

- `runtime/` is a developer runtime output directory and is ignored by Git together with demo databases, SQLite WAL/SHM files, logs, and pid files.
- `tools/dev/run_readonly_demo.ps1` records the read-only DataService process id under `runtime/dev/readonly_demo/ETFDataServiceReadonly.pid` and stops only the process it started by default.
- `-ForceRecreateDb` removes the old demo database, WAL, and SHM files before initialization.
- `-KeepDataService` leaves the read-only DataService process running for manual debugging.
- `-NoShell` starts only the read-only DataService host.
- `tools/dev/stop_readonly_demo.ps1` stops the pid-file process; it stops all `ETFDataService` processes only when `-ForceAll` is explicitly passed.

Common commands:

```powershell
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1 -ForceRecreateDb
powershell -ExecutionPolicy Bypass -File tools/dev/run_readonly_demo.ps1 -NoShell
powershell -ExecutionPolicy Bypass -File tools/dev/stop_readonly_demo.ps1
```

## TASK-037 Account Portfolio Read-Only Page

- `account_portfolio` now routes to `AccountPortfolioReadOnlyPage.qml` instead of a generic placeholder.
- The page binds `ShellReadOnlyDataController`, `ShellAccountListModel`, and `ShellPortfolioListModel`.
- It displays account name, account type, broker, base currency, active state, initial cash, portfolio name, base position ratio, and active state.
- The page is read-only: it provides no edit, deposit, withdrawal, accounting, confirmation, or trade buttons.
- QML still does not call `DataServiceClient` directly, access SQLite, call `data.audit.append`, or invoke any write action.

## TASK-038 Instrument Strategy Read-Only Page

- `strategy` now routes to `InstrumentStrategyReadOnlyPage.qml` and is labeled as a read-only prototype for instruments, strategies, and OTC channels.
- The page binds `ShellInstrumentListModel`, `ShellStrategyListModel`, and `ShellOtcChannelListModel` through `ShellReadOnlyDataController`.
- It displays instrument code, name, type, market, currency, enabled state, strategy code / name / enabled state, and OTC channel fields.
- The page can refresh instruments / strategies and OTC channels through explicit read-only controller methods.
- It does not support editing, deleting, strategy execution, TradeDraft generation, trading, accounting, `data.audit.append`, or any write action.

## TASK-039 Read-Only Table Components

- ETFDecisionShell now has reusable QML read-only display components under `apps/ETFDecisionShell/qml/components/readonly`.
- Account / portfolio / instrument / strategy / OTC lists use a shared table shell, section header, empty state, field label, and status badge styling.
- The reusable components improve table headers, row spacing, empty states, status badges, and visible readonly boundaries.
- These components are display-only: they do not connect sockets, access SQLite, call `DataServiceClient`, or expose edit / delete / accounting / trade / strategy execution controls.

## TASK-040 Read-Only Page Filter / Sort

- Account / portfolio / instrument / strategy / OTC lists now bind filtered proxy models exposed by `ShellReadOnlyDataController`.
- `ShellReadOnlyTableProxyModel` provides display-only search, active / enabled filtering, and role-based sorting without calling DataService or writing SQLite.
- `ReadOnlyTable` now supports sortable headers and stable column widths; `ReadOnlyFilterBar` provides search and status-filter input.
- Account / portfolio and instrument / strategy pages remain read-only and still expose no edit, delete, accounting, trade, strategy execution, or TradeDraft controls.

## TASK-041 Read-Only Column Visibility / Density

- Read-only business tables now support runtime column visibility controls and `compact` / `normal` / `comfortable` density settings.
- Required identity columns, such as account name, instrument code, strategy code, and OTC actual code, remain visible and cannot be hidden.
- These settings are display-only runtime state: they are not persisted to files, SQLite, registry, or service configuration.
- Account / portfolio and instrument / strategy pages remain read-only and still expose no edit, delete, accounting, trade, strategy execution, or TradeDraft controls.
- Column visibility and density changes do not call DataService, do not write SQLite, and do not mutate source models.

## TASK-042 Read-Only Business Pages Acceptance

- v0.2 ReadOnly Business Pages acceptance now has a manual checklist for account / portfolio, instrument / strategy, and read-only data preview pages.
- Current read-only business pages include:
  - Account and portfolio read-only page.
  - Instrument and strategy read-only page.
  - Filtering / sorting / column visibility / density switching.
- The acceptance checklist is documented in [Manual Acceptance - ReadOnly Business Pages](docs/16_manual_acceptance_readonly_business_pages.md).
- UI readability rules are documented in [UI Readability Checklist](docs/17_ui_readability_checklist.md).
- These pages remain read-only and still do not support writes, trading, accounting entry, strategy execution, TradeDraft generation, or direct QML access to `DataServiceClient` / SQLite.

## TASK-043 v0.2 ReadOnly Business Pages Milestone

- Current milestone: `v0.2 ReadOnly Business Pages`.
- v0.2 closes the first group of formal read-only business page prototypes: account / portfolio and instrument / strategy.
- The pages support read-only Connect / Refresh, search, filtering, sorting, column visibility, density switching, empty states, and error states.
- The pages remain read-only and do not support writes, trading, accounting entry, strategy execution, TradeDraft generation, direct QML SQLite access, or direct QML `DataServiceClient` usage.
- v0.2 milestone details: [Milestone - v0.2 ReadOnly Business Pages](docs/18_milestone_v0_2_readonly_business_pages.md).
- v0.2 release notes: [Release Notes - v0.2 ReadOnly Business Pages](docs/release_notes/v0_2_readonly_business_pages.md).
- v0.2 summary: [ReadOnly Business Pages Summary](docs/19_readonly_business_pages_summary.md).

## TASK-044 v0.3 Position Accounting Boundary Docs

- v0.3 planning begins with a documentation-only boundary for read-only position and accounting-derived data.
- The position page is not implemented yet.
- `trade_log` remains the fact ledger; `cash_snapshot`, `position_snapshot`, and `portfolio_summary` are derived data only.
- Future position, cash, PnL, base position, and sniper-pool pages must use ViewModel boundaries and must not calculate accounting values in QML.
- v0.3 design boundary: [Position Accounting Boundary](docs/20_position_accounting_boundary.md).
- v0.3 data contract draft: [Position Read-Only Data Contract Draft](docs/21_position_readonly_data_contract_draft.md).

## TASK-045 Position Accounting Data Contract Design

- v0.3 now has an implementation-before-contract document for future read-only position, cash, PnL, base-position, and sniper-pool DTOs.
- The contract defines `DataQualityStatus`, `AccountingErrorCode`, `CalculationMetadataDto`, `AccountingIssueDto`, future read-only action request / response shapes, and ShellServices ViewModel mapping drafts.
- Future accounting replay implementation must first use deterministic test fixtures; the fixture design is documented separately.
- This remains documentation-only: no position page, no replay code, no DataService action, no schema change, no writes, and no Git tag.
- Stable contract: [Position Accounting Data Contract](docs/22_position_accounting_data_contract.md).
- Fixture design: [Position Accounting Test Fixture Design](docs/23_position_accounting_test_fixture_design.md).

## TASK-046 Accounting Replay Fixture Samples

- FX001-FX013 now have detailed implementation-before fixture samples with input facts, expected Position / Cash / PnL outputs, expected issues, and blocking expectations.
- These samples are documentation only: they are not database seed data, do not write SQLite, do not call services, do not drive trading, and do not generate TradeDraft.
- Fixture samples: [Accounting Replay Fixture Samples](docs/24_accounting_replay_fixture_samples.md).

## TASK-047 Position Shell ViewModel Design

- v0.3 now has a ShellServices ViewModel / Model design for future read-only position, cash, PnL, base-position, sniper-pool, and accounting issue surfaces.
- The design maps DTOs from future DataService read-only actions into QObject / QAbstractListModel boundaries for QML.
- This remains documentation-only: no C++ ViewModel, no QML page, no DataService action, no replay implementation, no writes, and no Git tag.
- ViewModel design: [Position Shell ViewModel Design](docs/25_position_shell_viewmodel_design.md).
- DTO mapping: [Position DTO ViewModel Mapping](docs/26_position_dto_viewmodel_mapping.md).

## TASK-048 Accounting Health Read-Only Action

- `accounting.health` is now available as a minimal DataService read-only action.
- It reports the accounting module boundary, contract version, read-only status, implemented action list, future action list, derived-table names, and `REPLAY_NOT_IMPLEMENTED` warning.
- `accounting.health` does not implement accounting replay, position calculation, cash summary, PnL summary, base-position calculation, sniper-pool calculation, or snapshot generation.
- It does not write `audit_log`, `trade_log`, `trade_execution_group`, `trade_draft`, `cash_snapshot`, `position_snapshot`, or `portfolio_summary`.
- DataServiceClient exposes `accountingHealth()` as an explicit whitelist wrapper; no QML page is connected to it in this task.

## TASK-049 Accounting Replay Preview Guard

- `accounting.replay.preview` is now available as a read-only non-implementation guard.
- It returns `success=true` at the protocol layer because the guard endpoint is callable, while the payload states `implemented=false`, `replayExecuted=false`, `writeEnabled=false`, and `status=REPLAY_NOT_AVAILABLE`.
- The guard lists required fixtures `FX001` through `FX013`, future output DTO names, forbidden write tables, and a blocking `REPLAY_NOT_AVAILABLE` error.
- It does not execute replay, read `trade_log` for calculation, return position / cash / PnL preview data, write snapshots, or write audit / trade tables.
- DataServiceClient exposes `accountingReplayPreview(payloadJson)` as an explicit whitelist wrapper; no QML page is connected to it in this task.

## TASK-050 Accounting Replay Fixture Files

- FX001-FX013 accounting replay fixture samples are now available as static JSON files under `tests/fixtures/accounting_replay/`.
- `tests/fixtures/accounting_replay/fixtures_index.json` lists the full fixture set and confirms there are no external dependencies.
- `tests/AccountingFixtures/validate_accounting_replay_fixtures.py` performs static structure and forbidden-token validation.
- These fixtures are not database data, do not write SQLite, do not call DataService, do not implement replay, do not drive trading, and do not generate TradeDraft.
- Run the validator directly with:

```powershell
python tests/AccountingFixtures/validate_accounting_replay_fixtures.py --fixtures-dir tests/fixtures/accounting_replay
```

## TASK-051 Accounting Fixture Loader Boundary

- `tests/AccountingFixtures/AccountingFixtureLoader` is a test-support-only loader for static accounting replay fixtures.
- It reads `fixtures_index.json`, loads FX001-FX013 JSON files, parses input facts / expected outputs / expected issues into memory, and validates fixture structure.
- It does not implement replay, does not calculate cash, position, cost, or PnL, does not access SQLite, does not call DataService, and does not write output files.
- Loader test CTest: `accounting_replay_fixture_loader`.

## TASK-052 Accounting Replay Test Harness Skeleton

- `tests/AccountingFixtures/AccountingReplayTestHarness` is the fixture-backed test entry point for future accounting replay work.
- It loads FX001-FX013 through `AccountingFixtureLoader`, iterates every fixture, and passes each fixture to `AccountingReplayStubEngine`.
- The stub engine returns `implemented=false`, `replayExecuted=false`, and `status=NOT_IMPLEMENTED` for valid fixtures.
- This harness does not implement replay, does not calculate cash, position, or PnL, does not access SQLite, does not call DataService, and does not write output files or database tables.
- Harness CTest: `accounting_replay_harness_skeleton`.

## TASK-053 Accounting Replay Result Skeleton

- `tests/AccountingFixtures/AccountingReplayResult` defines the test-only replay result skeleton for future fixture-backed replay tests.
- `AccountingReplayResultMapper` maps valid fixtures to a uniform `NOT_IMPLEMENTED` result and invalid fixtures to `INVALID_FIXTURE`.
- `AccountingReplayStubEngine` now returns `AccountingReplayResult` with `implemented=false`, `replayExecuted=false`, `status=NOT_IMPLEMENTED`, and a `REPLAY_NOT_IMPLEMENTED` issue.
- Position, cash, PnL, base-position, and sniper-pool raw outputs remain empty; replay is still not implemented.
- Result skeleton CTest: `accounting_replay_result_skeleton`.

## TASK-054 Accounting Expected Output Assertion Skeleton

- `tests/AccountingFixtures/AccountingExpectedOutputInspector` inspects fixture `expectedOutputs` shape and expected issue codes.
- `tests/AccountingFixtures/AccountingReplayAssertionSkeleton` checks the current `NOT_IMPLEMENTED` guard result and requires empty position / cash / PnL / base-position / sniper-pool raw outputs.
- Real `assertPositionList`, `assertCashSummary`, and `assertPortfolioPnl` value assertions currently return `SKIPPED_BY_DESIGN`; replay math is still not implemented.
- The assertion skeleton covers FX001-FX013 and must not be removed or bypassed by future replay work.
- Expected-output assertion CTest: `accounting_replay_expected_output_assertions`.

## Current Milestone: v0.2 ReadOnly Business Pages

The current milestone is `v0.2 ReadOnly Business Pages`: a local desktop read-only business page prototype layer on top of the v0.1 DataService / Shell read-only loop.

This project is still in development and must not be used for real trading, broker operations, real order placement, or production accounting.

v0.2 pages are still read-only. They do not support account editing, portfolio editing, instrument editing, strategy editing, OTC editing, real accounting entry, TradeLog writes, strategy execution, broker operations, or automatic trading.

Milestone documents:

- [Quick Start - ReadOnly Shell Demo](docs/15_quick_start_readonly_shell_demo.md)
- [Manual Acceptance - ReadOnly Shell](docs/13_manual_acceptance_readonly_shell.md)
- [Milestone - v0.1 ReadOnly Shell Demo](docs/14_milestone_v0_1_readonly_shell_demo.md)
- [Release Notes - v0.1 ReadOnly Shell Demo](docs/release_notes/v0_1_readonly_shell_demo.md)
- [Manual Acceptance - ReadOnly Business Pages](docs/16_manual_acceptance_readonly_business_pages.md)
- [UI Readability Checklist](docs/17_ui_readability_checklist.md)
- [Milestone - v0.2 ReadOnly Business Pages](docs/18_milestone_v0_2_readonly_business_pages.md)
- [Release Notes - v0.2 ReadOnly Business Pages](docs/release_notes/v0_2_readonly_business_pages.md)
- [ReadOnly Business Pages Summary](docs/19_readonly_business_pages_summary.md)
- [Position Accounting Boundary](docs/20_position_accounting_boundary.md)
- [Position Read-Only Data Contract Draft](docs/21_position_readonly_data_contract_draft.md)
- [Position Accounting Data Contract](docs/22_position_accounting_data_contract.md)
- [Position Accounting Test Fixture Design](docs/23_position_accounting_test_fixture_design.md)
- [Accounting Replay Fixture Samples](docs/24_accounting_replay_fixture_samples.md)
- [Accounting Replay Fixture Files](tests/fixtures/accounting_replay/README.md)
- [Position Shell ViewModel Design](docs/25_position_shell_viewmodel_design.md)
- [Position DTO ViewModel Mapping](docs/26_position_dto_viewmodel_mapping.md)
- [Documentation Index](docs/README.md)

Run tests:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## 当前尚未实现

- 未实现真实策略计算、六档狙击、底仓保护、TradeDraft 生命周期。
- 未实现 Excel 导入、行情接口、PushPlus、自动交易或券商接口。
- 未实现 QML 主界面。
- 未实现云同步、多用户权限、资讯、研报、完整回测。
- 未实现真实业务 action 分发；ServiceRuntime 当前只提供路由骨架、协议错误响应和 Ping/Health。
- 未实现任何业务 DataService 写入 action；当前唯一 socket 写入 action 是开发期 `data.audit.append`，且只写 `audit_log`。
- 未实现任何新增服务端 action；TASK-012 只增加客户端调用封装。
- 未实现 Windows Service 安装或后台常驻注册表写入；TASK-013 只提供 Watchdog 进程管理骨架。
- 未实现 Watchdog 自动重启；TASK-014 / TASK-015 / TASK-016 / TASK-017 只读取 `autoRestart` 字段，不执行自动重启。
- 未实现 Windows Service；TASK-015 只提供配置驱动启动指定服务并在命令结束前停止。
- TASK-016 / TASK-017 的 dry-run 不启动服务、不连接 socket、不访问数据库。
- 未实现 `--append-audit-demo`；TASK-010 仅提供 DataAccess 层事务和 audit_log 写入基础。
- 未实现任何 socket 写 action。
- 未实现任何绕过 `ETFDataService` 的数据库写入代码。
- 未实现业务 Repository、TradeLog 写入、账务重演或策略计算。
- 未实现任何业务写入 Repository；TASK-005 的 Repository 只有只读查询能力。

## 后续任务建议

1. TASK-018：为 Watchdog 增加状态报告消费方或更细的诊断输出，但仍不做自动重启和 Windows Service。
2. TASK-019：为 Shell / StrategyService 调用 DataServiceClient 增加更细的接口 DTO，但仍不引入 UI 或策略业务写入。
