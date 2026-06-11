# EPIC-273 Excel/VBA Import MVP Demo Workspace Test Plan

EPIC-273 adds local trial bootstrap documentation, a repo-local demo workspace
helper, and a smoke CTest. It does not add product features or production
trading capability.

## Smoke Test

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_demo_workspace --output-on-failure
```

The smoke test validates:

- Demo workspace, trial startup runbook, manual checklist, and this test plan
  are indexed.
- Bootstrap helper exists under `scripts/excel_vba_import_mvp/`.
- Bootstrap defaults to `.demo/excel_vba_import_mvp`.
- Demo DB path is explicit:
  `.demo/excel_vba_import_mvp/data/excel_vba_import_mvp_demo.sqlite`.
- Bootstrap can initialize only the explicit demo DB through existing
  `ETFDataService --init-db`.
- Sanitized JSON/TXT sample import path is documented.
- The sample JSON remains accepted by the existing read-only preview parser.
- Manual checklist covers Preview, `ACCEPTED`, persisted row counts, post-write
  refresh status, and duplicate import no-growth.
- Direct `.xlsx` import is not supported.
- Broker, network, credentials, endpoint, real order placement, and automatic
  trading remain out of scope.

## Required Regression

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_release_readiness --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

The EPIC-273 trial path writes no production DB, submits no broker order,
accesses no network endpoint or credential, and performs no automatic trading.
