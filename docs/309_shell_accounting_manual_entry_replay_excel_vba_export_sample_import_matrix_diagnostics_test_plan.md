# TASK-256 ShellAccounting Manual Entry Replay Excel/VBA Export Sample Import Matrix Diagnostics Test Plan

## Purpose

This test plan verifies the TASK-256 test-only Excel/VBA export sample import
matrix diagnostics read-only vertical slice. The test must prove that sanitized
samples are loaded, parsed, diagnosed, converted to replay fact DTOs when valid,
replayed read-only through AccountingEngine, and compared against expected
projection or diagnostic output.

## Test Matrix

- Sample matrix loading: at least five TASK-256-only samples load.
- Buy-only import: valid sample imports and matches expected cash / position /
  PnL projection.
- Buy-partial-sell import: valid sample imports and matches expected realized
  PnL and remaining position projection.
- Cash adjustment import: valid sample imports and matches expected sanitized
  unsupported replay issue output.
- Missing required header: invalid sample is rejected before replay and emits a
  sanitized `MISSING_REQUIRED_HEADER` diagnostic.
- Unsupported / issue sample: accepted sample replays and matches sanitized
  `SELL_EXCEEDS_POSITION` issue output.
- No writes: no SQLite, audit, ledger, snapshot, trade_log, or read model writes.
- No trading: no broker order, network, credential, endpoint, real order, or
  automatic trading behavior.
- Production boundary: apps, libs, migrations, production QML, startup,
  Presenter, Controller, AccountingEngine production code, and historical
  fixtures remain untouched.

## Required CTest

Run:

```powershell
ctest --test-dir build -R shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_readonly_vertical_slice --output-on-failure
```

The CTest must pass and emit TASK-256 evidence with
`sampleMatrixLoaded=true`, `sampleCount>=5`, `importerExecuted=true`,
`diagnosticsBuilt=true`, `replayFactsBuilt=true`, `replayExecuted=true`,
`accountingEngineCalled=true`, `validSamplesParityPassed=true`,
`invalidSamplesRejected=true`, `diagnosticsSanitized=true`, and
`rawUserDataExposed=false`.

## Required Regression Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_readonly_vertical_slice --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Go / No-Go Checklist

- GO only if the targeted TASK-256 CTest passes.
- GO only if full CTest passes.
- GO only if transport repeat 50 passes.
- GO only if `git diff --check` passes.
- GO only if `git status --short` is clean after commit.
- NO-GO if production code, migrations, historical fixtures, broker/network
  code, credentials, endpoints, real order placement, or automatic trading are
  modified.
