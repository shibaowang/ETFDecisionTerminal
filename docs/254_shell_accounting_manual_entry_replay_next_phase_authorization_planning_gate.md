# ShellAccounting Manual Entry Replay Next-Phase Authorization Planning Gate

## Purpose

TASK-229 defines the next-phase authorization planning gate after the
TASK-220 through TASK-228 negative fixture validator phase. It records the
authorized planning scope, future work tracks, required preconditions, risk
register, explicit non-authorization list, and Go / No-Go handoff rules for
manual entry replay work.

TASK-229 authorizes next-phase planning only. It does not authorize replay
implementation, AccountingEngine replay calls, parser / loader / reader
implementation, runtime SQL / SQLite read/write, audit / ledger / snapshot
writes, production integration, broker integration, real order placement, or
automatic trading.

## Background From TASK-220 Through TASK-228

TASK-220 through TASK-228 established the negative fixture validator phase:

- TASK-220 authorized negative fixture planning.
- TASK-221 authorized negative fixture scaffold planning.
- TASK-222 created the test-only negative fixture scaffold files.
- TASK-223 authorized the negative fixture static validator.
- TASK-224 implemented the test-only negative fixture static validator.
- TASK-225 added the regression matrix gate.
- TASK-226 hardened failure-mode behavior.
- TASK-227 added CI closeout coverage.
- TASK-228 closed out the phase and recorded the handoff inventory.

The next phase may build only from those completed artifacts. The phase
handoff does not weaken fixture immutability, validator behavior, clean-main
changed-path handling, or production boundary checks.

## Next-Phase Scope

The next phase may be planned but not implemented by TASK-229. Scope planning
may define authorization boundaries for manual entry replay input contracts,
test-only dry-run harnesses, AccountingEngine adequacy review, read model
reconciliation, and eventual runtime integration.

TASK-229 planning must remain static. It may document future tracks and
preconditions, but it must not add runtime code, replay execution, parser /
loader / reader behavior, or production integration.

## Future Work Tracks

Future candidate tracks are:

- manual entry replay input contract authorization
- test-only replay dry-run harness authorization
- AccountingEngine replay adequacy review
- read model replay reconciliation authorization
- runtime integration authorization

These tracks are candidates only. Each track requires a separate future TASK
before implementation, runtime execution, production integration, or schema /
repository behavior may change.

## Required Preconditions

Future manual entry replay tasks must require:

- TASK-220 through TASK-228 artifacts remain present and indexed.
- Negative fixture JSON remains unchanged unless a future fixture TASK
  explicitly authorizes a change.
- Positive fixture JSON remains unchanged unless separately authorized.
- TASK-224 validator validation behavior remains unchanged unless separately
  authorized.
- Clean-main gates allow an empty changed-path set and fail closed for
  non-empty unauthorized changed paths.
- Future replay work defines explicit input contracts before any parser,
  loader, reader, or replay execution is implemented.
- Future runtime integration defines DataService, read model, audit, ledger,
  privacy, rollback, and error-mapping boundaries before production use.
- Broker, network, credentials, endpoint, real order placement, and automatic
  trading remain out of scope unless separately authorized.

## Risk Register

- Fixture drift: future work may accidentally edit positive or negative fixture
  JSON. Mitigation: exact changed-path checks and fixture immutability probes.
- Validator drift: future work may alter TASK-224 validation behavior while
  adding planning artifacts. Mitigation: restrict validator edits to exact-path
  changed-set self-consistency unless a future validator TASK authorizes more.
- Replay leakage: planning may accidentally become parser, loader, reader, or
  replay implementation. Mitigation: no replay implementation path and no
  AccountingEngine replay call checks.
- Runtime write leakage: planning may accidentally add SQL, SQLite, audit,
  ledger, or snapshot writes. Mitigation: static forbidden token and production
  path scans.
- Production boundary drift: planning may touch apps, libs, migrations, QML,
  DataServiceActions, repositories, or broker code. Mitigation: exact
  changed-path allowlists and production prefix checks.
- Broker/network leakage: planning may introduce broker, network, credentials,
  endpoint, real order, or automatic trading behavior. Mitigation: static
  broker and network boundary probes.

## Explicit Non-Authorization List

- TASK-229 does not authorize replay implementation.
- TASK-229 does not authorize AccountingEngine replay calls.
- TASK-229 does not authorize parser / loader / reader implementation.
- TASK-229 does not authorize runtime SQL / SQLite read/write.
- TASK-229 does not authorize audit / ledger / snapshot writes.
- TASK-229 does not authorize production integration.
- TASK-229 does not authorize QML / DataServiceActions / repositories changes.
- TASK-229 does not authorize broker / network / credentials / endpoint.
- TASK-229 does not authorize real order placement or automatic trading.

## Replay And AccountingEngine Boundary

TASK-229 does not implement replay and does not call AccountingEngine replay.
Future replay work must first define an input contract, dry-run or review
boundary, fixture expectations, and failure semantics. A future TASK must
separately authorize any parser, loader, reader, replay harness, or
AccountingEngine replay invocation.

## Runtime SQL And Ledger Boundary

TASK-229 does not authorize runtime SQL / SQLite read/write. It does not
authorize audit writes, ledger writes, snapshot writes, cash fact writes, or
manual entry replay persistence. Future runtime tasks must define whether
operations are read-only, dry-run, audit-only, ledger-producing, or production
integrated before any write path is introduced.

## Broker And Network Boundary

TASK-229 does not authorize broker SDK access, broker adapter changes, network
calls, credentials, endpoints, real broker order ids, real order placement, or
automatic trading. Future replay work must remain independent of broker and
strategy execution unless a separate task explicitly authorizes that boundary.

## Fixture And Validator Boundary

TASK-229 does not modify fixture JSON and does not modify validator validation
behavior. If the TASK-224 test-only validator needs an exact-path changed-set
self-consistency update for TASK-229 paths, that update must not alter input
semantics, schema checks, issue-code mapping, sanitized summary output,
failure JSON behavior, or failure-mode behavior.

## Go / No-Go Handoff Rules

Go only if:

- TASK-229 planning gate passes.
- TASK-228 phase closeout still passes.
- TASK-227 CI closeout still passes.
- TASK-226, TASK-225, and TASK-224 validator gates still pass.
- Fixture JSON remains unchanged.
- Validator validation behavior remains unchanged.
- Production code remains unchanged.
- Broker and network boundaries remain unchanged.
- Full CTest is run externally and passes.
- `git diff --check` passes.

No-Go if:

- Any required CTest pattern matches zero tests.
- The planning gate depends on changed paths being non-empty.
- Fixture JSON changes.
- Validator validation behavior changes.
- Production code, QML, DataServiceActions, repositories, or migrations change.
- Parser, loader, reader, replay, or AccountingEngine replay behavior appears.
- Runtime SQL / SQLite / audit / ledger / snapshot write appears.
- Broker, network, credentials, endpoint, real order, or automatic trading
  appears.

## Formal Conclusion And Next Task

TASK-229 authorizes next-phase planning only.
TASK-229 does not authorize replay implementation.
TASK-229 does not authorize AccountingEngine replay calls.
TASK-229 does not authorize parser, loader, or reader implementation.
TASK-229 does not authorize runtime SQL / SQLite read/write.
TASK-229 does not authorize audit / ledger / snapshot writes.
TASK-229 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-230 manual entry replay test-only dry-run harness authorization gate.
