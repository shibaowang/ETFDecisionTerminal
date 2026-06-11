# ShellAccounting Portfolio Replay VBA Parity Test Plan

EPIC-276 is verified by:

```text
shell_accounting_portfolio_replay_vba_parity_full_delivery
```

## Coverage

- Verifies docs 359 through 363 are present and document the EPIC-276
  read-only broker boundary.
- Verifies ten positive sample fixtures under
  `samples/shell_accounting_replay/`.
- Verifies ten negative sample fixtures under
  `samples/shell_accounting_replay/`.
- Verifies positive samples match expected cash, position, fee, market value,
  realized PnL, unrealized PnL, and data-quality projections exactly.
- Verifies negative samples fail closed with sanitized issue codes and no
  projection output.
- Verifies DataService action
  `accounting.portfolio_replay.readonly_summary` is registered.
- Verifies a real DataServiceClient calls the read-only action through a local
  service host.
- Verifies ShellServices and Presenter map replay status, issue codes, counts,
  and PnL state.
- Verifies temp SQLite `trade_log`, `cash_adjustment`, and `audit_log` remain
  unchanged by read-only replay.
- Verifies no production database, broker, network, credentials, endpoint, real
  order placement, or automatic trading behavior is used.

## Required Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
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
