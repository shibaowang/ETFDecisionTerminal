# Codex 提示词模板

## TASK-018 diagnostic consumer guardrails

- Diagnostic consumer tasks must state whether they may start services or
  access databases; default behavior is no service start, no socket connection,
  and no SQLite access.
- JSON consumer tasks must validate missing fields and field types explicitly;
  invalid JSON or malformed schema must return a controlled non-zero result.
- Diagnostic model tasks must remain passive: no database access, no service
  start, no socket connection, and no business decisions.
- UI tasks must not reimplement diagnostic JSON parsing in QML; they should
  reuse the Diagnostics library through a C++ boundary.
- Shell / UI related tasks must explicitly declare their data source and
  binding boundary.
- UI code must not directly access SQLite or parse complex diagnostics JSON; it
  should consume C++ ViewModel objects such as `ShellDiagnosticViewModel`.
- Shell / UI filtering, sorting, aggregation, and refresh detection should be
  implemented in the C++ ViewModel layer unless a task explicitly authorizes a
  narrower UI-only presentation change.
- Shell Presenter tasks must not access databases, start services, connect
  sockets, or implement trading / strategy behavior.
- Mock data must be clearly scoped to tests or UI prototypes and must not be
  presented as real service state.
- UI Model Adapter tasks must reuse Presenter / ViewModel data and must not
  parse raw Watchdog JSON directly.
- Qt adapters may use Qt Core model classes only when explicitly allowed; QML /
  Qt Quick remains out of scope unless a task explicitly authorizes it.
- QML tasks must state their data source, must not access SQLite, must not
  connect services unless explicitly authorized, and must not parse complex JSON
  directly.
- QML filtering / sorting should delegate to C++ Presenter / Adapter methods.
- UI page tasks must explicitly state whether real data access is allowed.
- By default, UI tasks may use only mock / placeholder data unless the task
  authorizes service or database integration.

## 当前状态

v0.1 草案。

## 后续补充方向

补充任务模板、验收格式、禁止范围和跨任务上下文传递规范。

## 每次给 Codex 的任务必须包含

- 项目背景
- 当前任务目标
- 禁止修改范围
- 必须遵守的账务规则
- 涉及数据库表
- 涉及服务
- 输入输出协议
- 错误处理要求
- 测试要求
- 验收标准
- 不得自行扩展范围

## 固定硬约束

1. QML 不允许写策略、账务、资金池、底仓、TradeLog 逻辑。
2. 所有业务规则必须在 C++ 服务层。
3. SQLite 只能由 ETFDataService 写入。
4. trade_log 是唯一事实账本。
5. position_snapshot / cash_snapshot / portfolio_summary 只能由 trade_log 派生。
6. TradeDraft 是建议，不是成交。
7. 用户确认成交后才创建 trade_execution_group。
8. trade_execution_group、trade_log、trade_tier_allocation、base_position_allocation 必须同事务写入。
9. 场外 A/C 类必须按实际代码分腿记录。
10. 底仓保护按剩余持仓成本，不按市值。
11. 狙击周期按 grid_cycle_id 管理。
12. 底仓修复不占用狙击资金池。
13. 资金分配计划必须可解释、可复盘。
14. 任何手工入账必须标记 manual_entry 并写 audit_log。
15. 不允许用刷新覆盖历史建议，必须使用状态生命周期。
16. 所有涉及数据库写入的任务必须声明 ETFDataService 边界。
17. 非 ETFDataService 进程不得直接写 SQLite。
18. 任何 Repository 任务必须声明读写边界。
19. 除非任务明确允许，不得新增 INSERT / UPDATE / DELETE / 写入类 Repository 方法。
20. 只读 Repository 只能在 ETFDataService 进程内部使用，其他服务必须通过 Protocol 请求 DataService。
21. 涉及 Transport 的任务必须声明是否允许引入 Qt。
22. Transport 不得访问 SQLite，不得依赖 DataAccess，不得实现业务 action 分发。
23. 涉及 ServiceRuntime 的任务必须声明 action 路由边界。
24. ServiceRuntime 不得访问 SQLite，不得依赖 DataAccess，不得把 Repository 暴露成 socket API。
25. 除非任务明确允许，ServiceRuntime 只能实现协议级 Ping/Health 或路由骨架，不得新增业务 action。
26. 涉及 ServiceHost 的任务必须声明是否允许接入业务 handler。
27. 默认 ServiceHost 只能接入协议级 action，不得新增业务写入、账务、策略、行情或 TradeDraft action。
28. ServiceHost 不得访问 SQLite，不得依赖 DataAccess，不得绕过 ETFDataService 数据库边界。
29. ServiceHost 只能把 Transport 收到的 JSON 交给 ServiceRuntime，并返回 ProtocolResponse，不得在 Host 层实现业务规则。
30. 任何 `data.*` action 任务必须声明 read-only 或 write 边界。
31. 默认所有 `data.*` action 禁止写入，除非任务明确授权并定义事务、审计和 TradeLog 事实账本规则。
32. 只读 `data.*` action 不得拼接任意 SQL，不得注册动态 action，不得写业务表或派生快照表。
33. 所有写入任务必须声明允许写哪些表，未列出的表一律禁止写入。
34. 默认禁止写 `trade_log`，除非任务明确授权并定义账务事实、纠错、审计和回滚规则。
35. 所有写入任务必须有事务边界和审计要求；高风险写入必须与 `audit_log` 同事务提交或回滚。
36. 严禁通过 payload 传入任意 SQL，严禁新增任意 SQL 执行 action。
37. 默认禁止任何 `data.*` 写入 action，除非任务明确授权并加入写入白名单。
38. 未授权写入 action 必须返回协议错误，不能静默忽略或降级为查询。
39. 当前 `data.audit.append` 只允许写 `audit_log`，不得扩展为业务入账、TradeDraft 状态变更或通用 SQL 通道。
40. 客户端封装任务不得新增服务端 action，不得访问数据库，不得引入业务写入逻辑。
41. Client 只能通过 Transport + Protocol 请求 DataService，不得依赖 DataAccess 或绕过 DataService。
42. Client 不得接受或传递任意 SQL payload。
43. 涉及 Watchdog 的任务不得直接访问 SQLite，不得依赖 DataAccess。
44. Watchdog 只能管理进程和服务状态，不得实现业务策略、交易、账务重演或 TradeDraft 逻辑。
45. Watchdog 健康检查必须通过 DataServiceClient 或协议请求完成，不得绕过 DataService。
46. 涉及配置文件的任务必须定义配置格式、必填字段、默认值、校验规则和错误行为。
47. 配置错误不得静默忽略；配置无效时不得尝试启动服务。
48. `enabled=false` 的服务不得被 Watchdog 启动。
49. `autoRestart` 等预留字段不得在未授权任务中启用真实自动重启行为。
50. 涉及 Watchdog 启动服务的任务必须声明是否允许自动重启；默认禁止自动重启。
51. Watchdog 启动服务测试必须确认命令结束后无遗留子进程。
52. 涉及 Watchdog dry-run 的任务必须明确 dry-run 是否允许启动服务；默认 dry-run 不启动服务。
53. dry-run 不得连接 socket，不得访问数据库，不得写入任何状态。
54. 涉及 JSON 报告的任务必须保证输出可被标准 JSON parser 解析，不得输出伪 JSON。
55. JSON 报告字段必须稳定；布尔和数字字段不得以字符串代替。

## TASK 模板

```text
任务编号：
任务目标：
允许修改：
禁止修改：
涉及文档：
涉及数据库表：
涉及服务：
测试要求：
验收标准：
输出要求：
```
## Shell Navigation Task Rules

- UI navigation tasks must declare the navigation data source.
- Shell page metadata should come from ShellCore, not scattered across QML files.
- QML must not access SQLite or directly connect services unless a task explicitly authorizes it.
- QML must not directly parse complex JSON; diagnostic data must flow through ShellCore / Diagnostics.

## Shell Status Model Task Rules

- UI status model tasks must declare the data source.
- By default, UI status may only come from C++ mock / view-model layers unless a task explicitly authorizes real data.
- QML must not access SQLite or directly connect services for status updates.
- QML must not implement service health, accounting, trading, or strategy status logic.

## Shell Metrics Task Rules

- UI metrics tasks must declare the metric data source.
- By default, metrics and action hints may only come from C++ mock / view-model layers unless a task explicitly authorizes real data.
- Action hints are non-executable by default.
- QML must not access SQLite, connect services, or calculate business metrics directly.
- Future real metric integration must define service boundaries, tests, and read/write restrictions explicitly.

## Shell Read-Only Data Task Rules

- UI real-data tasks must pass through a C++ facade / ViewModel boundary.
- QML must not call `DataServiceClient` directly.
- Read-only facades must use explicit whitelist methods and must not expose arbitrary action forwarding.
- Read-only facades must not call write actions such as `data.audit.append`.
- Shell-side facades must not access SQLite directly or depend on DataAccess / DataServiceApi.

## Shell Read-Only ViewModel Task Rules

- UI read-only data models must obtain data through the Shell facade / controller layer, not directly through protocol clients.
- QML must not call `DataServiceClient`, parse DataService payloads, or connect sockets directly.
- Read-only ViewModels and controllers must not call write actions such as `data.audit.append`.
- Controllers must expose explicit whitelist methods and must not provide arbitrary action forwarding to UI code.

## Shell Read-Only QML Task Rules

- UI tasks that bind real read-only data must explicitly name the authorized Controller / ViewModel.
- QML must not call `DataServiceClient` directly and must not expose arbitrary protocol action calls.
- QML must not call write actions or provide edit / trade / accounting controls unless a later task explicitly authorizes them.
- Error states must be displayed through controller state; QML must not silently substitute mock data for failed real reads.

## Shell Read-Only State Flow Task Rules

- UI state-flow tasks must declare the source of connection, refresh, and error state.
- Refresh throttling and duplicate-request guards must be implemented in C++ Controller / ViewModel code.
- QML must bind controller state fields and must not implement complex throttling or connection-success inference.
- Failed refreshes should preserve prior successful data unless the task explicitly requires a clear-data behavior and tests it.
- Read-only state-flow tasks must not add write actions, arbitrary action forwarding, SQLite access, or direct `DataServiceClient` calls from QML.

## Shell Connection Preset Task Rules

- UI connection configuration tasks must state whether persistence is allowed; default is no persistence.
- Default behavior must not auto-start DataService, Watchdog, or any service process.
- Presets must be runtime guidance only unless a task explicitly authorizes service discovery or config storage.
- QML must not execute command hints and must not call `DataServiceClient` directly.
- Presets must not expand the read/write action boundary or add arbitrary action forwarding.

## Developer Runtime Output Rules

- Developer demo tasks that create runtime files must document where those files are written.
- `runtime/`, demo SQLite files, WAL/SHM files, logs, and pid files must stay ignored by Git.
- Demo cleanup scripts must prefer precise pid-file cleanup and must not stop all service processes unless the user explicitly passes a force option.
- Demo scripts must not modify migrations or call write actions unless a task explicitly authorizes that boundary.

## Business Read-Only Page Task Rules

- Business page tasks must declare whether the page is read-only or write-capable.
- Default business pages are read-only and must bind Controller / ViewModel / Model objects rather than low-level clients.
- QML must not expose edit, accounting, confirmation, trade, or write buttons unless a later task explicitly authorizes writes.
- QML must not call write actions, `data.audit.append`, or `DataServiceClient` directly.
- Strategy-related page prototypes are read-only by default.
- Strategy prototypes must not execute strategies, generate signals, or create TradeDraft records unless a later task explicitly authorizes that scope.
- OTC channel prototypes must not edit channel mappings unless a later task explicitly authorizes writes.
- Read-only UI component tasks must not change service boundaries or add write capabilities.
- UI readability work must keep components display-only and must not make mock / read-only screens look trade-capable.
- Read-only UI filter / sort tasks must keep filtering in C++ Model / ProxyModel / Controller layers.
- Filtering and sorting must not trigger service writes, backend mutations, direct SQLite access, or arbitrary action forwarding.
- UI display setting tasks must explicitly state whether settings are persisted.
- By default, column visibility and table density settings are runtime-only and must not be saved to files, SQLite, registry, or service configuration.
- Column visibility and density changes must not trigger service calls, backend writes, or source model mutation.
- UI readability work must not change the read-only service boundary or make pages appear write-capable.
## Position And Accounting Task Rules

- Position / accounting tasks must declare whether writing `trade_log` is allowed.
- Default scope is no `trade_log` writes.
- Default scope is no `cash_snapshot`, `position_snapshot`, or `portfolio_summary` writes.
- Default scope is no QML accounting calculation for position, cost, cash, principal,收益, base position, or sniper pool.
- Default scope is no automatic trading, no broker API, no real order placement, and no TradeDraft generation.
- Future write-capable accounting tasks require explicit authorization, transaction rules, audit rules, rollback rules, and tests.
- Future read-only position pages must bind Controller / ViewModel / Model objects and must not directly call `DataServiceClient` or access SQLite.
- Accounting DTO / data contract tasks are documentation-only unless explicitly authorized to implement code.
- Accounting replay implementation tasks must first reference the fixture design and list exact fixture coverage.
- Default scope is no snapshot writes and no `trade_log` writes.
- Default scope is no QML accounting calculation.
- Accounting replay implementation tasks must reference `docs/24_accounting_replay_fixture_samples.md`.
- Replay implementation must not skip FX001-FX013 without an explicit task update.
- Fixtures without expected output are not acceptable.
- Fixtures must not use external market data as a hidden dependency.
- Position ViewModel implementation tasks must reference `docs/25_position_shell_viewmodel_design.md` and `docs/26_position_dto_viewmodel_mapping.md`.
- QML must not calculate accounting fields.
- ShellServices ViewModels must not write data.
- Future implementation must test fixture-to-DTO-to-ViewModel mapping.
- `accounting.health` only means the accounting boundary health action is callable; it must not be treated as replay, position, cash, PnL, base-position, or sniper-pool implementation.
- `accounting.health` must remain read-only and must not be extended into a write action.
- New accounting actions require separate task authorization, explicit read/write scope, fixture coverage, and boundary tests.
