# ShellAccounting Manual Entry Replay Fixture Scaffold Static Validator Authorization Test Plan

## Document Purpose

This test plan defines TASK-218 static probes for the ShellAccounting manual
entry replay fixture scaffold static validator authorization gate.

TASK-218 is gate-only / static-validator-authorization-only. It does not
implement parser, validator, loader, reader, replay, AccountingEngine replay
calls, runtime SQL / SQLite read/write, production code changes, fixture JSON
changes, audit / ledger / snapshot writes, backup/export/restore, broker,
network, credentials, endpoint, real order placement, or automatic trading.

## Test Matrix

- docs/232 exists.
- docs/233 exists.
- Documentation registration is present in README.md.
- Documentation registration is present in docs/README.md.
- Documentation registration is present in docs/12_codex_prompt_template.md.
- TASK-218 CTest is registered.
- Gate-only scope is documented.
- Static validator authorization purpose is documented.
- Future validator input boundary is documented.
- Future validator output boundary is documented.
- Future validator check matrix is documented.
- Fixture index validation rules are documented.
- Fixture JSON validation rules are documented.
- Privacy and forbidden token rules are documented.
- No-replay boundary is documented.
- No-runtime-dependency boundary is documented.
- Out-of-scope boundaries are documented.
- Formal conclusion is documented.
- No parser / validator creation is authorized.
- No loader / reader creation is authorized.
- No replay execution is authorized.
- No AccountingEngine replay call is authorized.
- No fixture JSON modification is authorized.
- No runtime SQL / SQLite read/write is authorized.
- Production drift is not authorized.
- No production drift is authorized.
- No audit / ledger / snapshot write is authorized.
- No backup/export/restore is authorized.
- No broker / network / credentials / endpoint is authorized.
- No real order placement is authorized.
- No automatic trading is authorized.
- TASK-217 regression is retained.
- TASK-216 regression is retained.
- TASK-215 regression is retained.
- TASK-214 regression is retained.
- TASK-213 regression is retained.
- TASK-212 regression is retained.
- Broker / real broker gates are retained.
- transport_local_socket_echo 50-repeat regression is retained.

## Required Probes

- Static source scan.
- Documentation token scan.
- Changed-path allowlist scan.
- Untracked-file detection scan.
- CTest registration scan.
- docs/232 section scan.
- docs/233 Test Matrix scan.
- Fixture index rule documentation scan.
- Fixture JSON rule documentation scan.
- Privacy and forbidden token documentation scan.
- No parser / validator / loader / reader implementation scan.
- No replay implementation scan.
- No AccountingEngine replay call scan.
- No runtime SQL / SQLite read/write scan.
- No fixture JSON content modification scan.
- No production source drift scan.
- No QML / startup / Presenter / Controller / ShellServices drift scan.
- No DataServiceActions / DataServiceActionRegistrar drift scan.
- No repository / migration drift scan.
- No audit / ledger / snapshot write scan.
- No backup/export/restore implementation scan.
- No broker / network / credentials / endpoint scan.
- No real order id / broker order id scan.
- No automatic trading scan.
- TASK-217 gate retention scan.
- TASK-216 gate retention scan.
- TASK-215 gate retention scan.
- TASK-214 gate retention scan.
- TASK-213 gate retention scan.
- TASK-212 gate retention scan.
- Broker and real broker gate retention scan.
- transport_local_socket_echo registration scan.

## Go / No-Go Checklist

Go only if:

- docs/232 and docs/233 exist and are indexed by README.md, docs/README.md, and docs/12_codex_prompt_template.md.
- TASK-218 CTest is registered.
- docs/232 covers future validator input, output, check matrix, fixture index rules, fixture JSON rules, privacy, no-replay, no-runtime-dependency, out-of-scope boundaries, and formal conclusion.
- docs/232 explicitly states TASK-218 authorizes validator policy only.
- docs/232 explicitly states TASK-218 does not authorize parser / validator implementation.
- docs/232 explicitly states TASK-218 does not authorize loader / reader implementation.
- docs/232 explicitly states TASK-218 does not authorize replay implementation.
- docs/232 explicitly states TASK-218 does not authorize AccountingEngine replay calls.
- docs/232 explicitly states TASK-218 does not authorize runtime SQL / SQLite read/write.
- docs/232 explicitly states TASK-218 does not authorize audit / ledger / snapshot writes.
- docs/232 explicitly states TASK-218 does not authorize backup/export/restore.
- docs/232 explicitly states TASK-218 does not authorize broker / network / credentials / endpoint / real order placement / automatic trading.
- No parser / validator / loader / reader is added.
- No replay execution is added.
- No AccountingEngine replay call is added.
- No tests/fixtures/manual_entry_replay/*.json file is modified.
- Production code, QML, Presenter, Controller, ShellServices, DataServiceActions, repositories, and migrations are unchanged.
- TASK-217, TASK-216, TASK-215, TASK-214, TASK-213, and TASK-212 gates are retained.
- Broker / real broker gates are retained.

No-Go if:

- A parser / validator / loader / reader implementation appears.
- A replay implementation appears.
- An AccountingEngine replay call appears.
- Fixture JSON content changes.
- Runtime SQL / SQLite read/write appears.
- DataServiceActions runtime behavior changes.
- DataServiceActionRegistrar changes.
- Repository or migration changes appear.
- QML / Presenter / Controller / ShellServices production drift appears.
- Audit / ledger / snapshot writes appear.
- Backup/export/restore implementation appears.
- Broker / network / credentials / endpoint integration appears.
- Real order placement or broker order id generation appears.
- Automatic trading appears.
