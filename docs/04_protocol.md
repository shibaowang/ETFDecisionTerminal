# 本地服务协议设计

## 当前状态

v0.1 草案。

## 后续补充方向

补充 Qt Local Socket 消息封装、请求响应约定、错误码和兼容性策略。

## 通信方式

Qt Local Socket + JSON。

TASK-003 定义纯 C++17 协议基础类型和 JSON 字符串生成。TASK-006 新增 Qt Local Socket 传输层骨架，但仍不实现业务 action 分发或数据库连接。

## Local Socket 协议帧

QLocalSocket 是流式传输，必须显式定义消息边界。当前 Transport 帧格式：

```text
uint32_be length
UTF-8 JSON payload
```

- `length` 为 4 字节无符号整数，大端序，只表示后续 JSON payload 字节数。
- 默认最大 payload 为 1 MB。
- payload 禁止为空。
- payload 当前必须看起来像 JSON object 或 array，即以 `{}` 或 `[]` 边界包裹。
- payload 内容通常是 `MessageEnvelope` 或 `ProtocolResponse` JSON。
- Transport 层只按字符串传输 payload，不解释 `action`，不做业务路由。
- 半包时等待更多数据；粘包时连续解析多个完整帧。
- 超过最大帧长度或明显非 JSON-looking payload 时返回传输错误。

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

MessageEnvelope 字段：

- protocolVersion：协议版本，当前为 `1.0`。
- msgId：消息 ID，不能为空。
- traceId：链路追踪 ID，不能为空。
- from：发送服务名称，取值为 `ETFDecisionShell`、`ETFWatchdog`、`ETFDataService`、`ETFMarketService`、`ETFStrategyService`、`ETFAlertService`。
- to：目标服务名称，取值同 from。
- action：动作名称，只做非空和基础格式校验，不登记业务 action。
- timestamp：UTC 时间字符串，对应 C++ 字段 `timestampUtc`。
- payload：原始 JSON 片段，对应 C++ 字段 `payloadJson`。

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

ProtocolResponse 字段：

- protocolVersion：协议版本，当前为 `1.0`。
- msgId：对应请求消息 ID，不能为空。
- traceId：对应链路追踪 ID，不能为空。
- success：请求是否成功。
- errorCode：失败时必须存在，成功时为空字符串。
- errorMessage：失败时必须存在，成功时为空字符串。
- payload：原始 JSON 片段，对应 C++ 字段 `payloadJson`。

响应校验规则：

- success = true 时，errorCode 和 errorMessage 必须为空。
- success = false 时，errorCode 必须是已定义错误码，errorMessage 不能为空。
- payloadJson 暂时作为原始 JSON 对象或数组片段嵌入，不做通用 JSON 解析。

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

当前已定义错误码：

协议类：

- E1000_UNKNOWN_PROTOCOL_ERROR
- E1001_INVALID_JSON
- E1002_MISSING_REQUIRED_FIELD
- E1003_UNSUPPORTED_PROTOCOL_VERSION
- E1004_INVALID_ACTION
- E1005_INVALID_SERVICE_NAME

数据库类：

- E2000_DATABASE_ERROR
- E2001_DATABASE_OPEN_FAILED
- E2002_MIGRATION_FAILED
- E2003_TRANSACTION_FAILED

账务类：

- E3000_ACCOUNTING_ERROR
- E3001_REPLAY_FAILED
- E3002_CASH_AUDIT_FAILED
- E3003_SAFE_READONLY_REQUIRED

策略类：

- E4000_STRATEGY_ERROR
- E4001_INVALID_STRATEGY_CONFIG

行情类：

- E5000_MARKET_ERROR
- E5001_MARKET_DATA_STALE
- E5002_MARKET_PROVIDER_FAILED

TradeDraft / 入账类：

- E6000_TRADE_DRAFT_ERROR
- E6001_DRAFT_EXPIRED
- E6002_DRAFT_SUPERSEDED
- E6003_EXECUTION_CONFIRM_FAILED

导入类：

- E7000_IMPORT_ERROR
- E7001_EXCEL_SCHEMA_INVALID
- E7002_IMPORT_VALIDATION_FAILED

安全类：

- E8000_SECURITY_ERROR
- E8001_AUTH_REQUIRED
- E8002_PERMISSION_DENIED

系统服务类：

- E9000_SERVICE_ERROR
- E9001_SERVICE_UNAVAILABLE
- E9002_HEARTBEAT_TIMEOUT
