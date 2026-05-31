# ShellAccounting Broker Sandbox Runtime Mode Source Scaffold (TASK-168)

## Document Purpose

TASK-168 adds a directly testable sandbox runtime mode source scaffold. This is a scaffold, not runtime enablement. The scaffold is only for direct tests and does not replace the default runtime source.

The default runtime source remains disabled-only. `DataServiceActions` is not connected to the sandbox source, and sandbox runtime remains disabled.

## Current Status

- TASK-167 authorized this scaffold boundary.
- TASK-166 disabled-only runtime mode source remains the runtime default.
- `SandboxShellAccountingBrokerRuntimeModeSourceScaffold` can be directly instantiated by tests.
- Direct scaffold use returns `ShellAccountingBrokerOrderPortMode::Sandbox`.
- Runtime dry-run continues to use the disabled/null provider semantics.
- `brokerPortMode` remains `disabled`, `brokerPortDisabled` remains `true`, and `brokerPortDryRunOnly` remains `true` for runtime.

## Scaffold Boundary

The scaffold has no constructor arguments, no setters, no registry, no global override, and no runtime injection API. It does not read payload, QML, config, environment, command line, files, database rows, or secret stores.

The scaffold does not read credentials, endpoints, account data, order data, portfolio data, broker payloads, or any external mode source. It only returns sandbox mode when directly invoked in tests.

## Runtime Policy

`defaultShellAccountingBrokerRuntimeModeSource()` still returns the disabled-only source. `DataServiceActions` continues to call the default source and does not instantiate or select the sandbox scaffold.

Direct sandbox mode still reaches only the existing TASK-161 sandbox order port scaffold unavailable / not configured behavior. Sandbox runtime is not enabled.

## Still Separate Future Tasks

The following remain separate authorization scopes:

- sandbox runtime enablement;
- sandbox adapter implementation;
- credentials provider;
- endpoint provider;
- real broker order id storage;
- reconciliation;
- cancellation;
- correction;
- paper runtime source;
- real runtime source;
- broker SDK integration;
- order placement;
- strategy execution;
- automatic trading.

## No Side Effects

TASK-168 adds no broker SDK, no network call, no URL, no host, no port, no endpoint, no credentials, no token, no key, no password, no secret value, no real broker order id, no real order placement, no database write, no audit write, no ledger write, no reconciliation, no cancellation, no correction, no strategy execution, and no automatic trading.

CI remains no-network, no-credentials, no real credential values, and no-real-order-placement.

## Privacy Policy

Real credentials, secrets, token values, key values, password values, endpoint values, account payloads, and order payloads must not enter code, docs, repository files, logs, or test data. Docs and tests may contain policy keywords only.

## Rollback Policy

Rollback is to remove the direct-test scaffold and keep the TASK-166 disabled-only runtime source. If the scaffold is unavailable, runtime remains disabled-only. No fallback may infer sandbox runtime, read external state, or place orders.

## Validation Summary

TASK-168 tests prove direct scaffold behavior, default disabled runtime behavior, `DataServiceActions` non-wiring, no external mode source, no credentials or endpoint source, no broker SDK, no network, no real order id, no order placement, no database/audit/ledger write, no reconciliation/cancellation/correction, no strategy execution, no automatic trading, and no schema modification.
