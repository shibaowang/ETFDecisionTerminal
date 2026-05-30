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

## Action 分发骨架

TASK-007 新增 ServiceRuntime action 分发骨架。当前 dispatcher 以结构化 `MessageEnvelope` 对象为输入；Protocol 尚未实现完整 `fromJson(MessageEnvelope)`，因此本任务不新增大型 JSON 解析器。

分发规则：

- `ActionDispatcher` 先调用 `validateMessageEnvelope`。
- envelope 校验失败时返回 `ProtocolResponse{success=false}`，错误码沿用协议校验结果。
- 未注册 action 返回 `E1004_INVALID_ACTION`。
- handler 抛异常返回 `E9000_SERVICE_ERROR`。
- response 保持 request 的 `msgId` 和 `traceId`。
- handler 返回的 success response 必须携带 JSON object 或 array payload。

当前内置 action：

- `system.ping`：返回 `{ "pong": true, "service": "<ServiceName>" }`。
- `system.health`：返回 `{ "healthy": true, "service": "<ServiceName>", "status": "ok", "receivedAtUtc": "<UTC>" }`。

这些 action 只用于服务连通性和运行时骨架测试，不访问数据库，不代表 DataService 业务 API。

## ServiceHost MessageEnvelope JSON 解析规则

TASK-008 的 DemoServiceHost 实现最小受控 MessageEnvelope JSON 解析，只解析本协议固定字段，不提供通用 JSON 解析器。

必填字段：

- `protocolVersion`
- `msgId`
- `traceId`
- `from`
- `to`
- `action`
- `timestamp`
- `payload`

解析规则：

- 根节点必须是 JSON object。
- `from` 和 `to` 必须能转换为已定义的 ServiceName。
- `payload` 必须是 JSON object 或 JSON array，并作为原始 JSON 片段传递给 ServiceRuntime。
- 缺少必填字段返回 `E1002_MISSING_REQUIRED_FIELD`。
- JSON 语法错误、根节点类型错误或 payload 类型错误返回 `E1001_INVALID_JSON`。
- 服务名无效返回 `E1005_INVALID_SERVICE_NAME`。
- action 为空、格式无效或未注册时返回协议错误；未注册 action 必须返回 `E1004_INVALID_ACTION`。

端到端示例：

```json
{
  "protocolVersion": "1.0",
  "msgId": "msg-1",
  "traceId": "trace-1",
  "from": "ETFDecisionShell",
  "to": "ETFDataService",
  "action": "system.ping",
  "timestamp": "2026-05-24T10:30:00Z",
  "payload": {}
}
```

成功响应示例：

```json
{
  "protocolVersion": "1.0",
  "msgId": "msg-1",
  "traceId": "trace-1",
  "success": true,
  "errorCode": "",
  "errorMessage": "",
  "payload": {
    "pong": true,
    "service": "ETFDataService"
  }
}
```

## DataService 只读 action

TASK-009 定义 `data.*` 只读 action 命名规则：`data.<resource>.<operation>`。当前所有 `data.*` action 只能查询数据库，不能写业务表，不能创建 TradeLog、TradeDraft、成交组或派生快照。

### data.health

请求 payload：

```json
{}
```

响应 payload：

```json
{
  "healthy": true,
  "schemaVersion": "001_initial_schema",
  "walEnabled": true,
  "foreignKeysEnabled": true
}
```

### data.summary

请求 payload：

```json
{}
```

响应 payload：

```json
{
  "activeAccount": {},
  "activePortfolio": {},
  "enabledInstrumentCount": 1,
  "enabledStrategyCount": 0,
  "otcChannelCount": 0,
  "schemaVersion": "001_initial_schema"
}
```

### data.accounts.list

响应 payload：

```json
{
  "accounts": []
}
```

### data.portfolios.list

响应 payload：

```json
{
  "portfolios": []
}
```

### data.instruments.list

响应 payload：

```json
{
  "instruments": []
}
```

### data.strategies.list

响应 payload：

```json
{
  "strategies": []
}
```

### data.otc.list

请求 payload：

```json
{
  "strategyCode": "159509"
}
```

响应 payload：

```json
{
  "strategyCode": "159509",
  "channels": []
}
```

`strategyCode` 缺失或 trim 后为空时返回 `success=false`，`errorCode=E1002_MISSING_REQUIRED_FIELD`。`strategyCode` 不存在或没有 OTC 通道时返回 `success=true` 且 `channels=[]`。

## DataService 审计写入 action

TASK-011 定义第一个受控写入 action：`data.audit.append`。TASK-144
新增受控 snapshot write action：`accounting.snapshot.write`。默认所有其他
`data.*` 或 accounting 写入 action 仍然禁止写入，未注册或未授权写入 action
必须返回 `E1004_INVALID_ACTION`。

### data.audit.append

请求 payload：

```json
{
  "entityType": "SYSTEM",
  "entityId": "1",
  "action": "DEMO_AUDIT",
  "reason": "manual audit test",
  "operatorName": "local-user",
  "oldValue": {},
  "newValue": {}
}
```

字段规则：

- `entityType` 必填，trim 后不能为空。
- `action` 必填，trim 后不能为空。
- `reason` 必填，trim 后不能为空。
- `entityId` 可选；当前审计表字段为整数，非整数值不会作为 `audit_log.entity_id` 写入。
- `operatorName` 可选，缺省为 `local-user`。
- `oldValue` 和 `newValue` 可选，缺省为 `{}`，提供时必须是 JSON object 或 array。

成功响应 payload：

```json
{
  "inserted": true,
  "auditLogId": 123,
  "entityType": "SYSTEM",
  "action": "DEMO_AUDIT"
}
```

错误规则：

- payload 不是 JSON object 或 `oldValue` / `newValue` 不是 JSON object / array 时返回 `E1001_INVALID_JSON`。
- 缺少 `entityType`、`action` 或 `reason` 时返回 `E1002_MISSING_REQUIRED_FIELD`。
- 数据库事务或写入失败时返回数据库类错误码。
- `data.audit.append` 只在开发期审计写入服务中注册；只读服务必须返回 `E1004_INVALID_ACTION`。
- 不允许任何通过 payload 传入 SQL 的 action。

### accounting.snapshot.write

TASK-144 defines `accounting.snapshot.write` as a DataService-internal snapshot
write action. The request must be authorized with `TASK-144_SNAPSHOT_WRITE`,
must declare `source=snapshotRebuildPreview`, and must use the TASK-142 preview
as input. The action may write only `cash_snapshot`, `position_snapshot`, and
`portfolio_summary`; it must not write `trade_log`, `trade_execution_group`,
`trade_draft`, or `audit_log`.

## DataServiceClient 请求和响应规则

TASK-012 的 DataServiceClient 只负责客户端侧协议封装，不新增服务端 action。

请求构造规则：

- `protocolVersion` 固定为 `1.0`。
- `msgId` 和 `traceId` 由客户端生成简单唯一字符串。
- `from` 默认 `ETFDecisionShell`，`to` 默认 `ETFDataService`。
- `action` 必须由调用方法明确指定，客户端不根据未知 action 动态生成业务语义。
- `payload` 必须是 JSON object 或 array；客户端不允许传入 SQL payload。
- `traceId` 用于请求追踪和响应匹配。

响应解析规则：

- 客户端只解析受控 `ProtocolResponse` 字段：`protocolVersion`、`msgId`、`traceId`、`success`、`errorCode`、`errorMessage`、`payload`。
- `success=true` 时，`payload` 作为原始 JSON object / array 字符串返回给调用方。
- `success=false` 时，客户端保留服务端返回的 `errorCode` 和 `errorMessage`。
- 响应 JSON 无效、缺字段、payload 类型错误或 `traceId` 不匹配时，客户端返回明确协议错误或服务错误。
- 客户端不实现大型通用 JSON 解析器，也不解释业务账务规则。

## Watchdog 健康检查协议

TASK-013 的 Watchdog 健康检查只调用既有协议 action，不新增服务端 action。

- 进程启动由 Watchdog 使用 `QProcess` 完成。
- 服务连通性检查使用 `system.ping`。
- DataService 数据库状态检查使用 `data.health`。
- 两个请求都成功时，Watchdog 将服务状态标记为 `HEALTHY`。
- 连接失败、超时、响应失败或 socket 名错误时，Watchdog 将服务状态标记为 `UNHEALTHY` 或 `STOPPED`，并返回诊断消息。
- Watchdog 不通过协议执行任何写入 action，不传递 SQL payload。

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

## TASK-048 accounting.health read-only action

`accounting.health` is a DataService read-only action. It is a capability and
boundary health check only. It does not run accounting replay, does not inspect
`trade_log` for calculation, and does not write any database table.

Response payload:

```json
{
  "module": "accounting",
  "healthy": true,
  "readOnly": true,
  "contractVersion": "0.3-draft",
  "calculationVersion": "not-implemented",
  "replayImplemented": false,
  "snapshotImplemented": false,
  "writeEnabled": false,
  "implementedActions": ["accounting.health"],
  "futureActions": [
    "accounting.replay.preview",
    "position.list",
    "cash.summary",
    "portfolio.pnl.summary",
    "base_position.summary",
    "sniper_pool.summary"
  ],
  "sourceOfTruth": "trade_log",
  "derivedTables": [
    "cash_snapshot",
    "position_snapshot",
    "portfolio_summary"
  ],
  "boundaries": [
    "trade_log is the factual ledger",
    "snapshots are derived data",
    "this action does not replay accounting",
    "this action does not write database tables",
    "QML must not calculate accounting fields"
  ],
  "warnings": [
    {
      "code": "REPLAY_NOT_IMPLEMENTED",
      "message": "Accounting replay is not implemented yet.",
      "blocking": false
    }
  ],
  "errors": []
}
```

Boundary rules:

- `healthy=true` means the health action itself is callable; it does not mean
  replay, positions, cash, or PnL are implemented.
- `replayImplemented=false`, `snapshotImplemented=false`, and
  `writeEnabled=false` are required for the current implementation.
- `implementedActions` currently contains only `accounting.health`.
- `futureActions` are listed for contract planning only and are not registered
  by this task.
- The action must not call `data.audit.append` and must not write `audit_log`,
  `trade_log`, `trade_execution_group`, `trade_draft`, `cash_snapshot`,
  `position_snapshot`, or `portfolio_summary`.

## TASK-049 accounting.replay.preview read-only guard

`accounting.replay.preview` is a read-only action, but it is not an accounting
replay implementation. It exists so clients can discover the future replay
preview contract and receive an explicit non-implementation response.

Success semantics: this action uses `ProtocolResponse.success=true` when the
guard endpoint is called with a JSON object payload. The payload then carries
the domain status:

- `implemented=false`
- `replayExecuted=false`
- `writeEnabled=false`
- `status=REPLAY_NOT_AVAILABLE`

This choice means the protocol call succeeded and the action returned the
expected guard. Callers must not treat it as a successful replay result.

Request payload:

```json
{
  "accountId": "optional",
  "portfolioId": "optional",
  "sourceFromTime": "optional ISO-8601",
  "sourceToTime": "optional ISO-8601",
  "fixtureMode": false,
  "requestedOutputs": [
    "positions",
    "cash",
    "pnl",
    "basePosition",
    "sniperPool"
  ]
}
```

Current request handling:

- Missing payload is treated as `{}` and returns the guard.
- Payload must be a JSON object; array or malformed payloads return a protocol
  error.
- `accountId`, `portfolioId`, time range, `fixtureMode`, and
  `requestedOutputs` do not trigger database calculation.
- `requestedOutputs` is echoed only as request scope metadata.

Response payload:

```json
{
  "module": "accounting",
  "action": "accounting.replay.preview",
  "implemented": false,
  "readOnly": true,
  "replayExecuted": false,
  "writeEnabled": false,
  "contractVersion": "0.3-draft",
  "calculationVersion": "not-implemented",
  "status": "REPLAY_NOT_AVAILABLE",
  "message": "Accounting replay preview is not implemented yet.",
  "requestedScope": {
    "accountId": "",
    "portfolioId": "",
    "sourceFromTime": "",
    "sourceToTime": "",
    "requestedOutputs": []
  },
  "requiredFixtures": [
    "FX001_EMPTY_LEDGER",
    "FX002_SINGLE_BUY",
    "FX003_BUY_SELL_PARTIAL",
    "FX004_SELL_EXCEEDS_POSITION",
    "FX005_MISSING_FEE",
    "FX006_NEGATIVE_CASH",
    "FX007_MULTI_INSTRUMENT",
    "FX008_MULTI_ACCOUNT",
    "FX009_BASE_POSITION_LOCKED",
    "FX010_SNIPER_TIER_COMPLETED",
    "FX011_STALE_SNAPSHOT",
    "FX012_MISSING_MARKET_PRICE",
    "FX013_MULTI_CURRENCY_UNSUPPORTED"
  ],
  "futureOutputs": [
    "PositionListResponse",
    "CashSummaryDto",
    "PortfolioPnlDto",
    "BasePositionDto",
    "SniperPoolDto",
    "AccountingIssueDto"
  ],
  "forbiddenWrites": [
    "trade_log",
    "trade_execution_group",
    "trade_draft",
    "cash_snapshot",
    "position_snapshot",
    "portfolio_summary",
    "audit_log"
  ],
  "nextRequiredTask": "implement fixture-backed accounting replay before returning preview results",
  "warnings": [
    {
      "code": "REPLAY_NOT_IMPLEMENTED",
      "message": "Fixture-backed accounting replay must be implemented in a future task.",
      "blocking": false
    }
  ],
  "errors": [
    {
      "code": "REPLAY_NOT_AVAILABLE",
      "message": "Accounting replay preview is not available yet.",
      "blocking": true
    }
  ]
}
```

Boundary rules:

- The action must not execute replay.
- The action must not read `trade_log` for calculation.
- The action must not return fake `PositionSummaryDto`, `CashSummaryDto`, or
  `PortfolioPnlDto` values.
- The action must not call `data.audit.append`.
- The action must not write `audit_log`, `trade_log`,
  `trade_execution_group`, `trade_draft`, `cash_snapshot`,
  `position_snapshot`, or `portfolio_summary`.
