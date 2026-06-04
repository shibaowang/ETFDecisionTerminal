# ShellAccounting Manual Entry Replay Test-Only Dry-Run Harness Authorization Gate

## Purpose

TASK-230 defines the authorization boundary for a future manual entry replay
test-only dry-run harness. It fixes the candidate directory, input contract,
output contract, CTest name, no-write policy, no-runtime policy, no-replay
policy, no-AccountingEngine policy, and no-production boundary before any
harness implementation may be created.

TASK-230 is an authorization gate only. It does not implement the dry-run
harness, does not implement replay, does not call AccountingEngine replay, does
not modify fixture JSON, and does not modify production code.

## Relationship To TASK-229

TASK-229 completed the manual entry replay next-phase authorization planning
gate after the TASK-220 through TASK-228 negative fixture validator phase.
TASK-230 follows that planning handoff and narrows one future track: a
test-only dry-run harness for manual entry replay fixtures.

TASK-230 does not expand TASK-229 into runtime behavior. It documents the
future harness boundary only, and all parser, loader, reader, replay,
AccountingEngine, production, SQL, audit, ledger, broker, network, and order
capabilities still require separate future authorization.

## Future Harness Scope

The future harness may be considered only as a test-only dry-run tool. Its
purpose would be to inspect authorized manual entry replay fixtures and return
sanitized planning output without executing replay, writing data, or invoking
production services.

The future harness must be separately implemented by a later TASK. TASK-230
only authorizes the boundary text and static gate that future work must satisfy.

## Future Directory Boundary

The only future harness directory candidate is:

```text
tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/
```

TASK-230 does not create this directory. The directory must remain absent until
a future implementation TASK explicitly authorizes it.

## Future CTest Name

The only future harness CTest name candidate is:

```text
shell_accounting_manual_entry_replay_test_only_dry_run_harness
```

TASK-230 does not register that CTest. The current authorization gate CTest is:

```text
shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization
```

## Future Input Boundary

The future harness may document these test-only fixture inputs, but TASK-230
does not implement any reader, parser, loader, or replay behavior:

```text
tests/fixtures/manual_entry_replay/fixtures_index.json
tests/fixtures/manual_entry_replay/*.json
tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json
tests/fixtures/manual_entry_replay_negative/NEG_MRF*.json
```

The future harness must not accept production data, UI raw payload, DataService
payload, broker payload, credentials, endpoint configuration, or runtime
database state as input.

## Future Output Boundary

The future dry-run output contract may include only sanitized test-only fields:

```text
fixtureId
dryRunStatus
sanitizedSummary
plannedStepCount
warnings
```

The output must not contain raw SQL, raw fixture payloads, full manual entry
payloads, ledger rows, audit rows, broker payloads, credentials, endpoints,
internal exception stacks, real broker order ids, or runtime database state.

## No-Write Boundary

TASK-230 does not authorize any write path. The future harness must remain
test-only and no-write unless another TASK separately authorizes a different
boundary.

Forbidden writes include runtime SQL / SQLite read/write, audit writes, ledger
writes, snapshot writes, cash fact writes, trade fact writes, fixture JSON
changes, generated replay results, broker status writes, and production state
changes.

## No-Replay Boundary

TASK-230 does not authorize replay implementation. It does not authorize replay
execution, replay adapters, replay readers, replay loaders, replay parsers, or
manual entry replay runtime integration.

Future dry-run harness work must still prove that it does not execute replay
unless a future TASK separately authorizes a replay execution boundary.

## AccountingEngine Boundary

TASK-230 does not authorize AccountingEngine replay calls. The future harness
must not instantiate AccountingEngine for replay, call an AccountingEngine
replay method, or use AccountingEngine output as a hidden runtime substitute.

AccountingEngine adequacy review and any future replay invocation remain
separate future work.

## Runtime SQL And Ledger Boundary

TASK-230 does not authorize runtime SQL / SQLite read/write. It does not
authorize audit / ledger / snapshot writes, manual transaction writes, manual
cash movement writes, TradeDraft writes, broker writes, or replay persistence.

Any future runtime SQL, ledger, audit, or snapshot behavior must be separately
authorized before implementation.

## Broker And Network Boundary

TASK-230 does not authorize broker SDK access, broker adapter changes, network
calls, credentials, endpoints, real broker order ids, real order placement, or
automatic trading.

The future harness must remain independent of broker and strategy execution.

## Explicit Non-Authorization List

- TASK-230 does not authorize dry-run harness implementation.
- TASK-230 does not authorize replay implementation.
- TASK-230 does not authorize AccountingEngine replay calls.
- TASK-230 does not authorize parser / loader / reader implementation.
- TASK-230 does not authorize runtime SQL / SQLite read/write.
- TASK-230 does not authorize audit / ledger / snapshot writes.
- TASK-230 does not authorize production integration.
- TASK-230 does not authorize QML / DataServiceActions / repositories changes.
- TASK-230 does not authorize broker / network / credentials / endpoint.
- TASK-230 does not authorize real order placement or automatic trading.

## Go / No-Go Handoff Rules

Go only if:

- TASK-230 authorization gate passes.
- Future harness directory remains absent.
- Future harness CTest is documented but not registered as an implementation.
- Fixture JSON remains unchanged.
- Validator validation behavior remains unchanged.
- Production code remains unchanged.
- No parser / loader / reader / replay implementation is added.
- No AccountingEngine replay call is added.
- No runtime SQL / SQLite / audit / ledger / snapshot write is added.
- No broker / network / credentials / endpoint behavior is added.
- Full CTest is run externally and passes.
- `git diff --check` passes.

No-Go if:

- Any required CTest pattern matches zero tests.
- The gate depends on changed paths being non-empty.
- The future harness directory or implementation appears.
- Fixture JSON changes.
- Validator validation behavior changes.
- Apps, libs, migrations, QML, DataServiceActions, or repositories change.
- Parser, loader, reader, replay, or AccountingEngine replay behavior appears.
- Runtime SQL / SQLite / audit / ledger / snapshot write appears.
- Broker, network, credentials, endpoint, real order, or automatic trading
  appears.

## Formal Conclusion And Next Task

TASK-230 authorizes the future test-only dry-run harness boundary only.
TASK-230 does not authorize dry-run harness implementation.
TASK-230 does not authorize replay implementation.
TASK-230 does not authorize AccountingEngine replay calls.
TASK-230 does not authorize parser, loader, or reader implementation.
TASK-230 does not authorize runtime SQL / SQLite read/write.
TASK-230 does not authorize audit / ledger / snapshot writes.
TASK-230 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-231 manual entry replay test-only dry-run harness implementation gate.
