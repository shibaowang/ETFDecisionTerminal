# EPIC-287 Global Chinese UI Readability Trial Feedback Fix

## Feedback

P1 local trial feedback: the Shell UI was functionally usable, but the visible
trial experience still exposed too much English operational copy, raw JSON
payload text, and mock/debug wording. Trial users should be able to understand
the local RC in Chinese without reading raw diagnostics.

EPIC-287 makes the visible ETFDecisionShell trial UI Chinese-first and hides raw
JSON / sample payload / mock diagnostic detail by default behind a developer
details affordance.

## Scope

Allowed scope:

- QML under `apps/ETFDecisionShell/qml/**/*.qml`
- `docs/400_global_chinese_ui_readability_trial_feedback_fix.md`
- README, docs index, and prompt-template index updates
- Dashboard / local-trial static acceptance tests
- `tests/GlobalChineseUiReadability/`
- necessary exact-path old gate repairs for EPIC-287 paths only

Out of scope:

- business behavior changes
- DataAccess write semantics
- DataServiceApi write semantics
- Excel/VBA import persistence semantics
- portfolio replay semantics
- market data provider or live-network semantics
- strategy recommendation semantics
- TradeDraft semantics
- OTCMap semantics
- migrations
- broker, credential, endpoint, real order placement, or automatic trading

## UI Glossary

- Page Info Mock: 页面信息（模拟）
- Metrics Mock: 指标概览（模拟）
- Action Hints Mock: 操作提示（模拟）
- Watchdog Manifest Report: 服务监控清单
- Read-only connection: 只读连接
- Load Healthy Mock: 加载健康样例
- Load Warning Mock: 加载警告样例
- Only Blocked: 仅显示阻塞项
- Only Issues: 仅显示问题项
- Mock only - not executable: 仅演示，不会执行真实操作
- Raw JSON / payload: 原始诊断数据 / 样例载荷
- Developer details: 开发者详情

Stable English identifiers may remain when they are technical contracts rather
than visible explanatory copy:

- QML `objectName`
- Presenter method names
- CTest names
- C++ class and method names
- JSON schema keys
- status enum values such as `ACCEPTED`, `REJECTED`, `PERSISTED`,
  `DUPLICATE`, and `CONFLICT`
- sample file names and IDs

## Raw JSON Folding

Raw JSON, sample payload text, replay payload text, market sample payload text,
strategy sample payload text, and OTCMap sample payload text must be hidden by
default. Each raw area must have a Chinese developer-detail control:

- `显示原始诊断数据`
- `隐藏原始诊断数据`
- `开发者详情：普通试用无需查看。`

The folded state must still preserve existing Presenter calls and sample data
contracts. The change is only a readability and trial UX correction.

## Required Chinese Safety Wording

The visible UI must continue to communicate:

- `这是内部草案，不是订单，不会提交券商。`
- `默认使用样例数据/禁用行情源，不会自动联网。`
- only explicit user confirmation can trigger persistence or draft creation
- local trial pages do not touch production databases
- broker, endpoint, credential, real order placement, and automatic trading are
  not part of this trial UI fix

## Acceptance

Static acceptance must prove:

- cross-page visible labels are Chinese-first
- the service-monitoring screen no longer exposes the old English watchdog
  wording
- raw JSON / sample payload text areas are hidden by default
- developer details use Chinese labels
- existing QML object names and Presenter wiring remain stable
- QML does not directly call DataServiceClient, SQLite, AccountingEngine,
  broker endpoints, order submission, or automatic trading code
- no production code, migrations, database semantics, broker/network behavior,
  or business logic changes are introduced

## Evidence JSON

Expected static evidence:

```json
{
  "task": "EPIC-287",
  "globalChineseUiReadabilityFixCreated": true,
  "visibleUiChineseFirst": true,
  "rawJsonHiddenByDefault": true,
  "developerDetailsGateCreated": true,
  "mockDebugInfoHiddenOrChinese": true,
  "businessLogicChanged": false,
  "dataAccessChanged": false,
  "migrationChanged": false,
  "productionDbTouched": false,
  "networkAccess": false,
  "brokerOrderSubmitted": false,
  "credentialAccess": false,
  "endpointAccess": false,
  "realOrderPlacement": false,
  "automaticTrading": false
}
```
