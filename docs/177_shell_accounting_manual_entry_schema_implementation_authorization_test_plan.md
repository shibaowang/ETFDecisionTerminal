# ShellAccounting Manual Entry Schema Implementation Authorization Test Plan

## Document Purpose

TASK-189 defines the test plan for a future manual entry schema implementation
authorization boundary. This document does not implement schema changes,
migrations, schema files, repository writes, SQL execution, SQLite writes,
broker integration, network calls, credentials, endpoints, real order
placement, or automatic trading.

## Test Matrix

A. Documentation and indexing

- docs/176 exists.
- docs/177 exists.
- README links docs/176 and docs/177.
- docs/README links docs/176 and docs/177.
- docs/12 registers TASK-189.
- docs/176 states schema implementation authorization-only.
- docs/176 states `migrations/001_initial_schema.sql` remains source of truth.

B. Schema implementation authorization boundary

- TASK-189 does not modify `migrations/001_initial_schema.sql`.
- TASK-189 does not add a migration.
- TASK-189 does not add a schema file.
- TASK-189 does not add tables, columns, indexes, or constraints.
- Future schema implementation must add an independent migration file.
- Future schema implementation must not directly edit
  `migrations/001_initial_schema.sql`.
- Future schema implementation must declare migration id / rollback /
  compatibility policy.
- Future schema implementation must be separately authorized.

C. Manual transaction candidate categories

- docs/176 lists manual transaction trace / idempotency fields.
- docs/176 lists `request_id`.
- docs/176 lists `idempotency_key`.
- docs/176 lists occurredAt / `occurred_at_utc`.
- docs/176 lists tax amount support / `tax_cents`.
- docs/176 lists duplicate handling constraints.

D. Manual cash movement candidate categories

- docs/176 lists manual cash movement trace / idempotency fields.
- docs/176 lists `request_id`.
- docs/176 lists `idempotency_key`.
- docs/176 lists occurredAt / `occurred_at_utc`.
- docs/176 lists movement type mapping support.
- docs/176 lists cash adjustment / trade_log linkage support.

E. Audit / rollback / privacy candidate categories

- docs/176 lists audit trace.
- docs/176 lists payload classification.
- docs/176 lists redaction support.
- docs/176 lists failure atomicity.
- docs/176 lists rollback policy support.
- docs/176 lists sanitized memo / source reference support.

F. Production boundary

- DataServiceActions.cpp is not modified by TASK-189.
- DataServiceActions.h is not modified by TASK-189.
- DataServiceActionRegistrar.cpp is not modified by TASK-189.
- TASK-178 validation production code is not modified by TASK-189.
- TASK-185 repository scaffold header/source are not modified by TASK-189.
- TASK-185 scaffold remains disabled and write-not-implemented.
- TASK-182 validation wiring remains validation-only with `writeImplemented=false`.
- No repository implementation is added.
- No SQL / SQLite write is added.
- No `trade_log`, cash facts, audit, or ledger write is added.
- Production QML / startup are not modified.
- Presenter / Controller are not modified.
- AccountingEngine replay is not modified.
- StrategyEngine / MarketEngine are not modified.

G. Trading and broker safety

- No TradeDraft / suggestion implementation is added.
- No broker SDK is added.
- No network call or endpoint is added.
- No credentials, secrets, tokens, keys, or passwords are added.
- No real broker order id is generated or stored.
- No real order placement is added.
- No automatic trading is added.
- Existing broker / real broker / no-network / no-credentials /
  no-order-placement gates are not deleted, weakened, or skipped.

H. Regression

- TASK-188 schema gap authorization tests pass.
- TASK-187 schema adequacy review tests pass.
- TASK-186 repository implementation authorization tests pass.
- TASK-185 repository scaffold tests pass.
- TASK-182 validation wiring tests pass.
- TASK-178 validation scaffold tests pass.
- TASK-177 MVP authorization gate tests pass.
- broker disabled / broker order / real broker gates pass.
- full CTest passes.
- transport 50 repeat passes.

## Required Probes

- static source scan.
- documentation keyword scan.
- docs index and README link scan.
- prompt template registration scan.
- migration directory scan.
- forbidden schema file scan.
- forbidden production source marker scan.
- forbidden repository implementation scan.
- forbidden SQL token scan.
- forbidden SQLite write token scan.
- forbidden `trade_log` / cash facts / audit / ledger write scan.
- forbidden QML / startup / Presenter / Controller drift scan.
- forbidden replay / strategy / market drift scan.
- forbidden TradeDraft / suggestion scan.
- forbidden broker SDK / network / endpoint scan.
- forbidden credentials / secrets / token / key / password scan.
- forbidden real broker order id and order placement scan.
- forbidden automatic trading scan.
- rollback documentation probe.
- broker gates retained probe.

## Go / No-Go Checklist

- [ ] docs/176 merged.
- [ ] docs/177 merged.
- [ ] README, docs/README, and docs/12 register TASK-189.
- [ ] TASK-189 schema implementation authorization gate tests pass.
- [ ] TASK-188 schema gap authorization gate tests pass.
- [ ] TASK-187 schema adequacy review tests pass.
- [ ] TASK-186 repository implementation authorization tests pass.
- [ ] TASK-185 repository scaffold tests pass.
- [ ] TASK-182 validation wiring tests pass.
- [ ] TASK-178 validation scaffold tests pass.
- [ ] TASK-177 gate tests pass.
- [ ] broker disabled / broker order / real broker gates pass.
- [ ] full CTest passes.
- [ ] transport 50 repeat passes.
- [ ] `git diff --check` passes.
- [ ] `git status --short` is clean after commit.
