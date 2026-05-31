# ShellAccounting Broker Sandbox Mode Selector (TASK-162)

## Document Purpose

TASK-162 adds a disabled-default selector / factory boundary for the ShellAccounting broker order port. The selector is not runtime broker enablement and does not make sandbox trading usable. It exists as a stable DataServiceApi-internal future wiring boundary and as a testable object for explicit mode selection.

Summary phrases for static gates: default provider remains disabled; sandbox mode only selects TASK-161 scaffold; unknown mode fail closed; credentials isolation; no real broker order id; no order placement.

## Current Status

- TASK-159 disabled/null provider remains the production default.
- TASK-160 sandbox authorization gate remains active.
- TASK-161 scaffold exists and is not a usable broker adapter.
- TASK-162 selector can explicitly select disabled mode, sandbox scaffold mode, or unsupported mode in tests.
- DataServiceActions runtime behavior is unchanged.
- Production QML, startup, Presenter, Controller, and ShellServices behavior are unchanged.

## Disabled-Default Selector Boundary

The selector default mode is `disabled`. `defaultShellAccountingBrokerOrderPort()` still returns the TASK-159 disabled/null provider. The selector does not replace the default provider and is not connected to production startup or DataServiceActions runtime wiring.

## Mode Mapping

- `disabled`: returns disabled/null provider semantics.
- `sandbox`: selects only the TASK-161 scaffold, returning `success=false`, `disabled=true`, `dryRunOnly=true`, `brokerMode="sandbox"`, and `BROKER_SANDBOX_NOT_CONFIGURED`.
- empty, unknown, unsupported, paper, or real mode: fail closed to disabled provider semantics.

## Sandbox Scaffold Policy

Sandbox mode only selects TASK-161 scaffold. It does not call a broker SDK, does not access a network, does not read credentials, does not generate a real broker order id, and does not place an order. It remains unavailable / not configured / dry-run-only.

## DataService-Only Policy

The selector is a DataServiceApi boundary for future DataService-internal wiring. QML, production startup, Presenter, Controller, ShellServices, and ShellCore must not instantiate or call the selector directly. Future runtime use must remain behind the DataService boundary.

## CI No-Network Policy

CI no-network behavior remains mandatory. Selector tests use direct object calls and static scans only. The selector must not import network APIs, open sockets, call broker endpoints, or require an external service.

## Credentials Isolation Policy

TASK-162 adds no credentials, secrets, tokens, keys, passwords, endpoint values, or usable examples. Documentation and tests may mention policy words such as credentials, secrets, token, key, password, and network only to describe forbidden behavior.

## No Runtime / No Order Policy

TASK-162 does not implement broker SDK integration, broker network calls, real broker order ids, order placement, reconciliation, cancellation, correction, strategy execution, automatic trading, database writes, audit writes, schema changes, production QML changes, startup wiring changes, or DataServiceActions runtime behavior changes.

## Error / Fail-Closed Policy

Unknown and unsupported modes must fail closed to disabled provider semantics. Sandbox mode must remain not configured and unavailable. There must be no silent success and no fake broker success.

## Rollback Policy

Rollback is to keep using the TASK-159 disabled/null provider. Disabling or removing selector use must not affect TASK-154 dry-run behavior, TASK-161 scaffold behavior, TradeDraft creation, TradeDraft confirmation, audit write, snapshot write, read-only replay, or production UI.

## Test Checklist

- Selector compiles.
- Default mode is disabled.
- Disabled mode returns disabled/null provider semantics.
- Sandbox mode returns TASK-161 scaffold semantics.
- Empty / unknown / unsupported modes fail closed.
- Default provider remains unchanged.
- DataServiceActions runtime behavior remains unchanged.
- No production QML / startup / Presenter exposure.
- No broker SDK, network, credentials, real order id, order placement, database write, audit write, reconciliation, cancellation, correction, strategy execution, or automatic trading.

## Unauthorized Follow-Up Scope

Runtime sandbox enablement, broker credential injection, endpoint configuration, paper / real adapters, broker SDK integration, order placement, broker order id storage, reconciliation, cancellation, correction, strategy execution, automatic trading, and real broker order implementation must each be separately authorized.

## TASK-163 Follow-Up Gate

TASK-163 adds the broker sandbox runtime selector authorization gate. Runtime selector wiring remains unimplemented after TASK-163. TASK-162 selector behavior remains disabled-default and test-only unless a later task explicitly authorizes DataService runtime wiring.
