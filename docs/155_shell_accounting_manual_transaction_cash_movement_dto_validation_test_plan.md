# TASK-178 ShellAccounting Manual Transaction and Cash Movement DTO Validation Test Plan

## Document Purpose

This test plan defines verification for the TASK-178 DTO / validation scaffold.
It proves that the scaffold accepts valid manual buy/sell and deposit/withdraw
inputs, rejects invalid local fields, and remains disconnected from DataService
actions, DataAccess repositories, SQLite writes, AccountingEngine replay,
production QML, broker SDKs, network calls, credentials, real order placement,
and automatic trading.

## Test Matrix

### A. Documentation and Registration

- docs/154 exists and documents TASK-178.
- docs/155 exists and contains this test matrix.
- README, docs/README, and docs/12 register TASK-178.
- docs/152 and docs/153 state that TASK-178 adds DTO / validation scaffold only.

### B. DTO / Validation Presence

- `ShellAccountingManualTransactionCashMovementValidation.h` exists.
- `ShellAccountingManualTransactionCashMovementValidation.cpp` exists.
- DataServiceApi CMake includes the validation source.
- The scaffold declares manual trade side and cash movement type enums.
- The scaffold declares transaction DTO, cash movement DTO, issue, and result
  structs.
- Validation functions are available for manual transaction and cash movement.

### C. Positive Validation

- Buy DTO validation accepts minimal valid input.
- Sell DTO validation accepts minimal valid input.
- Deposit DTO validation accepts minimal valid input.
- Withdrawal DTO validation accepts minimal valid input.

### D. Negative Validation

- Invalid trade side is rejected.
- Invalid cash movement type is rejected.
- `quantityUnits <= 0` is rejected.
- `priceAmountMinor <= 0` is rejected.
- `grossAmountMinor <= 0` is rejected.
- `feeAmountMinor < 0` is rejected.
- `taxAmountMinor < 0` is rejected.
- `amountMinor <= 0` is rejected.
- Missing `accountId`, `portfolioId`, or `securityCode` is rejected.
- Missing `occurredAt` is rejected.
- Sensitive memo tokens are rejected.

### E. Forbidden Boundary

- Validation does not access SQLite.
- Validation does not write `trade_log`.
- Validation does not write cash facts.
- Validation does not write audit / ledger.
- Validation does not create TradeDraft.
- Validation does not call broker code.
- Validation does not access network or endpoints.
- Validation does not read credentials, secrets, tokens, keys, passwords, or
  usable examples.
- `DataServiceActions.cpp` is not modified.
- `migrations/001_initial_schema.sql` is not modified.
- Production QML and production startup are not modified.
- DataAccess production write repository is not added.
- AccountingEngine replay is not modified.
- StrategyEngine and MarketEngine are not modified.
- Existing broker / real broker / no-network / no-credentials /
  no-order-placement gates are not deleted, weakened, or skipped.

## Required Probes

- Direct unit validation probes.
- Header/source existence scan.
- CMake source registration scan.
- Forbidden token scan.
- Production QML scan.
- Startup scan.
- DataServiceActions scan.
- DataAccess repository scan.
- AccountingEngine replay scan.
- StrategyEngine and MarketEngine scan.
- Migration unchanged and no-new-migration scan.
- Broker SDK scan.
- Network and endpoint scan.
- Credential / secret scan.
- Real order placement scan.
- Automatic trading scan.
- Existing broker gate retention scan.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/154 merged.
- [ ] docs/155 merged.
- [ ] DTO / validation scaffold compiles.
- [ ] TASK-178 validation scaffold tests pass.
- [ ] TASK-177 authorization gate passes.
- [ ] Broker disabled, broker order, and real broker gates pass.
- [ ] Full CTest passes.
- [ ] `transport_local_socket_echo` 50 repeat passes.
- [ ] `git diff --check` passes.
- [ ] `git status --short` is clean.
