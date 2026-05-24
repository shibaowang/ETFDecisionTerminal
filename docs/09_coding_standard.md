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
- StrategyEngine
- MarketEngine
- Quantization
- Security

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
