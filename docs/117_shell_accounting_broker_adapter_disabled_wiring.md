# ShellAccounting Broker Adapter Disabled Wiring (TASK-159)

## TASK-159 Purpose

TASK-159 wires the disabled/null broker adapter into the DataService internal broker order boundary. The goal is to make the default future adapter path explicit while preserving all current dry-run behavior and while keeping every real broker behavior disabled.

## TASK-158 Implementation Gate Complete

TASK-158 added the broker adapter implementation gate and test plan. TASK-159 builds on that gate by allowing only disabled/null broker adapter wiring. It does not implement sandbox, paper, or real broker adapters.

## Disabled/Null Adapter Wiring Only

The only production wiring introduced by this task is the default DataServiceApi provider for `DisabledShellAccountingBrokerOrderPort`. The provider returns a disabled/null port and the broker dry-run action records that the internal broker port remains disabled. No real adapter is introduced.

## Default Disabled Policy

The default broker adapter must be disabled. Missing mode, missing configuration, rollback mode, CI, and production startup all resolve to the disabled/null port unless a future task explicitly authorizes another mode.

## DataService-Only Wiring Policy

Disabled wiring is owned inside DataServiceApi. QML, Presenter, ShellServices, and ShellCore do not receive broker adapter objects and do not call broker adapter functions. The DataService boundary remains the only broker order entry point.

## No QML / Presenter / Shell Direct Adapter Policy

The adapter provider is not exposed to QML, Presenter, or Shell. Production UI can continue to call the existing DataService-backed TradeDraft and dry-run surfaces, but it cannot directly access adapter wiring or broker port objects.

## No Broker SDK Policy

TASK-159 does not import, link, initialize, or call any broker SDK. Future SDK work must be isolated behind a separately authorized adapter implementation task.

## No Network Call Policy

The disabled/null adapter does not open sockets, send HTTP requests, call sandbox/paper/real endpoints, or perform any broker I/O. CI must remain broker-network-free.

## No Credentials Policy

TASK-159 does not add broker API keys, passwords, tokens, secrets, private keys, credential files, or credential names to runtime behavior. Missing credentials must continue to fail closed in future tasks.

## No Real Order ID Policy

The disabled/null adapter does not generate, persist, or expose a real broker order id. Future broker order id storage, reconciliation, cancellation, and correction require separate tasks.

## No Real Order Placement Policy

The disabled/null adapter cannot place orders. The broker dry-run action remains dry-run-only and broker-disabled, with no sandbox, paper, or real order placement.

## Fake Adapter Test-Only Policy

The fake adapter remains test-only under `tests/ShellAccountingBrokerAdapterInterfaceScaffold`. It is not linked into production DataService runtime and is not used by QML, Presenter, ShellServices, or ShellCore.

## Dry-Run Compatibility Policy

`accounting.broker_order.dry_run` keeps its existing external semantics: it still reports dryRun=true, brokerDisabled=true, dryRunOnly=true, no broker SDK call, no real order placement, no strategy execution, no automatic trading, and audit-only write behavior. TASK-159 only adds internal disabled port state to the dry-run response.

## Error Mapping

Disabled wiring must map to explicit disabled status and error code:

- status: disabled
- errorCode: BROKER_ORDER_DISABLED
- brokerMode: disabled
- success: false for direct port submission
- no silent success
- no real order id
- no broker network call

Future sandbox / paper / real adapters must preserve explicit error mapping for mode mismatch, credentials missing, broker unavailable, broker rejected, timeout, duplicate order, unknown broker state, audit failure, and reconciliation required.

## Rollback Strategy

Rollback can disable broker adapter wiring by keeping the default provider bound to the disabled/null port. Disabling the adapter must not affect TASK-154 dry-run, TradeDraft create, TradeDraft confirm, audit, read-only accounting, or production trading UI review flows. Rollback must not create fake broker success and must not retry real broker placement.

## Future Scope Requires Separate Tasks

Sandbox adapter, paper adapter, real adapter, real order placement, broker order id storage, reconciliation, cancellation, correction, broker credentials, broker SDK integration, strategy execution, and automatic trading each require separate explicit authorization before implementation.
