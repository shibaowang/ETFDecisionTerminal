# ShellAccounting Manual Entry Replay Negative Fixture Static Validator Implementation Test Plan

## Document Purpose

This test plan defines the TASK-224 validation for a test-only manual entry
replay negative fixture static validator. It proves that the implementation is
limited to static CTest inspection of TASK-222 negative fixture scaffold JSON
and does not create production validator, parser, loader, reader, replay,
AccountingEngine replay, runtime database access, fixture mutation, production
code, or broker capability.

## Test Matrix

- docs/244 exists and documents TASK-224 scope.
- docs/245 exists and documents this test plan.
- README, docs/README, and docs/12 reference docs/244 and docs/245.
- CTest `shell_accounting_manual_entry_replay_negative_fixture_static_validator`
  is registered.
- Validator reads only negative fixture index, `NEG_MRF*.json`, and positive
  fixture index.
- Negative fixture directory exists.
- Exact negative fixture file set is enforced.
- All negative JSON files are valid JSON objects.
- Negative index references exactly ten `NEG_MRF` files.
- `negativeFixtureId` values are unique.
- File name and `negativeFixtureId` are aligned.
- Schema version, scaffold flags, severity, blocking, and synthetic-only flags
  are enforced.
- `sourcePositiveFixtureId` exists in the positive fixture index.
- Expected issue code mapping is enforced.
- Sanitized issue summary includes `negativeFixtureId`, `file`, `issueCode`,
  `severity`, `blocking`, `rule`, and `message`.
- No raw fixture payload, raw SQL, credentials, endpoint, broker payload, real
  order id, or stack trace is printed.
- No fixture JSON changes.
- No TASK-219 positive fixture static validator changes.
- No production code, QML, DataServiceActions, repositories, migrations, replay,
  AccountingEngine replay, broker, network, credentials, endpoint, real order,
  or automatic trading changes.

## Required Probes

- Documentation existence probe.
- Documentation index probe.
- CTest registration probe.
- Static validator execution probe.
- Negative directory exact-file-set probe.
- Negative index schema and reference probe.
- Ten `NEG_MRF` JSON schema probes.
- Positive fixture index reference probe.
- Expected issue code mapping probe.
- Privacy and sanitized placeholder probe.
- No DB / WAL / SHM / SQLite / SQL artifact probe.
- No production path reference probe.
- No raw payload / stack trace output probe.
- Changed-path exact allowlist probe.
- Clean-main empty changed-set support probe.
- TASK-223 authorization gate regression.
- TASK-222 scaffold files gate regression.
- TASK-221 / TASK-220 / TASK-219 / TASK-218 gate regressions.
- shell_diagnostics_facade regression.
- broker and real broker gate regressions.
- transport_local_socket_echo 50-repeat regression.
- Full CTest regression.
- git diff whitespace probe.

## Go / No-Go Checklist

Go only if:

- docs/244 and docs/245 exist and are indexed.
- The TASK-224 CTest exists and passes.
- The validator remains under
  `tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/`.
- The validator reads only authorized fixture indexes and negative fixture JSON.
- The validator returns sanitized summary output and no raw payload.
- Negative fixture JSON is unchanged.
- Positive fixture JSON is unchanged.
- TASK-219 positive fixture static validator is unchanged.
- TASK-223 authorization gate still passes.
- TASK-222 through TASK-218 fixture gates still pass.
- Full CTest passes.
- `git diff --check` passes.

No-Go if:

- Any production validator, parser, loader, reader, replay, or AccountingEngine
  replay path appears.
- Any negative or positive fixture JSON changes.
- TASK-219 positive fixture static validator changes.
- Any runtime SQL / SQLite / audit / ledger / snapshot write appears.
- Any production code, QML, DataServiceActions, repository, migration, broker,
  network, credential, endpoint, real order, or automatic trading change
  appears.
- Any `ctest -R` pattern matches zero tests.
