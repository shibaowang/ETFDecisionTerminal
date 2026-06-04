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
- `accounting.replay.preview` currently means the replay preview guard is callable; it must not be treated as replay implementation.
- The replay preview guard must remain read-only and must not be expanded into writes, snapshot generation, or fake DTO output.
- Real replay preview implementation must reference and test all FX001-FX013 fixture samples before returning calculated results.
- Accounting replay implementation tasks must use `tests/fixtures/accounting_replay/` as the fixture input set.
- Do not change fixture expected outputs to make an incorrect replay algorithm pass.
- Any fixture expected-output change must be explicitly justified as a contract correction.
- `tests/AccountingFixtures/validate_accounting_replay_fixtures.py` is static validation only and does not replace replay calculation tests.
- Replay implementation tasks must not bypass `tests/AccountingFixtures/AccountingFixtureLoader` or an explicitly compatible fixture loader.
- Fixture loader / parser tasks must stay read-only and must not implement replay, cash calculation, position calculation, PnL calculation, SQLite access, service calls, or output writes.
- Replay implementation tasks must preserve `AccountingReplayTestHarness` coverage for all FX001-FX013 fixtures.
- Do not make replay tests pass by deleting fixtures, skipping fixtures, or excluding error fixtures.
- Replacing `AccountingReplayStubEngine` with real replay logic requires explicit task authorization and fixture-backed assertions.
- Stub harness tasks must keep `NOT_IMPLEMENTED` as the expected result until replay implementation is authorized.
- Replay implementation tasks must preserve the `AccountingReplayResult` contract and must not hide fixture failures by changing result fields or statuses.
- Stub replacement must fill `positionListResponseRaw`, `cashSummaryRaw`, `portfolioPnlRaw`, `basePositionRaw`, and `sniperPoolRaw` gradually with explicit fixture-backed assertions.
- Replay implementation tasks must not delete or bypass `AccountingReplayAssertionSkeleton`.
- Replay implementation must gradually replace `SKIPPED_BY_DESIGN` position / cash / PnL assertions with real checks.
- Do not clear fixture `expectedOutputs` to make tests pass.
- Do not put fake position, cash, PnL, base-position, or sniper-pool outputs into a `NOT_IMPLEMENTED` result.
- Replay implementation must proceed fixture by fixture.
- Do not mix multiple fixture implementations into one task unless explicitly authorized.
- Each new replay fixture must preserve all previous fixture tests.
- Do not modify fixture expected outputs to fit a replay implementation.
- After FX002, do not jump directly to complex PnL, base-position, sniper-pool, or multi-fixture replay.
- Sell replay and partial-sell replay require separate task authorization.
- After FX003, do not jump directly to base-position, sniper-pool, multi-account, multi-instrument, or multi-currency replay.
- FX004 oversell handling must be implemented in its own task and must not be mixed into the FX003 partial-sell implementation.
- Do not mix missing-fee, negative-cash, market-price, or multi-currency error handling into unrelated fixture tasks.
- Every new fixture implementation must preserve FX001-FX003 tests and keep FX004-FX013 explicitly guarded until their own task is authorized.
- After FX004, do not jump directly to multi-account, multi-instrument, multi-currency, base-position, sniper-pool, or market valuation replay.
- FX005 missing-fee handling and FX006 negative-cash handling must remain separate tasks.
- Do not mix multiple error fixtures into one implementation task.
- Every new replay task after FX004 must preserve FX001-FX004 tests and keep FX005-FX013 explicitly guarded until their own task is authorized.
- After FX005, do not jump directly to multi-account, multi-instrument, multi-currency, base-position, sniper-pool, or market valuation replay.
- FX006 negative-cash handling must remain a separate task.
- Missing fee must not be silently defaulted to `fee=0`; any zero-fee policy must be explicitly authorized by a fixture contract or task.
- Every new replay task after FX005 must preserve FX001-FX005 tests and keep FX006-FX013 explicitly guarded until their own task is authorized.
- After FX006, do not jump directly to multi-account, multi-instrument, multi-currency, base-position, sniper-pool, or market valuation replay.
- FX007 multi-instrument handling must remain a separate task.
- Negative cash must not silently pass as an overdraft or produce normal position / cash / PnL success outputs.
- Every new replay task after FX006 must preserve FX001-FX006 tests and keep FX007-FX013 explicitly guarded until their own task is authorized.
- After FX007, do not jump directly to base-position, sniper-pool, multi-currency, or market valuation replay.
- FX008 multi-account handling must remain a separate task.
- Multi-instrument replay must not merge different `instrumentCode` values into one position.
- Missing market price must not be hidden by fabricated market value or unrealized PnL.
- Every new replay task after FX007 must preserve FX001-FX007 tests and keep FX008-FX013 explicitly guarded until their own task is authorized.
- After FX008, do not jump directly to sniper-pool, multi-currency, or market valuation replay.
- FX009 base-position handling must remain a separate task.
- Multi-account replay must not mix `accountId`, `portfolioId`, position, or cash dimensions.
- Every new replay task after FX008 must preserve FX001-FX008 tests and keep FX009-FX013 explicitly guarded until their own task is authorized.
- After FX009, do not jump directly to multi-currency, real market valuation, strategy execution, or TradeDraft generation.
- FX010 sniper tier handling must remain a separate task.
- `sellableAboveBaseAmountText` must not be interpreted as a sell recommendation, sell action, broker order, or TradeDraft.
- Every new replay task after FX009 must preserve FX001-FX009 tests and keep FX010-FX013 explicitly guarded until their own task is authorized.
- After FX010, do not jump directly to multi-currency, real market valuation, strategy execution, automatic trading, or TradeDraft generation.
- FX011 stale snapshot handling must remain a separate task.
- `remainingAmountText` in sniper-pool output must not be interpreted as a buy recommendation, sell recommendation, broker order, strategy command, or TradeDraft.
- Sniper tier completion must come from BUY fact aggregation or explicit fixture input metadata, not current market value.
- Sniper pool amount must not expand or shrink with floating profit or loss.
- Every new replay task after FX010 must preserve FX001-FX010 tests and keep FX011-FX013 explicitly guarded until their own task is authorized.
- After FX011, do not jump directly to multi-currency, real market valuation, strategy execution, automatic trading, or snapshot write paths.
- FX012 missing market price handling must remain a separate task.
- Stale snapshots must not be treated as fact sources.
- Stale snapshot handling must not write, refresh, or generate `position_snapshot`, `cash_snapshot`, or `portfolio_summary`.
- Every new replay task after FX011 must preserve FX001-FX011 tests and keep FX012-FX013 explicitly guarded until their own task is authorized.
- After FX012, do not jump directly to real market data integration, network market queries, multi-currency replay, strategy execution, or automatic trading.
- FX013 multi-currency unsupported handling must remain a separate task.
- Missing market prices must not be hidden by fabricated `marketValueText`.
- Missing market prices must not be hidden by fabricated `unrealizedPnlText`.
- FX012 missing market price replay must not query real market data, call a market service, or perform network requests.
- Every new replay task after FX012 must preserve FX001-FX012 tests and keep FX013 explicitly guarded until its own task is authorized.
- FX013 completion does not mean production accounting replay is complete.
- The next accounting replay milestone should be a test-only replay coverage or readiness review.
- Real DataService replay actions must be separately authorized.
- Do not move the test-only minimal engine into production code.
- Do not add real FX rate, market data, or network services unless a separate task authorizes them.
- Do not modify fixture expected outputs to fit a replay implementation.
- After FX001-FX013 test-only coverage, do not start production replay without readiness review.
- Production replay tasks must explicitly declare whether they are read-only or write-capable.
- Production replay tasks must explicitly declare DataAccess dependency boundaries.
- Production replay tasks must explicitly declare DataService action scope.
- Production replay tasks must explicitly declare snapshot behavior.
- Production replay tasks must explicitly declare TradeLog behavior.
- Production replay tasks must explicitly declare the QML boundary.
- Default remains no writes, no QML accounting calculation, no TradeDraft, and no auto-trading.
- Production replay tasks must reference `docs/33_production_accounting_replay_architecture.md` and `docs/34_accounting_engine_module_candidate.md`.
- Do not migrate `tests/AccountingFixtures` into production `libs` directly.
- DataService action implementation requires architecture boundary approval.
- AccountingEngine dependency on DataAccess requires explicit authorization.
- The default production replay phase is read-only and has no snapshot writes.
- AccountingEngine skeleton does not mean replay is implemented.
- Future AccountingEngine tasks must not introduce DataAccess unless explicitly authorized.
- Future AccountingEngine tasks must not copy `tests/AccountingFixtures` into production.
- Future replay implementation must keep AccountingEngine dependency boundary tests.
- AccountingEngine DTO parser tasks must not implement replay.
- DTO validation must not calculate positions, cash balances, cost, PnL, base position, or sniper pool.
- Future replay tasks must explicitly state whether they are read-only, whether they access DataAccess, and whether they expose a DataService action.
- The default remains no writes, no snapshot writes, no TradeLog writes, no TradeDraft, and no QML accounting calculation.
- AccountingEngine empty ledger skeleton does not mean full replay is implemented.
- Future production replay tasks must proceed one scenario at a time.
- Future replay tasks must not introduce DataAccess or DataService actions unless explicitly authorized.
- Future replay tasks must not write snapshots, TradeLog, TradeDraft, or business summary tables by default.
- Future replay tasks must not copy `tests/AccountingFixtures` implementations.
- AccountingEngine single buy skeleton does not mean complete replay is implemented.
- Future production replay tasks after single BUY must remain one scenario at a time.
- Future SELL, PnL, market value, base-position, sniper-pool, multi-account, and multi-instrument work requires separate authorization.
- AccountingEngine partial sell skeleton does not mean complete replay is implemented.
- Future production replay tasks after one BUY + one SELL must still remain one scenario at a time.
- Multi-transaction, multi-account, multi-instrument, market value, unrealized PnL, base-position, sniper-pool, DataService action, SQLite, snapshot write, and TradeLog write work requires separate authorization.
- AccountingEngine sell-exceeds-position scenario does not mean complete replay is implemented.
- Future production replay tasks after `SELL_EXCEEDS_POSITION` must still remain one scenario at a time.
- Full missing-fee handling, full negative-cash fixture handling, multi-transaction replay, multi-account replay, multi-instrument replay, market value, unrealized PnL, base-position, sniper-pool, DataService action, SQLite, snapshot write, and TradeLog write work requires separate authorization.
- AccountingEngine missing-fee scenario does not mean complete replay is implemented.
- Missing fee must not be defaulted to zero.
- Future production replay tasks after `MISSING_FEE` must still remain one scenario at a time.
- Full negative-cash fixture handling, multi-transaction replay, multi-account replay, multi-instrument replay, market value, unrealized PnL, base-position, sniper-pool, DataService action, SQLite, snapshot write, and TradeLog write work requires separate authorization.
- AccountingEngine negative-cash scenario does not mean complete replay is implemented.
- Negative cash must not pass as an implicit overdraft.
- Missing fee must keep returning `MISSING_FEE` and must not be overwritten by `NEGATIVE_CASH`.
- Future production replay tasks after `NEGATIVE_CASH` must still remain one scenario at a time.
- Multi-transaction replay, multi-account replay, multi-instrument replay, market value, unrealized PnL, base-position, sniper-pool, DataService action, SQLite, snapshot write, and TradeLog write work requires separate authorization.
- AccountingEngine multi-instrument BUY scenario does not mean complete replay is implemented.
- Multi-instrument BUY must group by `instrumentCode`; different instruments must not be merged or confused.
- Multi-instrument BUY must not fabricate market value or unrealized PnL.
- Future production replay tasks after multi-instrument BUY must still remain one scenario at a time.
- Multi-account replay, multi-currency replay, multi-instrument SELL, mixed multi-instrument BUY/SELL, base-position, sniper-pool, DataService action, SQLite, snapshot write, and TradeLog write work requires separate authorization.
- AccountingEngine multi-account BUY scenario does not mean complete replay is implemented.
- Multi-account BUY must group by `accountId + portfolioId + instrumentCode`; different accounts must not be merged or confused.
- Multi-account BUY must keep account cash summaries separate and must not hide account boundaries behind one combined cash number.
- Multi-account BUY must not fabricate market value or unrealized PnL.
- Future production replay tasks after multi-account BUY must still remain one scenario at a time.
- Multi-currency replay, multi-account SELL, cross-portfolio aggregation, base-position, sniper-pool, DataService action, SQLite, snapshot write, and TradeLog write work requires separate authorization.
- AccountingEngine multi-currency unsupported scenario does not mean multi-currency replay is supported.
- Multi-currency unsupported detection must reject CNY plus non-CNY facts without FX rate facts.
- USD must not be treated as CNY or directly added to CNY totals.
- FX rates must not be fabricated, fetched over the network, or inferred from market data.
- Multi-currency unsupported results must not fabricate `totalAssetsText`, `marketValueText`, or `unrealizedPnlText`.
- Future production replay tasks after multi-currency unsupported detection must still remain one scenario at a time.
- Real FX rate support, successful multi-currency valuation, DataService action, SQLite, snapshot write, and TradeLog write work requires separate authorization.
- AccountingEngine missing market price scenario does not mean market valuation is supported.
- Missing market price handling must return a visible warning when valuation / `pnl` output is requested and no valid price exists.
- Missing market price handling may return quantity, cost, and cash, but must not fabricate `marketValueText`, `unrealizedPnlText`, or `totalAssetsText`.
- Missing market price handling must not query market services or use network access.
- Future production replay tasks after missing market price detection must still remain one scenario at a time.
- Real market price support, valuation, DataService action, SQLite, snapshot write, and TradeLog write work requires separate authorization.
- After TASK-082, do not start DataService or SQLite work without an explicit task boundary.
- A scenario coverage milestone does not imply productionReady=true.
- Future tasks must state whether they are DTO-only, DataService action, SQLite read-only, write-enabled, or QML-facing.
- Default remains no writes, no DataService action, and no SQLite unless explicitly authorized.
- Any DataService accounting action task must reference `docs/37_dataservice_readonly_accounting_action_contracts.md` and `docs/38_dataservice_accounting_no_write_test_plan.md`.
- No-write tests are required before implementing a DataService accounting action.
- Default is no action implementation unless the task explicitly requests one.
- DataService accounting actions cannot write snapshots or TradeLog rows.
- QML must not call DataServiceClient directly.
- Any SQLite accounting facts query task must reference `docs/39_sqlite_readonly_facts_query_boundary.md` and `docs/40_accounting_facts_source_mapping.md`.
- No SQLite query implementation is allowed unless the task explicitly requests it.
- Default remains no write for SQLite facts query tasks.
- Query layer must not calculate replay.
- Query layer must not use snapshot as facts source.
- Query layer must not call external market or FX services.
- DataService action guard tasks must not pretend to be real implementations.
- `implemented=false` guards may exist, but they must not return real accounting data.
- `position.list` guard does not equal a real position query.
- Future real DataService accounting actions must reference `docs/37_dataservice_readonly_accounting_action_contracts.md`, `docs/38_dataservice_accounting_no_write_test_plan.md`, `docs/39_sqlite_readonly_facts_query_boundary.md`, and `docs/40_accounting_facts_source_mapping.md`, and must include no-write tests.
- SQLite facts query tasks must use the test-only no-write harness skeleton
  before implementation.
- Do not calculate replay in the SQLite query layer.
- Do not write from the SQLite query layer.
- Do not bypass the forbidden SQL scanner.
- Do not place the no-write harness in production libs.
- DataAccess read-only query skeletons do not equal replay.
- Query layer must not compute accounting.
- Query layer must not call AccountingEngine.
- Query layer must not write.
- Future facts query tasks must use the no-write harness and SQL scanner.
- Cash facts query tasks must reference
  `docs/41_cash_facts_source_boundary.md` and
  `docs/42_cash_facts_query_decision.md`.
- Never derive `CashFactDto` from `cash_snapshot` or `portfolio_summary`.
- Query layer must not compute cash balance.
- If schema does not contain a cash fact source, stop and report the schema gap;
  do not fake facts.
- DataService `cash.summary` guard tasks must not pretend to be real
  implementations.
- `implemented=false` cash guards may exist, but they must not return real cash
  balance data.
- Future real `cash.summary` tasks must reference
  `docs/37_dataservice_readonly_accounting_action_contracts.md`,
  `docs/38_dataservice_accounting_no_write_test_plan.md`,
  `docs/39_sqlite_readonly_facts_query_boundary.md`,
  `docs/40_accounting_facts_source_mapping.md`,
  `docs/41_cash_facts_source_boundary.md`, and
  `docs/42_cash_facts_query_decision.md`, and must include no-write tests.
- DataService `portfolio.pnl.summary` guard tasks must not pretend to be real
  implementations.
- `implemented=false` PnL guards may exist, but they must not return real
  `totalAssets`, `realizedPnl`, `unrealizedPnl`, `totalPnl`, or
  `totalReturnRatio` data.
- Future real `portfolio.pnl.summary` tasks must reference
  `docs/37_dataservice_readonly_accounting_action_contracts.md`,
  `docs/38_dataservice_accounting_no_write_test_plan.md`,
  `docs/39_sqlite_readonly_facts_query_boundary.md`, and
  `docs/40_accounting_facts_source_mapping.md`, and must include no-write
  tests.
- DataService `base_position.summary` guard tasks must not pretend to be real
  implementations.
- `implemented=false` base position guards may exist, but they must not return
  real `targetBaseRatioText`, `lockedBaseAmountText`, or
  `sellableAboveBaseAmountText` data.
- `sellableAboveBaseAmountText` is not a sell suggestion or trade advice.
- Base position guard or implementation tasks must not generate TradeDraft
  rows unless a later task explicitly authorizes writes.
- Future real `base_position.summary` tasks must reference
  `docs/37_dataservice_readonly_accounting_action_contracts.md`,
  `docs/38_dataservice_accounting_no_write_test_plan.md`,
  `docs/39_sqlite_readonly_facts_query_boundary.md`, and
  `docs/40_accounting_facts_source_mapping.md`, and must include no-write
  tests.
- DataService `sniper_pool.summary` guard tasks must not pretend to be real
  implementations.
- `implemented=false` sniper-pool guards may exist, but they must not return
  real `poolAmountText`, `usedAmountText`, `remainingAmountText`, or
  `tierSummary` data.
- `remainingAmountText` is not a trade suggestion.
- `completed` is not derived from current market value in a guard task.
- Sniper-pool guard or implementation tasks must not generate TradeDraft rows
  unless a later task explicitly authorizes writes.
- Sniper-pool guard tasks must not generate strategy execution.
- Future real `sniper_pool.summary` tasks must reference
  `docs/37_dataservice_readonly_accounting_action_contracts.md`,
  `docs/38_dataservice_accounting_no_write_test_plan.md`,
  `docs/39_sqlite_readonly_facts_query_boundary.md`, and
  `docs/40_accounting_facts_source_mapping.md`, and must include no-write
  tests.
- The DataService accounting guard suite milestone does not equal real action
  implementation.
- DataService accounting guard suite milestone does not equal real action implementation.
- Future tasks must state whether they modify an existing guard payload or
  implement a real action.
- Real action tasks must include no-write expansion for real success, domain
  failure, invalid request, and exception paths.
- Real action tasks must explicitly authorize SQLite facts query integration
  and AccountingEngine integration before using either.
- QML must not directly call DataServiceClient.
- Guard suite readiness does not authorize snapshot writes, TradeLog writes,
  TradeDraft generation, trade suggestions, strategy execution, or broker
  orders.
- Real `position.list` tasks must reference
  `docs/45_position_list_real_implementation_boundary.md` and
  `docs/46_position_list_real_implementation_readiness_checklist.md`.
- Do not change the `position.list` guard from `implemented=false` to
  `implemented=true` without explicit task authorization.
- Real `position.list` tasks must declare the exact first-stage scope.
- No-write tests must be written before or with real `position.list`
  implementation.
- DataService must not compute replay directly; it should coordinate
  DataAccess facts and AccountingEngine mapping only when explicitly
  authorized.
- `position.list` mapping contract tests do not equal real action
  implementation.
- Mapping helpers must stay in `tests/` or another test-only directory, not in
  production libs.
- Do not bypass the no-write plan or readiness checklist when moving from
  mapping contracts to a real action.
- Changing `position.list` to `implemented=true` requires separate explicit
  authorization.
- Real `position.list` tasks must not bypass the TASK-096 no-write skeleton.
- The no-write skeleton does not equal real action implementation.
- Test-only fake callbacks used by the no-write skeleton must not move into
  production libs.
- Real `position.list` implementation must keep the skeleton no-write matrix
  passing and add real action no-write tests before changing
  `implemented=false`.
- Real `position.list` implementation tasks must reference
  `docs/47_position_list_readiness_hardening.md` and
  `docs/48_position_list_first_stage_real_action_scope.md`.
- Do not implement true `position.list` without explicit authorization.
- Do not expand first implementation scope beyond
  `docs/48_position_list_first_stage_real_action_scope.md`.
- The no-write hardening matrix in docs/47 must pass before merge.
- Guard fallback must remain documented and available during first real
  `position.list` implementation.
- Real `position.list` tasks must not bypass TASK-098 first-stage scaffolding.
- Scaffolding does not equal real action implementation.
- Test-only scenario descriptors must not move into production libs.
- Switching `position.list` to `implemented=true` still requires separate
  explicit authorization.
- ShellServices accounting tasks must reference
  `docs/49_shellservices_accounting_controller_contract.md` and
  `docs/50_shell_accounting_viewmodel_state_contract.md`.
- QML must not call DataServiceClient directly.
- QML must not calculate accounting.
- ShellServices must not generate TradeDraft or trade suggestions.
- Guard payload `implemented=false` maps to `Unavailable`, not `Empty`.
- ShellServices accounting implementation tasks must pass TASK-100 scaffolding:
  `shell_accounting_controller_state_contract`,
  `shell_accounting_controller_issue_contract`,
  `shell_accounting_controller_privacy_contract`, and
  `shell_accounting_controller_no_trade_action_contract`.
- Test scaffolding is not a production controller.
- QML must not directly call DataServiceClient.
- Do not generate TradeDraft or trade suggestion from Shell accounting
  ViewModels.
- TASK-101 ShellAccountingReadOnlyController skeleton is not real service
  integration.
- Future ShellServices tasks must not call DataServiceClient unless explicitly
  authorized.
- QML integration requires a separate task.
- No TradeDraft / no trade suggestion remains mandatory.
- TASK-102 ShellServices fake/client boundary tests do not equal real
  DataServiceClient integration.
- Fake client must remain test-only.
- Do not call real DataServiceClient unless explicitly authorized.
- QML integration requires a separate task.
- No TradeDraft / no trade suggestion remains mandatory.
- ShellServices service integration tasks must reference
  `docs/51_shell_accounting_service_adapter_boundary.md` and
  `docs/52_shell_accounting_service_integration_readiness.md`.
- Do not call real DataServiceClient directly from
  `ShellAccountingReadOnlyController` unless explicitly authorized.
- Prefer a `ShellAccountingServiceAdapter` abstraction for service integration.
- Fake adapter must remain test-only.
- Real adapter requires a separate task.
- QML integration requires a separate task.
- TASK-104 ShellAccountingServiceAdapter interface skeleton does not equal a
  real adapter.
- Do not implement ShellAccountingDataServiceAdapter without explicit
  authorization.
- Do not call real DataServiceClient without explicit authorization.
- Fake adapter must remain test-only.
- QML integration requires a separate task.
- TASK-105 ShellAccounting fake adapter injection behavior does not equal real
  DataServiceClient integration.
- Fake adapter must remain test-only and must not move into production libs.
- Do not call real DataServiceClient unless explicitly authorized.
- QML integration requires a separate task.
- No TradeDraft / no trade suggestion / no strategy execution remains
  mandatory for Shell accounting refresh paths.
- ShellAccountingDataServiceAdapter implementation tasks must reference
  `docs/53_shell_accounting_dataservice_adapter_boundary.md` and
  `docs/54_shell_accounting_dataservice_adapter_test_plan.md`.
- Do not implement the real adapter without explicit authorization.
- Do not call real DataServiceClient unless the task explicitly allows it.
- The adapter may only call read-only accounting wrappers.
- No write action invocation is allowed.
- QML integration requires a separate task.
- ShellAccountingDataServiceAdapter real implementation must not bypass
  TASK-107 spy/wrapper tests.
- Real adapter may only call read-only accounting wrappers.
- No write action invocation is allowed.
- Fake/spy wrapper must remain test-only.
- QML integration requires a separate task.
- TASK-108 ShellAccountingDataServiceAdapter skeleton is not real
  DataServiceClient integration.
- Do not add live DataServiceClient calls without explicit authorization.
- Current ShellAccountingDataServiceAdapter skeleton must return not connected /
  unavailable.
- ShellAccountingDataServiceAdapter must not include DataServiceClient,
  DataServiceApi, DataAccess, AccountingEngine, SQLite, QtQuick, or QML until a
  separately authorized real adapter task changes that boundary.
- QML integration requires a separate task.
- Do not implement ShellAccountingDataServiceAdapter live calls without
  `docs/55_shell_accounting_dataservice_adapter_live_call_gate.md` and
  `docs/56_shell_accounting_dataservice_adapter_live_call_acceptance_checklist.md`.
- TASK-109 live-call gate is not live implementation.
- A live adapter may only call allowlisted read-only wrappers.
- Live-call tasks must keep no QML, no SQLite, no AccountingEngine, no writes,
  no TradeDraft, and no trade suggestion unless separately authorized.
- TASK-110 live-call skeleton is not real DataServiceClient integration.
- Do not include DataServiceClient in `ShellAccountingDataServiceAdapter` unless
  explicitly authorized.
- Future concrete DataServiceClient port implementation requires a separate
  task.
- Keep no QML, no SQLite, no AccountingEngine, no writes, no TradeDraft, and no
  trade suggestion for Shell accounting adapter work unless separately
  authorized.
- TASK-111 ShellAccountingDataServiceClientPort concrete implementation may
  include the real DataServiceClient only inside the concrete port class.
- The concrete port may call only `positionList`, `cashSummary`,
  `portfolioPnlSummary`, `basePositionSummary`, and `sniperPoolSummary` guard
  wrappers.
- Do not expand the concrete port to write actions, `data.audit.append`,
  TradeDraft, strategy execution, broker orders, SQLite, or AccountingEngine.
- Do not wire QML without an explicit separate task.
- Do not implement real `position.list` or any other real accounting action in
  a Shell adapter / port task.
- TASK-112 Shell controller concrete port integration hardening does not imply
  QML integration.
- Do not wire QML without an explicit task.
- Do not implement real accounting actions in Shell controller / adapter tasks.
- Controller concrete port integration must keep no SQLite, no AccountingEngine,
  no writes, no TradeDraft, and no trade suggestion unless separately
  authorized.
- TASK-113 ShellAccounting ViewModel / Model read-only boundary does not imply
  QML integration.
- Shell accounting ViewModel / Model tasks must not wire QML unless explicitly
  authorized.
- ViewModel / Model code must not call DataServiceClient, access SQLite, call
  AccountingEngine, or calculate accounting.
- ViewModel / Model code must not expose TradeDraft, trade suggestion,
  strategy execution, broker order, or write action interfaces.
- ShellAccounting presenter tasks must reference
  `docs/57_shell_accounting_presenter_contract.md` and
  `docs/58_shell_accounting_qml_binding_readiness_plan.md`.
- QML binding tasks must reference
  `docs/58_shell_accounting_qml_binding_readiness_plan.md`.
- Do not modify QML without explicit authorization.
- QML must not call DataServiceClient and must not calculate accounting.
- No TradeDraft / no trade suggestion remains mandatory for read-only
  accounting UI.

## TASK-115 ShellAccountingPresenter Skeleton

- ShellAccountingPresenter skeleton is not QML integration.
- Do not register QML types without an explicit task.
- Do not call DataServiceClient from the presenter.
- Presenter code must only use ShellServices read-only state, issue, position
  model, and controller boundaries.
- Presenter must not expose TradeDraft, trade suggestion, strategy execution,
  or broker submission actions.

## TASK-116 ShellAccountingPresenter Concrete Port Integration

- Presenter concrete port integration does not imply QML binding.
- Do not wire QML or register QML types without an explicit task.
- Do not implement real accounting actions in presenter tasks.
- Presenter integration may consume DataServiceClient guard wrappers only
  through controller -> adapter -> concrete port.
- Keep no SQLite, no AccountingEngine, no writes, no TradeDraft, no trade
  suggestion, no strategy execution, and no broker order.

## TASK-117 ShellAccountingPresenter All Guard Refresh

- Presenter all guard refresh does not imply QML binding.
- Do not wire QML or register QML types without an explicit task.
- Do not implement real accounting actions in presenter tasks.
- Presenter refresh methods may only call ShellAccountingReadOnlyController.
- Presenter must not directly include or call DataServiceClient,
  DataServiceApi, DataAccess, AccountingEngine, SQLite, QML, adapters, or
  concrete ports.
- `refreshAllReadOnly` must stay read-only and must not write tables, generate
  TradeDraft, generate trade suggestions, execute strategies, or submit broker
  orders.

## TASK-118 ShellAccounting QML Binding Smoke Readiness

- QML binding tasks must reference
  `docs/59_shell_accounting_qml_binding_smoke_plan.md` and
  `docs/60_shell_accounting_qml_static_gate.md`.
- Do not modify QML without explicit authorization.
- QML must not call DataServiceClient, access SQLite / DataAccess, or call
  AccountingEngine.
- QML must not calculate accounting values.
- QML must not expose TradeDraft, trade suggestion, broker order, strategy
  execution, write action, buy, or sell controls.
- Empty vs Unavailable must remain distinct.
- QML smoke tests and static gates require a separate implementation task.

## TASK-119 ShellAccounting QML Static Gate CTest

- QML binding tasks must pass the TASK-119 static gate CTests.
- New accounting QML must not include DataServiceClient, DataServiceApi,
  SQLite, DataAccess, AccountingEngine, ShellAccountingDataServiceAdapter,
  ShellAccountingDataServiceClientPortAdapter, write action, TradeDraft,
  brokerOrder, or strategyExecute tokens.
- Do not modify QML without explicit authorization.
- Existing placeholder trade pages are not accounting binding pages and must not
  be used to justify accounting trade controls.
- Future accounting QML must keep Empty vs Unavailable distinct and must not
  show trading buttons, generate TradeDraft, generate trade suggestions,
  execute strategies, submit broker orders, access SQLite, or write database
  tables.

## TASK-120 ShellAccounting Read-only UI Readiness

- Future ShellAccounting QML tasks must reference
  `docs/61_shell_accounting_readonly_ui_readiness_milestone.md` and
  `docs/62_shell_accounting_next_phase_review.md`.
- v0.6 readiness milestone is not QML binding.
- QML binding requires explicit authorization.
- Real accounting action implementation requires separate authorization.
- no-write, no-trade, and the TASK-119 QML static gate remain mandatory.
- Do not create the suggested v0.6 Git tag from an implementation task unless
  the user explicitly requests it after merge.

## TASK-121 ShellAccounting QML Binding Smoke Scaffold

- QML binding tasks must pass the TASK-119 static gate and TASK-121 smoke
  scaffold.
- Do not modify QML without explicit authorization.
- The smoke scaffold is not QML binding and does not register QML types.
- Future QML may bind only ShellServices read-only objects.
- Empty vs Unavailable, issue visibility, privacy display, no TradeDraft, no
  trade suggestion, no write action, no strategy, and no broker order remain
  mandatory.

## TASK-122 ShellAccounting QML Smoke CTest

- QML binding tasks must pass the TASK-119 static gate, TASK-121 smoke
  scaffold, and TASK-122 runtime smoke CTests.
- TASK-122 uses test-only inline QML; it is not production QML binding.
- Do not modify production QML without explicit authorization.
- Do not register production ShellAccounting QML types without explicit
  authorization.
- Smoke QML must remain read-only and must not include buy, sell,
  createTradeDraft, brokerOrder, strategyExecute, `data.audit.append`, or
  `writeEnabled: true`.
- No TradeDraft, no trade suggestion, no write action, no SQLite, and no
  AccountingEngine access remain mandatory.

## TASK-123 ShellAccounting Production QML Binding Boundary

- Production QML binding tasks must reference
  `docs/63_shell_accounting_production_qml_binding_boundary.md` and
  `docs/64_shell_accounting_first_stage_qml_binding_checklist.md`.
- Do not modify QML without explicit authorization.
- Production QML binding must pass TASK-119, TASK-121, and TASK-122 gates.
- First-stage QML must be read-only placeholder / Unavailable / issue /
  privacy smoke only.
- No real accounting action, no write action, no TradeDraft, no trade
  suggestion, no broker order, and no strategy execution are allowed.

## TASK-124 ShellAccounting QML Type Registration Boundary

- QML type registration tasks must reference
  `docs/65_shell_accounting_qml_type_registration_boundary.md` and
  `docs/66_shell_accounting_qml_type_registration_test_plan.md`.
- Do not register QML type without explicit authorization.
- Do not expose DataServiceClient, SQLite, AccountingEngine, DataAccess,
  adapter, concrete port, write methods, TradeDraft, trade suggestion, broker
  order, or strategy execution to QML.
- Registration must pass TASK-119 static gate, TASK-121 smoke scaffold, and
  TASK-122 runtime smoke CTest.
- QML type registration does not imply production QML page binding or real
  accounting action implementation.

## TASK-125 ShellAccounting QML Type Registration Scaffold

- QML type registration tasks must pass the TASK-125
  `shell_accounting_qml_type_registration` scaffold.
- Do not register QML types without explicit authorization.
- QML registration must not expose DataServiceClient, SQLite, AccountingEngine,
  DataAccess, write methods, trade methods, TradeDraft, broker order, or
  strategy execution.
- The TASK-125 scaffold is not production QML type registration and does not
  authorize production QML edits.

## TASK-126 ShellAccounting QML Type Registration Implementation Gate

- QML type registration implementation tasks must pass the TASK-126
  implementation gates.
- Do not register QML types without explicit authorization.
- Registration must preserve rollback / disable strategy from
  `docs/68_shell_accounting_qml_type_registration_rollback_strategy.md`.
- Registration must not expose DataServiceClient, SQLite, AccountingEngine,
  DataAccess, adapter, concrete port, write methods, trade methods, TradeDraft,
  broker order, or strategy execution.
- TASK-126 gate is not production registration and does not authorize production
  QML edits.

## TASK-127 ShellAccounting QML Type Registration Implementation

- QML type registration implementation does not imply production QML page
  binding.
- Do not add production QML imports without an explicit task.
- Registration is allowlist-only for `ShellAccountingPresenter`,
  `ShellAccountingStatusObject`, `ShellAccountingIssueListModel`, and
  `ShellPositionListModel`.
- Use centralized registration through `ShellAccountingQmlRegistration`.
- Do not expose DataServiceClient, SQLite, DataAccess, AccountingEngine,
  adapter / concrete port, write methods, TradeDraft, trade suggestion, broker
  order, or strategy execution.
- Future production QML binding must still pass TASK-119 / TASK-121 / TASK-122 /
  TASK-125 / TASK-126 / TASK-127 gates.

## TASK-128 ShellAccounting QML Startup Registration Gate

- Startup wiring tasks must reference
  `docs/69_shell_accounting_qml_startup_registration_gate.md` and
  `docs/70_shell_accounting_qml_startup_registration_test_plan.md`.
- Do not wire `registerShellAccountingQmlTypes` into production startup without
  explicit authorization.
- Startup wiring is not production QML page binding and is not real accounting
  action implementation.
- Startup wiring must not expose `DataServiceClient`, SQLite, `DataAccess`,
  `AccountingEngine`, adapters, concrete ports, write actions, trade actions,
  `TradeDraft`, trade suggestions, strategy execution, or broker orders.

## TASK-129 ShellAccounting QML Startup Registration Wiring

- Startup wiring tasks must reference
  `docs/69_shell_accounting_qml_startup_registration_gate.md`,
  `docs/70_shell_accounting_qml_startup_registration_test_plan.md`, and
  `docs/71_shell_accounting_qml_startup_registration_wiring.md`.
- TASK-129 allows only one centralized call to
  `registerShellAccountingQmlTypes`.
- Do not bind production QML without explicit authorization.
- Startup wiring is not QML page binding and is not real accounting action.
- Do not add production QML imports, `accountingPresenter`, presenter context
  exposure, DataServiceClient, SQLite, DataAccess, AccountingEngine,
  write/trade exposure, TradeDraft, trade suggestion, strategy, or broker
  behavior.

## TASK-130 ShellAccounting Production QML Binding Gate

- Production QML binding tasks must reference
  `docs/72_shell_accounting_production_qml_binding_gate.md` and
  `docs/73_shell_accounting_production_qml_binding_test_plan.md`.
- Do not modify production QML without explicit authorization.
- Production QML binding is not real accounting action.
- Do not add production QML imports, page routes, page components,
  `accountingPresenter`, presenter context exposure, DataServiceClient, SQLite,
  DataAccess, AccountingEngine, write/trade exposure, TradeDraft, trade
  suggestion, strategy, broker behavior, or trading UI unless the task
  explicitly authorizes them.

## TASK-131 ShellAccounting Production QML Binding Implementation

- Production QML binding implementation tasks must reference
  `docs/72_shell_accounting_production_qml_binding_gate.md`,
  `docs/73_shell_accounting_production_qml_binding_test_plan.md`, and
  `docs/74_shell_accounting_production_qml_binding_implementation.md`.
- TASK-131 authorizes only the read-only, unavailable-safe
  `ShellAccountingReadOnlyPage.qml` shell.
- The only production QML import is
  `import ETFDecisionTerminal.ShellAccounting 1.0` in the authorized shell page.
- The only presenter binding is a nullable
  `property ShellAccountingPresenter accountingPresenter: null`.
- Do not create `ShellAccountingPresenter`, do not set an accounting presenter
  context property, and do not call DataServiceClient, SQLite, DataAccess,
  AccountingEngine, write actions, TradeDraft, trade suggestions, strategy,
  broker behavior, or trading UI.

## TASK-132 ShellAccounting Presenter Lifecycle Gate

- Presenter lifecycle tasks must reference
  `docs/75_shell_accounting_presenter_lifecycle_gate.md` and
  `docs/76_shell_accounting_presenter_lifecycle_test_plan.md`.
- Do not create `ShellAccountingPresenter` without explicit authorization.
- Do not expose an `accountingPresenter` context property without explicit
  authorization.
- Presenter lifecycle is not real accounting action and does not authorize
  DataServiceClient, SQLite, DataAccess, AccountingEngine, writes, TradeDraft,
  trade suggestions, strategy, broker behavior, or trading UI.
- The production QML shell must keep supporting null / Unavailable presenter
  state until a separate implementation task authorizes lifecycle wiring.

## TASK-133 ShellAccounting Presenter Lifecycle Implementation

- Presenter lifecycle implementation tasks must reference
  `docs/75_shell_accounting_presenter_lifecycle_gate.md`,
  `docs/76_shell_accounting_presenter_lifecycle_test_plan.md`, and
  `docs/77_shell_accounting_presenter_lifecycle_implementation.md`.
- TASK-133 authorizes only centralized `ShellAccountingPresenter` creation and
  one `setContextProperty("accountingPresenter", ...)` exposure.
- Presenter lifecycle is not DataService / SQLite / AccountingEngine
  integration and is not real accounting action.
- Do not expose DataServiceClient, DataAccess, SQLite, AccountingEngine,
  concrete adapters, write-enabled objects, TradeDraft, trade suggestions,
  strategy execution, broker behavior, or trading UI.
- The ShellAccounting page must remain read-only and Unavailable-safe until a
  separate real data task is authorized.

## TASK-134 ShellAccounting Real Data Adapter Gate

- Real data adapter tasks must reference
  `docs/78_shell_accounting_real_data_adapter_gate.md` and
  `docs/79_shell_accounting_real_data_adapter_test_plan.md`.
- Do not implement a DataService read-only action without explicit
  authorization.
- The real adapter is not write path and does not authorize TradeDraft,
  strategy execution, broker order, trade suggestion, or trading UI.
- Do not add QML direct DataServiceClient access, presenter direct SQLite
  access, presenter direct AccountingEngine access, DataAccess bypasses, raw
  SQL logging, sensitive payload exposure, or write-enabled actions.

## TASK-135 ShellAccounting Real Data Adapter Implementation

- Real data adapter implementation tasks must reference
  `docs/78_shell_accounting_real_data_adapter_gate.md`,
  `docs/79_shell_accounting_real_data_adapter_test_plan.md`, and
  `docs/80_shell_accounting_real_data_adapter_implementation.md`.
- TASK-135 authorizes only `position.list`, `cash.summary`,
  `portfolio.pnl.summary`, `base_position.summary`, and `sniper_pool.summary`.
- Read-only adapter integration is not a write path and does not authorize
  TradeDraft, trade suggestions, strategy execution, broker order, or trading UI.
- QML must not directly call `DataServiceClient`; presenter/controller must not
  directly access SQLite, DataAccess, or AccountingEngine.
- No raw SQL, full trade_log payload, raw transport payload, or sensitive raw
  payload may be exposed to QML.

## TASK-136 ShellAccounting DataService Facts Query Gate

- DataService facts query tasks must reference
  `docs/81_shell_accounting_dataservice_readonly_facts_query_gate.md` and
  `docs/82_shell_accounting_dataservice_readonly_facts_query_test_plan.md`.
- Do not implement SQLite facts query without explicit authorization.
- facts query is not write path.
- facts query is not AccountingEngine replay.
- No TradeDraft / no trading UI.

## TASK-137 ShellAccounting DataService Facts Query Implementation

- DataService facts query implementation tasks must reference
  `docs/81_shell_accounting_dataservice_readonly_facts_query_gate.md`,
  `docs/82_shell_accounting_dataservice_readonly_facts_query_test_plan.md`, and
  `docs/83_shell_accounting_dataservice_readonly_facts_query_implementation.md`.
- Preserve the DataService-only read boundary. Shell, Presenter, and QML must
  not bypass DataService to access SQLite or DataAccess.
- Authorized facts queries are SELECT-only and limited to `position.list`,
  `cash.summary`, `portfolio.pnl.summary`, `base_position.summary`, and
  `sniper_pool.summary`.
- Do not call AccountingEngine replay, `replayFromFacts`, snapshot rebuild,
  correction, reversal, TradeDraft generation, trade suggestion, broker order,
  strategy execution, or any write-enabled action without a separate task.

## TASK-138 ShellAccounting Replay Snapshot Rebuild Gate

- Future replay / rebuild tasks must reference
  `docs/84_shell_accounting_replay_snapshot_rebuild_gate.md` and
  `docs/85_shell_accounting_replay_snapshot_rebuild_test_plan.md`.
- TASK-138 is a gate only. It does not authorize AccountingEngine replay,
  `replayFromFacts`, snapshot rebuild, snapshot writes, TradeDraft generation,
  trade suggestions, strategy execution, broker order, or trading UI.
- Do not add direct Shell / QML / Presenter AccountingEngine access.
- Do not add direct Shell / QML / Presenter SQLite or DataAccess access.
- Do not add snapshot write without explicit authorization.
- The TASK-137 read-only facts query remains the fallback if replay or rebuild
  is disabled in a future implementation.

## TASK-139 ShellAccounting Read-only Replay Implementation

- TASK-139 implementation tasks must reference
  `docs/86_shell_accounting_readonly_replay_implementation.md`.
- Read-only replay may occur only inside the DataService boundary.
- Shell, QML, Presenter, and ShellServices must not call AccountingEngine,
  SQLite, or DataAccess directly.
- Snapshot rebuild/write remains unimplemented and requires separate explicit
  authorization.
- Read-only replay is not TradeDraft, trade suggestion, strategy execution,
  broker order, trading UI, or any write path.

## TASK-140 ShellAccounting Snapshot Rebuild Write Gate

- Future snapshot rebuild/write tasks must reference
  `docs/87_shell_accounting_snapshot_rebuild_write_gate.md` and
  `docs/88_shell_accounting_snapshot_rebuild_write_test_plan.md`.
- Do not add direct Shell / QML snapshot write or rebuild triggers.
- Do not add snapshot write without explicit authorization.
- Do not add generic write-enabled escape hatches through DataServiceApi or
  DataServiceClient.
- Do not add TradeDraft, trade suggestion, strategy execution, broker order, or
  trading UI.
- TASK-139 read-only replay remains the fallback until a separate
  snapshot rebuild/write implementation is authorized.

## TASK-141 ShellAccounting Snapshot Rebuild Implementation Gate

- Future snapshot rebuild implementation tasks must reference
  `docs/89_shell_accounting_snapshot_rebuild_implementation_gate.md` and
  `docs/90_shell_accounting_snapshot_rebuild_implementation_test_plan.md`.
- Snapshot rebuild must be DataService-only, use TASK-139 read-only replay
  input, and produce only an in-memory derived snapshot preview unless a later
  task explicitly authorizes snapshot write.
- Do not add direct Shell / QML / Presenter snapshot rebuild triggers.
- Do not add snapshot write, database write, TradeDraft generation, trade
  suggestion, strategy execution, broker order, or trading UI.
- TASK-139 read-only replay remains the fallback until a separate snapshot
  rebuild implementation is authorized and completed.

## TASK-142 ShellAccounting Snapshot Rebuild Preview Implementation

- Snapshot rebuild preview is implemented as a DataService-internal read-only
  mapping from TASK-139 read-only replay to `snapshotRebuildPreview`.
- Do not treat preview as snapshot write. `snapshotRebuilt` must remain false
  until a separate write task is authorized.
- Do not add production QML/startup changes, direct Shell/QML/Presenter
  SQLite/DataAccess/AccountingEngine access, TradeDraft generation, trade
  suggestion, strategy execution, broker order, or trading UI.
- Future snapshot write tasks must reference TASK-140 through TASK-142 docs and
  require separate explicit authorization.

## TASK-143 ShellAccounting Snapshot Write Authorization Gate

- Future snapshot write tasks must reference
  `docs/92_shell_accounting_snapshot_write_authorization_gate.md` and
  `docs/93_shell_accounting_snapshot_write_authorization_test_plan.md`.
- Snapshot write implementation must be a separate explicitly authorized TASK.
- Snapshot write input must come only from TASK-142 `snapshotRebuildPreview`.
- Snapshot write must be DataService-only, table-allowlisted, transactional,
  idempotent, rollback-ready, and disableable.
- Do not write `trade_log`, `trade_execution_group`, `trade_draft`, or
  `audit_log` unless a future task separately authorizes the specific write.
- Do not add production QML/startup changes, direct Shell/QML/Presenter write
  triggers, schema migration, TradeDraft generation, trade suggestion, strategy
  execution, broker order, or trading UI.

## TASK-144 ShellAccounting Snapshot Write Implementation

- Snapshot write is authorized only as `accounting.snapshot.write` inside the
  DataService boundary.
- Input must come from TASK-142 `snapshotRebuildPreview` with explicit
  `TASK-144_SNAPSHOT_WRITE` authorization.
- The only allowed write tables are `cash_snapshot`, `position_snapshot`, and
  `portfolio_summary`.
- Do not write `trade_log`, `trade_execution_group`, `trade_draft`, or
  `audit_log`.
- Keep transaction rollback, idempotency, duplicate handling, privacy, and
  no-partial-write tests in place.
- Do not add production QML/startup changes, Shell/QML/Presenter write triggers,
  schema migration, TradeDraft generation, trade suggestion, strategy execution,
  broker order, or trading UI.

## TASK-145 ShellAccounting Audit Write Authorization Gate

- Future audit write tasks must reference
  `docs/95_shell_accounting_audit_write_authorization_gate.md` and
  `docs/96_shell_accounting_audit_write_authorization_test_plan.md`.
- Audit write implementation must be a separate explicitly authorized TASK.
- Audit input must come only from an authorized DataService write action result;
  the current future candidate source is TASK-144 snapshot write result.
- Audit write must be DataService-only, sanitized, transactional or explicitly
  scoped by the future task, idempotent, rollback-ready, and disableable.
- Do not write `audit_log`, `trade_log`, `trade_execution_group`, or
  `trade_draft` in a gate-only task.
- Do not expose raw SQL, full trade_log payloads, full snapshot payloads, or
  internal stack traces in audit payloads or QML.
- Do not add production QML/startup changes, Shell/QML/Presenter audit write
  triggers, schema migration, TradeDraft generation, trade suggestion, strategy
  execution, broker order, or trading UI.

## TASK-146 ShellAccounting Audit Write Implementation

- Audit write is authorized only as `accounting.audit.write` inside the
  DataService boundary.
- Input must come from TASK-144 snapshot write result with explicit
  `TASK-146_AUDIT_WRITE` authorization.
- The only allowed write table is `audit_log`.
- Do not write `trade_log`, `trade_execution_group`, or `trade_draft`.
- Keep sanitized payload, transaction rollback, idempotency, duplicate handling,
  privacy, and no-partial-audit-write tests in place.
- Do not add production QML/startup changes, Shell/QML/Presenter audit triggers,
  schema migration, TradeDraft generation, trade suggestion, strategy execution,
  broker order, or trading UI.

## TASK-147 ShellAccounting TradeDraft Authorization Gate

- Future TradeDraft tasks must reference
  `docs/98_shell_accounting_tradedraft_authorization_gate.md` and
  `docs/99_shell_accounting_tradedraft_authorization_test_plan.md`.
- TradeDraft implementation must be a separate explicitly authorized TASK.
- TradeDraft is not trade execution, not strategy execution, and not broker
  order.
- Future TradeDraft must be DataService-only, explicitly authorized, sanitized,
  audited, idempotent, rollback-ready, and disableable.
- Do not write `trade_draft`, `trade_log`, or `trade_execution_group` in a
  gate-only task.
- Do not add production QML/startup changes, Shell/QML/Presenter draft write
  triggers, schema migration, strategy execution, broker order, trade execution,
  or trading UI.

## TASK-148 ShellAccounting TradeDraft Implementation

- TradeDraft creation is authorized only as `accounting.tradedraft.create`
  inside the DataService boundary.
- The required authorization token is `TASK-148_TRADEDRAFT_WRITE`.
- TradeDraft writes are limited to `trade_draft` and a sanitized `audit_log`
  event; audit failure must roll back the draft.
- TradeDraft is not trade execution, not broker order, not strategy execution,
  and not user confirmation.
- Do not write `trade_log` or `trade_execution_group`.
- Do not add production QML/startup changes, Shell/QML/Presenter draft write
  triggers, schema migration, broker order, strategy execution, trade
  confirmation, or trading UI.

## TASK-149 ShellAccounting TradeDraft Confirmation Authorization Gate

- Future confirmation tasks must reference
  `docs/101_shell_accounting_tradedraft_confirmation_authorization_gate.md`
  and
  `docs/102_shell_accounting_tradedraft_confirmation_authorization_test_plan.md`.
- TradeDraft confirmation implementation must be a separate explicitly
  authorized TASK.
- Confirmation is the high-risk boundary from draft to `trade_log`; it is not
  broker order, not automatic trading, and not strategy execution.
- Do not write `trade_log` or `trade_execution_group` in a gate-only task.
- Do not modify TASK-148 TradeDraft creation behavior, production QML/startup,
  schema, broker/order code, StrategyEngine behavior, or DataServiceClient
  transport/API behavior.

## TASK-150 ShellAccounting TradeDraft Confirmation Implementation

- TradeDraft confirmation is authorized only as
  `accounting.tradedraft.confirm` inside the DataService boundary.
- The required authorization token is `TASK-150_TRADEDRAFT_CONFIRM`.
- Confirmation input must reference an existing unconfirmed TradeDraft and must
  validate the persisted draft before writing ledger facts.
- Confirmation may atomically write `trade_execution_group`, `trade_log`,
  sanitized `audit_log`, and the confirmed `trade_draft` status / confirmation
  metadata only.
- Audit failure must roll back `trade_execution_group`, `trade_log`, and
  `trade_draft` status changes.
- Duplicate confirmation must not create duplicate `trade_log` rows.
- Confirmation is not broker order, not strategy execution, not automatic
  trading, and not production trading UI.
- Do not modify production QML/startup, schema, broker/order code,
  StrategyEngine behavior, or DataServiceClient transport/API behavior.

## TASK-151 ShellAccounting Production Trading UI Authorization Gate

- Future production trading UI tasks must reference
  `docs/104_shell_accounting_production_trading_ui_authorization_gate.md` and
  `docs/105_shell_accounting_production_trading_ui_authorization_test_plan.md`.
- TASK-151 is a gate only. It does not authorize production QML edits, trading
  pages, buy / sell / order / confirm buttons, or QML bindings to
  `accounting.tradedraft.create` / `accounting.tradedraft.confirm`.
- Production UI may only route through presenter/controller/adapter/DataService
  boundaries. QML must not directly call DataServiceClient, SQLite, DataAccess,
  `trade_draft`, `trade_log`, or `trade_execution_group` writes.
- TradeDraft create must keep `TASK-148_TRADEDRAFT_WRITE`; TradeDraft confirm
  must keep `TASK-150_TRADEDRAFT_CONFIRM`.
- Future UI must keep draft creation and confirmation as separate explicit user
  steps and must show draft-only versus confirmed-ledger state.
- Do not add broker order placement, StrategyEngine execution, automatic
  trading, direct QML DB writes, generic write escape hatches, or silent
  success behavior.

## TASK-152 ShellAccounting Production Trading UI Implementation

- Production trading UI is authorized only for Draft creation, Draft review,
  and Draft confirmation through the ShellAccounting
  presenter/controller/adapter/DataService boundary.
- QML must not directly call DataServiceClient, SQLite, DataAccess, or database
  write paths.
- Draft creation must continue to use `TASK-148_TRADEDRAFT_WRITE`; Draft
  confirmation must continue to use `TASK-150_TRADEDRAFT_CONFIRM`.
- Confirmation requires an explicit user step and must show Draft-only versus
  confirmed-ledger state.
- Do not extend this UI into broker order placement, StrategyEngine execution,
  automatic trading, direct order placement, schema migration, or a generic
  write escape hatch.
- Future broker order / strategy execution tasks must be separately authorized.

## TASK-153 ShellAccounting Broker Order Authorization Gate

- Future broker order tasks must reference
  `docs/107_shell_accounting_broker_order_authorization_gate.md` and
  `docs/108_shell_accounting_broker_order_authorization_test_plan.md`.
- TASK-153 is a gate only. It does not authorize broker order implementation,
  broker adapters, broker SDK calls, StrategyEngine execution, automatic
  trading, real order placement, production QML changes, production startup
  changes, or schema migration.
- Future broker order must be DataService-only, based on an already confirmed
  TradeDraft / ledger fact, independently authorized, explicitly user-confirmed,
  audited with sanitized payload, dry-run/rollback-ready, and disableable.
- Broker order must not accept raw QML payload, must not bypass TASK-148 Draft
  creation or TASK-150 Draft confirmation, and must not auto-trigger after
  confirmation.

## TASK-154 ShellAccounting Broker Order Dry-run Implementation

- Broker dry-run is authorized only as `accounting.broker_order.dry_run` inside
  the DataService boundary.
- The required authorization token is `TASK-154_BROKER_ORDER_DRY_RUN`.
- Input must reference an already confirmed TradeDraft / ledger fact and must
  not come from raw QML order payload.
- The only allowed write table is `audit_log`, with sanitized dry-run audit
  payload.
- Do not write `trade_log`, `trade_execution_group`, `trade_draft`, or broker
  real order status tables.
- Do not call broker SDK, place a real order, execute strategy, enable
  automatic trading, modify production QML/startup, or change schema.
- Future real broker order placement must be separately authorized.

## TASK-155 ShellAccounting Real Broker Order Authorization Gate

- Future real broker order tasks must reference
  `docs/110_shell_accounting_real_broker_order_authorization_gate.md` and
  `docs/111_shell_accounting_real_broker_order_authorization_test_plan.md`.
- TASK-155 is a gate only. It does not authorize real broker order
  implementation, broker adapters, broker SDK calls, sandbox broker calls,
  paper trading broker calls, real order placement, StrategyEngine execution,
  automatic trading, production QML changes, production startup changes, or
  schema migration.
- Future real broker order must require successful TASK-154 dry-run preflight,
  confirmed-ledger input, final user confirmation, an independent authorization
  token, kill switch, idempotency key, sanitized audit, reconciliation policy,
  and rollback / incident containment.
- Real broker order must not accept raw QML payload, must not bypass
  DataService, must not be triggered directly by Presenter or QML, and must not
  execute strategy or automatic trading.

## TASK-156 ShellAccounting Real Broker Order Implementation Gate

- Future real broker order implementation tasks must reference
  `docs/112_shell_accounting_real_broker_order_implementation_gate.md` and
  `docs/113_shell_accounting_real_broker_order_implementation_test_plan.md`.
- TASK-156 is a gate only. It does not authorize real broker order
  implementation, broker adapter implementation, broker SDK import/link/call,
  sandbox broker call, paper broker call, broker network call, real order
  placement, StrategyEngine execution, automatic trading, production QML
  changes, production startup changes, credentials, or schema migration.
- Future implementation must remain DataService-only, default-disabled,
  independently authorized, dry-run-preflighted, final-user-confirmed,
  risk-guarded, idempotent, audited, reconciled, kill-switchable, and
  incident-containment-ready.
- Future implementation must not accept raw QML order payloads, must not bypass
  TASK-154 dry-run, TASK-150 confirmed ledger input, or TASK-152 user
  confirmation, and must not introduce credentials or broker network calls into
  tests.

## TASK-157 ShellAccounting Broker Adapter Interface Scaffold

- Future broker adapter tasks must reference
  `docs/114_shell_accounting_broker_adapter_interface_scaffold.md`.
- TASK-157 is a scaffold only. It permits abstract DataServiceApi interface /
  DTOs, disabled/null port behavior, and test-only fake coverage.
- Do not implement a real broker adapter, import/link/call a broker SDK, call
  sandbox / paper / real broker endpoints, place orders, add credentials,
  modify production QML/startup, change TASK-148 / TASK-150 / TASK-152 /
  TASK-154 behavior, execute strategy, enable automatic trading, or modify
  schema.
- Keep concrete future broker SDK types behind DataService, never in QML,
  Shell, Presenter, ShellServices, ShellCore, DataAccess, tests, or docs.
## TASK-158 Broker Adapter Implementation Gate Prompt Rule

Future ShellAccounting broker adapter implementation tasks must reference
docs/115 and docs/116. Do not implement a concrete broker adapter, import a
broker SDK, call broker network, place a real order, add credentials/secrets, or
enable strategy execution / automatic trading unless the new task explicitly
authorizes that scope. Keep broker adapter access DataService-only, default
disabled, mode-isolated across fake test-only / sandbox / paper / real, and
rollback / kill-switch ready. QML, Presenter, ShellServices, and ShellCore must
not directly call broker adapter code.

## TASK-159 Broker Adapter Disabled Wiring Prompt Rule

Future broker adapter work must preserve TASK-159's default disabled/null
wiring until a later task explicitly authorizes sandbox, paper, or real adapter
behavior. Do not expose adapter wiring to QML / Presenter / Shell, do not
replace the disabled provider with a live adapter, do not add broker SDK,
network calls, credentials, real broker order ids, real placement, strategy
execution, automatic trading, or schema changes unless the task explicitly
authorizes that scope.

## TASK-160 Broker Sandbox Adapter Authorization Prompt Rule

Future sandbox broker adapter tasks must reference docs/118 and docs/119. Keep
disabled/null as the default mode, keep sandbox / paper / real modes isolated,
keep fake adapters test-only, and keep broker adapter access DataService-only.
Do not commit real credentials, secrets, tokens, keys, password values, usable
examples, broker SDKs, broker endpoints, broker network calls, real order ids,
real order placement, reconciliation, cancellation, correction, strategy
execution, or automatic trading unless a later task explicitly authorizes that
specific implementation scope.

## TASK-161 Broker Sandbox Adapter Scaffold Prompt Rule

Future sandbox scaffold or adapter tasks must reference docs/120 and docs/121.
The TASK-161 scaffold is not a usable broker adapter and must not become the
default provider without a separate authorization task. Preserve the TASK-159
disabled/null default and TASK-154 dry-run behavior. Do not add broker SDKs,
broker network calls, credential reads, usable credential examples, real broker
order ids, order placement, database writes, audit writes, reconciliation,
cancellation, correction, strategy execution, automatic trading, production
QML/startup exposure, or schema changes unless a later task explicitly
authorizes that exact scope.

## TASK-162 Broker Sandbox Mode Selector Prompt Rule

Future sandbox mode selector or factory tasks must reference docs/122 and
docs/123. Keep the selector disabled-default and do not treat selector presence
as runtime broker enablement. Sandbox mode may select only the TASK-161 scaffold
until a later task authorizes usable sandbox behavior. Unknown, empty, paper,
real, or unsupported modes must fail closed. Do not wire selector into
production QML, startup, Presenter, Controller, or DataServiceActions runtime
behavior unless explicitly authorized. Do not add broker SDKs, network calls,
credential reads, usable credential examples, real broker order ids, order
placement, database writes, audit writes, reconciliation, cancellation,
correction, strategy execution, automatic trading, or schema changes unless a
later task explicitly authorizes that exact scope.

## TASK-163 Broker Sandbox Runtime Selector Authorization Prompt Rule

Future broker sandbox runtime selector wiring tasks must reference docs/124 and
docs/125. TASK-163 itself is gate-only and does not connect the selector to
`DataServiceActions` runtime. Keep runtime wiring separately authorized,
DataService-only, default-disabled, CI no-network, credential-isolated, and
fail-closed for empty / unknown / unsupported / paper / real modes. Do not add
runtime mode sources, credentials injection, broker SDKs, network calls,
endpoint values, real broker order ids, order placement, database writes, audit
writes, ledger writes, reconciliation, cancellation, correction, strategy
execution, automatic trading, production QML/startup/Presenter exposure, or
schema changes unless a later task explicitly authorizes that exact scope.

## TASK-196 Manual Cash Movement Repository Dual-Write Prompt Rule

Future manual cash movement follow-up work must reference docs/188 and
docs/189 after TASK-196. TASK-196 authorizes only DataAccess-only repository
dual-write for Deposit / Withdrawal via `trade_log` + `cash_adjustment` direct
tests. It does not authorize DataServiceActions runtime write wiring, QML,
startup, Presenter, Controller, replay/read-model, audit write, ledger write,
broker SDK, network, credentials, endpoints, real order placement, or automatic
trading.

Under the current schema, cash_adjustment-only write remains invalid because
`cash_adjustment.trade_log_id` is `NOT NULL` and references `trade_log(id)`.
Any future DataService write action, UI exposure, replay integration, audit
policy, ledger policy, standalone cash_adjustment schema change, broker work,
or automatic trading must be separately authorized.

## TASK-197 Manual Entry DataService Write Wiring Authorization Gate Prompt Rule

Future manual entry DataService write wiring work after TASK-197 must reference
docs/190 and docs/191. TASK-197 is an authorization gate only. It does not
authorize modifying `DataServiceActions.cpp`, `DataServiceActions.h`, or
`DataServiceActionRegistrar.cpp`, wiring DataAccess repositories into runtime
DataService actions, executing runtime SQL, writing SQLite rows, returning
persistent ids from DataService actions, changing production QML/startup,
Presenter, Controller, AccountingEngine replay, StrategyEngine, MarketEngine,
TradeDraft or suggestion implementation, broker SDKs, network calls,
credentials, endpoints, real order placement, or automatic trading.

Future DataService write wiring must be a separate TASK, remain
validation-first, call TASK-192 `ShellAccountingManualTransactionRepository` or
TASK-196 `ShellAccountingManualCashMovementRepository` only through DataAccess
repository boundaries, avoid scattered SQL in DataServiceActions, preserve
repository idempotency / rollback semantics, and return sanitized local write
results with safe error mapping. Future replay/read-model/UI, audit write,
ledger policy, broker, reconciliation, cancellation, correction, and automatic
trading work must remain separately authorized. Broker sandbox new capability
development remains paused and existing broker / real broker / no-network /
no-credentials / no-order-placement gates must remain present and passing.

## TASK-165 Broker Runtime Mode Source Authorization Gate Prompt Rule

Future broker runtime mode source tasks must reference docs/128 and docs/129.
TASK-165 itself is gate-only: do not implement runtime mode source selection,
do not enable sandbox runtime, do not change DataServiceActions disabled-default
wiring, and do not read broker mode from payload, QML, config, environment,
command line, files, database, or secret stores. Do not add broker SDK,
network, credentials, endpoint, real broker order id, order placement,
database/audit/ledger write, reconciliation, cancellation, correction, strategy
execution, or automatic trading without separate explicit authorization.

## TASK-167 Broker Sandbox Runtime Mode Source Authorization Gate Prompt Rule

Future sandbox runtime mode source work must reference docs/132 and docs/133.
TASK-167 itself is gate-only: do not implement a sandbox runtime mode source,
do not enable sandbox runtime, do not change the TASK-166 disabled-only source,
and do not read broker mode from payload, QML, config, environment, command
line, files, database, or secret stores. A future mode source may only select
mode and must not read credentials, endpoints, account data, order data, or
broker payloads. Do not add SDK, network, real credentials, endpoint values,
real broker order id, order placement, DB/audit/ledger write, reconciliation,
cancellation, correction, strategy execution, or automatic trading without
separate explicit authorization.

## TASK-168 Broker Sandbox Runtime Mode Source Scaffold Prompt Rule

Future sandbox runtime mode source work after TASK-168 must reference docs/134
and docs/135. TASK-168 authorizes only a directly testable scaffold class that
returns sandbox mode; it does not authorize runtime enablement. Keep
`defaultShellAccountingBrokerRuntimeModeSource()` disabled-only, keep
`DataServiceActions` unwired from the sandbox scaffold, and do not read broker
mode from payload, QML, config, environment, command line, files, database, or
secret stores. Do not add SDK, network, credentials, endpoint values, real
broker order id, order placement, database/audit/ledger writes, reconciliation,
cancellation, correction, strategy execution, or automatic trading without a
separate explicit task.

## TASK-169 Broker Sandbox Runtime Mode Source Selector Authorization Prompt Rule

Future sandbox runtime mode source selector work must reference docs/136 and
docs/137. TASK-169 itself is gate-only. TASK-170 later authorizes only a
direct-test selector scaffold in `ShellAccountingBrokerRuntimeModeSource.h/.cpp`;
do not wire it into `DataServiceActions`, and do not enable sandbox runtime.
Keep the TASK-168 sandbox scaffold direct-test-only and keep the default runtime
source disabled-only. A selector must not read mode from payload, QML, config,
environment, command line, files, database, or secret stores, and must not carry
credentials, endpoints, account data, order data, or broker payloads. Do not add
SDK, network, real credentials, endpoint values, real broker order id, order
placement, DB/audit/ledger write, reconciliation, cancellation, correction,
strategy execution, or automatic trading without a separate explicit task.

## TASK-170 Broker Sandbox Runtime Mode Source Selector Scaffold Prompt Rule

Future sandbox runtime mode source selector work after TASK-170 must reference
docs/138 and docs/139. TASK-170 authorizes only
`shellAccountingBrokerRuntimeModeSourceForMode(ShellAccountingBrokerOrderPortMode)`
as a direct-test scaffold. It may select disabled or the TASK-168 sandbox
scaffold from an explicit enum and must fail closed to disabled for unsupported
or unknown modes. Do not read mode from payload, QML, config, environment,
command line, files, database, or secret stores. Do not wire the selector into
`DataServiceActions`, do not change `defaultShellAccountingBrokerRuntimeModeSource()`,
and do not enable sandbox runtime. SDK, network, credentials, endpoints, real
order ids, order placement, DB/audit/ledger writes, reconciliation,
cancellation, correction, strategy execution, automatic trading, and schema
changes require separate explicit authorization.

## TASK-171 Broker Sandbox Runtime Mode Source Selector Wiring Authorization Gate Prompt Rule

Future DataServiceActions selector wiring work after TASK-171 must reference
docs/140 and docs/141. TASK-171 is gate-only: it does not implement runtime
wiring, does not modify `DataServiceActions`, does not modify
`ShellAccountingBrokerRuntimeModeSource.h/.cpp`, and does not enable sandbox
runtime. A future wiring task may only start with disabled-default selector
wiring unless another explicit TASK authorizes sandbox runtime mode source
enablement. Do not read mode from payload, QML, config, environment, command
line, files, database, or secret stores. Do not add SDK, network, credentials,
endpoints, real broker order ids, order placement, DB/audit/ledger writes,
reconciliation, cancellation, correction, strategy execution, automatic
trading, or schema changes without separate explicit authorization.

## TASK-172 Broker Sandbox Runtime Mode Source Selector Disabled Wiring Prompt Rule

Future broker runtime mode source work after TASK-172 must reference docs/142
and docs/143. TASK-172 authorizes only disabled-default selector wiring in
`DataServiceActions`: read `defaultShellAccountingBrokerRuntimeModeSource()
.brokerOrderPortMode()`, pass that value to
`shellAccountingBrokerRuntimeModeSourceForMode(defaultBrokerPortMode)`, then
use the selected source mode for `shellAccountingBrokerOrderPortForMode`.
Do not pass Sandbox, Paper, Real, payload fields, QML state, config,
environment, command-line values, files, database values, or secret-store values
into the selector. Do not enable sandbox runtime. SDK, network, credentials,
endpoints, real broker order ids, order placement, DB/audit/ledger writes,
reconciliation, cancellation, correction, strategy execution, automatic
trading, and schema changes require separate explicit authorization.

## TASK-173 Broker Sandbox Runtime Mode Enablement Authorization Gate Prompt Rule

Future sandbox runtime mode enablement work after TASK-173 must reference
docs/144 and docs/145. TASK-173 is gate-only: do not modify
`DataServiceActions.cpp`, do not modify `ShellAccountingBrokerRuntimeModeSource.h/.cpp`,
and do not enable sandbox runtime. Keep TASK-172 disabled-default selector
wiring driven only by the default disabled source. Do not add external mode
sources, credentials providers, endpoint providers, sandbox adapters, broker
SDK, network, real order placement, real broker order id storage,
reconciliation, cancellation, correction, strategy execution, automatic
trading, or schema changes without separate explicit authorization.

## TASK-174 Broker Sandbox Runtime Enablement State Scaffold Prompt Rule

Future sandbox runtime enablement state work after TASK-174 must reference
docs/146 and docs/147. TASK-174 authorizes only a direct-test-only
disabled/unavailable/fail-closed state scaffold. Do not modify
`DataServiceActions.cpp`, do not modify `ShellAccountingBrokerRuntimeModeSource.h/.cpp`,
do not connect the scaffold to runtime, and do not enable sandbox runtime. Do
not add external mode sources, credentials or endpoint reads, broker SDK,
network, real order placement, real broker order id storage, DB/audit/ledger
writes, reconciliation, cancellation, correction, strategy execution, automatic
trading, or schema changes without separate explicit authorization.

## TASK-175 Broker Sandbox Runtime Enablement State Wiring Authorization Gate Prompt Rule

Future sandbox runtime enablement state wiring work after TASK-175 must
reference docs/148 and docs/149. TASK-175 is gate-only: do not modify
`DataServiceActions.cpp`, do not modify
`ShellAccountingBrokerSandboxRuntimeEnablement.h/.cpp`, do not connect the
enablement state to runtime, and do not enable sandbox runtime. A future wiring
implementation must be separately authorized and may only start by reading the
default disabled/fail-closed state while preserving disabled/null provider
behavior. Do not add external mode sources, credentials or endpoint reads,
broker SDK, network, real order placement, real broker order id storage,
DB/audit/ledger writes, reconciliation, cancellation, correction, strategy
execution, automatic trading, or schema changes without separate explicit
authorization.

## TASK-176 Broker Sandbox Runtime Enablement State Disabled Wiring Prompt Rule

Future broker sandbox runtime enablement work after TASK-176 must reference
docs/150 and docs/151. TASK-176 only permits `DataServiceActions.cpp` to read
`defaultShellAccountingBrokerSandboxRuntimeEnablementState()` as a disabled,
unavailable, fail-closed guard. It must not write that state into the response,
must not add request or response fields, must not enable sandbox runtime, and
must not change `brokerPortMode`, `brokerPortDisabled`,
`brokerPortDryRunOnly`, or `BROKER_ORDER_DISABLED` behavior. Keep selector
runtime driven by the disabled default source. Do not add external mode sources,
payload/QML/config/env/CLI/file/DB/secret-store mode reads, credentials,
endpoints, broker SDK, network, real broker order ids, order placement,
DB/audit/ledger writes, reconciliation, cancellation, correction, strategy
execution, automatic trading, or schema changes without separate explicit
authorization.

## TASK-177 Manual Transaction and Cash Movement MVP Authorization Gate Prompt Rule

Future local assisted decision MVP work after TASK-177 must reference docs/152
and docs/153. broker sandbox new capability development is paused, while
existing broker, real-broker, no-network, no-credentials, no-order-placement,
disabled-runtime, and sandbox-disabled gates must keep passing. The MVP
priority is manual buy/sell records, deposit/withdraw records,
TradeLog/cash-facts/position-replay stability, daily holdings/cash/PnL page
usability, index retracement golden-tower and sniper suggestions,
on-exchange/off-exchange buy/sell suggestions, and local backup/export/
verification/package readiness. Do not add production QML, startup
registration, Presenter/Controller behavior, DataServiceActions manual entry
actions, DataAccess manual write repositories, AccountingEngine manual replay
behavior, StrategyEngine or MarketEngine implementation, TradeDraft or
suggestion implementation, broker SDK, network, credentials, endpoints, real
broker order ids, order placement, automatic trading, or schema changes without
separate explicit authorization.

## TASK-178 Manual Transaction and Cash Movement DTO Validation Scaffold Prompt Rule

Future manual transaction and cash movement work after TASK-178 must reference
docs/154 and docs/155. TASK-178 authorizes only in-memory DTO / validation
scaffold in DataServiceApi. This is an in-memory DTO / validation scaffold
boundary. Do not add DataService actions, do not modify
`DataServiceActions.cpp`, do not add DataAccess write repositories, do not write
SQLite, cash facts, audit, ledger, `trade_log`, or snapshots, do not modify
AccountingEngine replay, do not modify production QML, startup, Presenter, or
Controller behavior, and do not modify migrations. Follow-up TASKs must
separately authorize DataService actions, repositories, AccountingEngine replay
integration, and UI. Broker sandbox new capability development remains paused;
do not add broker SDK, network, credentials, endpoints, real broker order ids,
real order placement, strategy execution, automatic trading, reconciliation,
cancellation, correction, or schema changes without separate explicit
authorization.

## TASK-179 Manual Entry DataService Action Authorization Gate Prompt Rule

Future manual entry DataService action work after TASK-179 must reference
docs/156 and docs/157. TASK-179 was a DataService action authorization gate
only. TASK-180 evolves that rule by allowing only disabled DataService action
scaffold registration; it still does not authorize write implementation,
repositories, replay integration, read model updates, or UI. Future manual
buy/sell and deposit/withdraw actions must remain DataService-only, must
validate through the TASK-178 DTO / validation scaffold or an authorized
successor, and must keep repository, AccountingEngine replay, read model, and
UI changes in separate authorized tasks. Do not write SQLite, `trade_log`, cash
facts, cash ledger, audit, or other ledger rows, do not add DataAccess write
repositories, do not modify production QML, startup, Presenter, Controller,
migrations, StrategyEngine, or MarketEngine, and do not add TradeDraft, trading
suggestions, broker SDK, network, credentials, endpoints, real broker order
ids, real order placement, automatic trading, reconciliation, cancellation,
correction, or schema changes without separate explicit authorization. Broker
sandbox new capability development remains paused and existing broker / real
broker / no-network / no-credentials / no-order-placement gates must remain
present and passing.

## TASK-180 Manual Entry DataService Action Scaffold Prompt Rule

Future work after TASK-180 must reference docs/158 and docs/159. TASK-180 is a
manual entry DataService action scaffold only: it may register
`accounting.manual_transaction.create` and
`accounting.manual_cash_movement.create`, but each handler must return
unavailable / not implemented / disabled scaffold semantics. Do not implement
manual entry writes, do not add DataAccess write repositories, do not write
SQLite, `trade_log`, cash facts, audit, or ledger records, do not modify
migrations, production QML, startup, Presenter, Controller, AccountingEngine
replay, StrategyEngine, MarketEngine, TradeDraft, suggestions, broker SDK,
network, credentials, endpoint, real order placement, or automatic trading.
Broker sandbox new capability development remains paused and existing broker
gates must remain present and passing.

TASK-180 allows disabled DataService action scaffold registration only.

## TASK-181 Manual Entry DataService Action Implementation Authorization Gate Prompt Rule

Future work after TASK-181 must reference docs/160 and docs/161. TASK-181 is a
future implementation authorization gate only. Do not modify
`DataServiceActions.cpp`, `DataServiceActions.h`, or
`DataServiceActionRegistrar.cpp` unless a later implementation task explicitly
authorizes it. Do not implement manual entry writes, do not parse payloads into
persistent facts, do not add DataAccess write repositories, do not write SQLite,
`trade_log`, cash facts, audit, or ledger records, and do not modify migrations,
production QML, startup, Presenter, Controller, AccountingEngine replay,
StrategyEngine, MarketEngine, TradeDraft, suggestions, broker SDK, network,
credentials, endpoints, real order placement, or automatic trading.

After TASK-182, the TASK-180 scaffold actions may parse payloads and run
TASK-178 validators, but only as validation-only unavailable /
write-not-implemented responses. Repository, persistence, replay, read model,
and UI work must each remain separately authorized. Broker sandbox new
capability work remains paused and existing broker disabled / fail-closed /
no-real-order / no-network / no-credentials / no-order-placement gates must
remain present and passing.

## TASK-182 Manual Entry DataService Action Validation Wiring Prompt Rule

Future work after TASK-182 must reference docs/162 and docs/163. TASK-182
authorizes only payload parsing plus TASK-178 validation wiring for
`accounting.manual_transaction.create` and
`accounting.manual_cash_movement.create`. Valid payloads may be marked
`validationAccepted=true`, but the actions must still return write not
implemented / unavailable and must not report persistence success or persistent
ids.

Do not add DataAccess write repositories, SQLite writes, `trade_log`, cash
facts, cash ledger, audit, ledger writes, schema changes, production QML,
startup, Presenter, Controller, AccountingEngine replay, StrategyEngine,
MarketEngine, TradeDraft, suggestions, broker SDK, network, credentials,
endpoints, real order placement, or automatic trading without separate explicit
authorization.

## TASK-183 Manual Entry Persistence Authorization Gate Prompt Rule

Future work after TASK-183 must reference docs/164 and docs/165. TASK-183 is a
manual entry persistence authorization gate only. It does not authorize
repository implementation, SQLite writes, `trade_log`, cash facts, cash ledger,
audit, ledger writes, schema changes, replay, read models, production QML,
startup, Presenter, Controller, StrategyEngine, MarketEngine, TradeDraft,
suggestions, broker SDK, network, credentials, endpoints, real order placement,
or automatic trading.

TASK-182 validation wiring must remain validation-only until a later write
implementation task explicitly authorizes persistence. Valid manual transaction
and manual cash movement payloads may pass validation, but must still report
`writeImplemented=false` and must not return persistent ids. Future persistence
must be DataService-only, validation-first, repository-boundary based,
transactional, rollback-capable, idempotent, duplicate-safe, auditable, and
privacy-sanitized. Repository scaffold, repository implementation, manual
transaction write, manual cash movement write, replay/read-model, and UI work
must remain separately authorized. Broker sandbox new capability work remains
paused and existing broker / real broker / no-network / no-credentials /
no-order-placement gates must remain present and passing.

## TASK-184 Manual Entry Repository Scaffold Authorization Gate Prompt Rule

Future work after TASK-184 must reference docs/166 and docs/167. TASK-184 is a
manual entry repository scaffold authorization gate only. It does not authorize
repository scaffold implementation, repository header/source creation,
DataAccess production code changes, DataAccess CMake changes, DataServiceActions
changes, TASK-178 validation code changes, SQLite writes, `trade_log`, cash
facts, cash ledger, audit, ledger writes, schema changes, replay, read models,
production QML, startup, Presenter, Controller, StrategyEngine, MarketEngine,
TradeDraft, suggestions, broker SDK, network, credentials, endpoints, real
order placement, or automatic trading.

Future repository scaffold work must be DataService-only and validation-first,
must define only interface / command / result boundaries, and must keep
repository implementation and write implementation in separately authorized
tasks. Broker sandbox new capability remains paused; existing broker disabled,
real broker, no-network, no-credentials, no-order-placement, and
no-automatic-trading gates must remain retained and passing.

## TASK-185 Manual Entry Repository Scaffold Prompt Rule

Future work after TASK-185 must reference docs/168 and docs/169. TASK-185 adds
only the disabled DataAccess repository scaffold for manual transaction and
manual cash movement persistence. It does not authorize repository write
implementation, DataService action write implementation, SQL execution, SQLite
writes, `trade_log`, cash facts, cash ledger, audit, ledger writes, schema
changes, replay, read models, production QML, startup, Presenter, Controller,
StrategyEngine, MarketEngine, TradeDraft, suggestions, broker SDK, network,
credentials, endpoints, real order placement, or automatic trading.

The scaffold result must remain disabled / write-not-implemented and must not
return persistent ids. TASK-182 validation wiring must continue to return
`writeImplemented=false` until a later DataService action write implementation
task explicitly authorizes persistence.

## TASK-186 Manual Entry Repository Implementation Authorization Gate Prompt Rule

Future work after TASK-186 must reference docs/170 and docs/171. TASK-186 is a
gate-only task for future manual entry DataAccess repository implementation. It
does not authorize repository write implementation, SQL execution, SQLite
writes, schema changes, DataServiceActions changes, QML/startup changes,
AccountingEngine replay changes, broker SDK integration, network calls,
credentials, endpoint configuration, real order placement, or automatic
trading.

Future repository implementation must be separately authorized, must confirm
schema adequacy before writing, must use a repository boundary instead of
scattered action-handler SQL, and must define transaction, rollback,
idempotency, duplicate handling, audit, privacy, and failure atomicity policies.
If schema is insufficient, open a separate schema authorization TASK before
modifying migrations.

## TASK-187 Manual Entry Schema Adequacy Review Gate Prompt Rule

Future work after TASK-187 must reference docs/172 and docs/173. TASK-187 is a
schema adequacy review gate only. It reviews `migrations/001_initial_schema.sql`
as the source of truth for future manual transaction and manual cash movement
repository implementation. It does not authorize schema changes, migrations,
repository implementation, SQL execution, SQLite writes, DataServiceActions
changes, TASK-185 scaffold changes, TASK-178 validation production code
changes, QML/startup changes, AccountingEngine replay changes, StrategyEngine
or MarketEngine changes, TradeDraft or suggestion implementation, broker SDK,
network, credentials, endpoints, real order placement, or automatic trading.
Because TASK-187 found schema gaps, a future schema authorization TASK is
required before write enablement unless a later authorized review documents a
complete lossless mapping using existing fields.

## TASK-188 Manual Entry Schema Gap Authorization Gate Prompt Rule

Future work after TASK-188 must reference docs/174 and docs/175. TASK-188 is a
schema gap authorization gate only. It documents future candidate schema design
for manual transaction request trace / idempotency, manual cash movement
request trace / idempotency, and audit / rollback / privacy support. TASK-188
does not authorize modifying `migrations/001_initial_schema.sql`, adding a
migration, adding a schema file, implementing repository writes, executing SQL,
writing SQLite, changing DataServiceActions, changing TASK-178 validation
production code, changing TASK-185 repository scaffold code, changing
QML/startup, changing AccountingEngine replay, changing StrategyEngine or
MarketEngine, adding TradeDraft or suggestion implementation, adding broker
SDK, adding network calls, adding credentials or endpoints, placing real
orders, or enabling automatic trading. Future schema implementation must be a
separately authorized TASK, must add an independent migration file, and must
not directly edit `migrations/001_initial_schema.sql`.

## TASK-189 Manual Entry Schema Implementation Authorization Gate Prompt Rule

Future work after TASK-189 must reference docs/176 and docs/177. TASK-189 is a
schema implementation authorization gate only. It defines future migration
readiness, migration filename / id policy, rollback / forward-fix policy,
compatibility policy, manual transaction candidate categories, manual cash
movement candidate categories, and audit / rollback / privacy candidate
categories. TASK-189 does not authorize modifying
`migrations/001_initial_schema.sql`, adding a migration, adding a schema file,
adding tables / columns / indexes / constraints, implementing repository
writes, executing SQL, writing SQLite, changing DataServiceActions, changing
TASK-178 validation production code, changing TASK-185 repository scaffold
code, changing QML/startup, changing AccountingEngine replay, changing
StrategyEngine or MarketEngine, adding TradeDraft or suggestion
implementation, adding broker SDK, adding network calls, adding credentials or
endpoints, placing real orders, or enabling automatic trading. Future schema
implementation must be a separately authorized TASK, must add an independent
migration file, must declare migration id / rollback / compatibility policy,
and must not directly edit `migrations/001_initial_schema.sql`.

## TASK-190 Manual Entry Schema Migration Implementation Prompt Rule

Future manual entry repository or DataService write work after TASK-190 must
reference docs/178 and docs/179. TASK-190 only authorizes the independent
DDL-only migration `migrations/002_shell_accounting_manual_entry_schema.sql`.
It does not authorize modifying `migrations/001_initial_schema.sql`, adding
runtime SQL execution, writing SQLite at runtime, implementing manual entry
repositories, changing DataServiceActions, changing TASK-178 validation
production code, changing TASK-185 repository scaffold code, changing
QML/startup, changing AccountingEngine replay, adding TradeDraft or suggestion
implementation, adding broker SDK, adding network calls, adding credentials or
endpoints, placing real orders, or enabling automatic trading. Future
repository implementation and DataService write implementation must remain
separately authorized tasks.

## TASK-191 Manual Entry Repository Implementation Post-Migration Authorization Prompt Rule

Future manual entry repository implementation work after TASK-191 must
reference docs/180 and docs/181. TASK-191 is a post-migration authorization
gate only. It does not authorize modifying `migrations/001_initial_schema.sql`
or `migrations/002_shell_accounting_manual_entry_schema.sql`, adding a
repository implementation, executing runtime SQL, writing SQLite at runtime,
changing DataServiceActions, changing TASK-178 validation production code,
changing TASK-185 repository scaffold code, changing QML/startup, changing
AccountingEngine replay, adding TradeDraft or suggestion implementation,
adding broker SDK, adding network calls, adding credentials or endpoints,
placing real orders, or enabling automatic trading. Future repository
implementation must be a separate TASK inside the DataAccess repository
boundary, must use the TASK-190 schema fields, and must not scatter SQL in
DataService action handlers. Future DataService action write implementation,
replay, read model, and UI work must remain separately authorized.

## TASK-192 Manual Transaction Repository Write Implementation Prompt Rule

Future work after TASK-192 must reference docs/182 and docs/183. TASK-192
authorizes only DataAccess direct repository write implementation for
ShellAccounting manual BUY / SELL transactions. It does not authorize manual
cash movement write, DataServiceActions changes, DataService action write
wiring, production QML/startup changes, Presenter / Controller changes,
AccountingEngine replay integration, read model integration, audit write,
TradeDraft or suggestion implementation, broker SDKs, network calls,
credentials, endpoint values, real order placement, or automatic trading.
Future manual cash movement repository write and future DataService write
wiring must remain separate TASKs.

## TASK-194 Manual Cash Movement Repository Write Authorization Gate Prompt Rule

Future work after TASK-194 must reference docs/184 and docs/185. TASK-194 is a
manual cash movement repository write implementation authorization gate only.
It does not authorize implementing manual cash movement repository write,
modifying TASK-192 manual transaction repository behavior, modifying
DataAccess production implementation, modifying DataServiceActions,
modifying TASK-178 validation production code, modifying migrations 001 or
002, executing runtime SQL, writing SQLite, writing `cash_adjustment`,
writing `trade_log` cash movement facts, writing `audit_log`, writing ledger,
changing QML/startup/Presenter/Controller, changing AccountingEngine replay,
adding TradeDraft or suggestion implementation, adding broker SDK, adding
network calls, adding credentials or endpoints, placing real orders, storing
broker order ids, or enabling automatic trading. Future manual cash movement
repository implementation must be a separate TASK inside the DataAccess
repository boundary. Future DataService action write implementation must be a
separate TASK. Replay, read model, UI, audit integration, and broker work must
remain separately authorized.

## TASK-195 Manual Cash Movement Schema Contract Alignment Prompt Rule

Future manual cash movement repository work after TASK-195 must reference
docs/186 and docs/187. TASK-195 is a schema-contract alignment gate only. It
does not authorize repository implementation, migration changes,
DataServiceActions changes, runtime SQL, SQLite writes, QML/startup changes,
AccountingEngine replay changes, broker SDKs, network calls, credentials,
endpoints, real order placement, or automatic trading.

The current schema blocks cash_adjustment-only writes because
`cash_adjustment.trade_log_id` is `NOT NULL` and references `trade_log(id)`.
Future manual cash movement repository implementation must use an atomic
`trade_log` + `cash_adjustment` dual-write under the current schema, or a
future schema migration TASK must separately authorize standalone
cash_adjustment. Future DataService action write wiring, replay/read model/UI,
audit policy, and broker work must remain separately authorized.

## TASK-198 Manual Entry DataService Write Wiring Implementation Prompt Rule

Future manual entry DataService write work after TASK-198 must reference
docs/192 and docs/193. TASK-198 authorizes only validation-first DataService
action wiring into the existing DataAccess repositories:
`ShellAccountingManualTransactionRepository` for manual BUY / SELL writes and
`ShellAccountingManualCashMovementRepository` for Deposit / Withdrawal
`trade_log + cash_adjustment` writes.

Do not use TASK-198 as authorization to modify migrations, modify repository
implementations, modify production QML/startup, modify Presenter or Controller
behavior, trigger AccountingEngine replay, add read model integration, write
`audit_log` or ledger rows, add TradeDraft or suggestion implementation, add
broker SDKs, add network calls, add credentials or endpoints, place real
orders, store broker order ids, or enable automatic trading. SQL must remain in
DataAccess repositories; DataServiceActions must not scatter direct SQL.

## TASK-199 Manual Entry QML Presenter Authorization Gate Prompt Rule

Future manual entry QML / Presenter work after TASK-199 must reference docs/194
and docs/195. TASK-199 is a gate-only authorization task and does not implement
UI, modify production QML/startup, modify Presenter or Controller behavior,
modify DataServiceActions, modify repositories, modify migrations, write
SQLite directly, trigger AccountingEngine replay, write audit or ledger rows,
add broker SDKs, add network calls, add credentials or endpoints, place real
orders, or enable automatic trading.

Future UI implementation must be separately authorized, must route manual
transaction and manual cash movement submissions only through
`accounting.manual_transaction.create` and
`accounting.manual_cash_movement.create`, must not call DataAccess or SQLite
directly from QML / Presenter / Controller, and must display only sanitized
DataService responses and safe errors. Future readback / refresh / replay,
audit write, ledger policy, broker capability, and automatic trading work must
remain separately authorized.

## TASK-200 Manual Entry QML Presenter Implementation Prompt Rule

Future manual entry UI work after TASK-200 must reference docs/196 and docs/197.
TASK-200 authorizes only production ShellAccounting QML and ShellServices
Presenter / Controller / adapter wiring for manual transaction and manual cash
movement submissions through `accounting.manual_transaction.create` and
`accounting.manual_cash_movement.create`.

Do not use TASK-200 as authorization to modify migrations, modify
DataServiceActions or DataServiceActionRegistrar, modify DataAccess
repositories, write SQLite directly from QML / Presenter / Controller, trigger
AccountingEngine replay or read model refresh, write audit or ledger rows from
ShellServices, add TradeDraft or suggestion expansion, add broker SDKs, add
network calls, add credentials or endpoints, place real orders, store real
broker order ids, or enable automatic trading.

## TASK-201 Manual Entry Post-Write Readback Refresh Authorization Gate Prompt Rule

Future manual entry post-write readback / refresh work after TASK-201 must
reference docs/198 and docs/199. TASK-201 is an authorization gate only and
does not implement readback, refresh, AccountingEngine replay integration,
read model refresh, snapshot refresh, UI auto-refresh, refresh buttons, audit
write, ledger write, broker access, network calls, credentials, endpoints,
real order placement, or automatic trading.

Do not use TASK-201 as authorization to modify production QML, startup,
Presenter, Controller, ShellServices adapter / port code, DataServiceActions,
DataServiceActionRegistrar, DataAccess repositories, validation production
code, migrations, schema files, AccountingEngine replay, StrategyEngine,
MarketEngine, or broker code. Future readback must use a DataService read
boundary or separately authorized replay/read-model boundary, and QML /
Presenter / Controller must not directly access SQLite, DataAccess, raw SQL,
or calculate accounting facts.

## TASK-202 Manual Entry Post-Write Readback Refresh Implementation Prompt Rule

Future manual entry refresh work after TASK-202 must reference docs/200 and
docs/201. TASK-202 authorizes only minimal post-write readback refresh through
existing DataService read actions after successful manual transaction or manual
cash movement writes.

Manual transaction success may refresh only `position.list`, `cash.summary`,
and `portfolio.pnl.summary` through Presenter / Controller / DataService read
boundaries. Manual cash movement success may refresh only `cash.summary` and
`portfolio.pnl.summary` through the same boundary. Do not use TASK-202 as
authorization to modify DataServiceActions, DataServiceActionRegistrar,
DataAccess repositories, migrations, validation production code, AccountingEngine
replay, snapshot rebuild, derived table refresh, audit writes, ledger writes,
TradeDraft or suggestion behavior, broker SDKs, network calls, credentials,
endpoints, real order placement, real broker order ids, or automatic trading.
QML / Presenter / Controller must not directly access SQLite or DataAccess and
QML must not calculate position, cash, or PnL facts.

## TASK-203 Manual Entry MVP E2E Acceptance Authorization Gate Prompt Rule

Future manual entry MVP E2E acceptance work after TASK-203 must reference
docs/202 and docs/203. TASK-203 is a gate-only authorization task. It documents
the current QML -> Presenter -> Controller -> ShellServices adapter ->
DataService action -> DataAccess repository chain and requires acceptance to
prove no layer bypasses its boundary.

Do not use TASK-203 as authorization to implement new runtime E2E behavior,
modify production QML, startup, Presenter, Controller, ShellServices adapter /
port code, DataServiceActions, DataServiceActionRegistrar, DataAccess
repositories, validation production code, migrations, schema files,
AccountingEngine replay, audit or ledger writes, broker SDKs, network calls,
credentials, endpoints, real order placement, or automatic trading. Runtime E2E
fixture tests, acceptance implementation, audit policy, ledger policy, broker
capability, and automatic trading must remain separately authorized.

## TASK-204 Manual Entry MVP Runtime E2E Acceptance Prompt Rule

Future manual entry MVP runtime E2E work after TASK-204 must reference docs/204
and docs/205. TASK-204 authorizes tests and documentation only: temporary SQLite
DB fixtures, migrations 001 / 002 application, synthetic manual transaction BUY
and manual cash movement Deposit payloads through existing DataService action
boundaries, duplicate idempotency checks, invalid payload no-write checks,
sanitized response checks, and TASK-202 post-write readback boundary checks.

Do not use TASK-204 as authorization to add production functionality, modify
production QML, startup, Presenter, Controller, ShellServices adapter / port
code, DataServiceActions, DataServiceActionRegistrar, DataAccess repositories,
validation production code, migrations, schema files, AccountingEngine replay,
audit writes, ledger writes, broker SDKs, network calls, credentials, endpoints,
real order placement, real broker order ids, or automatic trading. Broader SELL
/ Withdrawal fixture expansion, audit policy, ledger policy, broker capability,
and automatic trading must remain separately authorized. Broker sandbox new
capability development remains paused.

## TASK-205 Manual Entry Readback Replay Adequacy Review Gate Prompt Rule

Future manual entry readback / replay adequacy work after TASK-205 must
reference
`docs/206_shell_accounting_manual_entry_readback_replay_adequacy_review_gate.md`
and
`docs/207_shell_accounting_manual_entry_readback_replay_adequacy_review_test_plan.md`.
TASK-205 is review-gate-only: it documents a formal conclusion and gap matrix
for manual transaction facts, manual cash movement facts, existing DataService
read actions, future AccountingEngine replay, and daily-use accounting pages.

Do not use TASK-205 as authorization to implement readback, implement replay,
modify production QML, startup, Presenter, Controller, ShellServices adapter /
port code, DataServiceActions, DataServiceActionRegistrar, DataAccess
repositories, migrations, schema files, AccountingEngine production code,
StrategyEngine, MarketEngine, broker SDKs, network calls, credentials,
endpoints, real order placement, real broker order ids, or automatic trading.
Future readback implementation, AccountingEngine replay implementation, audit
write / ledger policy, backup / export / packaging acceptance, broker sandbox
capability, and real broker order placement must remain separately authorized.
Broker sandbox new capability development remains paused.

## TASK-206 Manual Entry Readback Mapping Authorization Gate Prompt Rule

Future manual entry readback mapping work after TASK-206 must reference
`docs/208_shell_accounting_manual_entry_readback_mapping_authorization_gate.md`
and
`docs/209_shell_accounting_manual_entry_readback_mapping_authorization_test_plan.md`.
TASK-206 is readback mapping authorization gate-only: it documents mapping
policy for `position.list`, `cash.summary`, and `portfolio.pnl.summary`, but it
does not implement readback mapping.

Do not use TASK-206 as authorization to modify production code, production QML,
startup, Presenter, Controller, ShellServices adapter / port code,
DataServiceActions, DataServiceActionRegistrar, DataAccess repositories,
migrations, AccountingEngine, StrategyEngine, MarketEngine, broker code,
network, credentials, endpoints, real order placement, or automatic trading.

Future TASK-207 may implement readback mapping if separately authorized. Future
replay, audit, ledger, backup/export, packaging, broker sandbox, real broker
order, and automatic trading work remain separate TASKs. First phase remains no
real broker, no automatic trading, no real order placement, and broker sandbox
new capability remains paused.

## TASK-207 Manual Entry Readback Mapping Implementation Prompt Rule

Future work after TASK-207 must reference
`docs/210_shell_accounting_manual_entry_readback_mapping_implementation.md`
and
`docs/211_shell_accounting_manual_entry_readback_mapping_implementation_test_plan.md`.
TASK-207 implements DataService readback mapping only for `position.list`,
`cash.summary`, and safe partial `portfolio.pnl.summary` using persisted manual
entry facts.

Do not treat TASK-207 as authorization to modify QML, startup, Presenter,
Controller, ShellServices adapter / port code, DataServiceActionRegistrar,
manual write repositories, migrations, AccountingEngine replay, snapshot
rebuild, derived table refresh, audit / ledger writes, broker SDKs, network,
credentials, endpoints, real order placement, real broker order ids, or
automatic trading. Unsupported PnL must remain safe partial / unavailable and
must not be fabricated. Future replay, audit, ledger, backup/export, packaging,
broker sandbox, real broker order, strategy execution, and automatic trading
remain separate TASKs.

## TASK-208 Manual Entry Readback Daily-Use Acceptance Authorization Gate Prompt Rule

Future manual entry daily-use acceptance work after TASK-208 must reference
`docs/212_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_gate.md`
and
`docs/213_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_test_plan.md`.
TASK-208 is daily-use acceptance authorization gate-only. It defines acceptance
standards for `position.list`, `cash.summary`, and safe partial
`portfolio.pnl.summary` after TASK-207 readback mapping, but it does not
implement runtime daily-use acceptance.

Do not treat TASK-208 as authorization to modify production code, production
QML, startup, Presenter, Controller, ShellServices adapter / port code,
DataServiceActions, DataServiceActionRegistrar, DataAccess repositories,
migrations, AccountingEngine replay, runtime SQL / SQLite read/write behavior,
snapshot rebuild, derived table refresh, audit / ledger writes, broker SDKs,
network, credentials, endpoints, real order placement, real broker order ids,
or automatic trading. Future TASK-209 may implement runtime daily-use
acceptance tests if separately authorized. Future replay, audit, ledger,
backup/export, packaging, broker sandbox, real broker order, strategy
execution, and automatic trading remain separate TASKs. First phase remains no
real broker, no automatic trading, no real order placement, and broker sandbox
new capability remains paused.

## TASK-209 Manual Entry Readback Daily-Use Runtime Acceptance Prompt Rule

TASK-209 adds tests-and-docs-only runtime acceptance coverage for the manual
entry BUY + Deposit daily-use baseline. Future work after TASK-209 must
reference
`docs/214_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance.md`
and
`docs/215_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance_test_plan.md`.

Do not treat TASK-209 as authorization to modify production code, production
QML, startup, Presenter, Controller, ShellServices adapter / port code,
DataServiceActions, DataServiceActionRegistrar, DataAccess repositories,
migrations, AccountingEngine replay, runtime implementation, audit / ledger
writes, broker SDKs, network, credentials, endpoints, real order placement,
real broker order ids, or automatic trading. TASK-209 acceptance tests may use
temporary SQLite fixtures and synthetic local facts only.

## TASK-210 Manual Entry SELL Withdrawal Daily-Use Acceptance Authorization Gate Prompt Rule

Future SELL / Withdrawal daily-use acceptance work after TASK-210 must
reference
`docs/216_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.md`
and
`docs/217_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_test_plan.md`.
TASK-210 is authorization gate-only. It defines acceptance boundaries for
manual SELL quantity reduction, sell-exceeds-position safe issues, SELL cash
inflow, Withdrawal cash outflow, insufficient cash / negative cash policy,
currency mapping, principal flow safe status, no fabricated realized PnL, no
fabricated unrealized PnL, no fabricated cash or PnL, and no replay.

Do not treat TASK-210 as authorization to implement runtime acceptance, modify
production code, production QML, startup, Presenter, Controller, ShellServices
adapter / port code, DataServiceActions, DataServiceActionRegistrar, DataAccess
repositories, migrations, AccountingEngine replay, runtime SQL / SQLite
read/write behavior, snapshot rebuild, derived table refresh, audit / ledger
writes, broker SDKs, network, credentials, endpoints, real order placement,
real broker order ids, or automatic trading. Future TASK-211 may implement
SELL / Withdrawal runtime acceptance tests if separately authorized.

## TASK-211 Manual Entry SELL Withdrawal Daily-Use Runtime Acceptance Prompt Rule

TASK-211 adds tests-and-docs-only runtime acceptance coverage for the manual
entry SELL / Withdrawal daily-use expansion. Future work after TASK-211 must
reference
`docs/218_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance.md`
and
`docs/219_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance_test_plan.md`.

Do not treat TASK-211 as authorization to modify production code, production
QML, startup, Presenter, Controller, ShellServices adapter / port code,
DataServiceActions, DataServiceActionRegistrar, DataAccess repositories,
migrations, AccountingEngine replay, production runtime implementation, audit /
ledger writes, broker SDKs, network, credentials, endpoints, real order
placement, real broker order ids, or automatic trading. TASK-211 acceptance
tests may use temporary SQLite fixtures, migrations 001 / 002, and synthetic
local manual entry facts only. Future replay, market price valuation, realized
PnL computation, production UI changes, broker order placement, strategy
execution, and automatic trading remain separate TASKs.

## TASK-212 Manual Entry Replay Audit Ledger Adequacy Review Gate Prompt Rule

Future manual entry replay, audit, ledger, backup/export, or restore work after
TASK-212 must reference
`docs/220_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_gate.md`
and
`docs/221_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_test_plan.md`.

TASK-212 is review-gate-only. It establishes adequacy conclusions for manual
entry facts entering future replay / audit / ledger / backup-export chains, but
it does not implement replay, audit / ledger writes, backup/export/restore,
runtime SQL / SQLite read/write, read model refresh, broker SDKs, network,
credentials, endpoints, real order placement, real broker order ids, or
automatic trading. Do not treat TASK-212 as authorization to modify production
code, production QML, startup, Presenter, Controller, ShellServices adapter /
port code, DataServiceActions, DataServiceActionRegistrar, DataAccess
repositories, migrations, AccountingEngine, StrategyEngine, or MarketEngine.

TASK-212 concludes that manual entry daily-use local MVP is ready at readback
acceptance level. Replay is not ready for implementation until replay
input/output policy is separately authorized. Audit / ledger are not ready for
implementation until policy is separately authorized. Backup/export/restore
requires separate acceptance. Recommended next task: TASK-213 manual entry
replay policy authorization gate, unless the project owner chooses audit or
backup first with explicit rationale. First phase remains no real broker, no
automatic trading, and no real order placement. Broker sandbox new capability
remains paused.

## TASK-213 Manual Entry Replay Policy Authorization Gate Prompt Rule

Future manual entry replay fixture matrix, AccountingEngine replay
implementation, replay output mapping, or replay integration work after
TASK-213 must reference
`docs/222_shell_accounting_manual_entry_replay_policy_authorization_gate.md`
and
`docs/223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md`.

TASK-213 authorizes replay policy only. It defines replay input, ordering,
position, cash, PnL, output, unsupported state, privacy, sanitization, and
integration boundaries for future work. Do not treat TASK-213 as authorization
to implement replay, call AccountingEngine replay, modify production code,
production QML, startup, Presenter, Controller, ShellServices adapter / port
code, DataServiceActions, DataServiceActionRegistrar, DataAccess repositories,
migrations, audit / ledger writes, backup/export/restore, broker SDKs, network,
credentials, endpoints, real order placement, real broker order ids, or
automatic trading.

Recommended next task: TASK-214 manual entry replay fixture matrix
authorization gate.

## TASK-214 Manual Entry Replay Fixture Matrix Authorization Gate Prompt Rule

Future manual entry replay fixture or implementation work must reference
`docs/224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md`
and
`docs/225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md`.

TASK-214 authorizes replay fixture matrix design only. TASK-214 does not
authorize replay implementation. TASK-214 does not authorize creation of replay
fixture files. TASK-214 does not authorize AccountingEngine replay calls.
TASK-214 does not authorize runtime SQL / SQLite read/write. TASK-214 does not
authorize audit / ledger writes. TASK-214 does not authorize backup/export/restore.
TASK-214 does not authorize broker / network / credentials / endpoint / real
order / automatic trading.

Recommended next task: TASK-215 manual entry replay fixture files
authorization gate.

## TASK-215 Manual Entry Replay Fixture Files Authorization Gate Prompt Rule

Future manual entry replay fixture file creation or replay implementation work
must reference
`docs/226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md`
and
`docs/227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md`.

TASK-215 authorizes replay fixture files contract only. TASK-215 does not
authorize creation of replay fixture files. TASK-215 does not authorize replay
implementation. TASK-215 does not authorize AccountingEngine replay calls.
TASK-215 does not authorize runtime SQL / SQLite read/write. TASK-215 does not
authorize audit / ledger / snapshot writes. TASK-215 does not authorize
backup/export/restore. TASK-215 does not authorize broker / network /
credentials / endpoint / real order / automatic trading.

Recommended next task: TASK-216 manual entry replay fixture files scaffold
authorization gate.

## TASK-216 Manual Entry Replay Fixture Files Scaffold Authorization Gate Prompt Rule

Future manual entry replay fixture scaffold, fixture file creation, fixture
parser / validator, or replay implementation work must reference
`docs/228_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_gate.md`
and
`docs/229_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_test_plan.md`.

TASK-216 authorizes replay fixture files scaffold policy only. TASK-216 does
not authorize creation of replay fixture files, creation of replay fixture
JSON, creation of fixture index files, parser or validator implementation,
replay implementation, AccountingEngine replay calls, runtime SQL / SQLite
read/write, audit / ledger / snapshot writes, backup/export/restore, broker /
network / credentials / endpoint / real order / automatic trading, production
code changes, production QML changes, DataServiceActions changes, repository
changes, or migration changes.

Recommended next task: TASK-217 manual entry replay fixture files scaffold.

## TASK-217 Manual Entry Replay Fixture Files Scaffold Prompt Rule

Future manual entry replay fixture validator or replay implementation work must
reference
`docs/230_shell_accounting_manual_entry_replay_fixture_files_scaffold.md` and
`docs/231_shell_accounting_manual_entry_replay_fixture_files_scaffold_test_plan.md`.

TASK-217 creates test-only replay fixture scaffold files only. TASK-217 does
not authorize replay implementation, parser / validator / loader / reader
implementation, AccountingEngine replay calls, runtime SQL / SQLite
read/write, audit / ledger / snapshot writes, backup/export/restore, broker /
network / credentials / endpoint / real order / automatic trading, production
code changes, production QML changes, DataServiceActions changes, repository
changes, or migration changes.

Recommended next task: TASK-218 manual entry replay fixture scaffold static
validator authorization gate.

## TASK-218 Manual Entry Replay Fixture Scaffold Static Validator Authorization Gate Prompt Rule

Future manual entry replay fixture validator or replay implementation work must
reference
`docs/232_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_gate.md`
and
`docs/233_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_test_plan.md`.

TASK-218 authorizes replay fixture scaffold static validator policy only.
TASK-218 does not authorize parser / validator / loader / reader
implementation. TASK-218 does not authorize replay implementation. TASK-218
does not authorize AccountingEngine replay calls. TASK-218 does not authorize
runtime SQL / SQLite read/write. TASK-218 does not authorize audit / ledger /
snapshot writes. TASK-218 does not authorize backup/export/restore. TASK-218
does not authorize broker / network / credentials / endpoint / real order /
automatic trading.

Recommended next task: TASK-219 manual entry replay fixture scaffold static
validator.

## TASK-219 Manual Entry Replay Fixture Scaffold Static Validator Prompt Rule

Future manual entry replay fixture validator negative-fixture work or replay
implementation work must reference
`docs/234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md`
and
`docs/235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md`.

TASK-219 implements a test-only static validator for replay fixture scaffold
files. TASK-219 does not authorize replay implementation. TASK-219 does not
authorize AccountingEngine replay calls. TASK-219 does not authorize fixture
JSON modification. TASK-219 does not authorize production module imports.
TASK-219 does not authorize runtime SQL / SQLite read/write. TASK-219 does not
authorize audit / ledger / snapshot writes. TASK-219 does not authorize
backup/export/restore. TASK-219 does not authorize broker / network /
credentials / endpoint / real order / automatic trading.

Recommended next task: TASK-220 manual entry replay fixture static validator
negative fixtures authorization gate.

## TASK-220 Manual Entry Replay Fixture Static Validator Negative Fixtures Authorization Gate Prompt Rule

Future manual entry replay negative fixture scaffold, negative validator, or
replay implementation work must reference
`docs/236_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_gate.md`
and
`docs/237_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_test_plan.md`.

TASK-220 authorizes replay fixture static validator negative fixture policy only.
TASK-220 does not authorize creation of negative fixture files. TASK-220 does
not authorize negative validator implementation. TASK-220 does not authorize
parser / loader / reader implementation. TASK-220 does not authorize replay
implementation. TASK-220 does not authorize AccountingEngine replay calls.
TASK-220 does not authorize runtime SQL / SQLite read/write. TASK-220 does not
authorize audit / ledger / snapshot writes. TASK-220 does not authorize
backup/export/restore. TASK-220 does not authorize broker / network /
credentials / endpoint / real order / automatic trading.

Recommended next task: TASK-221 manual entry replay fixture negative fixtures
scaffold authorization gate.

## TASK-166 Broker Runtime Mode Source Disabled Scaffold Prompt Rule

Future broker runtime mode source work must reference docs/130 and docs/131
after TASK-166. The only authorized source is disabled-only:
`defaultShellAccountingBrokerRuntimeModeSource()` may return a source whose
`brokerOrderPortMode()` is disabled, and `DataServiceActions` may pass that
mode to `shellAccountingBrokerOrderPortForMode(brokerPortMode)`. Do not read
mode from payload, QML, config, environment, command line, files, database, or
secret stores. Do not enable sandbox, paper, or real runtime. Do not add SDK,
network, credentials, endpoint, real broker order id, order placement,
database/audit/ledger write, reconciliation, cancellation, correction, strategy
execution, or automatic trading without separate explicit authorization.

## TASK-164 Broker Sandbox Runtime Selector Disabled Wiring Prompt Rule

Future broker runtime selector work must reference docs/126 and docs/127 after
TASK-164. DataServiceActions may use the selector only through
`defaultShellAccountingBrokerOrderPortMode()` unless a later task explicitly
authorizes a runtime mode source. Preserve disabled/null provider behavior,
keep sandbox runtime disabled, and do not read broker mode from request
payloads, QML, config, environment variables, command-line arguments, files,
databases, or secret stores. Do not add broker SDKs, network calls,
credentials, endpoint values, real broker order ids, order placement, database
writes, audit writes, ledger writes, reconciliation, cancellation, correction,
strategy execution, automatic trading, production QML/startup exposure, or
schema changes unless a later task explicitly authorizes that exact scope.
