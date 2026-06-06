# TASK-255 ShellAccounting Manual Entry Replay Excel/VBA Export Sample Import Test Plan

## Purpose

This test plan verifies the TASK-255 test-only Excel/VBA export sample import
read-only vertical slice. The test must prove that a sanitized sample can be
loaded, parsed into replay fact DTOs, replayed read-only through AccountingEngine,
matched against expected projection output, and reported through sanitized stdout
JSON evidence.

## Test Matrix

- Sample loading: TASK-255-only sanitized sample exists and is loaded.
- Importer execution: test-only importer reads sheet headers and rows.
- Fact DTO build: imported rows build cash and trade replay fact DTOs.
- Replay execution: AccountingEngine read-only replay is called.
- Parity: cash, position, PnL, and issue output match expected projection.
- Sanitization: evidence does not expose raw user data.
- No writes: no SQLite, audit, ledger, snapshot, trade_log, or read model writes.
- No trading: no broker order, network, credential, endpoint, real order, or
  automatic trading behavior.
- Production boundary: apps, libs, migrations, production QML, startup,
  Presenter, Controller, AccountingEngine production code, and historical
  fixtures remain untouched.

## Required CTest

Run:

```powershell
ctest --test-dir build -R shell_accounting_manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice --output-on-failure
```

The CTest must pass and emit TASK-255 evidence with `parityPassed=true`,
`sampleLoaded=true`, `importerExecuted=true`, `replayFactsBuilt=true`,
`replayExecuted=true`, `accountingEngineCalled=true`, and
`rawUserDataExposed=false`.

## Required Regression Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Go / No-Go Checklist

- GO only if the targeted TASK-255 CTest passes.
- GO only if full CTest passes.
- GO only if transport repeat 50 passes.
- GO only if `git diff --check` passes.
- GO only if `git status --short` is clean after commit.
- NO-GO if production code, migrations, historical fixtures, broker/network
  code, credentials, endpoints, real order placement, or automatic trading are
  modified.
