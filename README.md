# 跨境 ETF 智能投资决策终端

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

Transport 稳定性重复验证命令：

```powershell
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
- 未实现 Watchdog 自动重启；TASK-014 只读取 `autoRestart` 字段，不执行自动重启。
- 未实现 `--append-audit-demo`；TASK-010 仅提供 DataAccess 层事务和 audit_log 写入基础。
- 未实现任何 socket 写 action。
- 未实现任何绕过 `ETFDataService` 的数据库写入代码。
- 未实现业务 Repository、TradeLog 写入、账务重演或策略计算。
- 未实现任何业务写入 Repository；TASK-005 的 Repository 只有只读查询能力。

## 后续任务建议

1. TASK-015：为 Watchdog 增加按配置启动指定服务或批量状态展示，但仍不做自动重启和 Windows Service。
2. TASK-016：为 Shell / StrategyService 调用 DataServiceClient 增加更细的接口 DTO，但仍不引入 UI 或策略业务写入。
