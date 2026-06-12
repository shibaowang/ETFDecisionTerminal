# EPIC-279 OTCMap A/C Multi-Channel Test Plan

Primary CTest:

```powershell
ctest --test-dir build -R otcmap_ac_multichannel_draft_engine_full_delivery --output-on-failure
```

The test verifies:

- Existing schema supports `trade_draft` and `trade_draft_leg`.
- All 12 positive samples produce exact engine legs.
- All 15 negative samples fail closed with sanitized issue codes.
- BUY split by channel limit, cash cap, disabled channel skip, and below-minimum
  skip.
- SELL C-class-first, C then A split, base protection pass/block, and daily
  limit partial sell.
- Decimal NAV quantity output.
- DataService read-only preview action and write action registration.
- DataServiceClient typed preview/create mapping.
- ShellServices and Presenter preview/create mapping.
- Multi-leg TradeDraft create and readback through a local service host with a
  temporary SQLite DB.
- Duplicate and idempotency conflict create no row growth.
- `trade_log` and `cash_adjustment` remain unchanged.
- No broker, network, credential, endpoint, real order, or automatic trading
  path is used.

Regression commands:

```powershell
ctest --test-dir build -R tradedraft_manual_recommendation_flow_full_delivery --output-on-failure
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

