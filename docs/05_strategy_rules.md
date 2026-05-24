# 策略规则总纲

## 当前状态

v0.1 草案。

## 后续补充方向

补充策略参数来源、建议生成边界、资金池约束和禁止自动交易说明。

## 组合资金池

账户只有一套全局资金池，不是每个 ETF 各自一套资金池。

资金池分为：

- 战略底仓池
- 狙击资金池

## 标的权重

每个标的通过 portfolio_target 配置：

- target_weight
- max_weight
- priority

## 战略底仓

组合底仓比例来自：

- portfolio.base_position_ratio_ppm
- 默认 20%，即 200000 ppm

单标的底仓目标：

```text
principal_base
× portfolio.base_position_ratio
× 当前 ETF target_weight / 全部启用 ETF target_weight 合计
```

底仓保护按剩余持仓成本判断，不按实时市值判断。

## 狙击资金池

组合狙击资金池：

```text
principal_base - 全部底仓实际占用
```

单 ETF 狙击预算：

```text
组合狙击资金池
× 当前 ETF target_weight
/ 全部启用 ETF target_weight 合计
```

## 六档规则

六档规则放在 strategy_tier_rule 表。

默认：

- 一档 -5%，权重 1
- 二档 -10%，权重 2
- 三档 -15%，权重 4
- 四档 -20%，权重 8
- 五档 -25%，权重 16
- 六档 -30%，权重 32

支持每个策略独立配置。

## 跳档买入

指数直接跳到三档时，累计补买到当前触发档位：

```text
应买额度 =
一档预算 + 二档预算 + 三档预算
- 当前周期已执行狙击金额
```

成交金额档位归属通过 trade_tier_allocation 记录。

## 场内 / 场外替代

- ETF 溢价 > 场外替代阈值：走场外替代。
- ETF 溢价 <= 阈值：走场内直投。

场外买入走 otc_channel A/C 类拆单。
