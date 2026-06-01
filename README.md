# 跨境 ETF 智能投资决策终端

## v0.6 ShellAccounting Read-only UI Readiness

ShellAccounting read-only UI readiness is now documented as a milestone:

- [docs/61_shell_accounting_readonly_ui_readiness_milestone.md](docs/61_shell_accounting_readonly_ui_readiness_milestone.md)
- [docs/62_shell_accounting_next_phase_review.md](docs/62_shell_accounting_next_phase_review.md)
- [docs/release_notes/v0_6_shell_accounting_readonly_ui_readiness.md](docs/release_notes/v0_6_shell_accounting_readonly_ui_readiness.md)

The milestone closes the current ShellServices / Presenter / ViewModel / QML
static gate readiness loop. It is not QML binding and it is not real accounting
action implementation. QML is still not wired, no ShellAccounting QML type is
registered, real accounting actions remain unimplemented, SQLite facts query is
not connected to DataService, DataService still does not call AccountingEngine,
and no database writes, TradeDraft, trade suggestion, strategy execution, or
broker order behavior is enabled.

## TASK-121 ShellAccounting QML Binding Smoke Scaffold

ShellAccounting QML binding smoke scaffold tests are now added under
`tests/ShellAccountingQmlBindingSmoke`.

The scaffold fixes the future read-only QML binding smoke matrix without
modifying QML:

- allowed object contract for ShellAccountingPresenter, status object, issue
  model, position model, and future cash / PnL / base / sniper models.
- Idle / Loading / Loaded / Empty / Unavailable / Warning / Error / Stale state
  display matrix.
- five guard payload display rules mapping `*_NOT_AVAILABLE` to Unavailable.
- issue visibility and privacy display rules.
- no-trade UI rules.
- dependency on the TASK-119 QML static gate.

QML remains unwired, QML static gate remains a prerequisite, no real accounting
action is implemented, no SQLite or AccountingEngine integration is added, no
database write is enabled, and no TradeDraft or trade suggestion is generated.

## TASK-122 ShellAccounting QML Smoke CTest

ShellAccounting QML smoke CTests are now added under
`tests/ShellAccountingQmlBindingSmokeRuntime`.

The runtime smoke tests use only test-only inline QML and `QVariantMap` /
`QVariantList` objects to verify:

- object availability for fake read-only presenter, status, issue, and position
  bindings.
- Idle / Loading / Loaded / Empty / Unavailable / Warning / Error / Stale state
  display.
- five guard unavailable statuses mapping to Unavailable.
- issue visibility for code, level, message, blocking, and sourceId.
- privacy display using `displayText` without QML calculating `maskedText`.
- no-trade UI tokens and no write affordances.
- dependency on TASK-119 QML static gate.

This remains test-only. Production QML is still not modified, no production QML
type is registered, no production QML page is implemented, no DataServiceClient,
SQLite, DataAccess, or AccountingEngine path is called, and no writes,
TradeDraft, trade suggestion, strategy execution, or broker order behavior is
enabled.

## TASK-123 ShellAccounting Production QML Binding Boundary

ShellAccounting production QML binding boundary docs are now added:

- [docs/63_shell_accounting_production_qml_binding_boundary.md](docs/63_shell_accounting_production_qml_binding_boundary.md)
- [docs/64_shell_accounting_first_stage_qml_binding_checklist.md](docs/64_shell_accounting_first_stage_qml_binding_checklist.md)

The boundary defines allowed read-only ShellServices binding objects, forbidden
direct service / SQLite / AccountingEngine / write tokens, QML type
registration limits, first-stage placeholder / Unavailable / issue / privacy
scope, required TASK-119 / TASK-121 / TASK-122 gates, and rollback behavior.

Production QML is still not modified, no QML type is registered, no production
QML page is implemented, no real accounting action is implemented, no database
write is enabled, and no TradeDraft or trade suggestion is generated.

## TASK-124 ShellAccounting QML Type Registration Boundary

ShellAccounting QML type registration boundary docs are now added:

- [docs/65_shell_accounting_qml_type_registration_boundary.md](docs/65_shell_accounting_qml_type_registration_boundary.md)
- [docs/66_shell_accounting_qml_type_registration_test_plan.md](docs/66_shell_accounting_qml_type_registration_test_plan.md)

The boundary defines the first-stage read-only type allowlist, forbidden service
/ SQLite / AccountingEngine / write / trade exposure, registration location and
module constraints, required TASK-119 / TASK-121 / TASK-122 gates, and rollback
behavior. QML type registration still requires a separate explicit task.

Production QML is still not modified, no QML type is registered, no production
QML page is implemented, no real accounting action is implemented, no database
write is enabled, and no TradeDraft or trade suggestion is generated.

## TASK-125 ShellAccounting QML Type Registration Test Scaffold

ShellAccounting QML type registration test scaffold is now added under
`tests/ShellAccountingQmlTypeRegistrationScaffold`.

The scaffold fixes the future registration boundary without registering QML
types:

- first-stage allowlist for `ShellAccountingPresenter`,
  `ShellAccountingStatusObject`, `ShellAccountingIssueListModel`, and
  `ShellPositionListModel`.
- forbidden type denylist for DataServiceClient, DataServiceApi, adapters,
  concrete port, DataAccess, SQLite, AccountingEngine, StrategyEngine,
  MarketEngine, broker/order API, and write services.
- forbidden method / property denylist for buy/sell, TradeDraft, broker,
  strategy, write, snapshot, and suggestion tokens.
- future module/import boundary for `ETFDecisionTerminal.ShellAccounting`.
- TASK-119 / TASK-121 / TASK-122 gate dependency.
- no-production-change scan for current QML and registration code.

QML type registration still requires a separate explicit task. Production QML is
still not modified, no QML type is registered, no production QML page is
implemented, no real accounting action is implemented, no database write is
enabled, and no TradeDraft or trade suggestion is generated.

## TASK-127 ShellAccounting QML Type Registration Implementation

ShellAccounting first-stage QML type registration is now implemented through the
centralized ShellServices helper
`ShellAccountingQmlRegistration`. It registers only the read-only allowlist
types under `ETFDecisionTerminal.ShellAccounting` 1.0:

- `ShellAccountingPresenter`
- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`

The registration uses uncreatable QML types because ShellAccounting objects are
provided by ShellServices and remain read-only. Production QML is still not
modified, no production QML page imports the module, no real accounting action
is implemented, no database write is enabled, and no TradeDraft or trade
suggestion is generated.

## TASK-128 ShellAccounting QML Startup Registration Gate

ShellAccounting QML startup registration gate docs and CTest have been added:

- `docs/69_shell_accounting_qml_startup_registration_gate.md`
- `docs/70_shell_accounting_qml_startup_registration_test_plan.md`
- `tests/ShellAccountingQmlStartupRegistrationGate`

TASK-128 did not wire `registerShellAccountingQmlTypes` into production app
startup; TASK-129 adds that startup wiring separately. Production QML still does
not import `ETFDecisionTerminal.ShellAccounting`, and no production accounting
page or component is implemented. The gate fixes
the future startup wiring policy: centralized call only, no direct
`DataServiceClient`, SQLite, `DataAccess`, `AccountingEngine`, adapter, concrete
port, write action, `TradeDraft`, trade suggestion, strategy, or broker
exposure. Real accounting actions remain unimplemented.

## TASK-129 ShellAccounting QML Startup Registration Wiring

ShellAccounting QML startup registration wiring is now added:

- [docs/71_shell_accounting_qml_startup_registration_wiring.md](docs/71_shell_accounting_qml_startup_registration_wiring.md)
- `tests/ShellAccountingQmlStartupRegistrationWiring`

Production startup calls `registerShellAccountingQmlTypes` once from the
centralized app QML type registration location. The module remains
`ETFDecisionTerminal.ShellAccounting` 1.0 and the TASK-127 read-only allowlist
remains unchanged. Production QML still does not import the module, no
production QML page or component is implemented, no `accountingPresenter`
binding is added, no real accounting action is implemented, no database write is
enabled, and no TradeDraft or trade suggestion is generated.

## TASK-130 ShellAccounting Production QML Binding Gate

ShellAccounting production QML binding gate docs and CTest have been added:

- [docs/72_shell_accounting_production_qml_binding_gate.md](docs/72_shell_accounting_production_qml_binding_gate.md)
- [docs/73_shell_accounting_production_qml_binding_test_plan.md](docs/73_shell_accounting_production_qml_binding_test_plan.md)
- `tests/ShellAccountingProductionQmlBindingGate`

This task does not modify production QML. Production QML still does not import
`ETFDecisionTerminal.ShellAccounting`, no ShellAccounting page/component is
implemented, no `accountingPresenter` binding is added, no real accounting
action is implemented, no database write is enabled, and no TradeDraft or trade
suggestion is generated.

## TASK-131 ShellAccounting Production QML Binding Shell

ShellAccounting production QML binding shell is now added as a minimal read-only shell:

- [docs/74_shell_accounting_production_qml_binding_implementation.md](docs/74_shell_accounting_production_qml_binding_implementation.md)
- `apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml`
- `tests/ShellAccountingProductionQmlBindingImplementation`

The shell adds one authorized `ETFDecisionTerminal.ShellAccounting 1.0` import,
declares only a nullable `ShellAccountingPresenter` property, and shows an
Unavailable / disabled / read-only state when no presenter is provided. It does
not create a presenter, does not expose a context property, does not call
DataServiceClient, SQLite, or AccountingEngine, does not write any database
table, does not generate TradeDraft or trade suggestions, and does not show
trading buttons.

## TASK-132 ShellAccounting Presenter Lifecycle Gate

ShellAccounting presenter lifecycle and context exposure gate docs and CTest are
now added:

- [docs/75_shell_accounting_presenter_lifecycle_gate.md](docs/75_shell_accounting_presenter_lifecycle_gate.md)
- [docs/76_shell_accounting_presenter_lifecycle_test_plan.md](docs/76_shell_accounting_presenter_lifecycle_test_plan.md)
- `tests/ShellAccountingPresenterLifecycleGate`

Presenter lifecycle remains unimplemented. No `ShellAccountingPresenter` is
created, no `accountingPresenter` context property is exposed, and the
production QML shell continues to tolerate a null presenter. No real accounting
action, database write, TradeDraft, trade suggestion, or trading UI is enabled.

## TASK-133 ShellAccounting Presenter Lifecycle Implementation

ShellAccounting presenter lifecycle and context exposure are now wired for the
read-only production QML shell:

- [docs/77_shell_accounting_presenter_lifecycle_implementation.md](docs/77_shell_accounting_presenter_lifecycle_implementation.md)
- `tests/ShellAccountingPresenterLifecycleImplementation`

The presenter is production-owned in startup and exposed through the explicit
`accountingPresenter` context property. The UI remains Unavailable-safe and
read-only: no real accounting action is connected, no database write is enabled,
no TradeDraft or trade suggestion is generated, and no trading UI is shown.

## TASK-134 ShellAccounting Real Data Adapter Gate

ShellAccounting real data adapter / DataService read-only action integration
gate docs and test-only CTest coverage are now added:

- [docs/78_shell_accounting_real_data_adapter_gate.md](docs/78_shell_accounting_real_data_adapter_gate.md)
- [docs/79_shell_accounting_real_data_adapter_test_plan.md](docs/79_shell_accounting_real_data_adapter_test_plan.md)
- `tests/ShellAccountingRealDataAdapterGate`

The real data adapter is still not implemented. DataService read-only actions
are still not implemented. The presenter remains read-only and
Unavailable-safe: no direct SQLite access, no direct AccountingEngine access, no
database writes, no TradeDraft or trade suggestion, and no trading UI are
enabled.

## TASK-126 ShellAccounting QML Type Registration Implementation Gate

ShellAccounting QML type registration implementation gate docs are now added:

- [docs/67_shell_accounting_qml_type_registration_implementation_gate.md](docs/67_shell_accounting_qml_type_registration_implementation_gate.md)
- [docs/68_shell_accounting_qml_type_registration_rollback_strategy.md](docs/68_shell_accounting_qml_type_registration_rollback_strategy.md)

Test-only gate CTests are now added under
`tests/ShellAccountingQmlTypeRegistrationGate`. They lock the final pre-gates,
centralized registration location policy, read-only allowlist, forbidden
type/method/property exposure, rollback / disable rules, and current
no-production-registration state.

QML type registration still requires a separate explicit task. Production QML is
still not modified, no QML type is registered, no production QML page is
implemented, no real accounting action is implemented, no database write is
enabled, and no TradeDraft or trade suggestion is generated.

## TASK-115 ShellAccountingPresenter skeleton

ShellServices now includes a production-side `ShellAccountingPresenter`
skeleton. It owns the read-only accounting presentation boundary for future QML
work by holding:

- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`
- an optional `ShellAccountingReadOnlyController`

The presenter remains read-only, keeps `writeEnabled=false`, does not expose
TradeDraft / trade suggestion / strategy / broker submission paths, and only
refreshes through `ShellAccountingReadOnlyController`. QML is still not wired,
no QML type is registered, the presenter does not call DataServiceClient, does
not access SQLite, does not call AccountingEngine, and writes no database
tables.

## TASK-118 ShellAccounting QML binding smoke plan

ShellAccounting QML binding readiness smoke docs are now defined:

- [docs/59_shell_accounting_qml_binding_smoke_plan.md](docs/59_shell_accounting_qml_binding_smoke_plan.md)
- [docs/60_shell_accounting_qml_static_gate.md](docs/60_shell_accounting_qml_static_gate.md)

These documents define future read-only QML binding targets, the QML binding
smoke matrix, Empty vs Unavailable display rules, privacy display rules,
no-trade UI rules, and static gates that forbid direct DataServiceClient,
SQLite, DataAccess, AccountingEngine, write action, TradeDraft, trade
suggestion, strategy, and broker order access from QML.

QML remains unwired, no QML type is registered, no QML page is implemented, no
real accounting action is added, no SQLite or AccountingEngine access is
enabled, no database table is written, and no TradeDraft or trade suggestion is
generated.

## TASK-119 ShellAccounting QML static gate CTest

ShellAccounting QML static gate CTests are now added under
`tests/ShellAccountingQmlStaticGate`.

The gate scans `apps/ETFDecisionShell/qml` without modifying QML and verifies:

- no direct DataServiceClient / DataServiceApi / SQLite / DataAccess /
  AccountingEngine references.
- no write tokens such as `data.audit.append`, `writeEnabled: true`, SQL write
  statements, or snapshot / portfolio / trade table names.
- no ShellAccounting QML binding yet.
- no trading, TradeDraft, broker, strategy, or write-action tokens inside
  accounting-scoped QML files.
- docs/59, docs/60, docs/12, and README remain synchronized with the gate.

The accounting-scoped no-trade scan is content and scope based so existing
non-accounting placeholder pages such as `TradeDraftPlaceholderPage` and
`TradeConfirmPlaceholderPage` are not false positives. QML remains unwired, no
QML type is registered, no QML page is implemented, no DataServiceClient call is
made from QML, no SQLite or AccountingEngine access is added, no database table
is written, and no TradeDraft or trade suggestion is generated.

## TASK-117 ShellAccountingPresenter all guard actions refresh

ShellAccountingPresenter now exposes read-only refresh methods for all five
accounting guard actions:

- `refreshPositionList`
- `refreshCashSummary`
- `refreshPortfolioPnlSummary`
- `refreshBasePositionSummary`
- `refreshSniperPoolSummary`
- `refreshAllReadOnly`

Each method routes only through `ShellAccountingReadOnlyController`, then the
adapter / concrete port chain, before reaching the DataServiceClient read-only
guard wrappers. `refreshAllReadOnly` calls the five guard actions in fixed
order: `position.list`, `cash.summary`, `portfolio.pnl.summary`,
`base_position.summary`, and `sniper_pool.summary`, and aggregates the visible
guard issues.

All five guard payloads still map to `Unavailable`, keep their
`*_NOT_AVAILABLE` issues visible, preserve `readOnly=true` and
`writeEnabled=false`, and keep Empty distinct from Unavailable. QML remains
unwired, no QML type is registered, no real accounting action is implemented,
SQLite facts queries and AccountingEngine are not accessed, and no database
tables, TradeDraft, trade suggestion, strategy, or broker order path is
enabled.

## TASK-116 ShellAccountingPresenter concrete port integration

ShellAccountingPresenter concrete-port integration hardening is now covered by
tests. The presenter can refresh through:

`ShellAccountingPresenter -> ShellAccountingReadOnlyController ->
ShellAccountingDataServiceAdapter -> ShellAccountingDataServiceClientPortAdapter
-> DataServiceClient guard wrapper`.

The current presenter skeleton exposes only `refreshPositionList` and
`refreshAllReadOnly`, with `refreshAllReadOnly` intentionally delegating to
`position.list`. The integration verifies the `position.list` guard payload
maps to `Unavailable`, keeps `POSITION_LIST_NOT_AVAILABLE` visible, preserves
`readOnly=true` and `writeEnabled=false`, and keeps no TradeDraft, no trade
suggestion, no strategy execution, and no broker order.

QML remains unwired, no QML type is registered, no real accounting action is
implemented, SQLite facts queries and AccountingEngine are not accessed, and no
database tables are written.

## TASK-114 ShellAccounting presenter / QML binding plan

ShellAccounting presenter and QML binding plan docs are now defined:

- [docs/57_shell_accounting_presenter_contract.md](docs/57_shell_accounting_presenter_contract.md)
- [docs/58_shell_accounting_qml_binding_readiness_plan.md](docs/58_shell_accounting_qml_binding_readiness_plan.md)

The presenter is still not implemented. QML is still not wired, no QML type is
registered, no DataServiceClient call is made, no SQLite or AccountingEngine
access is added, no database tables are written, and no TradeDraft or trade
suggestion is generated.

## TASK-113 ShellAccounting ViewModel / Model read-only boundary

ShellServices now includes read-only Shell accounting ViewModel / Model
skeletons:

- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`

These objects consume only in-memory accounting state, issue, and display-text
data. They do not call DataServiceClient, access SQLite, call AccountingEngine,
write database tables, generate TradeDraft, generate trade suggestions, execute
strategies, or submit broker orders. QML remains unwired and no QML type is
registered in this task.

## TASK-112 ShellAccounting controller concrete port integration hardening

`ShellAccountingReadOnlyController` now has concrete-port integration tests that
exercise the chain:

`ShellAccountingReadOnlyController -> ShellAccountingDataServiceAdapter ->
ShellAccountingDataServiceClientPortAdapter -> DataServiceClient guard wrapper`.

The tests cover `position.list`, `cash.summary`, `portfolio.pnl.summary`,
`base_position.summary`, and `sniper_pool.summary`. Each guard payload remains
`Unavailable` with visible issues, `readOnly=true`, and `writeEnabled=false`.

This does not implement real accounting actions. QML remains unwired, SQLite
facts queries remain unaccessed, AccountingEngine remains uncalled, no database
tables are written, and no TradeDraft or trade suggestion is generated.

## TASK-111 ShellAccounting DataServiceClient port for guard wrappers

`ShellAccountingDataServiceClientPortAdapter` is the production concrete port for guard wrappers and the concrete implementation of the ShellServices
`ShellAccountingDataServiceClientPort` boundary. It is the only ShellServices
class allowed to include and hold the real `DataServiceClient`, and it only
wraps the five read-only accounting guard wrappers:

- `DataServiceClient::positionList`
- `DataServiceClient::cashSummary`
- `DataServiceClient::portfolioPnlSummary`
- `DataServiceClient::basePositionSummary`
- `DataServiceClient::sniperPoolSummary`

`ShellAccountingDataServiceAdapter` and `ShellAccountingReadOnlyController`
still do not directly include `DataServiceClient`. QML remains disconnected.
No real accounting action is implemented, no SQLite or AccountingEngine access
is added, no database tables are written, and no TradeDraft or trade suggestion
is generated.

## TASK-110 ShellAccountingDataServiceAdapter live-call skeleton

`ShellAccountingDataServiceAdapter` now has a test-first live-call skeleton
through an internal `ShellAccountingDataServiceClientPort` abstraction. The
adapter can call test-only spy port methods shaped like the five read-only
accounting wrappers and map request / response / error fields into
`ShellAccountingServiceResult`.

This is not real DataServiceClient integration. Production code still does not
include or call DataServiceClient, DataServiceApi, DataAccess, AccountingEngine,
SQLite, QtQuick, or QML. With no port configured the adapter still returns
`SHELL_ACCOUNTING_DATASERVICE_ADAPTER_NOT_CONNECTED`; with a test spy port it
only exercises allowlisted read-only paths. QML remains disconnected, no
database tables are written, and no TradeDraft or trade suggestion is generated.

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

TASK-144 supersedes the TASK-011 write allowlist summary by adding the explicit
DataService-internal `accounting.snapshot.write` action. That action is limited
to `cash_snapshot`, `position_snapshot`, and `portfolio_summary`; it still does
not write `trade_log`, `trade_execution_group`, `trade_draft`, or `audit_log`.

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

## TASK-055 Accounting Replay Minimal FX001

- `tests/AccountingFixtures/AccountingReplayMinimalEngine` is a test-only minimal replay engine.
- It supports only `FX001_EMPTY_LEDGER`; FX001 returns `implemented=true`, `replayExecuted=true`, `status=OK`, empty `positions`, and zero cash / PnL summary raw objects.
- `FX002` through `FX013` still return `NOT_IMPLEMENTED`; buy, sell, cost, cash-flow, PnL, base-position, and sniper-pool replay are not implemented.
- `AccountingReplayTestHarness::runAllWithMinimalEngine()` runs all FX001-FX013 fixtures through the minimal mode while preserving full fixture coverage.
- This remains test-only and does not access SQLite, call DataService, write output files, write database tables, or add DataService actions.
- Minimal FX001 CTest: `accounting_replay_minimal_fx001`.

## TASK-056 Accounting Replay Minimal FX002

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER` and `FX002_SINGLE_BUY`.
- FX002 returns `implemented=true`, `replayExecuted=true`, `status=OK`, one `159509` position with `quantityText=1000`, `costAmountText=1001.00 CNY`, and `cashBalanceText=98999.00 CNY`.
- FX003-FX013 still return `NOT_IMPLEMENTED`.
- This does not implement sell replay, realized / unrealized PnL, market valuation, base-position, sniper-pool, SQLite access, DataService calls, output file writes, database writes, or DataService actions.
- Minimal FX002 CTest: `accounting_replay_minimal_fx002`.

## TASK-057 Accounting Replay Minimal FX003

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, and `FX003_BUY_SELL_PARTIAL`.
- FX003 returns `implemented=true`, `replayExecuted=true`, `status=OK`, one remaining `159509` position with `quantityText=600`, `costAmountText=600.60 CNY`, `cashBalanceText=99478.00 CNY`, and `realizedPnlText=78.60 CNY`.
- FX004-FX013 still return `NOT_IMPLEMENTED`.
- Realized PnL uses the fixture standard: buy fee enters cost, sell fee is deducted from sell cash inflow, and `realizedPnl = netSellInflow - allocatedSoldCost`.
- This does not implement FX004, multi-account, multi-instrument, market valuation, unrealized PnL, base-position, sniper-pool, SQLite access, DataService calls, output file writes, database writes, or DataService actions.
- Minimal FX003 CTest: `accounting_replay_minimal_fx003`.

## TASK-058 Accounting Replay Minimal FX004

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, `FX003_BUY_SELL_PARTIAL`, and `FX004_SELL_EXCEEDS_POSITION`.
- FX004 only detects oversell: if SELL quantity exceeds the prior BUY quantity, it returns `implemented=true`, `replayExecuted=true`, `status=ERROR`, and a blocking `SELL_EXCEEDS_POSITION` issue.
- FX004 does not generate negative holdings or normal position / cash / PnL success outputs.
- FX005-FX013 still return `NOT_IMPLEMENTED`.
- This does not implement missing-fee errors, negative-cash errors, multi-account, multi-instrument, market valuation, unrealized PnL, base-position, sniper-pool, SQLite access, DataService calls, output file writes, database writes, or DataService actions.
- Minimal FX004 CTest: `accounting_replay_minimal_fx004`.

## TASK-059 Accounting Replay Minimal FX005

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, `FX003_BUY_SELL_PARTIAL`, `FX004_SELL_EXCEEDS_POSITION`, and `FX005_MISSING_FEE`.
- FX005 only detects missing fee: when the BUY fact has missing / null / empty `feeText`, it returns `implemented=true`, `replayExecuted=true`, `status=WARNING`, and a non-blocking `MISSING_FEE` issue matching the fixture contract.
- FX005 does not default missing fee to `0`, and does not generate normal cost, position, cash, or PnL success outputs.
- FX006-FX013 still return `NOT_IMPLEMENTED`.
- This does not implement negative-cash errors, multi-account, multi-instrument, market valuation, unrealized PnL, base-position, sniper-pool, SQLite access, DataService calls, output file writes, database writes, or DataService actions.
- Minimal FX005 CTest: `accounting_replay_minimal_fx005`.

## TASK-060 Accounting Replay Minimal FX006

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, `FX003_BUY_SELL_PARTIAL`, `FX004_SELL_EXCEEDS_POSITION`, `FX005_MISSING_FEE`, and `FX006_NEGATIVE_CASH`.
- FX006 only detects negative cash: when BUY cash requirement exceeds initial cash, it returns `implemented=true`, `replayExecuted=true`, `status=ERROR`, and a blocking `NEGATIVE_CASH` issue.
- FX006 does not allow implicit overdraft and does not generate normal position, cash, or PnL success outputs.
- FX007-FX013 still return `NOT_IMPLEMENTED`.
- This does not implement multi-account, multi-instrument, market valuation, unrealized PnL, base-position, sniper-pool, SQLite access, DataService calls, output file writes, database writes, or DataService actions.
- Minimal FX006 CTest: `accounting_replay_minimal_fx006`.

## TASK-061 Accounting Replay Minimal FX007

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER` through `FX007_MULTI_INSTRUMENT`.
- FX007 only handles same-account, same-portfolio, multi-instrument BUY facts and keeps each `instrumentCode` in a separate position.
- FX007 returns `implemented=true`, `replayExecuted=true`, `status=WARNING`, two positions for `159509` and `518880`, and `cashBalanceText=96998.00 CNY`.
- FX007 reports non-blocking `MARKET_PRICE_MISSING` and does not fabricate `marketValueText` or `unrealizedPnlText`.
- FX008-FX013 still return `NOT_IMPLEMENTED`.
- This does not implement multi-account, multi-currency, base-position, sniper-pool, real market valuation, SQLite access, DataService calls, output file writes, database writes, or DataService actions.
- Minimal FX007 CTest: `accounting_replay_minimal_fx007`.

## TASK-062 Accounting Replay Minimal FX008

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER` through `FX008_MULTI_ACCOUNT`.
- FX008 only handles fixture-scoped multi-account BUY facts and keeps `ACC-DEMO-001` and `ACC-DEMO-002` separated by `accountId`, `portfolioId`, and `instrumentCode`.
- FX008 returns `implemented=true`, `replayExecuted=true`, `status=OK`, account-scoped `159509` positions, and account cash balances through `accountCashSummaries`.
- FX008 computes `cashBalanceText=48999.00 CNY` for `ACC-DEMO-001` and `cashBalanceText=47999.00 CNY` for `ACC-DEMO-002`; it does not mix account cash.
- FX008 does not fabricate `marketValueText` or `unrealizedPnlText`.
- FX009-FX013 still return `NOT_IMPLEMENTED`.
- This does not implement base-position, sniper-pool, multi-currency, real market valuation, SQLite access, DataService calls, output file writes, database writes, or DataService actions.
- Minimal FX008 CTest: `accounting_replay_minimal_fx008`.

## TASK-063 Accounting Replay Minimal FX009

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER` through `FX009_BASE_POSITION_LOCKED`.
- FX009 only derives readonly `basePositionRaw` for the 20% locked base-position fixture.
- FX009 returns `implemented=true`, `replayExecuted=true`, `status=OK`, `targetBaseRatioText=20%`, `lockedBaseAmountText=20000.00 CNY`, and `sellableAboveBaseAmountText=0.00 CNY`.
- `sellableAboveBaseAmountText` is a readonly display amount above the locked base; it is not a sell suggestion, order instruction, TradeDraft, or strategy output.
- FX009 does not produce `sniperPoolRaw`, TradeDraft output, sell actions, QML bindings, SQLite access, DataService calls, output file writes, database writes, or DataService actions.
- FX010-FX013 still return `NOT_IMPLEMENTED`.
- Minimal FX009 CTest: `accounting_replay_minimal_fx009`.

## TASK-064 Accounting Replay Minimal FX010

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER` through `FX010_SNIPER_TIER_COMPLETED`.
- FX010 only derives readonly `sniperPoolRaw` for the sniper tier completion fixture.
- FX010 returns `implemented=true`, `replayExecuted=true`, `status=OK`, `poolAmountText=80000.00 CNY`, `usedAmountText=1000.00 CNY`, `remainingAmountText=79000.00 CNY`, and `T1 completed=true`.
- T1 completion comes from BUY fact aggregation, not current market value.
- The 80% pool amount is fixed from the fixture cash scope and does not expand or shrink with floating profit or loss.
- `remainingAmountText` is readonly remaining pool capacity; it is not a buy suggestion, sell suggestion, order instruction, TradeDraft, or strategy output.
- FX010 does not produce TradeDraft output, buy actions, sell actions, QML bindings, SQLite access, DataService calls, output file writes, database writes, or DataService actions.
- FX011-FX012 are implemented in later test-only minimal replay tasks; FX013
  still returns `NOT_IMPLEMENTED`.
- Minimal FX010 CTest: `accounting_replay_minimal_fx010`.

## TASK-065 Accounting Replay Minimal FX011

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER` through `FX011_STALE_SNAPSHOT`.
- FX011 only detects stale snapshot metadata and reports `SNAPSHOT_STALE`.
- FX011 returns `implemented=true`, `replayExecuted=true`, `status=STALE`, and a blocking `SNAPSHOT_STALE` issue matching the fixture contract.
- Stale snapshot input is treated as derived cache metadata, not a fact source.
- FX011 does not produce normal position, cash, PnL, base-position, or sniper-pool success outputs.
- FX011 does not generate or write snapshots, does not refresh snapshots, and does not write `position_snapshot`, `cash_snapshot`, or `portfolio_summary`.
- FX012 is implemented in a later test-only minimal replay task; FX013 still
  returns `NOT_IMPLEMENTED`.
- This does not implement missing market price replay, multi-currency replay, real market valuation, SQLite access, DataService calls, output file writes, database writes, or DataService actions.
- Minimal FX011 CTest: `accounting_replay_minimal_fx011`.

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

## Current Accounting Replay Milestone: v0.3 Accounting Replay Test-only Coverage

FX001-FX013 test-only coverage is complete through the
`tests/AccountingFixtures` minimal replay engine. This is not production accounting replay.

Milestone documents:

- [Accounting Replay Test-only Coverage Milestone](docs/31_accounting_replay_testonly_coverage_milestone.md)
- [Production Accounting Replay Readiness Review](docs/32_production_accounting_replay_readiness_review.md)
- [Production Accounting Replay Architecture Boundary](docs/33_production_accounting_replay_architecture.md)
- [AccountingEngine Module Candidate](docs/34_accounting_engine_module_candidate.md)
- [Release Notes - v0.3 Accounting Replay Test-only Coverage](docs/release_notes/v0_3_accounting_replay_testonly_coverage.md)

Production accounting replay is still not implemented. DataService accounting
replay actions are still not implemented. The current project does not perform
SQLite replay writes, `trade_log` writes, snapshot writes, or production
portfolio summary writes.

The next phase is architecture boundary review and module placement, not direct code migration from `tests/AccountingFixtures` into production libraries.

## TASK-070 AccountingEngine skeleton

`libs/AccountingEngine` now exists as a production module skeleton. It currently
contains only module info, capability declarations, and boundary declarations.

Current declared state:

- `productionReady=false`.
- `replayImplemented=false`.
- `writeEnabled=false`.
- `snapshotWriteEnabled=false`.
- `tradeLogWriteEnabled=false`.
- No SQLite access.
- No DataService action.
- No database writes.
- No copied test-only replay implementation from `tests/AccountingFixtures`.

The boundary test is `accounting_engine_boundary`. This skeleton is not a replay
implementation and does not change the production accounting behavior.

## TASK-071 AccountingEngine replay DTO parser boundary

`libs/AccountingEngine` now includes production replay DTO parser and validation
boundary types. This is only an input contract boundary.

Added DTO boundary types:

- `ReplayRequestDto`
- `TradeFactDto`
- `CashFactDto`
- `MarketPriceFactDto`
- `FxRateFactDto`
- `AccountingIssueDto`

The validation layer checks required fields, supported enum values, source time
ranges, money text, quantity text, and three-letter uppercase currency codes.
It does not implement replay, calculate positions, calculate cash balances,
calculate cost, calculate PnL, access SQLite, call DataService, add server
actions, write tables, or copy test-only replay code.

AccountingEngine still declares `replayImplemented=false` and
`writeEnabled=false`. The DTO/parser boundary test is
`accounting_replay_dto_parser_boundary`.

## TASK-072 AccountingEngine empty ledger replay skeleton

`libs/AccountingEngine` now includes a production-side read-only replay skeleton
entry for empty ledger only.

The `AccountingReplayEngine::replayReadOnly` API accepts replay request DTOs and
fact DTO vectors. It only handles the FX001-equivalent empty ledger shape:

- `tradeFacts=[]`
- `cashFacts=[]`
- `marketPriceFacts=[]`
- `fxRateFacts=[]`

Empty ledger returns `implemented=true`, `replayExecuted=true`, `status=OK`,
empty positions, `cashSummary.cashBalanceText="0.00 CNY"`, and
`portfolioPnl.totalAssetsText="0.00 CNY"`.

Any non-empty facts currently return `UNSUPPORTED_SCENARIO` with
`REPLAY_NOT_IMPLEMENTED` and do not output successful position / cash / PnL
results. `replayImplemented=false` still means full production replay is not
implemented. This task does not implement FX002, buy/sell replay, costs, cash
flows, PnL, base position, sniper pool, SQLite access, DataService actions,
snapshot writes, TradeLog writes, or QML behavior.

The empty-ledger skeleton test is `accounting_replay_engine_empty_ledger`.

## TASK-073 AccountingEngine single buy replay skeleton

`libs/AccountingEngine` now supports a production-side read-only single BUY
replay skeleton in addition to empty ledger.

Supported shape:

- one `INITIAL_CASH` fact
- one `BUY` trade fact
- one account
- one portfolio
- one instrument
- CNY only

The single BUY skeleton returns one position with input quantity, `costAmount`
as `amount + fee`, `costPrice` as `costAmount / quantity`, and cash balance as
`initialCash - amount - fee`. Portfolio PnL remains an unavailable shell and
does not fabricate market value or unrealized PnL.

`replayImplemented=false` still means complete production replay is not
implemented. This task does not implement SELL, multi-BUY, multi-instrument,
multi-account, PnL, market value, base position, sniper pool, SQLite access,
DataService actions, snapshot writes, TradeLog writes, or QML behavior.

The single BUY skeleton test is `accounting_replay_engine_single_buy`.

Run tests:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## TASK-074 AccountingEngine partial sell replay skeleton

`libs/AccountingEngine` now includes a production-side read-only BUY + SELL
partial sell replay skeleton. It accepts exactly one `INITIAL_CASH` fact, one
CNY `BUY` trade fact, and one CNY `SELL` trade fact for the same account,
portfolio, and instrument.

The partial sell skeleton derives the remaining position quantity and cost,
cash balance, and realized PnL from DTO inputs only. It keeps market valuation
and unrealized PnL unavailable, and it does not output base-position or sniper
pool data.

Current production-side replay skeleton coverage is limited to empty ledger,
single BUY, and one BUY + one partial SELL. `replayImplemented=false` still
means complete production replay is not implemented.

This task does not implement multi-transaction replay, multi-instrument replay,
multi-account replay, market value, unrealized PnL, base position, sniper pool,
SQLite access, DataService actions, snapshot writes, TradeLog writes, or QML
behavior.

The partial sell skeleton test is `accounting_replay_engine_buy_sell_partial`.

Run tests:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## TASK-075 AccountingEngine sell exceeds position scenario

`libs/AccountingEngine` now includes a production-side read-only
`SELL_EXCEEDS_POSITION` controlled error scenario. It accepts exactly one CNY
`BUY` and one CNY `SELL` for the same account, portfolio, and instrument, and
returns a blocking error when `SELL` quantity is greater than the bought
quantity.

The controlled error returns `implemented=true`, `replayExecuted=true`,
`status=ERROR`, and one blocking `SELL_EXCEEDS_POSITION` issue. It does not
produce successful positions, cash summary, portfolio PnL, base-position, or
sniper-pool outputs, and it never emits a negative holding as a success result.

Current production-side replay skeleton coverage is limited to empty ledger,
single BUY, one BUY + one partial SELL, and one BUY + one SELL oversell
detection. `replayImplemented=false` still means complete production replay is
not implemented.

This task does not implement full missing-fee handling, full negative-cash
fixture handling, multi-transaction replay, multi-instrument replay,
multi-account replay, market value, unrealized PnL, base position, sniper pool,
SQLite access, DataService actions, snapshot writes, TradeLog writes, or QML
behavior.

The sell-exceeds-position test is
`accounting_replay_engine_sell_exceeds_position`.

Run tests:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## TASK-076 AccountingEngine missing fee scenario

`libs/AccountingEngine` now includes a production-side read-only `MISSING_FEE`
controlled warning scenario. It accepts the single BUY DTO shape and returns a
warning when the BUY `feeText` is missing, empty, or explicitly unavailable.

The controlled warning returns `implemented=true`, `replayExecuted=true`,
`status=WARNING`, and one non-blocking `MISSING_FEE` issue. It does not default
the missing fee to zero, and it does not produce successful positions, cash
summary, portfolio PnL, base-position, or sniper-pool outputs.

An explicit zero fee such as `feeText="0.00"` is not treated as missing and can
continue through the single BUY skeleton. Current production-side replay
skeleton coverage is limited to empty ledger, single BUY, one BUY + one partial
SELL, one BUY + one SELL oversell detection, and single BUY missing fee
detection. `replayImplemented=false` still means complete production replay is
not implemented.

This task does not implement the full negative-cash fixture scenario,
multi-transaction replay, multi-instrument replay, multi-account replay, market
value, unrealized PnL, base position, sniper pool, SQLite access, DataService
actions, snapshot writes, TradeLog writes, or QML behavior.

The missing-fee test is `accounting_replay_engine_missing_fee`.

Run tests:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## TASK-077 AccountingEngine negative cash scenario

`libs/AccountingEngine` now includes a production-side read-only
`NEGATIVE_CASH` controlled error scenario for single BUY DTO input. It accepts
one `INITIAL_CASH` fact and one CNY `BUY` trade fact, then returns an error
when `amount + fee` exceeds the available initial cash.

The controlled error returns `implemented=true`, `replayExecuted=true`,
`status=ERROR`, and one blocking `NEGATIVE_CASH` issue. It does not allow an
implicit overdraft and does not produce successful positions, cash summary,
portfolio PnL, base-position, or sniper-pool outputs.

`amount + fee == initialCash` is not negative cash and continues through the
single BUY skeleton with `cashBalanceText="0.00 CNY"`. Missing `feeText` still
returns `MISSING_FEE` instead of being overwritten by `NEGATIVE_CASH`; an
explicit zero fee such as `feeText="0.00"` remains valid.

Current production-side replay skeleton coverage is limited to empty ledger,
single BUY, one BUY + one partial SELL, one BUY + one SELL oversell detection,
single BUY missing fee detection, and single BUY negative cash detection.
`replayImplemented=false` still means complete production replay is not
implemented.

This task does not implement multi-transaction replay, multi-instrument replay,
multi-account replay, market value, unrealized PnL, base position, sniper pool,
SQLite access, DataService actions, snapshot writes, TradeLog writes, or QML
behavior.

The negative-cash test is `accounting_replay_engine_negative_cash`.

Run tests:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## TASK-078 AccountingEngine multi-instrument BUY scenario

`libs/AccountingEngine` now includes a production-side read-only
multi-instrument BUY scenario. It accepts one `INITIAL_CASH` fact and multiple
CNY `BUY` trade facts for the same account and portfolio, then groups positions
by `instrumentCode`.

The scenario returns `implemented=true`, `replayExecuted=true`, and
`status=OK`. Each instrument gets an independent position: `159509` and
`518880` remain separate, quantities and costs are not merged across
instrument codes, and cash is reduced by the sum of all BUY `amount + fee`.

Market valuation remains unavailable. Position `marketValueText` and
`unrealizedPnlText`, plus portfolio valuation fields, use `UNAVAILABLE`; this
does not fabricate market value or unrealized PnL. The scenario does not output
base-position or sniper-pool data.

Current production-side replay skeleton coverage is limited to empty ledger,
single BUY, one BUY + one partial SELL, one BUY + one SELL oversell detection,
single BUY missing fee detection, single BUY negative cash detection, and
same-account same-portfolio CNY multi-instrument BUY. `replayImplemented=false`
still means complete production replay is not implemented.

This task does not implement multi-account replay, multi-currency replay,
multi-instrument SELL, mixed multi-instrument BUY/SELL, real market price,
market value, unrealized PnL, base position, sniper pool, SQLite access,
DataService actions, snapshot writes, TradeLog writes, or QML behavior.

The multi-instrument BUY test is `accounting_replay_engine_multi_instrument_buy`.

Run tests:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## TASK-079 AccountingEngine multi-account BUY scenario

`libs/AccountingEngine` now includes a production-side read-only multi-account
BUY scenario. It accepts multiple `INITIAL_CASH` facts and multiple CNY `BUY`
trade facts for one portfolio, then groups positions by
`accountId + portfolioId + instrumentCode`.

The scenario returns `implemented=true`, `replayExecuted=true`, and
`status=OK`. Different accounts are not merged: `ACC-DEMO-001` and
`ACC-DEMO-002` keep independent positions, independent cost amounts, and
independent account cash summaries. Account cash is not combined into a single
summary, and `portfolioId` is preserved on positions and cash rows.

Market valuation remains unavailable. Position `marketValueText` and
`unrealizedPnlText`, plus portfolio valuation fields, use `UNAVAILABLE`; this
does not fabricate market value or unrealized PnL. The scenario does not output
base-position or sniper-pool data.

Current production-side replay skeleton coverage is limited to empty ledger,
single BUY, one BUY + one partial SELL, one BUY + one SELL oversell detection,
single BUY missing fee detection, single BUY negative cash detection,
same-account same-portfolio CNY multi-instrument BUY, and CNY BUY-only
multi-account isolation. `replayImplemented=false` still means complete
production replay is not implemented.

This task does not implement multi-currency replay, multi-account SELL,
cross-portfolio aggregation, real market price, market value, unrealized PnL,
base position, sniper pool, SQLite access, DataService actions, snapshot writes,
TradeLog writes, or QML behavior.

The multi-account BUY test is `accounting_replay_engine_multi_account_buy`.

Run tests:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## TASK-080 AccountingEngine multi-currency unsupported scenario

`libs/AccountingEngine` now includes a production-side read-only
multi-currency unsupported detection scenario. It detects CNY plus non-CNY
facts, such as CNY and USD BUY facts, when FX rate facts are missing.

The scenario returns `implemented=true`, `replayExecuted=true`,
`status=ERROR`, and blocking `MULTI_CURRENCY_UNSUPPORTED` plus
`FX_RATE_MISSING` issues. This is a rejection path only: it does not perform FX
conversion, does not fabricate FX rates, and does not treat USD as CNY.

No successful accounting outputs are emitted for the rejected multi-currency
case. The result does not output successful positions, cash summaries,
portfolio PnL, base-position, or sniper-pool data, and it does not fabricate
`totalAssetsText`, `marketValueText`, or `unrealizedPnlText`.

Current production-side replay skeleton coverage is limited to empty ledger,
single BUY, one BUY + one partial SELL, one BUY + one SELL oversell detection,
single BUY missing fee detection, single BUY negative cash detection,
same-account same-portfolio CNY multi-instrument BUY, CNY BUY-only
multi-account isolation, and multi-currency unsupported detection.
`supportsMultiCurrency=false`, `supportsFxRate=false`, and
`replayImplemented=false` still mean complete production replay, FX support,
and successful multi-currency replay are not implemented.

This task does not implement real FX rates, FX rate services, network access,
multi-currency valuation, market value, unrealized PnL, base position, sniper
pool, SQLite access, DataService actions, snapshot writes, TradeLog writes, or
QML behavior.

The multi-currency unsupported test is
`accounting_replay_engine_multi_currency_unsupported`.

Run tests:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## TASK-081 AccountingEngine missing market price scenario

`libs/AccountingEngine` now includes a production-side read-only missing market
price warning scenario. It detects a single CNY BUY replay request that asks for
`pnl` output while no valid matching `MarketPriceFactDto` is supplied.

The scenario returns `implemented=true`, `replayExecuted=true`,
`status=WARNING`, and a non-blocking `MARKET_PRICE_MISSING` issue. It may
return deterministic quantity, cost, and cash fields, but valuation fields stay
`UNAVAILABLE`.

This task does not fabricate `marketValueText`, `unrealizedPnlText`, or
`totalAssetsText`; it does not query market data, call a market service, use
network access, access SQLite, add DataService actions, write snapshots, write
TradeLog rows, modify QML, or modify schema. If a valid market price fact is
present, the skeleton still does not unlock successful market valuation.

Current production-side replay skeleton coverage is limited to empty ledger,
single BUY, one BUY + one partial SELL, one BUY + one SELL oversell detection,
single BUY missing fee detection, single BUY negative cash detection,
same-account same-portfolio CNY multi-instrument BUY, CNY BUY-only
multi-account isolation, multi-currency unsupported detection, and missing
market price detection. `supportsMarketPrice=false` and
`replayImplemented=false` still mean real market valuation and complete
production replay are not implemented.

The missing market price test is
`accounting_replay_engine_missing_market_price`.

Run tests:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## TASK-082 AccountingEngine replay skeleton milestone

The current milestone is v0.4 AccountingEngine Replay Skeleton. It documents
the production-side DTO-only skeleton scenario coverage and the next phase
boundary review:

- [AccountingEngine replay skeleton milestone](docs/35_accounting_engine_replay_skeleton_milestone.md)
- [AccountingEngine next phase boundary review](docs/36_accounting_engine_next_phase_boundary_review.md)
- [Release Notes - v0.4 AccountingEngine Replay Skeleton](docs/release_notes/v0_4_accounting_engine_replay_skeleton.md)

The production-side DTO-only skeleton scenarios completed so far are empty
ledger, single BUY, BUY + SELL partial sell, SELL_EXCEEDS_POSITION,
MISSING_FEE, NEGATIVE_CASH, multi-instrument BUY, multi-account BUY,
multi-currency unsupported, and missing market price.

This milestone does not mean full production replay is implemented.
`replayImplemented=false`, `productionReady=false`, and `writeEnabled=false`
remain correct. DataService actions are still not implemented, SQLite
integration is still not implemented, and no write path is enabled.

No snapshot writes, TradeLog writes, TradeDraft generation, QML accounting
calculation, real market feed, or FX rate service is included. The suggested
`v0.4.0-accounting-engine-replay-skeleton` tag is documented for manual use
after merge; this task does not create a tag.

## TASK-083 DataService read-only accounting action contracts

DataService accounting action contracts are now documented for future
read-only implementation review:

- [DataService read-only accounting action contracts](docs/37_dataservice_readonly_accounting_action_contracts.md)
- [DataService accounting no-write test plan](docs/38_dataservice_accounting_no_write_test_plan.md)

These documents define future contracts for `position.list`, `cash.summary`,
`portfolio.pnl.summary`, `base_position.summary`, and `sniper_pool.summary`.
They do not implement any action. DataService accounting replay is still not
implemented, DataService still does not call AccountingEngine, SQLite
integration is still not implemented, and no write path is enabled.

The contracts keep all future accounting actions read-only in their first
phase. They require no-write tests before implementation and explicitly forbid
snapshot writes, TradeLog writes, TradeDraft generation, direct QML
`DataServiceClient` calls, SQLite access from QML, and QML accounting
calculation.

## TASK-084 SQLite read-only facts query boundary

SQLite read-only facts query boundary docs are now defined for future
production accounting replay review:

- [SQLite read-only facts query boundary](docs/39_sqlite_readonly_facts_query_boundary.md)
- [Accounting facts source mapping](docs/40_accounting_facts_source_mapping.md)

These documents define how future production replay facts should be sourced and
mapped without implementing SQLite queries. `trade_log` is documented as the
fact ledger. Snapshots are documented as derived data, not facts sources.

This task does not implement SQLite query code, does not add a DataAccess
repository, does not add DataService actions, does not call AccountingEngine,
does not access SQLite, and does not write any database table. The next
implementation step still requires a separate authorization and no-write table
count tests.

## TASK-085 position.list DataService action guard

`position.list` is now registered as a DataService read-only action guard. The
guard returns `implemented=false`, `readOnly=true`, `writeEnabled=false`, and
`status=POSITION_LIST_NOT_AVAILABLE`.

The guard does not implement real `position.list`, does not return real
positions, does not access SQLite facts queries, does not call AccountingEngine,
does not add a DataAccess repository, does not write database tables, and does
not expose a QML entry point.

No-write coverage verifies that calling the guard does not change
`trade_log`, `trade_execution_group`, `trade_draft`, `cash_snapshot`,
`position_snapshot`, `portfolio_summary`, or `audit_log`.

New guard tests:

- `dataservice_position_list_guard`
- `dataservice_position_list_no_write`
- `dataservice_client_position_list_guard`

## TASK-086 SQLite read-only no-write harness skeleton

A test-only SQLite read-only / no-write harness skeleton now lives under
`tests/AccountingNoWrite`. It provides protected table row-count capture,
no-write comparison helpers, and a forbidden SQL scanner for future accounting
read-only action and facts query tests.

The harness protects `trade_log`, `trade_execution_group`, `trade_draft`,
`cash_snapshot`, `position_snapshot`, `portfolio_summary`, and `audit_log`.
The scanner detects write-oriented SQL such as `INSERT`, `UPDATE`, `DELETE`,
`CREATE`, `DROP`, `ALTER`, `REPLACE`, `UPSERT`, `VACUUM`,
`PRAGMA writable_schema`, `BEGIN IMMEDIATE`, and `BEGIN EXCLUSIVE`, while
allowing `SELECT`-only SQL.

This is test infrastructure only. It does not implement a real SQLite facts
query, does not add a DataAccess repository, does not add a DataService action,
does not modify `position.list` guard behavior, does not call AccountingEngine,
and does not write database tables.

New harness tests:

- `accounting_forbidden_sql_scanner`
- `accounting_no_write_harness`

## TASK-087 SQLite read-only trade facts query skeleton

DataAccess now has a read-only `trade_log` facts query skeleton for future
accounting replay integration. It only reads `trade_log` with a `SELECT` query,
returns DataAccess-side `TradeFactRow` records, and does not expose
AccountingEngine DTOs.

The reader supports optional `accountId`, `portfolioId`, `sourceFromTime`,
`sourceToTime`, `limit`, and `offset` filters. It maps current schema fields
such as `uid`, `created_at_utc` / `local_time`, `account_id`, `portfolio_id`,
`actual_code`, `action_type`, `quantity_1e6`, `price_1e6`, `amount_cents`,
`fee_cents`, `net_cash_impact_cents`, `trade_source`, and `memo`.

This is not replay and does not implement real `position.list`. DataService
still does not call this reader, AccountingEngine is not called, snapshots are
not read as facts, and no database tables are written. Tests use the TASK-086
no-write harness and forbidden SQL scanner to validate the boundary.

New reader tests:

- `dataaccess_accounting_trade_facts_readonly_query`
- `dataaccess_accounting_trade_facts_no_write`
- `dataaccess_accounting_trade_facts_sql_scan`

## TASK-088 Cash facts source boundary docs

Cash facts source boundary and readiness docs are now defined for future
read-only cash facts query review:

- [Cash facts source boundary](docs/41_cash_facts_source_boundary.md)
- [Cash facts query decision](docs/42_cash_facts_query_decision.md)

These documents define the future `CashFactDto` source boundary and recommend
not implementing cash facts query until an auditable schema source is confirmed.
`cash_snapshot`, `portfolio_summary`, and `position_snapshot` remain derived
data and must not be used to reverse-engineer cash facts.

This task does not implement a cash facts query, does not add a DataAccess cash
facts repository, does not add a DataService action, does not access SQLite,
does not call AccountingEngine, does not modify schema, does not change QML,
and does not write database tables.

## TASK-089 cash.summary DataService action guard

`cash.summary` is now registered as a DataService read-only action guard. The
guard returns `implemented=false`, `readOnly=true`, `writeEnabled=false`, and
`status=CASH_SUMMARY_NOT_AVAILABLE`.

The guard does not implement real `cash.summary`, does not return real
`cashBalance`, does not read cash facts, does not read `cash_snapshot` or
`portfolio_summary`, does not access SQLite facts queries, does not call
AccountingEngine, does not add a DataAccess repository, does not write database
tables, and does not expose a QML entry point.

No-write coverage verifies that calling the guard does not change `trade_log`,
`trade_execution_group`, `trade_draft`, `cash_snapshot`, `position_snapshot`,
`portfolio_summary`, or `audit_log`.

New guard tests:

- `dataservice_cash_summary_guard`
- `dataservice_cash_summary_no_write`
- `dataservice_client_cash_summary_guard`

## TASK-090 portfolio.pnl.summary DataService action guard

`portfolio.pnl.summary` is now registered as a DataService read-only action
guard. The guard returns `implemented=false`, `readOnly=true`,
`writeEnabled=false`, and `status=PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE`.

The guard does not implement real `portfolio.pnl.summary`, does not return real
`totalAssets`, `realizedPnl`, `unrealizedPnl`, `totalPnl`, or
`totalReturnRatio`, does not read `trade_log`, cash facts, market price facts,
`cash_snapshot`, `position_snapshot`, or `portfolio_summary`, does not access
SQLite facts queries, does not call AccountingEngine, does not add a DataAccess
repository, does not write database tables, and does not expose a QML entry
point.

No-write coverage verifies that calling the guard does not change `trade_log`,
`trade_execution_group`, `trade_draft`, `cash_snapshot`, `position_snapshot`,
`portfolio_summary`, or `audit_log`.

New guard tests:

- `dataservice_portfolio_pnl_summary_guard`
- `dataservice_portfolio_pnl_summary_no_write`
- `dataservice_client_portfolio_pnl_summary_guard`

## TASK-091 base_position.summary DataService action guard

`base_position.summary` is now registered as a DataService read-only action
guard. The guard returns `implemented=false`, `readOnly=true`,
`writeEnabled=false`, and `status=BASE_POSITION_SUMMARY_NOT_AVAILABLE`.

The guard does not implement real `base_position.summary`, does not return real
`targetBaseRatioText`, `lockedBaseAmountText`, or
`sellableAboveBaseAmountText`, does not read `trade_log`, `position_snapshot`,
`cash_snapshot`, or `portfolio_summary`, does not access SQLite facts queries,
does not call AccountingEngine, does not add a DataAccess repository, does not
generate trade suggestions, does not generate TradeDraft rows, does not write
database tables, and does not expose a QML entry point.

No-write coverage verifies that calling the guard does not change `trade_log`,
`trade_execution_group`, `trade_draft`, `cash_snapshot`, `position_snapshot`,
`portfolio_summary`, or `audit_log`.

New guard tests:

- `dataservice_base_position_summary_guard`
- `dataservice_base_position_summary_no_write`
- `dataservice_client_base_position_summary_guard`

## TASK-092 sniper_pool.summary DataService action guard

`sniper_pool.summary` is now registered as a DataService read-only action
guard. The guard returns `implemented=false`, `readOnly=true`,
`writeEnabled=false`, and `status=SNIPER_POOL_SUMMARY_NOT_AVAILABLE`.

The guard does not implement real `sniper_pool.summary`, does not return real
`poolAmountText`, `usedAmountText`, `remainingAmountText`, or `tierSummary`,
does not calculate T1-T6 tiers, does not read `trade_log`,
`position_snapshot`, `cash_snapshot`, or `portfolio_summary`, does not access
SQLite facts queries, does not call AccountingEngine, does not add a DataAccess
repository, does not generate trade suggestions, does not generate TradeDraft
rows, does not write database tables, and does not expose a QML entry point.

No-write coverage verifies that calling the guard does not change `trade_log`,
`trade_execution_group`, `trade_draft`, `cash_snapshot`, `position_snapshot`,
`portfolio_summary`, or `audit_log`.

New guard tests:

- `dataservice_sniper_pool_summary_guard`
- `dataservice_sniper_pool_summary_no_write`
- `dataservice_client_sniper_pool_summary_guard`

## v0.5 DataService Accounting Guard Suite

The v0.5 DataService Accounting Guard Suite milestone documents the completed
read-only accounting guard layer:

- [DataService accounting guard suite milestone](docs/43_dataservice_accounting_guard_suite_milestone.md)
- [DataService accounting guard no-write readiness review](docs/44_dataservice_accounting_guard_no_write_readiness_review.md)
- [Release notes - v0.5 DataService Accounting Guard Suite](docs/release_notes/v0_5_dataservice_accounting_guard_suite.md)

The guard suite covers `accounting.health`, `accounting.replay.preview`,
`position.list`, `cash.summary`, `portfolio.pnl.summary`,
`base_position.summary`, and `sniper_pool.summary`. The suite is complete as a
guard milestone only. Real accounting actions are still not implemented,
DataService still does not call AccountingEngine, SQLite facts query is still
not integrated into DataService, QML is not connected to these actions, and no
database writes are enabled.

Every unavailable accounting guard remains `implemented=false`,
`readOnly=true`, and `writeEnabled=false`. The no-write tests protect
`trade_log`, `trade_execution_group`, `trade_draft`, `cash_snapshot`,
`position_snapshot`, `portfolio_summary`, and `audit_log`.

## TASK-094 position.list real implementation boundary

The real `position.list` implementation boundary is now documented:

- [position.list real implementation boundary](docs/45_position_list_real_implementation_boundary.md)
- [position.list real implementation readiness checklist](docs/46_position_list_real_implementation_readiness_checklist.md)

This is documentation only. Real `position.list` is still not implemented, the
`position.list` guard remains unchanged, DataAccess trade facts reader is not
connected to DataService, DataService still does not call AccountingEngine,
SQLite integration is still not connected to `position.list`, and no database
writes are enabled.

## TASK-095 position.list mapping contract tests

Test-only `position.list` mapping contract coverage has been added for the
future real implementation boundary:

- `position_list_request_mapping_contract`
- `position_list_trade_fact_mapping_contract`
- `position_list_response_mapping_contract`

The tests cover request payload -> AccountingEngine `ReplayRequestDto`,
DataAccess `TradeFactRow` -> AccountingEngine `TradeFactDto`, and
AccountingEngine `AccountingReplayResult` -> future `PositionListResponse`
payload mapping. Real `position.list` remains unimplemented, the guard behavior
is unchanged, the tests do not call DataService actions, do not access SQLite,
and do not write database tables.

## TASK-096 position.list real action no-write skeleton

Test-only no-write skeleton coverage has been added for future real
`position.list` action paths:

- `position_list_real_action_no_write_skeleton`
- `position_list_real_action_failure_no_write_skeleton`

The skeleton covers valid request guard-path, invalid request, replay
unavailable, facts query unavailable, mapping failure, missing market price,
multi-currency unsupported, missing FX rate, SQLite read-only open failure, and
SQLite query failure no-write paths. Real `position.list` remains
unimplemented, the guard behavior is unchanged, DataAccess is not connected to
DataService, AccountingEngine is not called, no real SQLite facts query is
accessed, and no database tables are written.

## TASK-097 position.list readiness hardening

Readiness hardening docs have been added for the future real `position.list`
implementation:

- [docs/47_position_list_readiness_hardening.md](docs/47_position_list_readiness_hardening.md)
- [docs/48_position_list_first_stage_real_action_scope.md](docs/48_position_list_first_stage_real_action_scope.md)

These docs define the final Go / No-Go gates, first-stage scope, degraded
output policy, failure behavior matrix, no-write gate, and guard fallback
requirements. Real `position.list` is still not implemented, the
`position.list` guard behavior is unchanged, and no DataAccess / DataService /
AccountingEngine / SQLite integration or writes are enabled.

## TASK-098 position.list first-stage implementation test scaffolding

Test-only scaffolding has been added for the future first-stage real
`position.list` implementation:

- `position_list_first_stage_scope_contract`
- `position_list_first_stage_guard_transition_contract`
- `position_list_first_stage_failure_matrix_contract`

The scaffolding locks the allowed first-stage scope, explicitly excluded
scope, guard -> real action transition expectations, and failure matrix. Real
`position.list` remains unimplemented, the guard behavior is unchanged, the
scaffolding does not access SQLite, does not call real DataService replay
paths, and does not write database tables.

## TASK-099 ShellServices accounting controller contract docs

ShellServices accounting controller contract docs have been defined:

- [docs/49_shellservices_accounting_controller_contract.md](docs/49_shellservices_accounting_controller_contract.md)
- [docs/50_shell_accounting_viewmodel_state_contract.md](docs/50_shell_accounting_viewmodel_state_contract.md)

No ShellServices controller is implemented, QML is not modified, QML still must
not call DataServiceClient directly, and no writes are enabled.

## TASK-100 ShellServices accounting controller test scaffolding

ShellServices accounting controller test scaffolding has been added under
`tests/ShellAccountingControllerScaffolding`:

- `shell_accounting_controller_state_contract`
- `shell_accounting_controller_issue_contract`
- `shell_accounting_controller_privacy_contract`
- `shell_accounting_controller_no_trade_action_contract`

The scaffolding locks state, issue, privacy, and no trade action boundaries for
the future Shell accounting controller. Production ShellServices controller code
is still not implemented, QML is still not connected, the scaffolding does not
call real DataServiceClient, does not access SQLite, and does not write
database tables.

## TASK-101 ShellAccountingReadOnlyController production skeleton

`ShellAccountingReadOnlyController` production skeleton has been added in
`libs/ShellServices`.

The skeleton exposes read-only state, issue, privacy display, and no-trade
action boundaries. It does not connect to real services, does not connect QML,
does not access SQLite, does not call AccountingEngine, and does not generate
TradeDraft or trade suggestions.

New skeleton CTests:

- `shell_accounting_readonly_controller_skeleton_state`
- `shell_accounting_readonly_controller_skeleton_issue`
- `shell_accounting_readonly_controller_skeleton_privacy`
- `shell_accounting_readonly_controller_skeleton_no_trade_action`
- `shell_accounting_readonly_controller_skeleton_no_service_dependency`

## TASK-102 ShellAccountingReadOnlyController fake/client boundary tests

ShellAccountingReadOnlyController fake/client boundary tests have been added
under `tests/ShellAccountingFakeClientBoundary`:

- `shell_accounting_fake_client_guard_payloads`
- `shell_accounting_fake_client_state_transitions`
- `shell_accounting_fake_client_issue_mapping`
- `shell_accounting_fake_client_privacy_and_no_trade`
- `shell_accounting_fake_client_no_real_dependency`

The tests use test-only fake payloads to verify guard payload mapping, state
transitions, issue preservation, privacy display, and no-trade-action
boundaries. They do not call real DataServiceClient, do not connect QML, do
not access SQLite, do not call AccountingEngine, do not write database tables,
and do not generate TradeDraft or trade suggestions.

## TASK-103 Shell accounting service adapter boundary docs

Shell accounting service adapter boundary docs have been defined:

- [docs/51_shell_accounting_service_adapter_boundary.md](docs/51_shell_accounting_service_adapter_boundary.md)
- [docs/52_shell_accounting_service_integration_readiness.md](docs/52_shell_accounting_service_integration_readiness.md)

The future controller should depend on a `ShellAccountingServiceAdapter`
contract instead of directly hard-depending on `DataServiceClient`. The real
DataServiceClient adapter is still unimplemented, QML is still not wired, and
no writes are enabled.

## TASK-104 ShellAccountingServiceAdapter interface skeleton

`ShellAccountingServiceAdapter` interface skeleton has been added in
`libs/ShellServices` with service request/result contract types.

New tests:

- `shell_accounting_service_adapter_interface`
- `shell_accounting_service_result_contract`
- `shell_accounting_service_adapter_no_real_dependency`
- `shell_accounting_controller_adapter_injection`

The real DataServiceClient adapter is still unimplemented. The controller has
only an optional adapter pointer skeleton; it does not call real services. QML
is still not wired, SQLite is not accessed, AccountingEngine is not called, and
no writes, TradeDraft, or trade suggestions are enabled.

## TASK-105 ShellAccountingReadOnlyController fake adapter injection behavior

`ShellAccountingReadOnlyController` now consumes the abstract
`ShellAccountingServiceAdapter` in refresh methods when a test-only fake adapter
is injected:

- `refreshPositionList`
- `refreshCashSummary`
- `refreshPortfolioPnlSummary`
- `refreshBasePositionSummary`
- `refreshSniperPoolSummary`

New fake adapter injection tests:

- `shell_accounting_controller_fake_adapter_refresh_success`
- `shell_accounting_controller_fake_adapter_unavailable`
- `shell_accounting_controller_fake_adapter_protocol_error`
- `shell_accounting_controller_fake_adapter_issue_mapping`
- `shell_accounting_controller_fake_adapter_privacy_no_trade`
- `shell_accounting_controller_fake_adapter_no_real_dependency`

This is still not a real DataServiceClient adapter. The fake adapter remains
test-only, QML is still not wired, SQLite is not accessed, AccountingEngine is
not called, and no database writes, TradeDraft, trade suggestions, strategies,
or broker orders are enabled.

## TASK-106 ShellAccountingDataServiceAdapter boundary / test plan

ShellAccountingDataServiceAdapter boundary and test plan docs have been added:

- [docs/53_shell_accounting_dataservice_adapter_boundary.md](docs/53_shell_accounting_dataservice_adapter_boundary.md)
- [docs/54_shell_accounting_dataservice_adapter_test_plan.md](docs/54_shell_accounting_dataservice_adapter_test_plan.md)

The future real adapter may only wrap read-only DataServiceClient accounting
guard wrappers. The real adapter is still unimplemented, real DataServiceClient
calls are still not wired, QML is still not wired, SQLite is not accessed,
AccountingEngine is not called, and no writes, TradeDraft, trade suggestions,
strategies, or broker orders are enabled.

## TASK-107 ShellAccountingDataServiceAdapter spy/wrapper scaffolding

ShellAccountingDataServiceAdapter spy/wrapper test scaffolding has been added
under `tests/ShellAccountingDataServiceAdapterScaffolding`.

New tests:

- `shell_accounting_dataservice_adapter_spy_allowlist`
- `shell_accounting_dataservice_adapter_spy_method_mapping`
- `shell_accounting_dataservice_adapter_spy_request_mapping`
- `shell_accounting_dataservice_adapter_spy_response_mapping`
- `shell_accounting_dataservice_adapter_spy_error_mapping`
- `shell_accounting_dataservice_adapter_spy_no_write_no_trade`
- `shell_accounting_dataservice_adapter_spy_no_real_dependency`

The scaffolding is test-only. The real adapter is still unimplemented, real
DataServiceClient calls are still not wired, QML is still not wired, and no
writes, TradeDraft, trade suggestions, strategies, or broker orders are
enabled.

## TASK-108 ShellAccountingDataServiceAdapter production skeleton

`ShellAccountingDataServiceAdapter` production skeleton has been added under
`libs/ShellServices`.

New skeleton files:

- `libs/ShellServices/include/ShellServices/ShellAccountingDataServiceAdapter.h`
- `libs/ShellServices/src/ShellAccountingDataServiceAdapter.cpp`

New tests:

- `shell_accounting_dataservice_adapter_skeleton_interface`
- `shell_accounting_dataservice_adapter_skeleton_not_connected`
- `shell_accounting_dataservice_adapter_skeleton_no_live_calls`
- `shell_accounting_dataservice_adapter_skeleton_no_write_no_trade`
- `shell_accounting_dataservice_adapter_skeleton_controller_integration`

The skeleton implements `ShellAccountingServiceAdapter`, but it does not include
or call DataServiceClient, DataServiceApi, DataAccess, AccountingEngine, SQLite,
QtQuick, or QML. Every fetch method returns a controlled
`SHELL_ACCOUNTING_DATASERVICE_ADAPTER_NOT_CONNECTED` / `UNAVAILABLE` result with
`implemented=false`, `readOnly=true`, and `writeEnabled=false`. QML is still not
wired, no live service calls are made, and no writes, TradeDraft, trade
suggestions, strategies, or broker orders are enabled.

## TASK-109 ShellAccountingDataServiceAdapter live-call gate

ShellAccountingDataServiceAdapter live-call gate docs and test-only gate
descriptors have been added:

- `docs/55_shell_accounting_dataservice_adapter_live_call_gate.md`
- `docs/56_shell_accounting_dataservice_adapter_live_call_acceptance_checklist.md`

New CTest gates:

- `shell_accounting_dataservice_adapter_live_call_allowlist_gate`
- `shell_accounting_dataservice_adapter_live_call_mapping_gate`
- `shell_accounting_dataservice_adapter_live_call_error_gate`
- `shell_accounting_dataservice_adapter_live_call_no_write_gate`
- `shell_accounting_dataservice_adapter_live_call_readiness_gate`

The live-call gate is not a live adapter implementation. The production
`ShellAccountingDataServiceAdapter` still returns not connected / unavailable.
No real DataServiceClient calls, QML wiring, SQLite access, AccountingEngine
calls, database writes, TradeDraft, trade suggestions, strategy execution, or
broker orders are enabled.

## TASK-066 Accounting Replay Minimal FX012

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER` through
  `FX012_MISSING_MARKET_PRICE`.
- FX012 only detects missing market price for the fixture-backed BUY position.
- FX012 returns `implemented=true`, `replayExecuted=true`, `status=WARNING`,
  and a non-blocking `MARKET_PRICE_MISSING` issue.
- FX012 exposes quantity / cost through `positionListResponseRaw`; it does not
  fabricate `marketValueText` or `unrealizedPnlText`.
- FX013 is implemented in a later test-only minimal replay task.
- This remains test-only code under `tests/AccountingFixtures`; it does not
  access SQLite, call DataService, query real market data, perform network
  requests, write files or database tables, add server actions, add write
  capability, modify QML, or modify `migrations/001_initial_schema.sql`.
- Minimal FX012 CTest: `accounting_replay_minimal_fx012`.

## TASK-067 Accounting Replay Minimal FX013

- `AccountingReplayMinimalEngine` now supports `FX001_EMPTY_LEDGER` through
  `FX013_MULTI_CURRENCY_UNSUPPORTED`.
- FX013 only detects unsupported multi-currency replay without an FX policy.
- FX013 returns `implemented=true`, `replayExecuted=true`, `status=ERROR`, and
  blocking `MULTI_CURRENCY_UNSUPPORTED` / `FX_RATE_MISSING` issues matching the
  fixture contract.
- FX013 does not perform FX conversion.
- FX013 does not fabricate `totalAssetsText`, `marketValueText`,
  `unrealizedPnlText`, or `totalReturnRatioText`.
- This completes test-only minimal engine coverage for FX001-FX013. It does not
  mean production accounting replay is complete.
- This remains test-only code under `tests/AccountingFixtures`; it does not
  access SQLite, call DataService, query FX or market services, perform network
  requests, write files or database tables, add server actions, add write
  capability, modify QML, or modify `migrations/001_initial_schema.sql`.
- Minimal FX013 CTest: `accounting_replay_minimal_fx013`.

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

## TASK-135 Real Read-only Data Adapter

ShellAccounting now has an authorized read-only adapter/DataService action
chain for `position.list`, `cash.summary`, `portfolio.pnl.summary`,
`base_position.summary`, and `sniper_pool.summary`. The UI remains read-only and
Unavailable-safe. No database writes, TradeDraft, trade suggestion, strategy
execution, broker order, trading buttons, direct SQLite access, or direct
AccountingEngine calls are enabled. See
`docs/80_shell_accounting_real_data_adapter_implementation.md`.

## TASK-136 DataService Read-only Facts Query Gate

ShellAccounting now has a DataService read-only facts query gate and test plan.
The facts query is still not implemented. SQLite and AccountingEngine replay are
still not connected. No database writes, TradeDraft, trade suggestion, strategy
execution, broker order, or trading UI are enabled. See
`docs/81_shell_accounting_dataservice_readonly_facts_query_gate.md` and
`docs/82_shell_accounting_dataservice_readonly_facts_query_test_plan.md`.

## TASK-137 DataService Read-only Facts Query Implementation

ShellAccounting now implements DataService-internal SELECT-only facts queries
for `position.list`, `cash.summary`, `portfolio.pnl.summary`,
`base_position.summary`, and `sniper_pool.summary`. The Shell / QML boundary
still reads through DataService only; there is no direct SQLite or DataAccess
access from Shell/QML, no AccountingEngine replay, no snapshot rebuild, no
database writes, no TradeDraft, no trade suggestion, and no trading UI. See
`docs/83_shell_accounting_dataservice_readonly_facts_query_implementation.md`.

## TASK-138 Replay Snapshot Rebuild Gate

ShellAccounting now has an AccountingEngine replay / snapshot rebuild gate and
test plan. AccountingEngine replay is still not connected to ShellAccounting,
snapshot rebuild is still not implemented, snapshot write is still not
authorized, and there is still no TradeDraft, trade suggestion, strategy
execution, broker order, or trading UI. See
`docs/84_shell_accounting_replay_snapshot_rebuild_gate.md` and
`docs/85_shell_accounting_replay_snapshot_rebuild_test_plan.md`.

## TASK-139 Read-only Replay Implementation

ShellAccounting now has a DataService-internal read-only replay path for the
authorized actions `position.list`, `cash.summary`, `portfolio.pnl.summary`,
`base_position.summary`, and `sniper_pool.summary`. Replay is explicit through
the DataService action boundary, Shell/QML/Presenter still do not call
AccountingEngine directly, and TASK-137 read-only facts query remains the
rollback fallback. Snapshot rebuild/write remains unimplemented and there is no
database write, TradeDraft, trade suggestion, strategy execution, broker order,
or trading UI. See
`docs/86_shell_accounting_readonly_replay_implementation.md`.

## TASK-140 Snapshot Rebuild Write Gate

ShellAccounting now has a snapshot rebuild / snapshot write gate and test plan.
Snapshot rebuild is still not implemented, snapshot write is still not
authorized, and TASK-139 read-only replay remains the fallback. There is still
no TradeDraft, trade suggestion, strategy execution, broker order, order
placement, or trading UI. See
`docs/87_shell_accounting_snapshot_rebuild_write_gate.md` and
`docs/88_shell_accounting_snapshot_rebuild_write_test_plan.md`.

## TASK-141 Snapshot Rebuild Implementation Gate

ShellAccounting now has a snapshot rebuild implementation gate and test plan.
Snapshot rebuild remains unimplemented, snapshot write remains separately
unauthorized, and TASK-139 read-only replay remains the fallback. The future
rebuild boundary is DataService-only, read-only replay input only, in-memory
preview output only, and still has no database write, TradeDraft, trade
suggestion, strategy execution, broker order, or trading UI. See
`docs/89_shell_accounting_snapshot_rebuild_implementation_gate.md` and
`docs/90_shell_accounting_snapshot_rebuild_implementation_test_plan.md`.

## TASK-142 Snapshot Rebuild Preview

ShellAccounting now has a DataService-internal snapshot rebuild preview. The
preview is built from TASK-139 read-only replay and is returned as an in-memory
`snapshotRebuildPreview` payload on read-only action responses. Snapshot write
is still unimplemented and separately unauthorized. There is still no database
write, TradeDraft, trade suggestion, strategy execution, broker order, or
trading UI. See
`docs/91_shell_accounting_snapshot_rebuild_implementation.md`.

## TASK-143 Snapshot Write Authorization Gate

ShellAccounting now has a snapshot write authorization gate and test plan.
Snapshot write remains unimplemented, no snapshot tables are written, and
TASK-142 snapshot rebuild preview remains an in-memory read-only payload. Future
snapshot write must be DataService-only, explicitly authorized, preview-input
only, allowlisted by table, transactional, rollback-ready, and still no
TradeDraft, trade suggestion, strategy execution, broker order, or trading UI.
See `docs/92_shell_accounting_snapshot_write_authorization_gate.md` and
`docs/93_shell_accounting_snapshot_write_authorization_test_plan.md`.

## TASK-144 Snapshot Write Implementation

ShellAccounting now has a DataService-internal snapshot write implementation.
The write action persists TASK-142 `snapshotRebuildPreview` only into
`cash_snapshot`, `position_snapshot`, and `portfolio_summary` with
transactional rollback and idempotent duplicate handling. It does not write
`trade_log`, `trade_execution_group`, `trade_draft`, or `audit_log`, and it
still does not generate TradeDraft, trade suggestion, strategy execution, broker
order, production QML changes, production startup changes, or trading UI. See
`docs/94_shell_accounting_snapshot_write_implementation.md`.

## TASK-145 Audit Write Authorization Gate

ShellAccounting now has an audit write authorization gate and test plan. Audit
write remains unimplemented, `audit_log` remains unwritten, and TASK-144
snapshot write behavior is unchanged. Future audit write must be DataService
only, explicitly authorized, sourced only from an authorized DataService write
result, sanitized, rollback-ready, and still no TradeDraft, trade suggestion,
strategy execution, broker order, production QML changes, production startup
changes, or trading UI. See
`docs/95_shell_accounting_audit_write_authorization_gate.md` and
`docs/96_shell_accounting_audit_write_authorization_test_plan.md`.

## TASK-146 Audit Write Implementation

ShellAccounting now has a DataService-internal `accounting.audit.write`
implementation. It records sanitized audit events for TASK-144 snapshot write
results only, requires `TASK-146_AUDIT_WRITE`, and writes only `audit_log`.
It does not write `trade_log`, `trade_execution_group`, or `trade_draft`, and it
does not modify production QML/startup, schema, TradeDraft, trade suggestions,
strategy execution, broker orders, or trading UI. See
`docs/97_shell_accounting_audit_write_implementation.md`.

## TASK-147 TradeDraft Authorization Gate

ShellAccounting now has a TradeDraft authorization gate and test plan.
TradeDraft remains unimplemented, `trade_draft` remains unwritten, and
`trade_log` / `trade_execution_group` remain untouched. Future TradeDraft must
be DataService-only, explicitly authorized, sanitized, audited,
idempotent, rollback-ready, and must not be treated as execution, strategy, or
broker order. Production QML/startup remain unchanged. See
`docs/98_shell_accounting_tradedraft_authorization_gate.md` and
`docs/99_shell_accounting_tradedraft_authorization_test_plan.md`.

## TASK-148 TradeDraft Implementation

ShellAccounting now implements DataService-internal TradeDraft creation through
`accounting.tradedraft.create` with `TASK-148_TRADEDRAFT_WRITE` authorization.
TradeDraft is a pending draft only: it is not execution, not broker order, not
strategy execution, and not user confirmation. The implementation writes only
`trade_draft` and sanitized `audit_log`, rolls back draft creation if audit
write fails, and keeps duplicate requests idempotent. It does not write
`trade_log` or `trade_execution_group`, does not modify production QML/startup
or schema, and does not add trading UI. See
`docs/100_shell_accounting_tradedraft_implementation.md`.

## TASK-149 TradeDraft Confirmation Authorization Gate

ShellAccounting now has a TradeDraft confirmation / TradeLog write
authorization gate and test plan. Confirmation remains unimplemented,
`trade_log` and `trade_execution_group` remain unwritten, and TASK-148
TradeDraft creation behavior remains unchanged. Future confirmation must be
DataService-only, explicitly authorized, audited, transactional, idempotent,
and must not be treated as broker order, strategy execution, automatic
trading, or production trading UI. See
`docs/101_shell_accounting_tradedraft_confirmation_authorization_gate.md` and
`docs/102_shell_accounting_tradedraft_confirmation_authorization_test_plan.md`.

## TASK-150 TradeDraft Confirmation Implementation

ShellAccounting now implements DataService-internal TradeDraft confirmation
through `accounting.tradedraft.confirm` with `TASK-150_TRADEDRAFT_CONFIRM`
authorization. Confirmation atomically writes `trade_execution_group` and
`trade_log`, updates only the confirmed `trade_draft` status / confirmation
metadata, and writes a sanitized `audit_log` event in the same transaction.
Audit failure rolls back the ledger write, and duplicate confirmation does not
create duplicate ledger rows. This is not broker order placement, strategy
execution, automatic trading, or production trading UI. Production QML/startup
and schema remain unchanged. See
`docs/103_shell_accounting_tradedraft_confirmation_implementation.md`.

## TASK-151 Production Trading UI Authorization Gate

ShellAccounting now has a production trading UI authorization gate and test
plan. Production trading UI remains unimplemented: QML does not create or
confirm TradeDraft records, does not bind `accounting.tradedraft.create` or
`accounting.tradedraft.confirm`, does not call broker/order APIs, does not
execute strategy, and does not place orders. Future UI must keep the
presenter/controller/adapter/DataService boundary, preserve
`TASK-148_TRADEDRAFT_WRITE` and `TASK-150_TRADEDRAFT_CONFIRM`, require explicit
user confirmation, and keep the read-only accounting page as rollback fallback.
See
`docs/104_shell_accounting_production_trading_ui_authorization_gate.md` and
`docs/105_shell_accounting_production_trading_ui_authorization_test_plan.md`.

## TASK-152 Production Trading UI Implementation

ShellAccounting now exposes a minimal production QML Draft workflow. The UI
can request Draft creation and explicit Draft confirmation only through the
Presenter / Controller / Adapter / DataService boundary, preserving
`TASK-148_TRADEDRAFT_WRITE` and `TASK-150_TRADEDRAFT_CONFIRM` backend
authorization. It displays Draft-only, confirmed-ledger, disabled, transport,
and error states without silent success. QML does not call DataServiceClient,
SQLite, DataAccess, broker/order APIs, or StrategyEngine directly, and schema
remains unchanged. See
`docs/106_shell_accounting_production_trading_ui_implementation.md`.

## TASK-153 Broker Order Authorization Gate

ShellAccounting now has a broker order authorization gate and test plan.
Broker order placement remains unimplemented: there is no broker adapter, no
broker SDK call, no real order placement, no StrategyEngine execution, and no
automatic trading. Future broker order work must be DataService-only, based on
an already confirmed TradeDraft / ledger fact, independently authorized,
explicitly user-confirmed, dry-run/rollback-ready, audited, and still must not
accept raw QML payload as an order. See
`docs/107_shell_accounting_broker_order_authorization_gate.md` and
`docs/108_shell_accounting_broker_order_authorization_test_plan.md`.

## TASK-154 Broker Order Dry-run Boundary

ShellAccounting now has a DataService-only broker order dry-run boundary:
`accounting.broker_order.dry_run` with
`TASK-154_BROKER_ORDER_DRY_RUN`. It accepts only a confirmed TradeDraft /
ledger fact, returns dry-run / broker-disabled semantics, and writes only a
sanitized `audit_log` event. It does not call a broker SDK, place a real order,
write `trade_log`, `trade_execution_group`, or `trade_draft`, execute strategy,
or enable automatic trading. See
`docs/109_shell_accounting_broker_order_dry_run_implementation.md`.

## TASK-155 Real Broker Order Authorization Gate

ShellAccounting now has a real broker order authorization gate and test plan.
Real broker order placement remains unimplemented: there is no broker adapter,
no broker SDK import/link/call, no sandbox or paper trading broker call, no
real order placement, no StrategyEngine execution, and no automatic trading.
Future real broker order work must require a successful TASK-154 dry-run
preflight, confirmed-ledger input, final user confirmation, an independent
authorization token, kill switch, idempotency key, sanitized audit,
reconciliation policy, and incident rollback / containment. See
`docs/110_shell_accounting_real_broker_order_authorization_gate.md` and
`docs/111_shell_accounting_real_broker_order_authorization_test_plan.md`.

## TASK-156 Real Broker Order Implementation Gate

ShellAccounting now has the final implementation gate and test plan before any
real broker order implementation can be considered. Real broker order remains
unimplemented: there is no broker adapter, no broker SDK import/link/call, no
broker network call, no sandbox or paper broker call, no real order placement,
no StrategyEngine execution, no automatic trading, no production QML/startup
change, and no schema change. Future implementation must remain DataService
only, default-disabled, dry-run-preflighted, final-user-confirmed,
independently authorized, risk-guarded, idempotent, audited, reconciled, and
kill-switchable. See
`docs/112_shell_accounting_real_broker_order_implementation_gate.md` and
`docs/113_shell_accounting_real_broker_order_implementation_test_plan.md`.

## TASK-157 Broker Adapter Interface Scaffold

ShellAccounting now has a DataServiceApi-owned broker adapter interface
scaffold: `ShellAccountingBrokerOrderPort`, minimal order intent request /
response DTOs, broker order status mapping, and a disabled/null implementation
that returns disabled with no side effects. A test-only fake lives only under
`tests/ShellAccountingBrokerAdapterInterfaceScaffold`. This does not implement
a real broker adapter, import a broker SDK, call sandbox / paper / real broker
endpoints, place orders, add credentials, change production QML/startup, change
TASK-148 / TASK-150 / TASK-152 / TASK-154 behavior, execute strategy, enable
automatic trading, or modify schema. See
`docs/114_shell_accounting_broker_adapter_interface_scaffold.md`.

## TASK-158 Broker Adapter Implementation Gate

ShellAccounting now has a broker adapter implementation gate and test plan.
The TASK-157 interface scaffold remains the only production broker-facing
adapter boundary: no concrete broker adapter is implemented, no broker SDK is
imported or linked, no broker network call is reachable, no sandbox / paper /
real broker is called, no real order is placed, no credentials or secrets are
added, and no production QML/startup or schema behavior changes. Future broker
adapter work must be separately authorized, default disabled, DataService-only,
mode-isolated across fake test-only / sandbox / paper / real, CI-network-free,
credential-isolated, kill-switchable, and still separate from reconciliation,
cancellation, correction, strategy execution, automatic trading, and real order
placement. See
`docs/115_shell_accounting_broker_adapter_implementation_gate.md` and
`docs/116_shell_accounting_broker_adapter_implementation_test_plan.md`.

## TASK-159 Broker Adapter Disabled Wiring

ShellAccounting now wires the disabled/null broker adapter into the DataService
internal broker boundary. The default provider returns
`DisabledShellAccountingBrokerOrderPort`, and `accounting.broker_order.dry_run`
continues to report dry-run-only / broker-disabled semantics while exposing the
internal disabled port status. This keeps fake adapters test-only and does not
implement a real broker adapter, import a broker SDK, call sandbox / paper /
real broker endpoints, place real orders, generate real broker order ids, add
credentials, change production QML/startup, change TASK-148 / TASK-150 /
TASK-152 / TASK-154 behavior, execute strategy, enable automatic trading, or
modify schema. See
`docs/117_shell_accounting_broker_adapter_disabled_wiring.md`.

## TASK-160 Broker Sandbox Adapter Authorization Gate

ShellAccounting now has a sandbox broker adapter authorization gate and test
plan. TASK-160 is gate-only: it does not implement sandbox, paper, or real
broker adapters; does not import broker SDKs; does not call broker networks; and
does not place real orders. Disabled/null broker adapter wiring from TASK-159
remains unchanged and remains the default.

The gate documents sandbox / paper / real mode isolation, fake-adapter
test-only scope, credential isolation, CI no-network policy, DataService-only
broker adapter access, and fail-closed rollback requirements. See
`docs/118_shell_accounting_broker_sandbox_adapter_authorization_gate.md` and
`docs/119_shell_accounting_broker_sandbox_adapter_authorization_test_plan.md`.

## TASK-161 Broker Sandbox Adapter Scaffold

ShellAccounting now has a scaffold-only sandbox broker adapter class behind the
existing DataServiceApi broker order port. The scaffold is not a usable broker
adapter: it returns sanitized not-configured / disabled / dry-run-only
semantics, returns no real broker order id, performs no order placement, reads
no credentials, calls no broker network or SDK, writes no database table or
audit row, and remains unwired from production QML/startup.

The TASK-159 disabled/null provider remains the default provider, and TASK-154
dry-run behavior remains unchanged. TASK-161 adds tests and docs for the
scaffold-only boundary, CI no-network policy, credential isolation, no real
order placement, and no strategy / automatic trading behavior. See
`docs/120_shell_accounting_broker_sandbox_adapter_scaffold.md` and
`docs/121_shell_accounting_broker_sandbox_adapter_scaffold_test_plan.md`.

## TASK-162 Broker Sandbox Mode Selector

ShellAccounting now has a disabled-default broker order port mode selector /
factory boundary. The selector can explicitly choose disabled mode, sandbox
scaffold mode, or unsupported mode in tests. It is not runtime broker
enablement: the default provider remains TASK-159 disabled/null, DataService
runtime actions are unchanged, and sandbox mode only returns the TASK-161
not-configured / unavailable / dry-run-only scaffold.

TASK-162 adds no broker SDK, no network calls, no credential reads or usable
credential examples, no real broker order id, no order placement, no database
or audit writes, no reconciliation / cancellation / correction, no strategy
execution, no automatic trading, and no production QML/startup changes. See
`docs/122_shell_accounting_broker_sandbox_mode_selector.md` and
`docs/123_shell_accounting_broker_sandbox_mode_selector_test_plan.md`.

## TASK-163 Broker Sandbox Runtime Selector Authorization Gate

ShellAccounting now has a gate for future broker sandbox runtime selector
wiring. TASK-163 is gate-only: it does not connect the TASK-162 selector to
`DataServiceActions`, does not enable sandbox runtime, does not replace the
TASK-159 disabled/null provider, and does not modify production QML, startup,
Presenter, Controller, ShellServices, ShellCore, schema, or broker behavior.

The gate requires future runtime selector wiring to be separately authorized,
DataService-only, default-disabled, CI no-network, credential-isolated, and
fail-closed for empty / unknown / unsupported / paper / real modes. It adds no
broker SDK, network calls, credentials, endpoints, real broker order ids, order
placement, database / audit / ledger writes, reconciliation, cancellation,
correction, strategy execution, or automatic trading. See
`docs/124_shell_accounting_broker_sandbox_runtime_selector_authorization_gate.md`
and
`docs/125_shell_accounting_broker_sandbox_runtime_selector_authorization_test_plan.md`.

## TASK-164 Broker Sandbox Runtime Selector Disabled Wiring

ShellAccounting now wires the broker dry-run path through the TASK-162 selector,
but only with `defaultShellAccountingBrokerOrderPortMode()`. The default mode is
still disabled, so runtime behavior remains equivalent to the TASK-159
disabled/null provider. This is not sandbox runtime enablement.

TASK-164 adds no runtime mode source, no request/QML/config/env/command-line
mode read, no sandbox / paper / real runtime selection, no broker SDK, no
network calls, no credentials or endpoints, no real broker order id, no order
placement, no database / audit / ledger write, no reconciliation /
cancellation / correction, no strategy execution, no automatic trading, and no
production QML/startup/schema change. See
`docs/126_shell_accounting_broker_sandbox_runtime_selector_disabled_wiring.md`
and
`docs/127_shell_accounting_broker_sandbox_runtime_selector_disabled_wiring_test_plan.md`.

## TASK-165 Broker Runtime Mode Source Authorization Gate

TASK-165 adds a gate-only authorization boundary for a future broker runtime
mode source. It does not implement a mode source, does not read
payload/QML/config/env/command-line/file/database/secret-store values, and does
not enable sandbox runtime. The TASK-164 disabled-default selector wiring
remains unchanged.

TASK-165 adds no broker SDK, network, credentials, endpoint, real broker order
id, order placement, database write, audit write, ledger write,
reconciliation, cancellation, correction, strategy execution, or automatic
trading. See
`docs/128_shell_accounting_broker_runtime_mode_source_authorization_gate.md`
and
`docs/129_shell_accounting_broker_runtime_mode_source_authorization_test_plan.md`.

## TASK-166 Broker Runtime Mode Source Disabled Scaffold

TASK-166 adds the internal disabled-only broker runtime mode source scaffold.
The source only returns disabled mode and `DataServiceActions` uses it before
calling `shellAccountingBrokerOrderPortForMode(brokerPortMode)`, preserving the
TASK-159 disabled/null provider behavior.

TASK-166 does not implement an external runtime mode source, does not enable
sandbox runtime, does not add SDK/network/credentials/endpoint values, does not
create a real broker order id, does not place orders, does not write database,
audit, or ledger tables, and does not add reconciliation, cancellation,
correction, strategy execution, or automatic trading. See
`docs/130_shell_accounting_broker_runtime_mode_source_disabled_scaffold.md`
and
`docs/131_shell_accounting_broker_runtime_mode_source_disabled_scaffold_test_plan.md`.

## TASK-167 Broker Sandbox Runtime Mode Source Authorization Gate

TASK-167 adds a gate-only boundary for a possible future sandbox runtime mode
source. It does not implement sandbox mode source selection, does not enable
sandbox runtime, and does not change the TASK-166 disabled-only source.

The gate keeps payload/QML/config/env/command-line/file/database/secret-store
mode sources unauthorized. It also keeps broker SDK, network, credentials,
endpoint, real broker order id, order placement, DB/audit/ledger write,
reconciliation, cancellation, correction, strategy execution, and automatic
trading out of the runtime path. See
`docs/132_shell_accounting_broker_sandbox_runtime_mode_source_authorization_gate.md`
and
`docs/133_shell_accounting_broker_sandbox_runtime_mode_source_authorization_test_plan.md`.

## TASK-168 Broker Sandbox Runtime Mode Source Scaffold

TASK-168 adds a directly testable sandbox runtime mode source scaffold. This is
a scaffold, not runtime enablement: direct tests may instantiate
`SandboxShellAccountingBrokerRuntimeModeSourceScaffold`, but
`defaultShellAccountingBrokerRuntimeModeSource()` remains disabled-only and
`DataServiceActions` is not wired to sandbox runtime.

TASK-168 does not add payload/QML/config/env/command-line/file/database/secret
store mode sources, broker SDK, network, credentials, endpoint, real broker
order id, order placement, DB/audit/ledger write, reconciliation,
cancellation, correction, strategy execution, or automatic trading. See
`docs/134_shell_accounting_broker_sandbox_runtime_mode_source_scaffold.md`
and
`docs/135_shell_accounting_broker_sandbox_runtime_mode_source_scaffold_test_plan.md`.

## TASK-169 Broker Sandbox Runtime Mode Source Selector Authorization Gate

TASK-169 adds a gate-only boundary for a possible future sandbox runtime mode
source selector. TASK-170 adds the direct-test-only selector scaffold, still
without modifying `DataServiceActions` or enabling sandbox runtime.

The TASK-168 sandbox source scaffold remains direct-test-only. The default
runtime source remains disabled-only, external mode sources remain unauthorized,
and broker SDK, network, credentials, endpoint, real broker order id, order
placement, DB/audit/ledger write, reconciliation, cancellation, correction,
strategy execution, and automatic trading remain out of scope. See
`docs/136_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_gate.md`
and
`docs/137_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_test_plan.md`.

## TASK-170 Broker Sandbox Runtime Mode Source Selector Scaffold

TASK-170 adds `shellAccountingBrokerRuntimeModeSourceForMode` as a directly
testable selector scaffold. The function accepts only an explicit
`ShellAccountingBrokerOrderPortMode`: disabled returns the disabled runtime mode
source, sandbox returns the TASK-168 sandbox source scaffold, and unsupported or
unknown values fail closed to disabled.

This is not runtime wiring. `defaultShellAccountingBrokerRuntimeModeSource()`
remains disabled-only, `DataServiceActions` is not modified, sandbox runtime
remains disabled, and the TASK-168 sandbox source scaffold remains direct-test
only. No payload/QML/config/env/command-line/file/database/secret-store mode
source, broker SDK, network, credentials, endpoint, real broker order id, order
placement, DB/audit/ledger write, reconciliation, cancellation, correction,
strategy execution, automatic trading, or schema change is added. See
`docs/138_shell_accounting_broker_sandbox_runtime_mode_source_selector_scaffold.md`
and
`docs/139_shell_accounting_broker_sandbox_runtime_mode_source_selector_scaffold_test_plan.md`.

## TASK-171 Broker Sandbox Runtime Mode Source Selector Wiring Authorization Gate

TASK-171 adds a gate-only boundary for a future task that may wire the TASK-170
selector scaffold into `DataServiceActions`. It does not implement runtime
wiring, does not modify `DataServiceActions`, does not modify runtime source
production code, and does not enable sandbox runtime.

The next implementation step remains separately authorized and may only start
with disabled-default selector wiring unless another TASK explicitly authorizes
sandbox runtime mode source enablement. Payload/QML/config/env/command-line/file
/database/secret-store mode sources remain unauthorized, and SDK, network,
credentials, endpoint, real broker order id, order placement, DB/audit/ledger
write, reconciliation, cancellation, correction, strategy execution, automatic
trading, and schema changes remain out of scope. See
`docs/140_shell_accounting_broker_sandbox_runtime_mode_source_selector_wiring_authorization_gate.md`
and
`docs/141_shell_accounting_broker_sandbox_runtime_mode_source_selector_wiring_authorization_test_plan.md`.

## TASK-172 Broker Sandbox Runtime Mode Source Selector Disabled Wiring

TASK-172 wires the TASK-170 selector scaffold into the broker dry-run path using
only the mode from `defaultShellAccountingBrokerRuntimeModeSource()`. Because
the default source remains disabled-only, runtime behavior remains equivalent to
the disabled/null broker provider: `brokerPortMode` is `disabled`,
`brokerPortDisabled` is true, and `brokerPortDryRunOnly` is true.

TASK-172 does not enable sandbox runtime and does not read broker mode from
payload, QML, config, environment, command line, files, database, or secret
stores. It adds no SDK, network, credentials, endpoint, real broker order id,
order placement, DB/audit/ledger write, reconciliation, cancellation,
correction, strategy execution, automatic trading, or schema change. See
`docs/142_shell_accounting_broker_sandbox_runtime_mode_source_selector_disabled_wiring.md`
and
`docs/143_shell_accounting_broker_sandbox_runtime_mode_source_selector_disabled_wiring_test_plan.md`.

## TASK-173 Broker Sandbox Runtime Mode Enablement Authorization Gate

TASK-173 adds a gate-only authorization boundary for any future task that may
enable sandbox runtime mode. It does not modify `DataServiceActions.cpp`, does
not modify `ShellAccountingBrokerRuntimeModeSource.h/.cpp`, and does not enable
sandbox runtime.

TASK-172 remains limited to disabled-default selector wiring. Future sandbox
runtime enablement, external mode sources, credentials providers, endpoint
providers, sandbox adapter implementation, real broker order id storage,
reconciliation, cancellation, correction, broker SDK, network, order placement,
strategy execution, and automatic trading still require separate TASK
authorization. CI continues to enforce no-network, no-credentials, and
no-real-order-placement policy. See
`docs/144_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_gate.md`
and
`docs/145_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_test_plan.md`.
