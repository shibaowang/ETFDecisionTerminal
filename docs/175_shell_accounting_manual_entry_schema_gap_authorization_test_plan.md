# ShellAccounting Manual Entry Schema Gap Authorization Test Plan

## Document Purpose

TASK-188 defines the test plan for a future manual entry schema gap resolution
authorization boundary. This document does not implement schema changes,
migrations, repository writes, SQL execution, SQLite writes, broker
integration, network calls, credentials, endpoints, real order placement, or
automatic trading.

## Test Matrix

A. Documentation and indexing

- docs/174 exists.
- docs/175 exists.
- README links docs/174 and docs/175.
- docs/README links docs/174 and docs/175.
- docs/12 registers TASK-188.
- docs/174 states schema gap authorization-only.
- docs/174 states `migrations/001_initial_schema.sql` remains source of truth.

B. Schema boundary

- TASK-188 does not modify `migrations/001_initial_schema.sql`.
- TASK-188 does not add a migration.
- TASK-188 does not add a schema file.
- Future schema implementation must add an independent migration file.
- Future schema implementation must not directly edit
  `migrations/001_initial_schema.sql`.
- Future schema implementation must be separately authorized.

C. Manual transaction gaps

- docs/174 lists `request_id`.
- docs/174 lists `idempotency_key`.
- docs/174 lists `occurred_at_utc`.
- docs/174 lists `tax_cents` or equivalent lossless tax mapping.
- docs/174 lists manual transaction duplicate handling.
- docs/174 lists transaction boundary between manual transaction write and
  audit write.
- docs/174 lists memo / sourceMemo sanitization.

D. Manual cash movement gaps

- docs/174 lists `request_id`.
- docs/174 lists `idempotency_key`.
- docs/174 lists `occurred_at_utc`.
- docs/174 lists `movement_type` stable mapping.
- docs/174 lists `cash_adjustment` / `trade_log` transaction linkage.
- docs/174 lists whether all manual cash movements require both `trade_log`
  and `cash_adjustment`.
- docs/174 lists cash movement duplicate handling.
- docs/174 lists cash movement write and audit write transaction boundary.
- docs/174 lists memo / sourceMemo sanitization.

E. Audit / rollback / privacy gaps

- docs/174 lists audit request trace.
- docs/174 lists audit idempotency.
- docs/174 lists payload classification.
- docs/174 lists redaction status.
- docs/174 lists sanitized payload policy.
- docs/174 lists failure atomicity.
- docs/174 lists partial write rollback.
- docs/174 lists sanitized memo and source reference policy.

F. Production boundary

- DataServiceActions.cpp is not modified by TASK-188.
- DataServiceActions.h is not modified by TASK-188.
- DataServiceActionRegistrar.cpp is not modified by TASK-188.
- TASK-178 validation production code is not modified by TASK-188.
- TASK-185 repository scaffold header/source are not modified by TASK-188.
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

- [ ] docs/174 merged.
- [ ] docs/175 merged.
- [ ] README, docs/README, and docs/12 register TASK-188.
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
