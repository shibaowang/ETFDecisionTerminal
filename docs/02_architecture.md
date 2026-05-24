# 系统架构设计

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
