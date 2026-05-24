# Excel V8.2 迁移总纲

## 当前状态

v0.1 草案。

## 后续补充方向

补充 Excel V8.2 字段映射、校验规则、导入 staging 和人工确认流程。

## Excel V8.2 的角色

Excel V8.2 是：

- 业务规则母版
- 历史数据来源
- 测试样本来源

不是：

- 逐行翻译对象
- 最终软件架构

## 迁移对象

- TradeLog
- Strategy
- Config
- OTCMap
- DailySnapshot，可选
- ErrorLog，可选

## 导入流程

1. 读取 Excel。
2. 写入 staging_trade_log / staging_strategy / staging_otc_channel。
3. 执行表头校验。
4. 执行现金重演校验。
5. 执行 A/C 类通道校验。
6. 生成迁移报告。
7. 用户确认。
8. 开启事务。
9. 写入正式表。
10. 提交或回滚。

## 重复数据处理

用户可选择：

- 跳过重复行
- 作为新账户导入
- 作为新组合导入
- 覆盖 staging，不直接覆盖正式表
