# Excel/VBA Import MVP Release Candidate Checklist

## Release-Candidate Readiness

- [ ] Compatibility matrix reviewed.
- [ ] EPIC-275 positive samples accepted by the preview parser.
- [ ] EPIC-275 negative samples fail closed or remain not persistable.
- [ ] Single cash + single BUY local service E2E passes.
- [ ] BUY + partial SELL local service E2E passes.
- [ ] Multi-trade + multi-cash local service E2E passes.
- [ ] Duplicate import does not increase `trade_log`, `cash_adjustment`, or
      `audit_log` rows.
- [ ] Idempotency conflict does not increase rows and reports a sanitized
      issue.
- [ ] Post-write readback refresh remains visible and sanitized.
- [ ] Demo workspace runbook is current.
- [ ] User guide states JSON/TXT only.
- [ ] Direct `.xlsx` import is not supported.
- [ ] Broker, network, credentials, endpoints, real order placement, strategy
      execution, TradeDraft generation, and automatic trading are out of scope.

## Required Test Commands

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_real_workbook_trial_hardening --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_export_helper_compatibility --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_demo_workspace --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_release_readiness --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
```

## Known Limits

- Only sanitized JSON/TXT export files are supported.
- Direct `.xlsx` import is not supported.
- Market price and FX facts are preview-only and cannot be persisted.
- The MVP requires existing account, portfolio, and instrument references in
  the target demo database.
- Persistence is explicit, user-confirmed, idempotent, and local-service
  scoped.

## Rollback And Cleanup

- Delete the local demo workspace after the trial.
- Keep the original workbook backup unchanged.
- If a trial import is wrong, discard the demo DB and re-create it from the
  runbook. Do not manually edit a production DB.
