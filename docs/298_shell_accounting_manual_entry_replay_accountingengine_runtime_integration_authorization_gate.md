# TASK-251 ShellAccounting manual entry replay AccountingEngine runtime integration authorization gate

## Purpose

TASK-251 authorizes policy for a future ShellAccounting manual entry replay
AccountingEngine runtime integration task.

The task is authorization-only and runtime-integration-policy-only. It records
the required boundaries, Go / No-Go conditions, and CI gates before any future
runtime integration can be implemented.

This document defines future runtime integration policy for the next separately
authorized implementation gate.

TASK-251 does not implement runtime integration.

## Relationship To TASK-245 Through TASK-250

TASK-245 authorized the test-only AccountingEngine bridge boundary.

TASK-246 added the stdout-only bridge contract shell.

TASK-247 added bridge regression matrix coverage.

TASK-248 hardened bridge failure modes and sanitized output.

TASK-249 added bridge CI closeout coverage.

TASK-250 closed out the test-only bridge phase and handed off to this runtime
integration authorization gate.

TASK-251 does not weaken any earlier bridge, adequacy review, dry-run harness,
fixture, no-write, no-replay, or no-production boundary.

## Authorization Scope

TASK-251 authorizes AccountingEngine runtime integration policy only.

The scope is limited to documentation, CTest registration, static checks,
changed-path allowlist checks, and future Go / No-Go requirements.

TASK-251 does not authorize implementation code, runtime calls, production
startup wiring, replay execution, database writes, read model refresh, broker
access, network access, credentials, endpoints, real order placement, or
automatic trading.

## Future Runtime Integration Boundary

A future runtime integration implementation must be separately authorized.

The future task must define a narrow runtime entry point, its input contract,
its output contract, its error mapping, its privacy policy, and its rollback
strategy before any production path can invoke AccountingEngine replay.

Future runtime integration must be fail-closed and must not depend on silent
success.

## Future Integration Entry Points

Future entry points must be explicitly named in a separate task.

Future entry points must remain behind a DataService or equivalent service
boundary. QML, Presenter, Controller, startup, and UI code must not directly
call AccountingEngine replay.

Future entry points must distinguish test-only bridge evidence from runtime
integration behavior.

## Future Data Flow Contract

Future runtime integration input must be sanitized, structured, and derived from
authorized manual entry replay data.

Future runtime integration must not consume raw UI payloads, raw SQL, raw
fixture payloads, broker payloads, credentials, endpoints, or exception stacks.

Future output must explicitly report partial, unavailable, failed, or disabled
states.

## AccountingEngine Call Authorization Boundary

TASK-251 does not call AccountingEngine replay.

Future AccountingEngine calls must be separately authorized by TASK-252 or a
later explicit implementation authorization task.

No future task may treat TASK-251 as permission to import `libs/AccountingEngine/`
from production runtime paths.

## Replay Boundary

TASK-251 does not implement replay.

TASK-251 does not authorize `replayReadOnly`, AccountingEngine replay bridge
execution, production replay parser, production replay loader, production replay
reader, or runtime replay integration.

Future replay must remain disabled until a separate implementation task provides
runtime tests and rollback behavior.

## Runtime Write Boundary

TASK-251 does not authorize runtime SQL / SQLite read/write.

TASK-251 does not authorize INSERT, UPDATE, DELETE, REPLACE, schema mutation, or
repository writes.

Future runtime integration must define whether it is read-only, no-write, or
write-enabled before any implementation can proceed.

## Audit Ledger Snapshot Boundary

TASK-251 does not authorize audit / ledger / snapshot writes.

Future audit, ledger, and snapshot changes must be separately authorized,
tested, sanitized, and rollback-ready.

## Read Model Refresh Boundary

TASK-251 does not authorize read model refresh.

Future readback refresh, position refresh, cash refresh, PnL refresh, and manual
entry replay refresh must be separately authorized and tested.

## AccountingEngine Code Boundary

TASK-251 does not modify AccountingEngine code.

`libs/AccountingEngine/` must remain unchanged. Future runtime integration must
not modify AccountingEngine internals unless a separate AccountingEngine task is
authorized.

## Production Code Boundary

TASK-251 does not modify production code.

TASK-251 does not modify apps, libs, migrations, DataServiceActions,
repositories, QML, Presenter, Controller, or startup code.

## Fixture Boundary

TASK-251 does not modify fixture JSON.

Positive and negative manual entry replay fixtures remain unchanged. Future
runtime integration must not mutate fixtures as runtime state.

## Broker And Network Boundary

TASK-251 does not authorize broker, network, credentials, endpoint, real order
placement, or automatic trading.

Future runtime integration must not create broker capability, place orders,
execute strategies, contact endpoints, read credentials, or perform automatic
trading.

## Go / No-Go Decision

GO only if TASK-245 through TASK-250 gates, this TASK-251 authorization gate,
dry-run / replay implementation gates, diagnostics facade, full CTest,
transport 50-run repeat, and `git diff --check` pass.

NO-GO if any production code, AccountingEngine code, fixture JSON, replay call,
runtime integration, runtime write, audit, ledger, snapshot, read model refresh,
broker, network, credentials, endpoint, real order placement, or automatic
trading boundary is crossed.

## Formal Conclusion

TASK-251 authorizes AccountingEngine runtime integration policy only.

TASK-251 does not implement runtime integration.

TASK-251 does not call AccountingEngine replay.

TASK-251 does not modify AccountingEngine code.

TASK-251 does not implement production bridge.

TASK-251 does not modify fixture JSON.

TASK-251 does not modify production code.

TASK-251 does not authorize runtime SQL / SQLite read/write.

TASK-251 does not authorize audit / ledger / snapshot writes.

TASK-251 does not authorize read model refresh.

TASK-251 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-252 ShellAccounting manual entry replay AccountingEngine runtime integration implementation authorization gate.

## Recommended Next Task

TASK-252 ShellAccounting manual entry replay AccountingEngine runtime
integration implementation authorization gate.
