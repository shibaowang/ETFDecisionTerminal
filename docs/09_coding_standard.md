# 代码规范总纲

## 当前状态

v0.1 草案。

## 后续补充方向

补充 C++/Qt 编码约定、CMake 目标规则、日志规范和代码审查清单。

## 项目组织

Monorepo + CMake 多目标工程。

推荐目录：

```text
ETFDecisionTerminal/
├─ apps/
├─ libs/
├─ tests/
├─ migrations/
├─ tools/
└─ docs/
```

## 公共库

- CoreDomain
- Protocol
- DataAccess
- Transport
- StrategyEngine
- MarketEngine
- Quantization
- Security

## Qt 依赖边界

- Qt 依赖只允许进入 apps、Shell、Transport 等明确允许的模块。
- `libs/Transport` 可以使用 Qt Core 和 Qt Network。
- `libs/CoreDomain`、`libs/Protocol`、`libs/DataAccess` 不得引入 Qt include、Qt 类型或 Qt 链接依赖。
- Protocol 负责消息格式，Transport 负责本地 socket 传输；Transport 不得实现业务 action 分发。
- DataAccess 负责 SQLite 基础设施和 DataService 内部查询边界；Transport 不得依赖 DataAccess。

## 代码规则

- C++ 使用统一命名规范。
- QML 页面和组件统一目录规范。
- 所有公共枚举集中定义。
- 金额类型禁止裸 double。
- 核心库开启严格编译警告。
- 使用 clang-format。
- 使用 clang-tidy。
- 关键模块必须有单元测试。

## Git 规则

- main：稳定版本
- dev：集成开发版本
- feature/*：单功能开发
- release/*：发布候选
- hotfix/*：紧急修复
- tag：v0.1.0、v0.2.0、v1.0.0

## QML boundary

- QML files are presentation-only.
- QML must bind C++ `QObject` / `QAbstractListModel` surfaces instead of parsing complex JSON directly.
- QML must not access SQLite, connect sockets, start services, or implement strategy / accounting / TradeLog / TradeDraft logic.
- Filtering and sorting should call C++ adapter / presenter methods.
- Mock data must be labeled as mock / demo data and must not be presented as real service state.
- QML pages must clearly label mock / placeholder state when no real service is connected.
- Business logic must remain in C++ service / domain layers, not in QML.
## Shell Navigation Metadata Rules

- QML must not hardcode the full shell navigation list.
- Shell page metadata should come from ShellCore `ShellPageRegistry`.
- QML navigation should bind `ShellNavigationModel` / `ShellNavigationController`.

## Shell Status Binding Rules

- UI state should come from C++ ShellCore models.
- QML must not derive business status or service health by itself.
- Mock / placeholder status must be visibly labeled as mock data.

## Shell Metrics Binding Rules

- UI metrics should come from C++ ShellCore models.
- QML must not calculate business metrics, trading state, cash state, or strategy state.
- Mock metrics and mock action hints must be visibly labeled and must not be executable.
- Future real metrics require an explicit task that authorizes the data source and service boundary.

## Shell Read-Only Data Binding Rules

- QML must call only authorized C++ controllers for real read-only data.
- QML must not call `DataServiceClient` directly.
- QML must not call write actions, including `data.audit.append`.
- QML must not parse DataService protocol payloads directly; Controller / Model layers own parsing and error state.
- Read-only prototype pages must be visibly labeled and must not expose edit, trade, accounting, or strategy controls.

## Shell Read-Only State Flow Rules

- UI state derivation for read-only data must live in C++ Controller / ViewModel code.
- Duplicate refresh prevention, refresh throttling, and error-state construction must not be implemented in QML.
- QML must bind `canRefresh`, `isBusy`, `refreshState`, and error fields instead of inferring connection success.
- Failed refreshes must not clear previously loaded successful models unless a task explicitly requires and tests that behavior.
- QML must not fake successful real-data reads with mock data after a connection or refresh error.

## Shell Connection Preset Rules

- Connection presets are UI guidance, not service discovery.
- Command hints must be displayed as text only and must not be executed by QML or controllers.
- Runtime socket names must not be persisted unless a task explicitly authorizes configuration storage.
- Presets must not auto-start Watchdog, DataService, or any other service.
- Development presets that point at writable-capable services must not call write actions unless a later task explicitly authorizes them.

## Business Read-Only Page Rules

- Business page prototypes are read-only by default unless a task explicitly authorizes writes.
- QML business pages must bind authorized Controller / Model objects and must not call `DataServiceClient` directly.
- Read-only business pages must not include edit, delete, deposit, withdrawal, accounting, confirmation, or trade buttons.
- Any future write page must define a separate service boundary, tests, and authorization before adding UI controls.
