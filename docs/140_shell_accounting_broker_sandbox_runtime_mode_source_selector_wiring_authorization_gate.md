# ShellAccounting Broker Sandbox Runtime Mode Source Selector Wiring Authorization Gate (TASK-171)

## Document Purpose

TASK-171 is an authorization gate-only task for future wiring of the TASK-170 runtime mode source selector scaffold into `DataServiceActions`. It does not implement runtime wiring, does not modify `DataServiceActions.cpp`, does not modify runtime source production code, and does not enable sandbox runtime.

This gate exists to keep the next implementation step narrow: a future wiring task must be separately authorized, and its first permitted step may only be disabled-default selector wiring unless another explicit TASK authorizes sandbox runtime mode source enablement.

TASK-172 implements that first permitted step: disabled-default selector wiring in `DataServiceActions`. It does not enable sandbox runtime and does not add external mode sources.

## Current Status

- TASK-166 established the disabled-only runtime mode source baseline.
- TASK-168 added the sandbox runtime mode source scaffold as direct-test-only.
- TASK-169 authorized a future selector boundary.
- TASK-170 added `shellAccountingBrokerRuntimeModeSourceForMode` as a direct-test-only selector scaffold.
- `defaultShellAccountingBrokerRuntimeModeSource()` remains disabled-only.
- `DataServiceActions.cpp` is not wired to the selector scaffold.
- Sandbox runtime remains disabled.
- Paper and real runtime mode sources remain unimplemented.
- TASK-172 later wires `DataServiceActions.cpp` to the selector scaffold using only the disabled default source mode.

## Future Wiring Boundary

Future runtime mode source selector wiring must be implemented in a separate TASK. TASK-171 does not allow production callers to use `shellAccountingBrokerRuntimeModeSourceForMode`.

The first future wiring step may only use disabled-default selector wiring. It must not infer sandbox from payload, QML, config, environment, command line, files, database, or secret stores. Sandbox runtime enablement requires a separate explicit TASK after wiring authorization.

After TASK-172, production `DataServiceActions.cpp` may call `shellAccountingBrokerRuntimeModeSourceForMode(defaultBrokerPortMode)`, where `defaultBrokerPortMode` comes only from `defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode()`.

Unknown, blank, unsupported, paper, and real modes must fail closed to disabled or to an explicit unavailable/error result. No path may silently infer sandbox, paper, or real mode.

## DataServiceActions Policy

`DataServiceActions.cpp` remains unchanged by TASK-171 itself. TASK-172 authorizes only disabled-default selector wiring. It must not read external runtime mode sources, and it must continue to be driven by the existing disabled default runtime mode source.

Future wiring may only happen through `DataServiceActions` after explicit authorization. It must be allowlisted, statically scannable, and testable.

## External Mode Source Policy

Payload, QML, config, environment, command line, file, database, and secret-store mode sources remain unauthorized. Future wiring must not read those sources as runtime broker mode inputs.

Future wiring must not carry credentials, endpoint, account data, order data, broker payloads, usable credential examples, or real credential values.

## No Broker Side Effect Policy

TASK-171 adds no broker SDK, network call, endpoint, URL, host, port, credentials, secrets, token, key, password, real broker order id, order placement, database write, audit write, ledger write, reconciliation, cancellation, correction, strategy execution, or automatic trading.

CI must continue enforcing no-network, no-credentials, and no-real-order-placement policy. Policy keywords may appear in docs/tests, but production source must remain free of implementation wiring and side-effect tokens.

## Static Scan Scope Policy

Static scans must distinguish production source from docs/tests:

- Production source is scanned for forbidden runtime implementation, external source, broker SDK, network, credential, order placement, write, reconciliation, cancellation, correction, strategy, and automatic trading tokens.
- Docs/tests are scanned only for required policy text.
- Policy keywords in docs/tests must not be treated as production implementation violations.

## Rollback Policy

Rollback is to keep using the TASK-166 disabled-only runtime source and TASK-164 disabled selector behavior. Disabling any future wiring must return to disabled-default behavior without enabling sandbox runtime or changing TradeDraft, confirmation, broker dry-run, audit, ledger, or UI behavior.

## Future Implementation Requirements

Any future disabled-default selector wiring, sandbox runtime enablement, external mode source, credentials provider, endpoint provider, real broker order id storage, reconciliation, cancellation, correction, broker SDK, network, order placement, strategy execution, or automatic trading must open a separate TASK and reference docs/140 and docs/141.
