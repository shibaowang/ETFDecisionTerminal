# EPIC-285 Dashboard Chinese Localization Trial Feedback Fix

## User Feedback

P1 feedback from the local trial: the ShellAccounting Dashboard MVP was
functionally usable, but too much visible UI text was English. Trial users could
not confidently distinguish preview, accepted state, persistence, internal
drafts, provider-disabled state, and diagnostics.

EPIC-285 fixes that feedback by localizing user-visible Dashboard text to
Chinese while preserving all technical contracts.

## Scope

Allowed scope:

- Chinese user-facing QML text in
  `apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml`
- README and docs index updates
- local trial runbook / manual checklist / known limits / evidence wording
- Dashboard and local trial static acceptance tests

Out of scope:

- business behavior changes
- DataAccess write semantics
- DataServiceApi write semantics
- Excel/VBA import persistence semantics
- portfolio replay semantics
- market provider semantics
- strategy, TradeDraft, or OTCMap semantics
- migrations
- broker, credential, endpoint, real order placement, or automatic trading

## Stable English Identifiers

The UI may still show English technical identifiers where those values are
stable contracts rather than explanatory copy:

- QML `objectName`
- CTest names
- C++ class and method names
- DataService action names
- JSON schema keys
- status enum values such as `ACCEPTED`, `REJECTED`, `DUPLICATE`, and
  `CONFLICT`
- file names and sample IDs

This keeps tests, automation, diagnostics, and downstream scripts stable while
making the visible workflow understandable in Chinese.

## Terminology

- Dashboard MVP: 本地试用看板
- Local trial RC: 本地试用 RC
- Mock: 模拟数据 / 演示数据
- Placeholder: 占位状态
- Provider disabled: 行情源已禁用
- Fixture: 样例数据
- Issue codes: 诊断代码
- Reset dashboard: 重置看板
- Read-only: 只读
- Accepted: 已通过
- Rejected: 已拒绝
- Partial: 部分可用
- Stale: 数据可能过期
- Duplicate: 重复提交
- Idempotency conflict: 幂等冲突
- Excel/VBA Import: Excel/VBA 导入
- Portfolio Replay: 持仓/现金重算
- Market Data: 行情数据
- Historical High: 历史高点
- Strategy Recommendation: 策略建议
- TradeDraft: 交易草案
- OTCMap: 场外基金映射
- OTCMap A/C Multi-channel: 场外 A/C 多通道
- Multi-leg draft: 多腿交易草案
- Load Sample: 加载样例
- Preview: 预览
- Persist: 确认写入
- Replay: 重算
- Refresh: 刷新
- Recommend: 生成建议
- Create Draft: 创建草案
- Create OTCMap Draft: 创建场外多通道草案
- Reset: 重置
- Copy diagnostics: 复制诊断信息

## Required Safety Text

The Dashboard must communicate these safety boundaries in Chinese:

- `这是内部草案，不是订单，不会提交券商。`
- `默认使用样例数据/禁用行情源，不会自动联网。`
- 需要手工确认。
- 不触碰生产数据库。
- 未启用自动交易。

## Acceptance

The Dashboard acceptance tests must prove:

- Chinese page title and module titles are present.
- Chinese action labels are present.
- Chinese safety warnings are present.
- Existing QML `objectName` values are preserved.
- QML still does not reference DataServiceClient, SQLite, AccountingEngine,
  StrategyEngine, MarketEngine, broker, credentials, endpoints, order
  placement, or automatic trading tokens.
- No business logic, migrations, database writes, broker/network capability, or
  default networking are introduced.

## Evidence JSON

Expected static evidence:

```json
{
  "task": "EPIC-285",
  "dashboardChineseLocalizationCompleted": true,
  "userFeedbackAddressed": "UI English text unreadable",
  "qmlUserVisibleChinese": true,
  "moduleTitlesChinese": true,
  "actionButtonsChinese": true,
  "safetyWarningsChinese": true,
  "draftNotOrderChineseWarning": true,
  "providerDisabledChineseWarning": true,
  "objectNamesPreserved": true,
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
