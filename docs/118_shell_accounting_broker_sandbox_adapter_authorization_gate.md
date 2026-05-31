# ShellAccounting Broker Sandbox Adapter Authorization Gate (TASK-160)

## TASK-160 Purpose

TASK-160 defines the authorization gate for a future ShellAccounting sandbox broker adapter. This task is documentation and test-only guard work. It does not implement a sandbox adapter, paper adapter, real adapter, broker SDK integration, broker network call, real order placement, real broker order id storage, reconciliation, cancellation, correction, strategy execution, or automatic trading.

## TASK-159 Prerequisite

TASK-159 is a prerequisite. The current production broker adapter wiring remains the DataServiceApi-owned disabled/null provider from TASK-159. TASK-160 does not change that disabled/null wiring or the TASK-154 dry-run behavior.

## Sandbox Adapter Future Boundary

A sandbox broker adapter is a future capability and must be implemented only in a separately authorized task. The future implementation must state whether it introduces sandbox behavior only, prove that paper and real modes remain isolated, and keep real order placement outside the sandbox adapter scope unless separately authorized.

## Disabled Default Policy

Disabled remains the default mode. Missing configuration, CI, rollback, production startup, and any unrecognized mode must fail closed to the disabled/null adapter. A future sandbox adapter must never become the implicit default.

## Sandbox / Paper / Real Mode Isolation

Sandbox, paper, and real modes must be isolated. Each mode must have a separate authorization boundary, configuration source, test matrix, audit policy, and rollback plan. A sandbox adapter must not share credentials, endpoints, order ids, or side effects with paper or real mode.

## Fake Adapter Test-Only Policy

The fake broker adapter remains test-only. It may be used by tests to prove port behavior, but it must not be linked into production runtime, exposed to QML, or used as a sandbox adapter replacement.

## Credentials Isolation Policy

Real credentials, secrets, tokens, keys, passwords, account secrets, or usable example values must not enter code, repository files, docs, logs, tests, or fixtures. Documentation and tests may mention policy words such as credentials, secrets, token, key, password, and network only to describe forbidden behavior. Future sandbox credentials must be injected by an approved runtime secret mechanism outside the repository.

## CI No-Network Policy

CI must not call broker networks. Sandbox adapter tests must use fakes, disabled/null ports, and static scans. No test may require an external endpoint, reachable broker host, account, credential, or network side effect.

## DataService-Only Broker Adapter Boundary

DataService remains the only broker adapter boundary. Future sandbox adapter wiring must live behind the DataService action and port boundary. QML, Presenter, ShellServices, ShellCore, and production UI must not instantiate, configure, or call a broker adapter directly.

## No QML / Presenter / Shell Direct Adapter Policy

QML, Presenter, ShellServices, and ShellCore must not receive broker adapter objects, broker SDK clients, endpoint handles, credential handles, or broker port provider access. They may display sanitized DataService results only.

## No Broker SDK Policy

TASK-160 does not import, link, initialize, or call any broker SDK. A future sandbox adapter task must explicitly authorize any SDK or protocol dependency and must still prove CI no-network behavior.

## No Real Order Placement Policy

Real order placement remains forbidden. Sandbox adapter authorization does not authorize real broker orders, paper orders, automatic submission, or broker execution.

## No Real Broker Order ID Storage Policy

Real broker order id storage is not authorized. Future storage, reconciliation, cancellation, correction, or external order state tracking must be split into separate tasks.

## No Reconciliation Policy

Broker reconciliation remains unimplemented and unauthorized. A sandbox adapter gate does not authorize reconciliation jobs, broker status polling, or ledger reconciliation.

## No Cancellation / Correction Policy

Broker cancellation and correction flows remain unimplemented and unauthorized. Future cancellation or correction must have independent authorization, audit, rollback, and idempotency checks.

## No Strategy Execution / Automatic Trading Policy

Strategy execution and automatic trading remain forbidden. A sandbox broker adapter must not become a path for StrategyEngine execution, automatic background submission, or unattended order flow.

## Rollback / Kill Switch / Fail-Closed Policy

Future sandbox adapter work must include a kill switch. Disabling sandbox mode must fall back to the TASK-159 disabled/null provider and preserve TASK-154 dry-run, TradeDraft create, TradeDraft confirm, audit, and UI review flows. Rollback must not fake broker success or retry order placement.

## Future Implementation PR Requirements

A future sandbox adapter implementation PR must reference docs/118 and docs/119, start from clean main, pass full CTest, pass transport 50 repeat, keep disabled default, prove sandbox / paper / real isolation, prove no real credential values are committed, prove CI no-network behavior, preserve DataService-only access, and explicitly state that real order placement remains unauthorized.
