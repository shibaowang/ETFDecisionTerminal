# EPIC-280 Market Data Test Plan

Primary CTest:

```powershell
ctest --test-dir build -R market_data_historical_high_refresh_engine_full_delivery --output-on-failure
```
The test verifies:

- 12 positive market data samples.
- 15 negative or safety-policy samples.
- ETF historical high, index historical high, and OTC NAV quote.
- ETF and index drawdown from high.
- Positive and negative premium/discount.
- Stale quote and partial quote/history behavior.
- Provider disabled and live-provider-disabled fail-closed behavior.
- Exact public host/path allowlist.
- Batch-only, per-host rate limit, daily historical high cache, 10-minute
  history failure circuit breaker, and no parallel same-host request policy.
- DataService read-only action registration.
- Real DataServiceClient mapping through a local service host.
- ShellServices / Presenter market data mapping.
- Strategy recommendation input consumption of read-only market fields.
- `trade_log`, `cash_adjustment`, and `audit_log` remain unchanged.
- No broker, network, credential, endpoint, real order, or automatic trading.
- No default live auto-refresh and no 2-second polling loop.

Regression commands:

```powershell
ctest --test-dir build -R otcmap_ac_multichannel_draft_engine_full_delivery --output-on-failure
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
