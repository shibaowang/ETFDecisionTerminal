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
