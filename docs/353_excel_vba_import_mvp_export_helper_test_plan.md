# Excel/VBA Import MVP Export Helper Test Plan

CTest:

```text
shell_accounting_excel_vba_import_mvp_export_helper_compatibility
```

## Coverage

The smoke CTest verifies:

- VBA helper file exists.
- Helper README exists.
- Helper text contains the MVP root contract fields.
- Helper text contains `InitialCash` and `TradeLog`.
- Helper text contains all required headers.
- Helper text avoids broker, network, credential, endpoint, and automatic trading
  tokens.
- Helper docs state direct `.xlsx` import is not supported.
- Mapping docs state the current path is Excel/VBA-side sanitized JSON/TXT
  export.
- Expected sample JSON parses and is accepted by the existing read-only parser.
- `docs/README.md` indexes the new docs, helper, README, and sample.
- No apps, production libraries, migrations, AccountingEngine, DataAccess, or
  DataServiceApi production behavior is modified by this Epic.

## Required Validation

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_export_helper_compatibility --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_demo_workspace --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_release_readiness --output-on-failure
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```
