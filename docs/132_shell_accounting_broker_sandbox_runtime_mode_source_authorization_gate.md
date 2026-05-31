# ShellAccounting Broker Sandbox Runtime Mode Source Authorization Gate (TASK-167)

## Document Purpose

TASK-167 is an authorization gate only. It defines the boundary for a possible future sandbox runtime mode source, but it does not implement that source, does not enable sandbox runtime, and does not change the TASK-166 disabled-only runtime mode source scaffold.

The current runtime source remains disabled-only. Broker dry-run continues to use the disabled/null provider semantics from TASK-159 through TASK-166.

Static gate summary: TASK-167 does not implement sandbox mode source selection; Sandbox runtime is not enabled; no real credential values; no broker SDK; no network; no endpoint; no real broker order id; no order placement.

TASK-168 follow-up: a direct-test sandbox runtime mode source scaffold is now authorized. That scaffold is not runtime enablement, `DataServiceActions` is not wired to it, and sandbox runtime remains disabled.

## Current Status

- TASK-166 added `ShellAccountingBrokerRuntimeModeSource` as a disabled-only internal source.
- `defaultShellAccountingBrokerRuntimeModeSource()` still returns disabled mode.
- `DataServiceActions` must not read broker mode from payload, QML, config, environment, command line, files, database rows, or secret stores.
- Sandbox runtime is not enabled.
- TASK-168 may add a directly instantiated sandbox source scaffold for tests only.
- Sandbox direct selector access remains scaffold unavailable / not configured and is not runtime mode.
- Paper and real runtime sources remain unauthorized.

## Future Sandbox Runtime Mode Source Boundary

A future sandbox runtime mode source must be a separate TASK with explicit authorization. It may only select a mode; it must not read credentials, endpoints, account data, order data, portfolio data, or broker payloads. It must be DataService-only, fail closed, testable, and rollback-ready.

Unknown, blank, unsupported, paper, and real values must fail closed to disabled or a clear error. They must never automatically enter sandbox, paper, or real mode.

## Isolation Policy

Sandbox runtime mode source is not sandbox adapter implementation. The following remain separate authorization scopes:

- sandbox adapter implementation;
- credentials provider;
- endpoint provider;
- broker SDK integration;
- real broker order id storage;
- reconciliation;
- cancellation;
- correction;
- strategy execution;
- automatic trading.

## Forbidden Current Behavior

TASK-167 does not allow production runtime wiring for a sandbox runtime source, payload mode source, QML mode source, config source, environment source, command-line source, file source, database source, or secret-store source. TASK-168 only permits the explicitly named direct-test scaffold.

TASK-167 adds no broker SDK, no network call, no URL, no host, no port, no endpoint, no credentials, no token, no key, no password, no secret value, no real broker order id, no real order placement, no DB write, no audit write, no ledger write, no reconciliation, no cancellation, no correction, no strategy execution, and no automatic trading.

## CI And Privacy Policy

CI must remain no-network, no-credentials, and no-real-order-placement. Documentation and tests may contain policy keywords, but they must not include real credential values, usable credential examples, real endpoint values, or default environment variable values.

## Rollback Policy

Rollback remains the TASK-166 disabled-only source and TASK-159 disabled/null provider behavior. If a future sandbox runtime mode source is disabled, invalid, blank, unsupported, unavailable, or not configured, the runtime must return disabled or an explicit error and must not infer sandbox runtime.

## Future Implementation Requirements

Any future sandbox runtime source implementation must open a separate TASK, reference this document and docs/133, preserve DataService-only coordination, add static and runtime tests, prove no credential/endpoint/order data is read by the mode source, prove CI no-network behavior, and preserve rollback to disabled/null provider.
