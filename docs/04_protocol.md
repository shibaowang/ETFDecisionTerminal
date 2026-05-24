# 本地服务协议设计

## 当前状态

v0.1 草案。

## 后续补充方向

补充 Qt Local Socket 消息封装、请求响应约定、错误码和兼容性策略。

## 通信方式

Qt Local Socket + JSON。

## 标准请求消息

```json
{
  "protocolVersion": "1.0",
  "msgId": "uuid",
  "traceId": "uuid",
  "from": "ETFDecisionShell",
  "to": "ETFStrategyService",
  "action": "BuildTradeDraft",
  "timestamp": "2026-05-24T10:30:00+08:00",
  "payload": {}
}
```

## 标准响应消息

```json
{
  "protocolVersion": "1.0",
  "msgId": "uuid",
  "traceId": "uuid",
  "success": true,
  "errorCode": "",
  "errorMessage": "",
  "payload": {}
}
```

## 通信模式

请求响应：

- 查询账户
- 查询组合
- 查询 TradeLog
- 查询持仓
- 确认成交
- 手工入账
- Excel 导入

事件推送：

- 行情更新
- 策略信号变化
- TradeDraft 变化
- 资金分配计划变化
- 服务状态变化
- 预警消息

## 错误码范围

- E1000-E1999：协议错误
- E2000-E2999：数据库错误
- E3000-E3999：账务重演错误
- E4000-E4999：策略计算错误
- E5000-E5999：行情错误
- E6000-E6999：TradeDraft / 成交入账错误
- E7000-E7999：导入迁移错误
- E8000-E8999：授权 / 安全错误
- E9000-E9999：系统服务错误
