# 数据库结构设计

## 当前状态

v0.1 草案。初始迁移脚本见 `migrations/001_initial_schema.sql`。

## 后续补充方向

补充表关系说明、迁移规则、派生缓存重建策略和审计要求。

## 数据库原则

- SQLite + WAL。
- 仅 ETFDataService.exe 可写数据库。
- trade_log 是唯一事实账本。
- 持仓、现金、组合汇总全部由 trade_log 派生。
- 金额用“分”为单位的整数。
- 价格、份额、比例使用定点整数。
- 内部 ID 使用自增整数，对外业务 ID 使用 UUID。
- 所有核心表带 account_id + portfolio_id。
- 所有结构变更走 migration。

## 定点字段约定

- amount_cents：金额，单位为分。
- fee_cents：手续费，单位为分。
- price_1e6：价格 × 1,000,000。
- quantity_1e6：数量 / 份额 × 1,000,000。
- ratio_ppm：比例 × 1,000,000。

核心账务金额禁止用裸 double / REAL 作为事实字段。

## 核心表分类

### 系统与配置

- schema_version
- app_setting
- secure_setting

### 账户与组合

- account
- portfolio
- portfolio_target
- portfolio_target_version

### 标的、策略与通道

- instrument
- strategy
- strategy_version
- strategy_tier_rule
- otc_channel
- market_calendar

### 行情

- market_quote_current
- market_quote_history

### TradeDraft

- trade_draft
- trade_draft_leg
- trade_draft_context

### 成交与账务

- trade_execution_group
- trade_log
- trade_tier_allocation
- base_position_allocation
- cash_adjustment

### 周期与预算

- grid_cycle
- budget_recalc_suggestion
- cash_allocation_plan_current
- cash_allocation_plan_history

### 派生快照

- position_snapshot
- cash_snapshot
- portfolio_summary

### 预警与日志

- alert_event
- audit_log
- system_log
- error_log

### Excel V8.2 导入

- import_job
- staging_trade_log
- staging_strategy
- staging_otc_channel
- import_issue

## 数据库写入硬规则

1. 所有写入只能通过 ETFDataService。
2. trade_log 是唯一事实账本。
3. position_snapshot、cash_snapshot、portfolio_summary 只能由 trade_log 派生。
4. TradeDraft 不是成交，不能改变现金和持仓。
5. 用户确认成交时才创建 trade_execution_group。
6. trade_execution_group、trade_log、trade_tier_allocation、base_position_allocation 必须同事务写入。
7. 手工入账必须 manual_entry = true，并写 audit_log。
8. 底仓保护按剩余持仓成本，不按实时市值。
9. 底仓修复不占用狙击资金池。
10. grid_cycle 必须结构化管理，不靠备注文本。
11. 不允许物理删除 trade_log。
12. 纠错必须通过 correction / reversal / voided。
13. 账务异常进入 SAFE_READONLY。
