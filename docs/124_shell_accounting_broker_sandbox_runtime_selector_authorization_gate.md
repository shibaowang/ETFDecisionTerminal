# ShellAccounting Broker Sandbox Runtime Selector Authorization Gate (TASK-163)

## Document Purpose

TASK-163 defines the authorization gate for a future ShellAccounting broker sandbox runtime selector wiring task. This document is gate-only: it does not wire the selector into `DataServiceActions` runtime behavior, does not enable sandbox runtime, does not modify production QML / startup / Presenter, does not implement a real broker adapter, and does not place orders.

Summary phrases for static gates: runtime selector wiring requires separate authorization; default must remain disabled; sandbox runtime can only select TASK-161 scaffold until separately authorized; unknown / empty / paper / real mode must fail closed; runtime mode source requires separate authorization; credentials injection requires separate authorization; CI no-network; rollback to TASK-159 disabled provider.

## Current Status

- TASK-159 disabled/null provider remains the production default.
- TASK-160 sandbox adapter authorization gate remains active.
- TASK-161 sandbox scaffold exists and remains not configured / unavailable / dry-run-only.
- TASK-162 disabled-default selector exists as an explicit testable factory boundary.
- TASK-163 does not connect the selector to `DataServiceActions` runtime.
- Production QML, startup, Presenter, Controller, ShellServices, and ShellCore remain unchanged.

## Future Runtime Wiring Boundary

Future runtime selector wiring must be implemented in a separate task. It must remain DataService-only, default-disabled, allowlist-based, statically scannable, and testable. Runtime selector wiring must not be inferred from the presence of TASK-162 selector code.

Future sandbox runtime can only select the TASK-161 scaffold unless a later task separately authorizes a real sandbox adapter. A future runtime mode source, configuration source, environment source, command-line source, or service setting source requires separate authorization.

## Fail-Closed Mode Policy

Default mode must remain disabled. Empty mode, unknown mode, unsupported mode, paper mode, and real mode must fail closed to disabled provider semantics until separately authorized. No mode may silently enable broker behavior.

## DataService-Only Policy

Runtime selector wiring, if later authorized, must be coordinated inside DataService. QML, startup, Presenter, Controller, ShellServices, and ShellCore must not directly instantiate, call, expose, or configure the broker selector. QML must not call `DataServiceClient` directly and must not bypass the DataService boundary.

## Credentials Isolation Policy

Credentials injection is not authorized by TASK-163. Future credentials injection requires a separate task. TASK-163 adds no environment-variable reads, config-file reads, secret-store reads, endpoint values, credential values, token values, key values, password values, or usable examples. Policy text may mention these terms only to define forbidden behavior.

## CI No-Network Policy

CI no-network behavior must remain mandatory. Runtime selector wiring tests must use static scans and local in-process probes only. No broker SDK, network client, socket, endpoint, host, API path, broker URL, or live service dependency may be introduced by this gate.

## No Runtime / No Broker Policy

TASK-163 does not add broker SDKs, network calls, credentials, endpoint values, real broker order ids, order placement, database writes, audit writes, ledger writes, reconciliation, cancellation, correction, strategy execution, automatic trading, production QML changes, startup changes, Presenter changes, Controller changes, schema changes, or migrations.

## Runtime Mode Source Policy

Runtime mode source selection is not authorized. Future work must explicitly define where the mode value comes from, how it is validated, how unknown values fail closed, how it is tested without network or secrets, and how rollback returns to the TASK-159 disabled/null provider.

## Rollback Policy

Rollback must return to TASK-159 disabled/null provider behavior. Disabling future runtime selector wiring must not affect TASK-154 dry-run behavior, TASK-161 scaffold behavior, TASK-162 selector tests, TradeDraft creation, TradeDraft confirmation, audit write, snapshot write, read-only replay, or production UI.

## Unauthorized Follow-Up Scope

Runtime selector wiring, runtime mode source, credentials injection, sandbox adapter implementation, paper adapter implementation, real broker adapter implementation, broker SDK integration, real order placement, real broker order id storage, reconciliation, cancellation, correction, strategy execution, automatic trading, and schema changes each require separate authorization tasks.

## Required Future Pre-Gates

- TASK-159 disabled/null provider remains default.
- TASK-160 sandbox authorization gate passes.
- TASK-161 scaffold tests pass.
- TASK-162 selector tests pass.
- TASK-163 authorization gate tests pass.
- Full CTest passes.
- `transport_local_socket_echo` 50 repeat passes.
- `git status --short` is clean.
- User explicitly authorizes runtime selector wiring.

## TASK-164 Baseline Update

TASK-164 completes disabled-default runtime selector wiring. The authorized post-TASK-164 state is: DataServiceActions may call `defaultShellAccountingBrokerOrderPortMode()` and `shellAccountingBrokerOrderPortForMode(mode)`, but no external runtime mode source is authorized. Sandbox runtime remains disabled, sandbox scaffold does not enter runtime, default behavior remains disabled/null provider behavior, and empty / unknown / paper / real modes still fail closed unless separately authorized.
