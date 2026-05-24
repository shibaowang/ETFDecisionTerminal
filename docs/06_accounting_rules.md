# 账务规则总纲

## 当前状态

v0.1 草案。

## 后续补充方向

补充成交确认入账、纠错、审计、派生快照重建和定点整数规范。

## 唯一事实源

trade_log 是唯一事实账本。position_snapshot、cash_snapshot、portfolio_summary 都只是派生缓存。

## 账务维度

所有账务按 account_id + portfolio_id 联合重演。

## 金额精度规则

- 金额事实字段使用 MoneyCents，数据库单位为分。
- 价格使用 Price1e6，数据库单位为价格 × 1,000,000。
- 数量 / 份额使用 Quantity1e6，数据库单位为数量 × 1,000,000。
- 比例使用 RatioPpm，数据库单位为比例 × 1,000,000。
- 核心账务事实不得使用裸 double 表示。
- 字符串解析失败必须显式返回错误，不能静默写入 0。

## 本金基准

采用已结算权益基准法：

```text
principal_base =
入金净额
- 出金净额
- 买入手续费
+ 已实现卖出盈亏
+ 分红净额
```

未实现浮盈、未实现浮亏、盘中市值波动不改变本金基准。

## 成交入账

用户确认成交后，同一事务写入：

- trade_execution_group
- trade_log
- trade_tier_allocation，若是狙击买入
- base_position_allocation，若是底仓 / 修复
- audit_log
- TradeDraft 状态更新

失败则全部回滚。

## TradeLog 纠错

- 原始 trade_log 原则上不可改。
- 纠错通过 correction / reversal / voided 处理。
- 不物理删除 trade_log。
- 作废必须写 audit_log。

## 现金校准

现金余额不能直接改。必须通过入金、出金、现金校准事件重演。

## 安全模式

发现账务不一致时进入 SAFE_READONLY。

禁止：

- 确认成交
- 手工入账
- 生成新 TradeDraft

允许：

- 查看
- 导出
- 诊断
- 修复向导

## 数据库初始化边界

- TASK-004 的数据库初始化不等于账务初始化。
- 初始化数据库只负责连接、PRAGMA、WAL、migration 和健康检查。
- 本任务不实现账务重演，不写 trade_log，不写 position_snapshot / cash_snapshot / portfolio_summary。
- trade_log 仍然是唯一事实源，后续账务功能必须通过 ETFDataService 受控事务写入。

## 只读 Repository 边界

- TASK-005 的只读 Repository 不改变任何账务事实。
- 只读查询不能写 trade_log、trade_execution_group、position_snapshot、cash_snapshot、portfolio_summary 或 audit_log。
- 只读查询不能触发账务重演、修账、TradeDraft 状态流转或策略计算。
- trade_log 仍然是唯一事实源；Repository 返回的账户、组合、标的、策略和 OTC 通道记录只是当前数据库状态视图。

## 事务和审计边界

- TASK-010 不实现账务重演，不写 `trade_log`，不写成交组，不写 TradeDraft。
- `audit_log` 只记录审计事实，不改变现金、持仓、本金、成本或收益。
- `audit_log` 不是账务事实源，不能替代 `trade_log`。
- 任何未来账务写入必须走 ETFDataService 受控事务边界。
- 任何未来高风险写入必须在同一事务中写业务事实和 `audit_log`，失败时整体 rollback。
- 本任务只验证事务提交、回滚和审计记录基础，不实现成交确认、手工入账或策略计算。

## 审计写入 action 边界

- TASK-011 的 `data.audit.append` 不是账务事件，不改变现金、持仓、本金、成本或收益。
- `data.audit.append` 只写 `audit_log`，用于验证写入白名单、事务边界和协议错误响应。
- `data.audit.append` 不写 `trade_log`，不创建 `trade_execution_group`，不改变 TradeDraft 状态。
- `data.audit.append` 不写 `position_snapshot`、`cash_snapshot` 或 `portfolio_summary`。
- trade_log 仍是唯一事实账本；任何真实业务入账必须由后续专门任务授权并定义完整事务、审计和回滚规则。
