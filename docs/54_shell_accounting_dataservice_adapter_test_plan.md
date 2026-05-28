# Shell Accounting DataService Adapter Test Plan

## Document Purpose

This document defines the tests required before a future
`ShellAccountingDataServiceAdapter` implementation can be accepted.

This document does not implement the adapter.

## Test Matrix

### A. Construction

- adapter can be constructed with a DataServiceClient dependency
- null dependency is rejected or maps to unavailable, depending on the future
  design
- no QML dependency

### B. Method Mapping

- `fetchPositionList` calls the `positionList` wrapper
- `fetchCashSummary` calls the `cashSummary` wrapper
- `fetchPortfolioPnlSummary` calls the `portfolioPnlSummary` wrapper
- `fetchBasePositionSummary` calls the `basePositionSummary` wrapper
- `fetchSniperPoolSummary` calls the `sniperPoolSummary` wrapper

### C. Request Mapping

- `accountId`
- `portfolioId`
- `sourceFromTime`
- `sourceToTime`
- `includeIssues`
- `requestedOutputs`
- `timeoutMs`

### D. Response Mapping

- `protocolSuccess`
- `implemented`
- `readOnly`
- `writeEnabled`
- `payloadStatus`
- `issues`
- `warnings`
- `errors`
- `rawPayload`

### E. Error Mapping

- protocol failure
- malformed response
- timeout
- transport error
- server unavailable
- domain unavailable

### F. No-write / No Trade

- no write action invoked
- no TradeDraft
- no trade suggestion
- no strategy
- no broker order

### G. Controller Integration

- controller refresh with real adapter using guard payload
- `Loading -> Unavailable`
- issue visible
- privacy preserved
- no QML

## Required Fakes / Spies

Future tests need:

- spy DataServiceClient wrapper
- fake ProtocolResponse
- fake timeout
- fake transport error
- fake malformed payload
- no-write action-name allowlist

## Read-only Action Allowlist

The future adapter may only call:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`
- `base_position.summary`
- `sniper_pool.summary`

It must not call:

- `data.audit.append`
- `trade.*` write actions
- draft actions
- snapshot write actions
- strategy execution actions
- broker order actions

## Go / No-Go Checklist

- [ ] Adapter boundary docs have been merged.
- [ ] Adapter test plan has been merged.
- [ ] DataServiceClient wrappers exist.
- [ ] No-write guard tests exist.
- [ ] Controller fake adapter tests exist.
- [ ] QML not wired has been confirmed.
- [ ] No TradeDraft has been confirmed.
- [ ] No trade suggestion has been confirmed.
- [ ] User explicitly authorized real adapter implementation.

## TASK-107 Spy / Wrapper Scaffolding Status

TASK-107 adds spy wrapper and mapping scaffolding tests for the future real
adapter.

Added scaffolding:

- spy DataServiceClient wrapper that is not the real DataServiceClient
- read-only accounting action allowlist
- forbidden action denylist
- future method mapping descriptors
- request mapping descriptor / payload builder
- fake ProtocolResponse -> ShellAccountingServiceResult mapper
- protocol / timeout / transport / domain issue mapping checks
- no-write / no-trade / no-TradeDraft checks
- no-real-dependency checks

The real `ShellAccountingDataServiceAdapter` implementation must pass these
tests or their real-adapter equivalents before it can merge.

## TASK-108 Skeleton Tests

TASK-108 adds production skeleton tests for the not-connected adapter boundary:

- `shell_accounting_dataservice_adapter_skeleton_interface`
- `shell_accounting_dataservice_adapter_skeleton_not_connected`
- `shell_accounting_dataservice_adapter_skeleton_no_live_calls`
- `shell_accounting_dataservice_adapter_skeleton_no_write_no_trade`
- `shell_accounting_dataservice_adapter_skeleton_controller_integration`

These tests verify construction through `ShellAccountingServiceAdapter`,
`hasLiveClient=false`, `readOnly=true`, `writeEnabled=false`, controlled
not-connected / unavailable fetch results, visible blocking issue propagation,
no DataServiceClient / DataServiceApi / DataAccess / AccountingEngine / SQLite /
QtQuick / QML include or call path, no TradeDraft, no trade suggestion, no
strategy, no broker order, and controller refresh mapping to `Unavailable`.

Future real adapter work must replace the not-connected behavior with
DataServiceClient wrapper calls only after explicit authorization, while still
passing the TASK-107 spy/wrapper checks or their real equivalents.

## TASK-109 Live-call Gate Tests

TASK-109 adds test-only gate tests for future live-call work:

- `shell_accounting_dataservice_adapter_live_call_allowlist_gate`
- `shell_accounting_dataservice_adapter_live_call_mapping_gate`
- `shell_accounting_dataservice_adapter_live_call_error_gate`
- `shell_accounting_dataservice_adapter_live_call_no_write_gate`
- `shell_accounting_dataservice_adapter_live_call_readiness_gate`

Future live implementation must pass these gates or their real-adapter
equivalents. TASK-109 does not implement live calls and does not change the
current skeleton not-connected behavior.

## TASK-110 Live-call Skeleton Tests

TASK-110 adds live-call skeleton tests against an abstract test port:

- `shell_accounting_dataservice_adapter_live_call_skeleton_default_not_connected`
- `shell_accounting_dataservice_adapter_live_call_skeleton_method_mapping`
- `shell_accounting_dataservice_adapter_live_call_skeleton_request_mapping`
- `shell_accounting_dataservice_adapter_live_call_skeleton_response_mapping`
- `shell_accounting_dataservice_adapter_live_call_skeleton_error_mapping`
- `shell_accounting_dataservice_adapter_live_call_skeleton_no_write_no_trade`
- `shell_accounting_dataservice_adapter_live_call_skeleton_no_real_dependency`
- `shell_accounting_controller_with_dataservice_adapter_spy_port`

These tests prove the default not-connected fallback, action-specific method
mapping through the abstract port, request field preservation, response field
mapping, protocol / timeout / transport / domain issue mapping, no-write /
no-trade flags, no real dependency, and controller refresh through the adapter
and spy port. Future real adapter work must replace only the concrete port
implementation and keep these contracts passing.

## TASK-111 Concrete Port Tests

TASK-111 adds concrete port tests for
`ShellAccountingDataServiceClientPortAdapter`:

- `shell_accounting_dataservice_client_port_adapter_construction`
- `shell_accounting_dataservice_client_port_adapter_method_mapping`
- `shell_accounting_dataservice_client_port_adapter_request_mapping`
- `shell_accounting_dataservice_client_port_adapter_response_mapping`
- `shell_accounting_dataservice_client_port_adapter_error_mapping`
- `shell_accounting_dataservice_client_port_adapter_no_write_no_trade`
- `shell_accounting_dataservice_client_port_adapter_no_forbidden_dependency`
- `shell_accounting_dataservice_adapter_with_client_port_adapter_guard_payload`

These tests cover null-client fallback, one-to-one wrapper method mapping,
payload and timeout forwarding, guard payload response mapping, protocol
failure, malformed response, timeout, transport error, domain issues,
no-write / no-trade flags, and forbidden dependency boundaries.

The tests use a test-only local fake wrapper server so the concrete port can
exercise real `DataServiceClient` wrapper methods without changing DataService
guard behavior. QML, SQLite, AccountingEngine, DataAccess, write actions,
TradeDraft generation, and trade suggestions remain outside this test scope.

## TASK-112 Controller Concrete Port Integration Tests

TASK-112 adds controller-level concrete port integration tests:

- `shell_accounting_controller_concrete_port_position_list_guard`
- `shell_accounting_controller_concrete_port_cash_summary_guard`
- `shell_accounting_controller_concrete_port_portfolio_pnl_summary_guard`
- `shell_accounting_controller_concrete_port_base_position_summary_guard`
- `shell_accounting_controller_concrete_port_sniper_pool_summary_guard`
- `shell_accounting_controller_concrete_port_no_write_no_trade`
- `shell_accounting_controller_concrete_port_no_qml_or_forbidden_dependency`

These tests verify `Unavailable` state mapping for all guard wrappers, visible
issues, read-only flags, no-write / no-trade behavior, no direct controller
DataServiceClient dependency, no direct adapter DataServiceClient dependency,
and no QML accounting action wiring.

## TASK-114 Presenter / QML Binding Plan

Adapter tests do not authorize QML integration. Future presenter and QML tasks
must reference [docs/57_shell_accounting_presenter_contract.md](57_shell_accounting_presenter_contract.md)
and [docs/58_shell_accounting_qml_binding_readiness_plan.md](58_shell_accounting_qml_binding_readiness_plan.md).

The adapter remains below ShellServices presenter / ViewModel boundaries.

## TASK-116 Presenter Concrete Port Integration Tests

TASK-116 adds presenter-level concrete port integration tests. They verify that
the presenter can consume `position.list` through controller -> adapter ->
concrete port -> DataServiceClient guard wrapper, and that the result remains
Unavailable with visible guard issues and no write / no trade behavior.

Future presenter action expansion for cash, portfolio PnL, base position, and
sniper pool requires separate task authorization.
