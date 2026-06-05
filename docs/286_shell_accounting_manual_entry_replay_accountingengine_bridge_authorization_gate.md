# TASK-245 ShellAccounting manual entry replay AccountingEngine bridge authorization gate

## Purpose

TASK-245 authorizes AccountingEngine bridge policy only. It establishes the boundary, Go / No-Go conditions, static gates, and CI guardrails for a future test-only bridge task.

TASK-245 does not implement AccountingEngine bridge, does not call AccountingEngine replay, does not implement replay, does not modify `libs/AccountingEngine/`, does not modify production code, does not modify fixture JSON, and does not authorize runtime integration.

## Relationship To TASK-239 Through TASK-244

TASK-239 through TASK-243 created and hardened the AccountingEngine adequacy review gates.

TASK-244 closed out the adequacy review phase and recommended a separate bridge authorization gate.

TASK-245 is that authorization gate. It converts the phase closeout handoff into a future bridge policy boundary, but still does not create bridge implementation.

## Authorization Scope

TASK-245 authorizes documentation and a test-only CTest gate for future bridge policy.

The CTest is `shell_accounting_manual_entry_replay_accountingengine_bridge_authorization`.

Authorized files are limited to docs/286, docs/287, the TASK-245 gate directory, and index updates. If older gates need changed-path self-consistency updates, they may only receive exact TASK-245 paths or exact old-gate script paths explicitly needed by the failing gate.

## Future Bridge Boundary

A future bridge task may propose a test-only adapter between manual entry replay summaries and AccountingEngine inputs.

That future task must separately authorize any bridge implementation, any replay invocation, any sanitized output contract, and any failure mapping. TASK-245 does not grant those implementation permissions.

## Future Test-Only Bridge Directory

The future bridge directory candidate is:

```text
tests/ShellAccountingManualEntryReplayAccountingEngineBridge/
```

TASK-245 does not create this directory.

The TASK-245 gate must fail if this directory exists during the authorization-only phase.

## Future Bridge Input Contract

Future bridge input may only come from authorized test-only manual entry replay fixture or dry-run summary artifacts.

Future bridge input must not come from production QML, DataServiceActions, DataAccess repositories, SQLite, broker payloads, network responses, credentials, endpoints, or raw UI payloads.

## Future Bridge Output Contract

Future bridge output must be sanitized, deterministic, test-only, and explicitly scoped by the future implementation task.

Future output must not write runtime SQL / SQLite, audit rows, ledger rows, snapshot rows, read models, broker state, or production state.

## Future AccountingEngine Call Boundary

TASK-245 does not call AccountingEngine replay.

A future implementation task must explicitly authorize any AccountingEngine call site and must keep that call test-only unless a later production task separately authorizes runtime integration.

## Future Replay Boundary

TASK-245 does not implement replay.

Future replay invocation must be separately authorized and must define input validation, failure mapping, sanitized output, and no-write behavior before any execution path exists.

## Runtime Isolation Boundary

TASK-245 is isolated from runtime behavior. It does not modify production startup, QML, Presenter, Controller, ShellServices, DataServiceActions, DataServiceActionRegistrar, DataAccess repositories, migrations, AccountingEngine, StrategyEngine, or MarketEngine.

## No-Write Boundary

TASK-245 does not authorize runtime SQL / SQLite read/write.

TASK-245 does not authorize audit / ledger / snapshot writes.

TASK-245 does not authorize read model refresh.

The gate must not write files or mutate fixture JSON.

## AccountingEngine Code Boundary

TASK-245 does not modify AccountingEngine code.

`libs/AccountingEngine/` must remain unchanged. Any AccountingEngine code change requires a separate task.

## Production Integration Boundary

TASK-245 does not authorize production integration.

Future production parser, loader, reader, runtime replay, read model refresh, ShellServices binding, QML binding, or DataService action integration must be separately authorized.

## Fixture Boundary

TASK-245 does not modify fixture JSON.

Positive and negative manual entry replay fixtures remain immutable in this authorization task.

## Broker And Network Boundary

TASK-245 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

The future bridge must not introduce broker SDK references, network clients, credentials, endpoints, real broker order ids, order placement, strategy execution, or automatic trading.

## Authorization Go / No-Go Decision

Go means TASK-245 docs, indexes, CTest registration, changed-path allowlist, prior adequacy gates, no bridge directory, no replay calls, no writes, no production drift, no fixture drift, and no broker/network drift all pass.

No-Go means any bridge implementation, AccountingEngine replay call, replay implementation, AccountingEngine code drift, fixture JSON drift, production code drift, runtime write, read model refresh, broker/network drift, missing docs, missing CTest registration, or unauthorized changed path.

## Formal Conclusion

TASK-245 authorizes AccountingEngine bridge policy only.

TASK-245 does not implement AccountingEngine bridge.

TASK-245 does not call AccountingEngine replay.

TASK-245 does not implement replay.

TASK-245 does not modify AccountingEngine code.

TASK-245 does not modify fixture JSON.

TASK-245 does not authorize production integration.

TASK-245 does not authorize runtime SQL / SQLite read/write.

TASK-245 does not authorize audit / ledger / snapshot writes.

TASK-245 does not authorize read model refresh.

TASK-245 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-246 ShellAccounting manual entry replay AccountingEngine bridge test-only implementation gate.

## Recommended Next Task

Recommended next task: TASK-246 ShellAccounting manual entry replay AccountingEngine bridge test-only implementation gate.

TASK-246 must explicitly authorize the test-only bridge implementation before creating `tests/ShellAccountingManualEntryReplayAccountingEngineBridge/` or calling AccountingEngine replay.
