# EPIC-272 Excel/VBA Import MVP Release Readiness

EPIC-272 moves the Excel/VBA import MVP from end-to-end test coverage to a
local trial-ready package. The release remains scoped to sanitized JSON/TXT
exports, read-only preview, explicit accepted-preview persistence, and local
post-write readback refresh.

## Release Checklist

- User guide created: `docs/341_excel_vba_import_mvp_user_guide.md`.
- Sanitized sample export created:
  `samples/excel_vba_import/TASK272_sanitized_excel_vba_import_mvp_sample.json`.
- Export format contract created:
  `docs/342_excel_vba_import_mvp_sample_export_format.md`.
- Local runbook created: `docs/343_excel_vba_import_mvp_local_runbook.md`.
- Known limits created: `docs/344_excel_vba_import_mvp_known_limits.md`.
- Smoke test created:
  `shell_accounting_excel_vba_import_mvp_release_readiness`.
- Existing local service E2E acceptance remains required:
  `shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance`.
- Broker, network, credentials, endpoints, real order placement, and automatic
  trading are explicitly outside this MVP.

## Scope

The MVP supports sanitized Excel/VBA-style JSON/TXT export preview and explicit
manual-entry persistence for accepted previews. It does not claim direct `.xlsx`
import support.

The QML page may show clearer steps, sample contract text, persisted row counts,
post-write refresh status, duplicate/conflict status, and sanitized diagnostics.
QML must still call Presenter methods only.

## Non-Goals

- No DataServiceApi write action implementation change.
- No DataAccess repository change.
- No migration change.
- No AccountingEngine production code change.
- No production database path or production database mutation logic change.
- No broker, network, credentials, endpoint, real order placement, or automatic
  trading.
- No TradeDraft or strategy execution.
- No historical fixture JSON changes.

## Smoke Evidence

The EPIC-272 smoke CTest verifies the sample export is sanitized and accepted by
the existing read-only parser, checks required QML panel object names, confirms
the docs and checklist are indexed, and emits sanitized JSON evidence for
release readiness.
