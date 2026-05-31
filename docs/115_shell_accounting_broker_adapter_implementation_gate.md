# ShellAccounting Broker Adapter Implementation Gate (TASK-158)

## Document Purpose

This document defines the gate that must pass before any future ShellAccounting broker adapter implementation is allowed. TASK-158 is documentation and test-only guard work. It does not implement a broker adapter, does not import or link a broker SDK, does not call a broker network endpoint, does not place a real order, does not modify production QML or startup, does not change TASK-148 / TASK-150 / TASK-152 / TASK-154 / TASK-157 behavior, does not modify schema, and does not add credentials or secrets.

## Current State

TASK-119 through TASK-157 are prerequisites. TASK-154 provides a broker order dry-run boundary. TASK-155 and TASK-156 define real broker order authorization and implementation gates. TASK-157 added the DataServiceApi-owned broker order port scaffold, request / response DTOs, a disabled/null implementation, and a test-only fake adapter. There is still no real broker adapter, no broker SDK, no sandbox / paper / real broker call, no real order placement, no strategy execution, and no automatic trading.

TASK-159 update: DataServiceApi now wires the disabled/null broker port as the default internal provider. The implementation gate's no-adapter-yet check has evolved to allow only disabled/null adapter wiring. Concrete sandbox, paper, and real adapter implementations remain forbidden.

TASK-160 update: a sandbox broker adapter authorization gate now exists. It is documentation and test-only guard work. Sandbox, paper, and real adapter implementations remain forbidden until a future implementation task explicitly authorizes one of those modes.

## TASK-157 Interface Scaffold Complete

The only production broker-facing type currently allowed is the abstract port scaffold from TASK-157. It is an interface boundary and disabled/null adapter, not a concrete broker adapter implementation. Future implementation work must preserve this separation: interface ownership can stay in DataServiceApi, while real adapter wiring must be separately authorized and default disabled.

TASK-159 keeps that contract and only adds default disabled wiring. The default provider is not a sandbox, paper, or real adapter.

## Future Adapter Implementation Gate Summary

Future broker adapter implementation must be a separate TASK. It must start from a clean main branch, pass this gate, pass full CTest, pass transport 50 repeat, and have explicit user authorization. This gate freezes the pre-implementation state so a future PR cannot silently add SDK calls, credentials, network calls, or real order placement.

## Adapter Mode Policy

Allowed future modes must be explicit and isolated:

- disabled: the default mode in all environments unless explicitly authorized.
- fake test-only: allowed only under tests and never in production runtime.
- sandbox: not equivalent to paper or real, and must be independently configured.
- paper: not equivalent to sandbox or real, and must be independently configured.
- real: highest-risk mode, not implemented in TASK-158, and still requires separate authorization.

Mode mismatch must fail closed. A sandbox adapter must not be usable as a real adapter. A paper adapter must not be treated as live execution. Real order placement remains separately authorized even after an adapter exists.

## Default-Disabled Policy

Any future adapter must default to disabled. Missing configuration, missing credentials, missing mode, unknown environment, test environment, CI, or rollback mode must return a disabled / unavailable response and must not attempt broker I/O.

## DataService-Only Boundary Policy

DataService remains the only allowed broker order entry point. QML, Presenter, ShellServices, ShellCore, and production UI must not directly instantiate, configure, or call a broker adapter. Future adapter use must be coordinated behind the DataService action boundary and existing TradeDraft / ledger checks.

## No QML / Presenter / Shell Direct Broker Policy

Production QML must not import a broker adapter, call a broker SDK, store broker credentials, or submit broker orders. Presenter and controller layers may expose safe state and errors only through existing ShellAccounting boundaries. They must not hold broker clients or order submission handles.

## No CI Network Call Policy

CI must never make broker network calls. Static tests must block broker endpoint tokens, submit-order HTTP helpers, sandbox / paper / real broker calls, and any production path that can perform outbound broker I/O during tests.

## Credentials Isolation Policy

Credentials must not enter the repository, docs, tests, QML, logs, or generated artifacts. Future adapter work must use external secret injection and must keep secret names, values, and raw broker payloads out of sanitized responses. Missing credentials must map to a clear error and must not fall back to fake success.

## Broker SDK Isolation Policy

No broker SDK is introduced in TASK-158. Future SDK integration must be isolated behind the adapter implementation, default disabled, and statically scannable. SDK import, link, initialization, and runtime calls must not appear in QML, Presenter, ShellCore, or generic DataService escape hatches.

## Order ID Policy

TASK-158 must not produce, persist, or expose a real broker order id. Future real order id storage requires a separate authorization task. Until then, responses must use disabled / unavailable / placeholder semantics and must not pretend that a real broker accepted an order.

## Reconciliation Handoff Policy

Broker adapter implementation does not automatically authorize reconciliation. Broker order id storage, broker status polling, fill reconciliation, partial fill handling, external cancellation status, and correction flow must be split into later tasks with their own gates.

## Cancellation / Correction Handoff Policy

Cancel, replace, correct, reverse, or manually reconcile broker orders are not part of TASK-158. Future cancellation or correction work must require independent authorization, explicit user-visible states, audit policy, rollback policy, and no silent success.

## Error Mapping Policy

Future adapter work must map at least:

- disabled
- credentials missing
- mode mismatch
- broker unavailable
- broker rejected
- timeout
- duplicate order
- unknown broker state
- audit failure
- reconciliation required
- no silent success

Unknown broker state must never be reported as accepted. Adapter failure must not be hidden behind a successful TradeDraft, ledger, or UI state.

## Rollback / Kill Switch / Incident Containment Policy

Future adapter implementation must include a kill switch. Disabling adapter execution must preserve TASK-154 dry-run behavior, TradeDraft creation, TradeDraft confirmation, audit, and read-only accounting features. Incident containment must stop broker I/O first, avoid blind retries, keep audit events sanitized, and not fabricate broker success.

## Forbidden Policy

TASK-158 explicitly forbids:

- no adapter implementation in TASK-158
- no SDK
- no network
- no real order
- no strategy
- no automatic trading
- no schema migration
- no credentials or secrets
- no production QML or startup change

## Future Implementation PR Requirements

A future broker adapter implementation PR must reference docs/115 and docs/116, keep the adapter default disabled, prove CI has no broker network call, isolate credentials outside the repo, preserve DataService-only access, include mode isolation tests, include rollback / kill switch tests, and explicitly state whether sandbox, paper, or real mode is being introduced. Real order placement still requires a separate authorization path if not explicitly included and tested.
