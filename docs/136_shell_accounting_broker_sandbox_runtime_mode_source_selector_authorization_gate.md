# ShellAccounting Broker Sandbox Runtime Mode Source Selector Authorization Gate (TASK-169)

## Document Purpose

TASK-169 is an authorization gate only. It defines the future boundary for a sandbox runtime mode source selector, but it does not implement that selector, does not enable sandbox runtime, does not modify runtime source production code, and does not modify `DataServiceActions`.

The TASK-168 sandbox runtime mode source scaffold remains direct-test-only. The default runtime source remains disabled-only; sandbox runtime remains disabled.

## Current Status

- TASK-168 added `SandboxShellAccountingBrokerRuntimeModeSourceScaffold` for direct tests.
- `defaultShellAccountingBrokerRuntimeModeSource()` still returns the disabled-only source.
- `DataServiceActions` is not wired to a sandbox source selector.
- Sandbox runtime remains disabled.
- Direct sandbox mode still reaches only sandbox order port scaffold unavailable / not configured behavior.
- Paper and real runtime sources remain unimplemented.

## Future Selector Authorization Boundary

A future runtime mode source selector must be implemented in a separate TASK. It may only choose between explicitly authorized sources, initially the disabled source and the TASK-168 sandbox scaffold source, and it must fail closed for unknown, blank, unsupported, paper, and real modes.

The selector must not read broker mode from payload, QML, config, environment, command line, files, database rows, or secret stores. It must not carry credentials, endpoints, account payloads, order payloads, portfolio payloads, or broker payloads.

## DataService And Runtime Policy

Future selector wiring must be DataService-only and separately authorized. TASK-169 does not allow `DataServiceActions` to instantiate or use a selector, does not allow QML/Presenter/Controller/startup wiring, and does not allow the sandbox scaffold to become the default runtime source.

`defaultShellAccountingBrokerRuntimeModeSource()` must continue to return disabled-only until a later implementation task explicitly changes that behavior.

## Isolation Policy

The future selector is not sandbox adapter implementation. These remain separate authorization scopes:

- sandbox adapter implementation;
- credentials provider;
- endpoint provider;
- broker SDK integration;
- real broker order id storage;
- reconciliation;
- cancellation;
- correction;
- paper runtime source;
- real runtime source;
- strategy execution;
- automatic trading.

## Forbidden Current Behavior

TASK-169 adds no runtime mode source selector implementation, no sandbox runtime enablement, no payload mode source, no QML mode source, no config source, no environment source, no command-line source, no file source, no database source, and no secret-store source.

TASK-169 adds no broker SDK, no network call, no URL, no host, no port, no endpoint, no credentials, no token, no key, no password, no secret value, no real broker order id, no order placement, no database write, no audit write, no ledger write, no reconciliation, no cancellation, no correction, no strategy execution, and no automatic trading.

## Static Scan Policy

Static scans must distinguish production source from docs/tests. Production source is scanned for forbidden selector implementation and runtime tokens. Docs and tests may contain policy keywords only to describe the gate.

## CI And Privacy Policy

CI remains no-network, no-credentials, and no-real-order-placement. Real credentials, secrets, token values, key values, password values, endpoint values, account payloads, and order payloads must not enter code, docs, repository files, logs, or test data.

## Rollback Policy

Rollback remains the TASK-166 disabled-only runtime source plus the TASK-168 direct-test-only scaffold. If a future selector is unavailable, disabled, invalid, blank, or unsupported, it must fail closed to disabled or an explicit unavailable/error result and must not infer sandbox, paper, or real mode.

## Future Implementation Requirements

Any future selector implementation must open a separate TASK, reference this document and docs/137, keep external mode sources unauthorized unless explicitly approved, keep credentials/endpoints/order data out of the selector, add static and runtime tests, and preserve rollback to disabled-only behavior.
