# 跨境 ETF 智能投资决策终端

Windows 本地桌面端投资决策辅助终端。第一阶段不做自动下单，不接券商真实交易接口；用户在外部平台手工下单，软件负责行情、策略建议、资金池、TradeDraft、成交确认、TradeLog、审计和复盘等辅助能力。

## TASK-001 范围

本仓库当前只完成初始工程骨架：

- Monorepo 目录结构。
- CMake 顶层工程、apps、libs、tests 基础目标。
- 最小可编译服务入口：`ETFDecisionShell`、`ETFWatchdog`、`ETFDataService`、`ETFMarketService`、`ETFStrategyService`、`ETFAlertService`。
- 公共库骨架：`CoreDomain`、`Protocol`、`DataAccess`、`StrategyEngine`、`MarketEngine`、`Quantization`、`Security`。
- 数据库迁移脚本 `migrations/001_initial_schema.sql`。
- CTest 数据库初始化验证，使用 Python 标准库 `sqlite3` 执行迁移并验证核心表。
- docs 文档骨架、`.gitignore`、`.clang-format`。

## 构建

环境要求：

- CMake 3.20 或更高版本。
- 支持 C++17 的编译器。
- Python 3，用于当前临时数据库迁移测试。

```powershell
cmake -S . -B build
cmake --build build
```

## 运行测试

```powershell
ctest --test-dir build --output-on-failure
```

当前测试 `database_initial_schema` 会：

- 创建临时 SQLite 数据库。
- 执行 `migrations/001_initial_schema.sql`。
- 验证核心表存在。
- 验证 `schema_version` 已记录初始迁移。

## 当前尚未实现

- 未实现真实策略计算、六档狙击、底仓保护、TradeDraft 生命周期。
- 未实现 Excel 导入、行情接口、PushPlus、自动交易或券商接口。
- 未实现 QML 主界面。
- 未实现云同步、多用户权限、资讯、研报、完整回测。
- 未实现服务间 Qt Local Socket JSON 协议，仅保留协议库骨架。
- 未实现任何绕过 `ETFDataService` 的数据库写入代码。

## 后续任务建议

1. TASK-002：引入 Qt 依赖探测和基础服务进程启动规范。
2. TASK-003：完善 Protocol 消息 Envelope、错误码和兼容性测试。
3. TASK-004：实现 DataService 只读迁移检查、数据库连接生命周期和 WAL 初始化。
4. TASK-005：为 `trade_log`、审计和派生快照设计最小 Repository 接口。
