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
