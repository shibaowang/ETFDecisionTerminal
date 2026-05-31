# ShellAccounting Broker Runtime Mode Source Authorization Gate (TASK-165)

## Document Purpose

TASK-165 defines the authorization gate for a future ShellAccounting broker runtime mode source. This document does not implement a runtime mode source, does not read broker mode from any runtime input, does not enable sandbox runtime, and does not change the TASK-164 disabled-default selector wiring.

Static gate summary phrases: runtime mode source authorization gate; mode source not implemented; sandbox runtime remains disabled; DataService-only future boundary; default disabled provider remains active; CI no-network; credentials isolation; rollback to TASK-159 disabled/null provider; no real broker order id; no order placement.

## Current Status

- TASK-159 disabled/null provider remains the effective broker runtime behavior.
- TASK-160 through TASK-163 established broker dry-run, sandbox adapter, mode selector, and runtime selector authorization gates.
- TASK-164 wired DataServiceActions to the selector only through `defaultShellAccountingBrokerOrderPortMode()` and `shellAccountingBrokerOrderPortForMode(brokerPortMode)`.
- TASK-165 adds no mode source and no sandbox runtime enablement.

## Future Runtime Mode Source Boundary

Future runtime mode source work must be separately authorized. It must be DataService-only, allowlist-based, fail closed, testable, and rollback-ready. The default must remain disabled unless a later task explicitly authorizes a non-disabled runtime source and its operational boundary.

Unknown, empty, unsupported, paper, and real values must fail closed. The policy is: sandbox runtime still requires a separate task. Sandbox may only be enabled by a later task that separately authorizes sandbox runtime behavior, sandbox adapter behavior, credentials isolation, and CI-safe test coverage.

## Forbidden Current Sources

TASK-165 must not read broker mode from request payload, QML, Presenter, config, environment, command line, local files, database rows, or secret store. It must not introduce a mode key, mode file, settings key, environment variable, credential lookup, or endpoint lookup.

Documentation and tests may contain policy terms, but production code must not contain a usable runtime source example, a usable credential example, or a real endpoint value.

## DataService-Only Policy

Future mode source selection can only be coordinated inside the DataService boundary. QML, ShellServices, ShellCore, Presenter, and Controller must not select broker mode directly. They must not call the selector directly, read broker mode, or pass a raw mode string that becomes a broker runtime decision.

## No Broker / No Network / No Credential Policy

TASK-165 adds no broker SDK, network call, endpoint, host, API path, credentials, token, key, password, certificate, real broker order id storage, real order placement, reconciliation, cancellation, correction, strategy execution, automatic trading, production QML change, production startup change, schema change, database write, audit write, or ledger write.

## Rollback Policy

Rollback remains TASK-159 disabled/null provider behavior. If a future mode source is disabled or fails validation, the broker order path must fall back to disabled/null provider semantics, not sandbox, paper, real, or inferred behavior.

## Required Future Gates

Future runtime mode source implementation requires a separate task, full CTest, transport repeat, CI no-network proof, credentials isolation proof, no direct QML mode source proof, and explicit user authorization. Sandbox runtime, credentials injection, endpoint configuration, broker SDK integration, real broker order id storage, reconciliation, cancellation, correction, strategy execution, and automatic trading each require separate authorization.
