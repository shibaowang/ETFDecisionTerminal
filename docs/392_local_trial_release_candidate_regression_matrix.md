# EPIC-282 Local Trial Regression Matrix

| Scope | CTest | Module | Critical Boundary | Expected Result | Blocker If Failed |
| --- | --- | --- | --- | --- | --- |
| TASK-271 | `shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance` | Excel/VBA local service E2E | temp DB only, local service only | preview, persist, refresh pass | E2E write/readback broken |
| EPIC-272 | `shell_accounting_excel_vba_import_mvp_release_readiness` | MVP release readiness | sanitized JSON/TXT only | docs, sample, smoke pass | release docs incomplete |
| EPIC-273 | `shell_accounting_excel_vba_import_mvp_demo_workspace` | demo workspace | explicit demo DB path | workspace smoke pass | demo path unsafe |
| EPIC-274 | `shell_accounting_excel_vba_import_mvp_export_helper_compatibility` | export helper | sanitized export contract | compatibility pass | helper format drift |
| EPIC-275 | `shell_accounting_excel_vba_import_mvp_real_workbook_trial_hardening` | workbook hardening | no raw private data | hardening pass | importer unsafe |
| EPIC-276 | `shell_accounting_portfolio_replay_vba_parity_full_delivery` | portfolio replay | read-only replay | parity pass | projection regression |
| EPIC-277 | `strategy_recommendation_engine_vba_parity_full_delivery` | strategy | recommendation only | parity pass | recommendation drift |
| EPIC-278 | `tradedraft_manual_recommendation_flow_full_delivery` | TradeDraft | internal draft, not order | draft flow pass | draft safety regression |
| EPIC-279 | `otcmap_ac_multichannel_draft_engine_full_delivery` | OTCMap | multi-leg internal draft | draft flow pass | OTCMap safety regression |
| EPIC-280 | `market_data_historical_high_refresh_engine_full_delivery` | MarketData | fixture/disabled provider default | refresh pass | live/default boundary drift |
| EPIC-281 | `dashboard_mvp_full_delivery` | Dashboard | Presenter-only QML calls | Dashboard pass | UI wiring regression |
| EPIC-282 | `local_trial_release_candidate_full_delivery` | Local trial RC | repo-local trial package | RC smoke pass | release candidate incomplete |

Each row must be treated as pass only when the named CTest returns success and
the associated boundary evidence is present in logs or stdout. No row may pass
if production DB access is observed during the local trial.
