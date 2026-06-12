# Dashboard MVP Test Plan

Run from the repository root:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build

ctest --test-dir build -R dashboard_mvp_full_delivery --output-on-failure
ctest --test-dir build -R market_data_historical_high_refresh_engine_full_delivery --output-on-failure
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

The Dashboard CTest verifies QML object names, forbidden-token boundaries,
Presenter method exposure, docs indexing, sanitized samples, explicit
confirmation controls, and no production DB, network, broker, real order, or
automatic trading behavior.
