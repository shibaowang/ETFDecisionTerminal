# TASK-182 ShellAccounting Manual Entry DataService Action Validation Wiring Test Plan

## Document Purpose

This plan verifies TASK-182 payload parsing and validation wiring for the
existing manual entry DataService actions. It does not authorize repositories,
SQLite writes, `trade_log`, cash facts, audit, ledger, replay, UI, broker,
network, credentials, endpoints, real order placement, or automatic trading.

## Test Matrix

### A. Validation wiring

- `DataServiceActions.cpp` includes the TASK-178 validation header.
- `accounting.manual_transaction.create` parses structured payload fields into
  `ShellAccountingManualTransactionEntry`.
- `accounting.manual_cash_movement.create` parses structured payload fields into
  `ShellAccountingManualCashMovementEntry`.
- The handlers call `validateManualTransactionEntry` and
  `validateManualCashMovement`.
- The action names and dispatcher registration remain unchanged.

### B. Manual transaction validation

- Valid `Buy` payload passes validation but returns write-not-implemented.
- Valid `Sell` payload passes validation but returns write-not-implemented.
- Missing account, portfolio, security code, occurred time, or side returns
  issue codes.
- Invalid side returns `TRADE_SIDE_INVALID`.
- Zero or negative quantity, price, gross amount, fee, or tax returns issue
  codes.
- Sensitive memo tokens return `SENSITIVE_MEMO_TOKEN`.

### C. Manual cash movement validation

- Valid `Deposit` payload passes validation but returns write-not-implemented.
- Valid `Withdrawal` payload passes validation but returns
  write-not-implemented.
- Missing account, portfolio, movement type, occurred time, or amount returns
  issue codes.
- Invalid movement type returns `CASH_MOVEMENT_TYPE_INVALID`.
- Zero or negative amount returns `CASH_AMOUNT_POSITIVE_REQUIRED`.
- Sensitive memo tokens return `SENSITIVE_MEMO_TOKEN`.

### D. Validation-only response contract

- All responses contain `validationOnly=true`.
- Invalid responses contain `validationAccepted=false` and a stable `issues`
  array with `code`, `field`, and `message`.
- Valid payloads contain `validationAccepted=true`,
  `writeImplemented=false`, and
  `status=VALIDATION_ACCEPTED_WRITE_NOT_IMPLEMENTED`.
- Valid payloads still fail closed and do not return persistence success.
- Responses do not return persistent ids, trade log ids, cash fact ids, or
  database ids.

### E. No persistence or runtime side effects

- No SQLite write is added.
- No DataAccess write repository is added or called.
- No `trade_log` write is added.
- No cash facts or cash ledger write is added.
- No audit or ledger write is added.
- No AccountingEngine replay is called.
- No schema migration changes are made.

### F. No UI / broker / strategy expansion

- Production QML and startup remain unchanged.
- Presenter, Controller, ShellServices, and ShellCore do not gain manual entry
  UI binding.
- No TradeDraft or trading suggestion is generated.
- No broker SDK, network, endpoint, credential, real order placement, or
  automatic trading path is added.
- Existing broker gates remain present and unweakened.

## Required Probes

- Static source scan.
- Dispatcher response probe.
- Manual transaction valid / invalid payload probes.
- Manual cash movement valid / invalid payload probes.
- Issue JSON structure probe.
- Validation-only fail-closed probe.
- No repository scan.
- No write SQL scan.
- No migration scan.
- No QML / startup scan.
- No replay scan.
- No TradeDraft / suggestion scan.
- No broker SDK scan.
- No network / endpoint / credential scan.
- No persistent id scan.
- Broker gate retention scan.

## Go / No-Go Checklist

- [ ] docs/162 merged.
- [ ] docs/163 merged.
- [ ] README, docs/README, and docs/12 register TASK-182.
- [ ] `DataServiceActions.cpp` wires payload parsing to TASK-178 validators.
- [ ] `DataServiceActions.h` remains unchanged.
- [ ] `DataServiceActionRegistrar.cpp` remains unchanged.
- [ ] Valid manual transaction payload returns validation accepted but write not
  implemented.
- [ ] Valid manual cash movement payload returns validation accepted but write
  not implemented.
- [ ] Invalid payloads return stable issues.
- [ ] No persistence success or persistent ids are returned.
- [ ] No DataAccess write repository exists.
- [ ] No database, `trade_log`, cash facts, audit, or ledger writes exist.
- [ ] No schema migration is changed or added.
- [ ] No production QML or startup change exists.
- [ ] No broker SDK, network, credentials, endpoint, real order, or automatic
  trading exists.
- [ ] TASK-182 validation wiring tests pass.
- [ ] TASK-181, TASK-180, TASK-179, and TASK-178 regression gates pass.
- [ ] Full CTest passes.
- [ ] `transport_local_socket_echo` 50 repeat passes.
- [ ] `git diff --check` passes.
- [ ] `git status --short` is clean.

TASK-183 adds the future manual entry persistence authorization gate. TASK-182
validation wiring remains validation-only after that gate: valid payloads still
return `writeImplemented=false`, no persistent ids, and no database, `trade_log`,
cash facts, audit, or ledger writes.
