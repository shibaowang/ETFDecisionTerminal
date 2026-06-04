# ShellAccounting Manual Entry Replay Negative Fixture Static Validator Authorization Test Plan

## Document Purpose

This test plan defines the TASK-223 authorization-gate validation for a future
manual entry replay negative fixture static validator. The validation proves
that TASK-223 adds only documentation, test-plan coverage, and a static gate for
future validator policy. It does not implement a validator, parser, loader,
reader, replay path, AccountingEngine call, runtime database access, production
code path, or broker capability.

## Test Matrix

- docs/242 exists.
- docs/243 exists.
- README references TASK-223 and docs/242 / docs/243.
- docs/README references docs/242 / docs/243.
- docs/12 references TASK-223 and docs/242 / docs/243.
- TASK-223 CTest is registered.
- docs/242 covers future validator scope.
- docs/242 covers future input boundary.
- docs/242 covers future output contract.
- docs/242 covers future issue code mapping.
- docs/242 covers future failure semantics.
- docs/242 covers privacy and sanitization.
- docs/242 covers no replay.
- docs/242 covers no runtime dependency.
- TASK-222 negative fixture scaffold files remain unchanged.
- Negative fixture JSON is not modified.
- Positive fixture JSON is not modified.
- TASK-219 static validator is not modified.
- No negative validator / parser / loader / reader / replay implementation is
  added.
- No AccountingEngine replay call is added.
- No runtime SQL / SQLite read/write is added.
- No audit / ledger / snapshot write is added.
- No production code, QML, DataServiceActions, repositories, migrations,
  broker, network, credentials, endpoint, real order, or automatic trading path
  changes.

## Required Probes

- Documentation existence scan.
- Documentation index scan.
- CTest registration scan.
- Changed-path allowlist scan.
- Untracked-file detection scan.
- Required section scan for docs/242.
- Relationship-to-TASK-220-through-TASK-222 scan.
- Future validator scope scan.
- Future input boundary scan.
- Future output contract scan.
- Future issue code mapping scan.
- Future failure semantics scan.
- Privacy and sanitization scan.
- No replay scan.
- No runtime dependency scan.
- Formal conclusion scan.
- TASK-222 negative fixture directory exact-file-set probe.
- TASK-222 negative fixture index existence probe.
- TASK-222 ten negative JSON existence probe.
- Negative fixture JSON unchanged probe.
- Positive fixture JSON unchanged probe.
- TASK-219 static validator unchanged probe.
- No future validator directory probe.
- No future validator Python file probe.
- No parser / loader / reader implementation scan.
- No replay implementation scan.
- No AccountingEngine replay call scan.
- No runtime SQL / SQLite read/write scan.
- No audit / ledger / snapshot write scan.
- No production code / QML / DataServiceActions / repository / migration scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- TASK-222 scaffold files regression.
- TASK-221 scaffold authorization regression.
- TASK-220 negative fixtures authorization regression.
- TASK-219 static validator regression.
- TASK-218 static validator authorization regression.
- TASK-217 fixture files scaffold regression.
- shell_diagnostics_facade regression.
- broker and real broker gate regressions.
- transport_local_socket_echo 50-repeat regression.

## Go / No-Go Checklist

Go only if:

- docs/242 and docs/243 exist and are indexed by README, docs/README, and docs/12.
- TASK-223 CTest is registered.
- docs/242 covers future validator scope, input boundary, output contract,
  issue code mapping, failure semantics, privacy, no replay, and no runtime
  dependency.
- TASK-222 negative fixture scaffold files remain unchanged.
- Negative fixture JSON is not modified.
- Positive fixture JSON is not modified.
- TASK-219 static validator is not modified.
- No negative validator / parser / loader / reader / replay implementation is
  present.
- No AccountingEngine replay call is present.
- No production code / QML / DataServiceActions / repositories / migrations /
  broker path changes are present.
- All required CTest patterns match at least one test and pass.
- Full CTest passes.
- `git diff --check` passes.

No-Go if:

- Any negative fixture JSON changes.
- Any positive fixture JSON changes.
- TASK-219 static validator changes.
- `tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/` is
  created.
- `manual_entry_replay_negative_fixture_static_validator.py` is created.
- Any negative validator, parser, loader, reader, replay, AccountingEngine
  replay call, runtime SQL / SQLite read/write, audit / ledger / snapshot write,
  production code, QML, DataServiceActions, repository, migration, broker,
  network, credential, endpoint, real order, or automatic trading change appears.
- Any changed path is outside the exact TASK-223 allowlist or an explicitly
  authorized old-gate exact-path self-consistency fix.
