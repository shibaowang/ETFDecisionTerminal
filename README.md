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

## 当前尚未实现

- 未实现真实策略计算、六档狙击、底仓保护、TradeDraft 生命周期。
- 未实现 Excel 导入、行情接口、PushPlus、自动交易或券商接口。
- 未实现 QML 主界面。
- 未实现云同步、多用户权限、资讯、研报、完整回测。
- 未实现服务间业务 action 分发；Transport 当前只提供 Qt Local Socket 传输层骨架和协议帧测试。
- 未实现任何绕过 `ETFDataService` 的数据库写入代码。
- 未实现业务 Repository、TradeLog 写入、账务重演或策略计算。
- 未实现任何业务写入 Repository；TASK-005 的 Repository 只有只读查询能力。

## 后续任务建议

1. TASK-007：在 Transport 基础上设计服务端 action 分发骨架，但不接业务写入。
2. TASK-008：设计 DataService 受控写入事务边界，但仍不得绕过 TradeLog 事实账本规则。
3. TASK-009：在只读 Repository 基础上增加受控服务命令处理，不把 SQL 泄漏到其他服务。
