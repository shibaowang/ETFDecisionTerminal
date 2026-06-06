# TASK-257 ShellAccounting Manual Entry Replay Excel/VBA Import Read-Only Production Parser Boundary Test Plan

## Purpose

This test plan verifies the TASK-257 read-only production parser boundary slice.
The test must prove that sanitized Excel/VBA-style import payloads pass through
the production parser boundary, produce normalized diagnostics or replay fact
DTOs, and are then replayed read-only from the TASK-257 test helper for
projection parity.

## Test Matrix

- Parser boundary existence: the production read-only parser header and source
  exist and are linked into DataServiceApi.
- Read-only parser contract: parser result reports read-only, no SQLite access,
  no write behavior, and no production runtime integration.
- Sheet detection: parser recognizes initial cash and TradeLog-style sheets.
- Header detection: parser recognizes stable headers and Chinese header names.
- Buy-only import: parser builds replay facts and the test helper matches the
  expected sanitized projection or issue summary.
- Chinese header buy-partial-sell import: parser builds replay facts and the
  test helper matches expected cash, position, and PnL projection.
- Cash adjustment import: parser builds replay facts and the test helper matches
  expected sanitized unsupported replay issue behavior.
- Missing required header: parser rejects the sample before replay and emits a
  sanitized `MISSING_REQUIRED_HEADER` diagnostic.
- Invalid import fields: parser rejects invalid action, non-numeric amount, and
  missing cash field with sanitized diagnostics.
- No writes: no SQLite, audit, ledger, snapshot, trade_log, read model, or
  TradeDraft write occurs.
- No trading: no broker order, network, credential, endpoint, real order, or
  automatic trading behavior occurs.
- Production boundary: apps, migrations, production QML, startup, Presenter,
  Controller, AccountingEngine production code, DataService action
  registration, DataAccess repositories, and historical fixtures remain
  untouched.

## Required CTest

Run:

```powershell
ctest --test-dir build -R shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary --output-on-failure
```

The CTest must pass and emit TASK-257 evidence with
`productionParserBoundaryCreated=true`, `parserReadOnly=true`,
`parserNoSqliteAccess=true`, `parserNoWrite=true`, `sampleMatrixLoaded=true`,
`sampleCount>=5`, `parserExecuted=true`, `diagnosticsBuilt=true`,
`replayFactsBuilt=true`, `accountingEngineCalledFromTestOnly=true`,
`replayExecutedFromTestOnly=true`, `validSamplesParityPassed=true`,
`invalidSamplesRejected=true`, `diagnosticsSanitized=true`,
`rawUserDataExposed=false`, and
`productionRuntimeIntegrationImplemented=false`.

## Required Regression Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Static Review Probes

- Parser source must not contain `INSERT`, `UPDATE`, `DELETE`, `REPLACE`,
  `CREATE TABLE`, or SQLite write calls.
- Parser source must not include broker, network, credential, endpoint, strategy,
  TradeDraft, audit, ledger, snapshot, or read model write behavior.
- Parser source must not call AccountingEngine. The TASK-257 test helper is the
  only place allowed to call AccountingEngine read-only replay.
- TASK-257 fixtures must be local to the TASK-257 CTest directory.
- Historical fixture JSON must remain unchanged.

## Go / No-Go Checklist

- GO only if the targeted TASK-257 CTest passes.
- GO only if full CTest passes.
- GO only if transport repeat 50 passes.
- GO only if `git diff --check` passes.
- GO only if `git status --short` is clean after commit.
- NO-GO if apps, migrations, production QML, production startup, production
  Presenter / Controller, AccountingEngine production code, DataService action
  registration, DataAccess repositories, historical fixtures, broker/network
  code, credentials, endpoints, real order placement, or automatic trading are
  modified.
