# EPIC-278 TradeDraft Test Plan

Primary CTest:

```powershell
ctest --test-dir build -R tradedraft_manual_recommendation_flow_full_delivery --output-on-failure
```

The CTest verifies:

- Existing TradeDraft schema is present.
- Existing TradeDraft repository is used.
- StrategyEngine converts accepted BUY / SELL recommendations into TradeDraft requests.
- DataService registers `accounting.tradedraft.create_from_recommendation`.
- DataService registers `accounting.tradedraft.readonly_summary`.
- DataServiceClient typed adapters call the local service host.
- Positive samples create `trade_draft`, `trade_draft_leg`, and `audit_log` rows.
- Negative samples fail closed without row growth.
- Duplicate same key / same digest produces no duplicate row growth.
- Same key / different digest returns sanitized idempotency conflict.
- Read-only summary returns the created TradeDraft summary without writes.
- Presenter preview requires explicit confirmation.
- Presenter create fails closed without confirmation and succeeds after confirmation.

Regression commands:

```powershell
ctest --test-dir build -R strategy_recommendation_engine_vba_parity_full_delivery --output-on-failure
ctest --test-dir build -R shell_accounting_portfolio_replay_vba_parity_full_delivery --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_real_workbook_trial_hardening --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_export_helper_compatibility --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_demo_workspace --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_release_readiness --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

Boundary checks:

- TradeDraft is not an order.
- No `trade_log` or `cash_adjustment` rows are written by TradeDraft creation.
- No broker / network / credential / endpoint code path is used.
- No real order placement or automatic trading is enabled.
- No production DB is touched; tests use temporary SQLite only.
