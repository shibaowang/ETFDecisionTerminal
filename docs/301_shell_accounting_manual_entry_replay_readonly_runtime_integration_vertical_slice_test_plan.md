# TASK-252 ShellAccounting Manual Entry Replay Read-Only Runtime Integration Vertical Slice Test Plan

## Test Objective

This plan verifies that TASK-252 creates a minimum test-only read-only vertical slice from synthetic manual entry facts to AccountingEngine read-only replay and an in-memory projection summary.

The plan also verifies that the slice does not become production runtime integration and does not write production data.

## Required CTest

The required CTest is:

`shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice`

The CTest must execute a test-only helper under `tests/`, call AccountingEngine read-only replay through a test-only adapter, and assert deterministic JSON evidence.

## Runtime Evidence Checks

The CTest must verify:

- `task=TASK-252`;
- `mode=test-only-readonly-runtime-integration-vertical-slice`;
- `runtimeIntegrationPath=test-only`;
- `replayExecuted=true`;
- `accountingEngineCalled=true`;
- `positionSummaryAvailable=true`;
- `cashSummaryAvailable=true`;
- `pnlSummaryAvailable=true`;
- in-memory summary only;
- stdout JSON evidence only.

## No-Write Checks

The CTest must verify:

- `productionWrite=false`;
- `sqliteProductionWrite=false`;
- `auditWritten=false`;
- `ledgerWritten=false`;
- `snapshotWritten=false`;
- `tradeLogWritten=false`;
- `readModelPersistentWrite=false`;
- no runtime SQL / SQLite write;
- no persistent read model refresh.

## No Trading Checks

The CTest must verify:

- `tradeDraftGenerated=false`;
- `tradeSuggestionGenerated=false`;
- `strategyExecuted=false`;
- `brokerOrderSubmitted=false`;
- `networkAccess=false`;
- `credentialAccess=false`;
- `endpointAccess=false`;
- `automaticTrading=false`.

## Production Boundary Checks

The CTest must verify that TASK-252 changed paths do not include:

- `apps/`;
- `libs/DataServiceApi/`;
- `libs/DataAccess/`;
- `libs/AccountingEngine/`;
- `migrations/`;
- production QML;
- startup;
- Presenter / Controller production wiring.

The CTest must also verify that QML, Presenter, Controller, and startup do not directly call AccountingEngine.

## Failure-Mode Checks

The CTest must cover:

- missing input;
- malformed input;
- unsupported scenario;
- replay issue returned;
- sanitized error output;
- no write on failure.

Each failure mode must remain sanitized and must not expose raw SQL, raw trade log payload, raw replay payload, internal stack traces, credentials, endpoints, broker payload, or real order identifiers.

## Changed-Path Allowlist

Allowed TASK-252 changed paths are:

- `README.md`;
- `docs/README.md`;
- `docs/12_codex_prompt_template.md`;
- `docs/300_shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.md`;
- `docs/301_shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice_test_plan.md`;
- `tests/CMakeLists.txt`;
- `tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/CMakeLists.txt`;
- `tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.py`;
- `tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice.cpp`;

No directory-wide allowlist is authorized.

## Regression Requirements

Before commit, run:

- CMake configure;
- CMake build;
- `ctest -N -R shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice`;
- `ctest -R shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice --output-on-failure`;
- targeted replay / bridge / AccountingEngine regression gates;
- full CTest;
- `transport_local_socket_echo` repeated 50 times;
- `git diff --check`;
- `git status --short`.

## Formal Conclusion

TASK-252 is a test-only read-only runtime integration vertical slice.

TASK-252 does not implement production runtime integration.

TASK-252 does not authorize production write.

TASK-252 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

## Recommended Next Task

The recommended next task is TASK-253 ShellAccounting manual entry replay fixture-backed VBA parity read-only vertical slice.
