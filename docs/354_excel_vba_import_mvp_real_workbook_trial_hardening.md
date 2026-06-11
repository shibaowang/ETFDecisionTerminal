# EPIC-275 Excel/VBA Import MVP Real Workbook Trial Hardening

EPIC-275 moves the Excel/VBA Import MVP from sample-only readiness to a local
real-workbook trial release-candidate path. The MVP remains constrained to
sanitized JSON/TXT exports produced from a workbook helper or equivalent local
export. Direct `.xlsx` import is not supported.

## Scope

- Harden the compatibility matrix for realistic workbook export shapes.
- Add sanitized EPIC-275 positive and negative sample exports.
- Verify parser preview behavior and persistence eligibility for those samples.
- Reuse the existing local service end-to-end path for selected positive
  scenarios.
- Clarify the manual real-workbook trial runbook and release-candidate
  checklist.

## Boundary

The EPIC-275 trial hardening does not change DataService write action
semantics, DataAccess repositories, migrations, AccountingEngine production
code, production QML, startup wiring, broker integration, network access,
credentials, endpoints, real order placement, or automatic trading.

All writes exercised by the CTest use temporary SQLite databases through the
existing local DataService path. Production databases are not touched.

## Deliverables

- Compatibility matrix:
  `docs/355_excel_vba_import_mvp_compatibility_matrix.md`
- Real-workbook trial runbook:
  `docs/356_excel_vba_import_mvp_real_workbook_trial_runbook.md`
- Release-candidate checklist:
  `docs/357_excel_vba_import_mvp_release_candidate_checklist.md`
- Test plan:
  `docs/358_excel_vba_import_mvp_real_workbook_trial_test_plan.md`
- EPIC-275 samples under `samples/excel_vba_import/`
- CTest:
  `shell_accounting_excel_vba_import_mvp_real_workbook_trial_hardening`

## Recommended Next Step

After EPIC-275 passes, the next work should be a controlled local trial closeout
or explicitly authorized user feedback task. It must continue to preserve the
sanitized JSON/TXT boundary and must not claim direct `.xlsx` import support.
