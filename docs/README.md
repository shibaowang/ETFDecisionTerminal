# Documentation Index

- [01 Product Spec](01_product_spec.md)
- [02 Architecture](02_architecture.md)
- [03 Database Schema](03_database_schema.md)
- [04 Protocol](04_protocol.md)
- [05 Strategy Rules](05_strategy_rules.md)
- [06 Accounting Rules](06_accounting_rules.md)
- [07 UI Design](07_ui_design.md)
- [08 Testing Plan](08_testing_plan.md)
- [09 Coding Standard](09_coding_standard.md)
- [10 Migration From Excel v8.2](10_migration_from_excel_v8_2.md)
- [11 Deferred Scope](11_deferred_scope.md)
- [12 Codex Prompt Template](12_codex_prompt_template.md)
- [350 Excel/VBA Import MVP Export Helper Compatibility Pack](350_excel_vba_import_mvp_export_helper_compatibility_pack.md)
- [351 Excel/VBA Import MVP Export Helper Mapping](351_excel_vba_import_mvp_export_helper_mapping.md)
- [352 Excel/VBA Import MVP Export Helper User Guide](352_excel_vba_import_mvp_export_helper_user_guide.md)
- [353 Excel/VBA Import MVP Export Helper Test Plan](353_excel_vba_import_mvp_export_helper_test_plan.md)
- Excel/VBA import MVP export helper:
  [tools/excel_vba_export/ExcelVbaImportMvpExportHelper.bas](../tools/excel_vba_export/ExcelVbaImportMvpExportHelper.bas)
- Excel/VBA import MVP export helper README:
  [tools/excel_vba_export/ExcelVbaImportMvpExportHelper_README.md](../tools/excel_vba_export/ExcelVbaImportMvpExportHelper_README.md)
- Excel/VBA import MVP export helper expected sample:
  [samples/excel_vba_import/TASK274_export_helper_expected_sample.json](../samples/excel_vba_import/TASK274_export_helper_expected_sample.json)
  The MVP supports Excel/VBA-side sanitized JSON/TXT export only; direct
  `.xlsx` import is not supported. The helper does not access broker, network,
  credentials, or endpoints, and does not enable automatic trading.
- [346 Excel/VBA Import MVP Demo Workspace](346_excel_vba_import_mvp_demo_workspace.md)
- [347 Excel/VBA Import MVP Trial Startup Runbook](347_excel_vba_import_mvp_trial_startup_runbook.md)
- [348 Excel/VBA Import MVP Manual Acceptance Checklist](348_excel_vba_import_mvp_manual_acceptance_checklist.md)
- [349 Excel/VBA Import MVP Demo Workspace Test Plan](349_excel_vba_import_mvp_demo_workspace_test_plan.md)
- Excel/VBA import MVP demo bootstrap:
  [scripts/excel_vba_import_mvp/New-ExcelVbaImportMvpDemoWorkspace.ps1](../scripts/excel_vba_import_mvp/New-ExcelVbaImportMvpDemoWorkspace.ps1)
  The demo workspace supports sanitized JSON/TXT export only; direct `.xlsx`
  import is not supported.
- [340 Excel/VBA Import MVP Release Readiness](340_excel_vba_import_mvp_release_readiness.md)
- [341 Excel/VBA Import MVP User Guide](341_excel_vba_import_mvp_user_guide.md)
- [342 Excel/VBA Import MVP Sample Export Format](342_excel_vba_import_mvp_sample_export_format.md)
- [343 Excel/VBA Import MVP Local Runbook](343_excel_vba_import_mvp_local_runbook.md)
- [344 Excel/VBA Import MVP Known Limits](344_excel_vba_import_mvp_known_limits.md)
- [345 Excel/VBA Import MVP Release Readiness Test Plan](345_excel_vba_import_mvp_release_readiness_test_plan.md)
- Excel/VBA import MVP sample export:
  [samples/excel_vba_import/TASK272_sanitized_excel_vba_import_mvp_sample.json](../samples/excel_vba_import/TASK272_sanitized_excel_vba_import_mvp_sample.json)
  This MVP supports sanitized JSON/TXT export only; direct `.xlsx` import is
  not supported.
- [338 ShellAccounting Excel/VBA Import MVP Local Service E2E Acceptance](338_shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance.md)
- [339 ShellAccounting Excel/VBA Import MVP Local Service E2E Acceptance Test Plan](339_shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance_test_plan.md)
- [334 ShellAccounting Excel/VBA Import Persist Supported Fact Types Alignment](334_shell_accounting_excel_vba_import_persist_supported_fact_types_alignment.md)
- [335 ShellAccounting Excel/VBA Import Persist Supported Fact Types Alignment Test Plan](335_shell_accounting_excel_vba_import_persist_supported_fact_types_alignment_test_plan.md)
- [13 Manual Acceptance - ReadOnly Shell](13_manual_acceptance_readonly_shell.md)
- [14 Milestone - v0.1 ReadOnly Shell Demo](14_milestone_v0_1_readonly_shell_demo.md)
- [15 Quick Start - ReadOnly Shell Demo](15_quick_start_readonly_shell_demo.md)
- [16 Manual Acceptance - ReadOnly Business Pages](16_manual_acceptance_readonly_business_pages.md)
- [17 UI Readability Checklist](17_ui_readability_checklist.md)
- [18 Milestone - v0.2 ReadOnly Business Pages](18_milestone_v0_2_readonly_business_pages.md)
- [19 ReadOnly Business Pages Summary](19_readonly_business_pages_summary.md)
- [20 Position Accounting Boundary](20_position_accounting_boundary.md)
- [21 Position Read-Only Data Contract Draft](21_position_readonly_data_contract_draft.md)
- [22 Position Accounting Data Contract](22_position_accounting_data_contract.md)
- [23 Position Accounting Test Fixture Design](23_position_accounting_test_fixture_design.md)
- [24 Accounting Replay Fixture Samples](24_accounting_replay_fixture_samples.md)
- Accounting replay fixture files: [tests/fixtures/accounting_replay](../tests/fixtures/accounting_replay/README.md)
- Accounting fixture loader boundary: [tests/AccountingFixtures](../tests/AccountingFixtures)
- [25 Position Shell ViewModel Design](25_position_shell_viewmodel_design.md)
- [26 Position DTO ViewModel Mapping](26_position_dto_viewmodel_mapping.md)
- [30 Accounting Replay Incremental Implementation Plan](30_accounting_replay_incremental_implementation_plan.md)
- [31 Accounting Replay Test-only Coverage Milestone](31_accounting_replay_testonly_coverage_milestone.md)
- [32 Production Accounting Replay Readiness Review](32_production_accounting_replay_readiness_review.md)
- [33 Production Accounting Replay Architecture Boundary](33_production_accounting_replay_architecture.md)
- [34 AccountingEngine Module Candidate](34_accounting_engine_module_candidate.md)
- [35 AccountingEngine Replay Skeleton Milestone](35_accounting_engine_replay_skeleton_milestone.md)
- [36 AccountingEngine Next Phase Boundary Review](36_accounting_engine_next_phase_boundary_review.md)
- [37 DataService Read-only Accounting Action Contracts](37_dataservice_readonly_accounting_action_contracts.md)
- [38 DataService Accounting No-write Test Plan](38_dataservice_accounting_no_write_test_plan.md)
- [39 SQLite Read-only Facts Query Boundary](39_sqlite_readonly_facts_query_boundary.md)
- [40 Accounting Facts Source Mapping](40_accounting_facts_source_mapping.md)
- [41 Cash Facts Source Boundary](41_cash_facts_source_boundary.md)
- [42 Cash Facts Query Decision](42_cash_facts_query_decision.md)
- [43 DataService Accounting Guard Suite Milestone](43_dataservice_accounting_guard_suite_milestone.md)
- [44 DataService Accounting Guard No-write Readiness Review](44_dataservice_accounting_guard_no_write_readiness_review.md)
- [45 position.list Real Implementation Boundary](45_position_list_real_implementation_boundary.md)
- [46 position.list Real Implementation Readiness Checklist](46_position_list_real_implementation_readiness_checklist.md)
- [47 position.list Readiness Hardening](47_position_list_readiness_hardening.md)
- [48 position.list First-stage Real Action Scope](48_position_list_first_stage_real_action_scope.md)
- [49 ShellServices Accounting Controller Contract](49_shellservices_accounting_controller_contract.md)
- [50 Shell Accounting ViewModel State Contract](50_shell_accounting_viewmodel_state_contract.md)
- [51 Shell Accounting Service Adapter Boundary](51_shell_accounting_service_adapter_boundary.md)
- [52 Shell Accounting Service Integration Readiness](52_shell_accounting_service_integration_readiness.md)
- [53 Shell Accounting DataService Adapter Boundary](53_shell_accounting_dataservice_adapter_boundary.md)
- [54 Shell Accounting DataService Adapter Test Plan](54_shell_accounting_dataservice_adapter_test_plan.md)
- [55 Shell Accounting DataService Adapter Live-call Gate](55_shell_accounting_dataservice_adapter_live_call_gate.md)
- [56 Shell Accounting DataService Adapter Live-call Acceptance Checklist](56_shell_accounting_dataservice_adapter_live_call_acceptance_checklist.md)
- ShellServices accounting controller test scaffolding:
  `tests/ShellAccountingControllerScaffolding`; production controller and QML
  integration remain unimplemented.
- ShellAccountingReadOnlyController production skeleton: state / issue /
  privacy / no-trade-action boundary only; no service calls and no QML
  integration.
- ShellAccountingReadOnlyController fake/client boundary tests:
  `tests/ShellAccountingFakeClientBoundary`; fake payloads remain test-only,
  real DataServiceClient integration and QML integration remain unimplemented.
- Shell accounting service adapter boundary: docs only; real adapter,
  controller service calls, DataServiceClient integration, and QML integration
  remain unimplemented.
- ShellAccountingServiceAdapter interface skeleton: production interface and
  request/result types only; real DataServiceClient adapter and QML integration
  remain unimplemented.
- ShellAccountingReadOnlyController fake adapter injection behavior:
  controller refresh methods can consume the abstract adapter with a test-only
  fake adapter; real DataServiceClient adapter and QML integration remain
  unimplemented.
- ShellAccountingDataServiceAdapter boundary and test plan: documentation only
  for a future real adapter wrapping read-only DataServiceClient guard
  wrappers; real adapter and QML integration remain unimplemented.
- ShellAccountingDataServiceAdapter spy/wrapper scaffolding:
  `tests/ShellAccountingDataServiceAdapterScaffolding`; real adapter, real
  DataServiceClient calls, and QML integration remain unimplemented.
- DataService `position.list` guard: registered read-only guard with
  `implemented=false`; real position query remains unimplemented.
- SQLite read-only no-write harness skeleton: test-only helpers under
  `tests/AccountingNoWrite`; real facts query, DataAccess repository, new
  DataService actions, and writes remain unimplemented.
- SQLite read-only trade facts query skeleton: DataAccess `trade_log` reader
  for DataAccess-side rows only; no replay, no `position.list` implementation,
  no AccountingEngine call, and no writes.
- Cash facts source boundary: `CashFactDto` source decision docs only; no cash
  facts query, no cash facts repository, no SQLite access, and no writes.
- DataService `cash.summary` guard: registered read-only guard with
  `implemented=false`; real cash summary, cash facts query, snapshot reads, and
  writes remain unimplemented.
- DataService `portfolio.pnl.summary` guard: registered read-only guard with
  `implemented=false`; real PnL summary, trade/cash/market facts integration,
  snapshot reads, and writes remain unimplemented.
- DataService `base_position.summary` guard: registered read-only guard with
  `implemented=false`; real base position calculation, trade suggestions,
  TradeDraft generation, snapshot reads, and writes remain unimplemented.
- DataService `sniper_pool.summary` guard: registered read-only guard with
  `implemented=false`; real sniper pool calculation, T1-T6 tier calculation,
  trade suggestions, TradeDraft generation, snapshot reads, and writes remain
  unimplemented.
- DataService accounting guard suite milestone: guard layer coverage is
  complete for the current read-only milestone; real accounting actions,
  SQLite integration, AccountingEngine integration, QML integration, and writes
  remain unimplemented.
- `position.list` real implementation boundary: design docs only; guard
  behavior, SQLite integration, AccountingEngine integration, QML integration,
  and writes remain unchanged.
- `position.list` mapping contract tests: test-only request, trade fact, and
  response mapping coverage; real `position.list` remains unimplemented and
  guard behavior remains unchanged.
- `position.list` real action no-write skeleton: test-only future path matrix
  for valid, invalid, replay unavailable, facts unavailable, mapping failure,
  missing market, multi-currency, FX, and SQLite failure paths; real
  `position.list` remains unimplemented.
- `position.list` readiness hardening: final Go / No-Go, failure matrix,
  no-write gate, degraded output policy, guard fallback, and first-stage scope
  docs; real `position.list` remains unimplemented.
- `position.list` first-stage implementation scaffolding: test-only scope,
  guard transition, and failure matrix contract tests; real `position.list`
  remains unimplemented.
- ShellServices accounting controller contract: documentation only for future
  read-only ShellServices controller / ViewModel boundaries; QML remains
  disconnected from accounting actions.
- ShellAccountingDataServiceAdapter production skeleton: implements the
  `ShellAccountingServiceAdapter` boundary but only returns not connected /
  unavailable; live DataServiceClient calls, QML integration, SQLite access, and
  writes remain unimplemented.
- ShellAccountingDataServiceAdapter live-call gate: docs 55 / 56 and test-only
  gate descriptors define the future live-call allowlist, mapping, error,
  no-write, rollback, and readiness requirements; live calls remain
  unimplemented. See
  [55_shell_accounting_dataservice_adapter_live_call_gate.md](55_shell_accounting_dataservice_adapter_live_call_gate.md)
  and
  [56_shell_accounting_dataservice_adapter_live_call_acceptance_checklist.md](56_shell_accounting_dataservice_adapter_live_call_acceptance_checklist.md).
- ShellAccountingDataServiceClientPort concrete implementation:
  `ShellAccountingDataServiceClientPortAdapter` is the only production
  ShellServices class that includes and holds the real DataServiceClient. It
  wraps only `positionList`, `cashSummary`, `portfolioPnlSummary`,
  `basePositionSummary`, and `sniperPoolSummary` guard wrappers. The controller,
  `ShellAccountingDataServiceAdapter` body, and QML still do not directly
  depend on DataServiceClient; no SQLite, AccountingEngine, writes, TradeDraft,
  or trade suggestion behavior is added.
- ShellAccountingReadOnlyController concrete port integration hardening:
  controller integration tests now exercise controller -> DataService adapter
  -> concrete DataServiceClient port -> guard wrapper for `position.list`,
  `cash.summary`, `portfolio.pnl.summary`, `base_position.summary`, and
  `sniper_pool.summary`. Guard payloads still map to Unavailable; QML,
  SQLite, AccountingEngine, writes, TradeDraft, and trade suggestions remain
  out of scope.
- ShellAccounting ViewModel / Model read-only boundary: ShellServices now has
  `ShellAccountingStatusObject`, `ShellAccountingIssueListModel`, and
  `ShellPositionListModel` skeletons for future QML binding. They consume only
  in-memory state / issue / display data and do not call DataServiceClient,
  SQLite, AccountingEngine, QML, writes, TradeDraft, or trade suggestions.
- ShellAccounting presenter contract:
  [57_shell_accounting_presenter_contract.md](57_shell_accounting_presenter_contract.md)
  defines the future presenter / facade boundary before any QML integration.
- ShellAccounting QML binding readiness plan:
  [58_shell_accounting_qml_binding_readiness_plan.md](58_shell_accounting_qml_binding_readiness_plan.md)
  defines future read-only QML binding rules, no-trade UI rules, and the
  explicit QML authorization checklist.
- ShellAccountingPresenter skeleton: production ShellServices presenter
  skeleton now holds the read-only status object, issue model, position list
  model, and optional controller boundary. QML remains unwired and the
  presenter does not call DataServiceClient, SQLite, AccountingEngine, writes,
  TradeDraft, or trade suggestions.
- ShellAccountingPresenter concrete port integration: presenter-level tests now
  verify `ShellAccountingPresenter -> ShellAccountingReadOnlyController ->
  ShellAccountingDataServiceAdapter -> ShellAccountingDataServiceClientPortAdapter
  -> DataServiceClient guard wrapper` for `position.list`. Guard payloads stay
  Unavailable with visible issues and no write / no trade behavior. QML remains
  unwired.
- ShellAccountingPresenter all guard refresh: presenter-level tests now cover
  `position.list`, `cash.summary`, `portfolio.pnl.summary`,
  `base_position.summary`, and `sniper_pool.summary` through the controller /
  adapter / concrete port guard-wrapper path. `refreshAllReadOnly` uses that
  fixed order and aggregates the visible `*_NOT_AVAILABLE` issues. QML remains
  unwired and no real accounting action, SQLite, AccountingEngine, write,
  TradeDraft, or trade suggestion path is enabled.
- ShellAccounting QML binding smoke plan:
  [59_shell_accounting_qml_binding_smoke_plan.md](59_shell_accounting_qml_binding_smoke_plan.md)
  defines the future read-only QML smoke matrix, state display rules, privacy
  rules, and no-trade UI scope before any QML files are modified.
- ShellAccounting QML static gate:
  [60_shell_accounting_qml_static_gate.md](60_shell_accounting_qml_static_gate.md)
  defines future QML denylist checks for direct DataServiceClient, SQLite,
  DataAccess, AccountingEngine, write action, TradeDraft, strategy, and broker
  references.
- ShellAccounting QML static gate CTest: `tests/ShellAccountingQmlStaticGate`
  automates docs/60 with no direct service, no write token, no premature
  accounting binding, accounting-scope no-trade, and docs sync tests. QML
  remains unwired.
- ShellAccounting read-only UI readiness milestone:
  [61_shell_accounting_readonly_ui_readiness_milestone.md](61_shell_accounting_readonly_ui_readiness_milestone.md)
  closes the current ShellAccounting readiness loop without QML binding or real
  accounting action implementation.
- ShellAccounting next phase review:
  [62_shell_accounting_next_phase_review.md](62_shell_accounting_next_phase_review.md)
  defines the Go / No-Go checklist for a future read-only QML binding task.
- [Release Notes - v0.6 ShellAccounting Read-only UI Readiness](release_notes/v0_6_shell_accounting_readonly_ui_readiness.md)
- ShellAccounting QML binding smoke scaffold: `tests/ShellAccountingQmlBindingSmoke`
  locks object contract, state matrix, guard payload, issue / privacy,
  no-trade UI, and static gate dependency expectations before any QML binding.
- ShellAccounting QML smoke CTest: `tests/ShellAccountingQmlBindingSmokeRuntime`
  uses test-only inline QML to verify object availability, state display, guard
  Unavailable display, issue visibility, privacy display, no-trade UI tokens,
  and static gate regression. Production QML remains unwired.
- ShellAccounting production QML binding boundary:
  [63_shell_accounting_production_qml_binding_boundary.md](63_shell_accounting_production_qml_binding_boundary.md)
  defines the production QML binding boundary before any QML file is modified.
- ShellAccounting first-stage QML binding checklist:
  [64_shell_accounting_first_stage_qml_binding_checklist.md](64_shell_accounting_first_stage_qml_binding_checklist.md)
  defines the Go / No-Go checklist for the first read-only production QML stage.
- ShellAccounting QML type registration boundary:
  [65_shell_accounting_qml_type_registration_boundary.md](65_shell_accounting_qml_type_registration_boundary.md)
  defines the future read-only QML type registration boundary before any type is
  registered.
- ShellAccounting QML type registration test plan:
  [66_shell_accounting_qml_type_registration_test_plan.md](66_shell_accounting_qml_type_registration_test_plan.md)
  defines the registration construction, allowlist, denylist, smoke, and static
  gate test matrix.
- ShellAccounting QML type registration scaffold:
  `tests/ShellAccountingQmlTypeRegistrationScaffold` fixes the first-stage type
  allowlist, forbidden type and method/property denylists, module import
  boundary, gate dependency, and no-production-change checks.
- ShellAccounting QML type registration implementation gate:
  [67_shell_accounting_qml_type_registration_implementation_gate.md](67_shell_accounting_qml_type_registration_implementation_gate.md)
  defines the final gate before any production QML type registration task.
- ShellAccounting QML type registration rollback strategy:
  [68_shell_accounting_qml_type_registration_rollback_strategy.md](68_shell_accounting_qml_type_registration_rollback_strategy.md)
  defines rollback triggers, fallback behavior, and verification after rollback.
- ShellAccounting QML type registration implementation:
  `libs/ShellServices/include/ShellServices/ShellAccountingQmlRegistration.h`
  registers only the first-stage read-only allowlist types under
  `ETFDecisionTerminal.ShellAccounting` 1.0. Production QML still does not
  import or bind the module.
- ShellAccounting QML startup registration gate:
  [69_shell_accounting_qml_startup_registration_gate.md](69_shell_accounting_qml_startup_registration_gate.md)
  defines the future production app-startup wiring gate. Startup wiring is not
  implemented.
- ShellAccounting QML startup registration test plan:
  [70_shell_accounting_qml_startup_registration_test_plan.md](70_shell_accounting_qml_startup_registration_test_plan.md)
  defines the future startup wiring test matrix and Go / No-Go checklist.
- ShellAccounting QML startup registration wiring:
  [71_shell_accounting_qml_startup_registration_wiring.md](71_shell_accounting_qml_startup_registration_wiring.md)
  records TASK-129 centralized startup wiring. Production QML still does not
  import or bind the module.
- ShellAccounting production QML binding gate:
  [72_shell_accounting_production_qml_binding_gate.md](72_shell_accounting_production_qml_binding_gate.md)
  defines the final gate before any production QML import or page binding.
- ShellAccounting production QML binding test plan:
  [73_shell_accounting_production_qml_binding_test_plan.md](73_shell_accounting_production_qml_binding_test_plan.md)
  defines the future production QML binding test matrix.
- ShellAccounting production QML binding implementation:
  [74_shell_accounting_production_qml_binding_implementation.md](74_shell_accounting_production_qml_binding_implementation.md)
  records TASK-131 read-only, unavailable-safe production QML shell binding.
- ShellAccounting presenter lifecycle gate:
  [75_shell_accounting_presenter_lifecycle_gate.md](75_shell_accounting_presenter_lifecycle_gate.md)
  defines the future presenter creation and context exposure gate.
- ShellAccounting presenter lifecycle test plan:
  [76_shell_accounting_presenter_lifecycle_test_plan.md](76_shell_accounting_presenter_lifecycle_test_plan.md)
  defines the future lifecycle / context exposure test matrix.
- ShellAccounting presenter lifecycle implementation:
  [77_shell_accounting_presenter_lifecycle_implementation.md](77_shell_accounting_presenter_lifecycle_implementation.md)
  records TASK-133 production-owned presenter lifecycle and context exposure.
- ShellAccounting real data adapter gate:
  [78_shell_accounting_real_data_adapter_gate.md](78_shell_accounting_real_data_adapter_gate.md)
  defines the future read-only real data adapter / DataService action gate.
- ShellAccounting real data adapter test plan:
  [79_shell_accounting_real_data_adapter_test_plan.md](79_shell_accounting_real_data_adapter_test_plan.md)
  defines the future adapter error, privacy, no-write, and rollback test matrix.
- ShellAccounting real data adapter implementation:
  [80_shell_accounting_real_data_adapter_implementation.md](80_shell_accounting_real_data_adapter_implementation.md)
  records TASK-135 read-only adapter wiring and its no-write/no-raw-payload
  boundary.
- ShellAccounting DataService read-only facts query gate:
  [81_shell_accounting_dataservice_readonly_facts_query_gate.md](81_shell_accounting_dataservice_readonly_facts_query_gate.md)
  defines the future DataService-only facts query boundary.
- ShellAccounting DataService read-only facts query test plan:
  [82_shell_accounting_dataservice_readonly_facts_query_test_plan.md](82_shell_accounting_dataservice_readonly_facts_query_test_plan.md)
  defines the future SELECT-only, no-replay, no-write test matrix.
- ShellAccounting DataService read-only facts query implementation:
  [83_shell_accounting_dataservice_readonly_facts_query_implementation.md](83_shell_accounting_dataservice_readonly_facts_query_implementation.md)
  records TASK-137 SELECT-only DataService facts query implementation and its
  no-write/no-replay/no-TradeDraft boundary.
- ShellAccounting replay snapshot rebuild gate:
  [84_shell_accounting_replay_snapshot_rebuild_gate.md](84_shell_accounting_replay_snapshot_rebuild_gate.md)
  defines the future AccountingEngine replay / snapshot rebuild boundary.
- ShellAccounting replay snapshot rebuild test plan:
  [85_shell_accounting_replay_snapshot_rebuild_test_plan.md](85_shell_accounting_replay_snapshot_rebuild_test_plan.md)
  defines the future replay, rebuild, no-write, no-trade, and rollback test
  matrix.
- ShellAccounting read-only replay implementation:
  [86_shell_accounting_readonly_replay_implementation.md](86_shell_accounting_readonly_replay_implementation.md)
  records TASK-139 DataService-internal read-only replay. Snapshot rebuild and
  snapshot write remain unimplemented, and Shell/QML still do not directly call
  AccountingEngine.
- ShellAccounting snapshot rebuild write gate:
  [87_shell_accounting_snapshot_rebuild_write_gate.md](87_shell_accounting_snapshot_rebuild_write_gate.md)
  defines the future snapshot rebuild / snapshot write boundary.
- ShellAccounting snapshot rebuild write test plan:
  [88_shell_accounting_snapshot_rebuild_write_test_plan.md](88_shell_accounting_snapshot_rebuild_write_test_plan.md)
  defines the future rebuild/write, no-trade, no-generic-write, and rollback
  test matrix.
- ShellAccounting snapshot rebuild implementation gate:
  [89_shell_accounting_snapshot_rebuild_implementation_gate.md](89_shell_accounting_snapshot_rebuild_implementation_gate.md)
  defines the future snapshot rebuild implementation boundary.
- ShellAccounting snapshot rebuild implementation test plan:
  [90_shell_accounting_snapshot_rebuild_implementation_test_plan.md](90_shell_accounting_snapshot_rebuild_implementation_test_plan.md)
  defines the future rebuild-only, in-memory preview, no-write, no-trade, and
  rollback test matrix.
- ShellAccounting snapshot rebuild implementation:
  [91_shell_accounting_snapshot_rebuild_implementation.md](91_shell_accounting_snapshot_rebuild_implementation.md)
  records TASK-142 DataService-internal snapshot rebuild preview. Snapshot write
  remains unimplemented and separately unauthorized.
- ShellAccounting snapshot write authorization gate:
  [92_shell_accounting_snapshot_write_authorization_gate.md](92_shell_accounting_snapshot_write_authorization_gate.md)
  defines the TASK-143 future snapshot write authorization boundary.
- ShellAccounting snapshot write authorization test plan:
  [93_shell_accounting_snapshot_write_authorization_test_plan.md](93_shell_accounting_snapshot_write_authorization_test_plan.md)
  defines the TASK-143 future preview-input-only, allowlisted, transactional, no-trade,
  rollback-ready snapshot write test matrix.
- ShellAccounting snapshot write implementation:
  [94_shell_accounting_snapshot_write_implementation.md](94_shell_accounting_snapshot_write_implementation.md)
  records TASK-144 DataService-internal snapshot write to the allowlist tables
  `cash_snapshot`, `position_snapshot`, and `portfolio_summary`.
- ShellAccounting audit write authorization gate:
  [95_shell_accounting_audit_write_authorization_gate.md](95_shell_accounting_audit_write_authorization_gate.md)
  defines the TASK-145 future audit write authorization boundary.
- ShellAccounting audit write authorization test plan:
  [96_shell_accounting_audit_write_authorization_test_plan.md](96_shell_accounting_audit_write_authorization_test_plan.md)
  defines the TASK-145 future sanitized, DataService-only, no-trade,
  rollback-ready audit write test matrix.
- ShellAccounting audit write implementation:
  [97_shell_accounting_audit_write_implementation.md](97_shell_accounting_audit_write_implementation.md)
  records TASK-146 DataService-internal sanitized audit write to `audit_log`.
- ShellAccounting TradeDraft authorization gate:
  [98_shell_accounting_tradedraft_authorization_gate.md](98_shell_accounting_tradedraft_authorization_gate.md)
  defines the TASK-147 future TradeDraft authorization boundary.
- ShellAccounting TradeDraft authorization test plan:
  [99_shell_accounting_tradedraft_authorization_test_plan.md](99_shell_accounting_tradedraft_authorization_test_plan.md)
  defines the TASK-147 DataService-only, sanitized, audited, no-execution
  TradeDraft test matrix.
- ShellAccounting TradeDraft implementation:
  [100_shell_accounting_tradedraft_implementation.md](100_shell_accounting_tradedraft_implementation.md)
  records TASK-148 DataService-internal, authorized, audited TradeDraft
  creation without trade execution, strategy execution, broker order, or
  production trading UI.
- ShellAccounting TradeDraft confirmation authorization gate:
  [101_shell_accounting_tradedraft_confirmation_authorization_gate.md](101_shell_accounting_tradedraft_confirmation_authorization_gate.md)
  defines the TASK-149 future confirmation / TradeLog write authorization
  boundary.
- ShellAccounting TradeDraft confirmation authorization test plan:
  [102_shell_accounting_tradedraft_confirmation_authorization_test_plan.md](102_shell_accounting_tradedraft_confirmation_authorization_test_plan.md)
  defines the TASK-149 DataService-only, audited, transactional,
  no-broker/no-strategy confirmation test matrix.
- ShellAccounting TradeDraft confirmation implementation:
  [103_shell_accounting_tradedraft_confirmation_implementation.md](103_shell_accounting_tradedraft_confirmation_implementation.md)
  records TASK-150 DataService-internal, authorized, audited, transactional
  confirmation into `trade_execution_group` / `trade_log` without broker order,
  strategy execution, automatic trading, or production trading UI.
- ShellAccounting production trading UI authorization gate:
  [104_shell_accounting_production_trading_ui_authorization_gate.md](104_shell_accounting_production_trading_ui_authorization_gate.md)
  defines the TASK-151 future production trading UI boundary before QML exposes
  TradeDraft creation or confirmation.
- ShellAccounting production trading UI authorization test plan:
  [105_shell_accounting_production_trading_ui_authorization_test_plan.md](105_shell_accounting_production_trading_ui_authorization_test_plan.md)
  defines the TASK-151 no-QML-binding, DataService-only, user-confirmed,
  no-broker/no-strategy production trading UI test matrix.
- ShellAccounting production trading UI implementation:
  [106_shell_accounting_production_trading_ui_implementation.md](106_shell_accounting_production_trading_ui_implementation.md)
  records TASK-152 production QML Draft creation / confirmation UI routed only
  through the ShellAccounting presenter/controller/adapter/DataService boundary.
- ShellAccounting broker order authorization gate:
  [107_shell_accounting_broker_order_authorization_gate.md](107_shell_accounting_broker_order_authorization_gate.md)
  defines the TASK-153 future broker order / order placement boundary.
- ShellAccounting broker order authorization test plan:
  [108_shell_accounting_broker_order_authorization_test_plan.md](108_shell_accounting_broker_order_authorization_test_plan.md)
  defines the TASK-153 no-broker-yet, DataService-only, confirmed-ledger-input,
  no-strategy/no-automatic-trading test matrix.
- ShellAccounting broker order dry-run implementation:
  [109_shell_accounting_broker_order_dry_run_implementation.md](109_shell_accounting_broker_order_dry_run_implementation.md)
  records TASK-154 DataService-only dry-run boundary, sanitized audit-only
  writes, no broker SDK call, no real order placement, no strategy execution,
  and no automatic trading.
- ShellAccounting real broker order authorization gate:
  [110_shell_accounting_real_broker_order_authorization_gate.md](110_shell_accounting_real_broker_order_authorization_gate.md)
  defines the TASK-155 future real broker order / real order placement gate.
- ShellAccounting real broker order authorization test plan:
  [111_shell_accounting_real_broker_order_authorization_test_plan.md](111_shell_accounting_real_broker_order_authorization_test_plan.md)
  defines TASK-155 dry-run preflight, confirmed-ledger input, final
  confirmation, independent authorization, risk control, reconciliation, and
  rollback / incident containment probes.
- ShellAccounting real broker order implementation gate:
  [112_shell_accounting_real_broker_order_implementation_gate.md](112_shell_accounting_real_broker_order_implementation_gate.md)
  defines the TASK-156 final gate before any real broker order implementation,
  broker adapter, SDK import, or real placement work.
- ShellAccounting real broker order implementation test plan:
  [113_shell_accounting_real_broker_order_implementation_test_plan.md](113_shell_accounting_real_broker_order_implementation_test_plan.md)
  defines TASK-156 static probes for no real order, no broker adapter, no SDK,
  no broker network call, no credentials, no strategy execution, and no
  automatic trading.
- ShellAccounting broker adapter interface scaffold:
  [114_shell_accounting_broker_adapter_interface_scaffold.md](114_shell_accounting_broker_adapter_interface_scaffold.md)
  records TASK-157 DataServiceApi-owned broker order port / DTO scaffold,
  disabled/null adapter, test-only fake adapter, and no-SDK/no-network/no-order
  boundary.
- ShellAccounting broker adapter implementation gate:
  [115_shell_accounting_broker_adapter_implementation_gate.md](115_shell_accounting_broker_adapter_implementation_gate.md)
  records TASK-158 future adapter implementation boundaries: default disabled,
  fake test-only / sandbox / paper / real mode isolation, DataService-only
  access, no SDK, no network, no credentials, no real order, no strategy, no
  automatic trading, and rollback / kill switch policy.
- ShellAccounting broker adapter implementation test plan:
  [116_shell_accounting_broker_adapter_implementation_test_plan.md](116_shell_accounting_broker_adapter_implementation_test_plan.md)
  defines TASK-158 static probes for no adapter implementation yet, no SDK, no
  network call, no credentials, no QML / Presenter direct broker access, no
  real order id, reconciliation/cancellation handoff, and regression coverage.
- ShellAccounting broker adapter disabled wiring:
  [117_shell_accounting_broker_adapter_disabled_wiring.md](117_shell_accounting_broker_adapter_disabled_wiring.md)
  records TASK-159 DataService-internal default wiring for the disabled/null
  broker port, while preserving dry-run behavior and keeping fake adapters
  test-only with no SDK, no network, no credentials, no real order id, and no
  real placement.
- ShellAccounting broker sandbox adapter authorization gate:
  [118_shell_accounting_broker_sandbox_adapter_authorization_gate.md](118_shell_accounting_broker_sandbox_adapter_authorization_gate.md)
  defines TASK-160 sandbox adapter authorization, mode isolation, credential
  isolation, CI no-network, DataService-only, and fail-closed policies.
- ShellAccounting broker sandbox adapter authorization test plan:
  [119_shell_accounting_broker_sandbox_adapter_authorization_test_plan.md](119_shell_accounting_broker_sandbox_adapter_authorization_test_plan.md)
  defines TASK-160 static probes for no sandbox/paper/real adapter, no SDK, no
  network call, no credential values, and TASK-159/TASK-154 regression
  preservation.
- ShellAccounting broker sandbox adapter scaffold:
  [120_shell_accounting_broker_sandbox_adapter_scaffold.md](120_shell_accounting_broker_sandbox_adapter_scaffold.md)
  records TASK-161 scaffold-only sandbox broker adapter behavior: not a usable
  broker adapter, not default-wired, CI no-network, no credentials, no real
  broker order id, no order placement, no database write, and no strategy or
  automatic trading.
- ShellAccounting broker sandbox adapter scaffold test plan:
  [121_shell_accounting_broker_sandbox_adapter_scaffold_test_plan.md](121_shell_accounting_broker_sandbox_adapter_scaffold_test_plan.md)
  defines TASK-161 direct response checks, static scans, docs probes, TASK-160
  gate regression checks, and TASK-159/TASK-154 disabled / dry-run regression
  preservation.
- ShellAccounting broker sandbox mode selector:
  [122_shell_accounting_broker_sandbox_mode_selector.md](122_shell_accounting_broker_sandbox_mode_selector.md)
  records TASK-162 disabled-default selector / factory behavior for disabled,
  sandbox scaffold, and unsupported modes without runtime broker enablement.
- ShellAccounting broker sandbox mode selector test plan:
  [123_shell_accounting_broker_sandbox_mode_selector_test_plan.md](123_shell_accounting_broker_sandbox_mode_selector_test_plan.md)
  defines TASK-162 selector response checks, DataServiceActions regression,
  QML/startup exposure scans, and no SDK / no network / no credentials / no
  order-placement probes.
- ShellAccounting broker sandbox runtime selector authorization gate:
  [124_shell_accounting_broker_sandbox_runtime_selector_authorization_gate.md](124_shell_accounting_broker_sandbox_runtime_selector_authorization_gate.md)
  defines TASK-163 future runtime selector wiring authorization boundaries:
  default disabled, DataService-only, fail-closed unknown modes, no runtime
  mode source, no credentials injection, no SDK/network/order placement, and
  rollback to TASK-159 disabled provider.
- ShellAccounting broker sandbox runtime selector authorization test plan:
  [125_shell_accounting_broker_sandbox_runtime_selector_authorization_test_plan.md](125_shell_accounting_broker_sandbox_runtime_selector_authorization_test_plan.md)
  defines TASK-163 static probes for no DataServiceActions runtime wiring, no
  QML/startup/Presenter exposure, no SDK, no network, no credentials, no order
  placement, and TASK-160/161/162 regression coverage.
- ShellAccounting broker sandbox runtime selector disabled wiring:
  [126_shell_accounting_broker_sandbox_runtime_selector_disabled_wiring.md](126_shell_accounting_broker_sandbox_runtime_selector_disabled_wiring.md)
  records TASK-164 DataServiceActions broker dry-run selector wiring through
  `defaultShellAccountingBrokerOrderPortMode()` only, preserving disabled/null
  provider runtime behavior and keeping sandbox runtime unavailable.
- ShellAccounting broker sandbox runtime selector disabled wiring test plan:
  [127_shell_accounting_broker_sandbox_runtime_selector_disabled_wiring_test_plan.md](127_shell_accounting_broker_sandbox_runtime_selector_disabled_wiring_test_plan.md)
  defines TASK-164 static and runtime probes for disabled-default selector
  wiring, no external mode source, no sandbox runtime, and no SDK / network /
  credentials / order-placement side effects.
- ShellAccounting broker runtime mode source authorization gate:
  [128_shell_accounting_broker_runtime_mode_source_authorization_gate.md](128_shell_accounting_broker_runtime_mode_source_authorization_gate.md)
  records TASK-165 gate-only policy for any future broker runtime mode source.
- ShellAccounting broker runtime mode source authorization test plan:
  [129_shell_accounting_broker_runtime_mode_source_authorization_test_plan.md](129_shell_accounting_broker_runtime_mode_source_authorization_test_plan.md)
  defines TASK-165 static probes that keep mode source unimplemented and
  sandbox runtime disabled.
- ShellAccounting broker runtime mode source disabled scaffold:
  [130_shell_accounting_broker_runtime_mode_source_disabled_scaffold.md](130_shell_accounting_broker_runtime_mode_source_disabled_scaffold.md)
  records TASK-166 disabled-only internal source wiring with no external
  runtime source and no sandbox runtime.
- ShellAccounting broker runtime mode source disabled scaffold test plan:
  [131_shell_accounting_broker_runtime_mode_source_disabled_scaffold_test_plan.md](131_shell_accounting_broker_runtime_mode_source_disabled_scaffold_test_plan.md)
  defines TASK-166 source, DataServiceActions, no external source, and no
  side-effect probes.
- ShellAccounting broker sandbox runtime mode source authorization gate:
  [132_shell_accounting_broker_sandbox_runtime_mode_source_authorization_gate.md](132_shell_accounting_broker_sandbox_runtime_mode_source_authorization_gate.md)
  records TASK-167 gate-only policy for any future sandbox runtime mode source.
- ShellAccounting broker sandbox runtime mode source authorization test plan:
  [133_shell_accounting_broker_sandbox_runtime_mode_source_authorization_test_plan.md](133_shell_accounting_broker_sandbox_runtime_mode_source_authorization_test_plan.md)
  defines TASK-167 static and runtime probes that keep sandbox runtime disabled.
- ShellAccounting broker sandbox runtime mode source scaffold:
  [134_shell_accounting_broker_sandbox_runtime_mode_source_scaffold.md](134_shell_accounting_broker_sandbox_runtime_mode_source_scaffold.md)
  records TASK-168 direct-test scaffold while default runtime stays disabled-only.
- ShellAccounting broker sandbox runtime mode source scaffold test plan:
  [135_shell_accounting_broker_sandbox_runtime_mode_source_scaffold_test_plan.md](135_shell_accounting_broker_sandbox_runtime_mode_source_scaffold_test_plan.md)
  defines TASK-168 probes for direct scaffold behavior and no runtime wiring.
- ShellAccounting broker sandbox runtime mode source selector authorization gate:
  [136_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_gate.md](136_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_gate.md)
  records TASK-169 gate-only policy for any future runtime mode source selector.
- ShellAccounting broker sandbox runtime mode source selector authorization test plan:
  [137_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_test_plan.md](137_shell_accounting_broker_sandbox_runtime_mode_source_selector_authorization_test_plan.md)
  defines TASK-169 probes that keep selector implementation and sandbox runtime disabled.
- ShellAccounting broker sandbox runtime mode source selector scaffold:
  [138_shell_accounting_broker_sandbox_runtime_mode_source_selector_scaffold.md](138_shell_accounting_broker_sandbox_runtime_mode_source_selector_scaffold.md)
  records TASK-170 direct-test selector scaffold while runtime stays disabled-only.
- ShellAccounting broker sandbox runtime mode source selector scaffold test plan:
  [139_shell_accounting_broker_sandbox_runtime_mode_source_selector_scaffold_test_plan.md](139_shell_accounting_broker_sandbox_runtime_mode_source_selector_scaffold_test_plan.md)
  defines TASK-170 probes for explicit mode selection, fail-closed behavior, and no runtime wiring.
- ShellAccounting broker sandbox runtime mode source selector wiring authorization gate:
  [140_shell_accounting_broker_sandbox_runtime_mode_source_selector_wiring_authorization_gate.md](140_shell_accounting_broker_sandbox_runtime_mode_source_selector_wiring_authorization_gate.md)
  records TASK-171 gate-only policy for future DataServiceActions selector wiring.
- ShellAccounting broker sandbox runtime mode source selector wiring authorization test plan:
  [141_shell_accounting_broker_sandbox_runtime_mode_source_selector_wiring_authorization_test_plan.md](141_shell_accounting_broker_sandbox_runtime_mode_source_selector_wiring_authorization_test_plan.md)
  defines TASK-171 probes that keep runtime wiring and sandbox runtime disabled.
- ShellAccounting broker sandbox runtime mode source selector disabled wiring:
  [142_shell_accounting_broker_sandbox_runtime_mode_source_selector_disabled_wiring.md](142_shell_accounting_broker_sandbox_runtime_mode_source_selector_disabled_wiring.md)
  records TASK-172 disabled-default selector wiring while sandbox runtime stays disabled.
- ShellAccounting broker sandbox runtime mode source selector disabled wiring test plan:
  [143_shell_accounting_broker_sandbox_runtime_mode_source_selector_disabled_wiring_test_plan.md](143_shell_accounting_broker_sandbox_runtime_mode_source_selector_disabled_wiring_test_plan.md)
  defines TASK-172 probes for default-source-driven selector wiring and disabled/null provider behavior.
- ShellAccounting broker sandbox runtime mode enablement authorization gate:
  [144_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_gate.md](144_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_gate.md)
  records TASK-173 gate-only policy for future sandbox runtime enablement.
- ShellAccounting broker sandbox runtime mode enablement authorization test plan:
  [145_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_test_plan.md](145_shell_accounting_broker_sandbox_runtime_mode_enablement_authorization_test_plan.md)
  defines TASK-173 static and runtime probes that keep sandbox runtime disabled.
- ShellAccounting broker sandbox runtime enablement state scaffold:
  [146_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold.md](146_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold.md)
  records TASK-174 direct-test-only disabled/unavailable/fail-closed state scaffold.
- ShellAccounting broker sandbox runtime enablement state scaffold test plan:
  [147_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold_test_plan.md](147_shell_accounting_broker_sandbox_runtime_enablement_state_scaffold_test_plan.md)
  defines TASK-174 probes proving the scaffold is not connected to runtime.
- ShellAccounting broker sandbox runtime enablement state wiring authorization gate:
  [148_shell_accounting_broker_sandbox_runtime_enablement_state_wiring_authorization_gate.md](148_shell_accounting_broker_sandbox_runtime_enablement_state_wiring_authorization_gate.md)
  records TASK-175 gate-only policy for future enablement state wiring.
- ShellAccounting broker sandbox runtime enablement state wiring authorization test plan:
  [149_shell_accounting_broker_sandbox_runtime_enablement_state_wiring_authorization_test_plan.md](149_shell_accounting_broker_sandbox_runtime_enablement_state_wiring_authorization_test_plan.md)
  defines TASK-175 probes proving no runtime wiring has been implemented.
- ShellAccounting broker sandbox runtime enablement state disabled wiring:
  [150_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring.md](150_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring.md)
  records TASK-176 disabled/fail-closed enablement-state wiring in DataServiceActions.
- ShellAccounting broker sandbox runtime enablement state disabled wiring test plan:
  [151_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring_test_plan.md](151_shell_accounting_broker_sandbox_runtime_enablement_state_disabled_wiring_test_plan.md)
  defines TASK-176 probes proving broker dry-run behavior remains disabled-only.
- ShellAccounting manual transaction and cash movement MVP authorization gate:
  [152_shell_accounting_manual_transaction_cash_movement_mvp_authorization_gate.md](152_shell_accounting_manual_transaction_cash_movement_mvp_authorization_gate.md)
  records TASK-177 gate-only policy for pivoting back to local manual
  accounting records and assisted decision MVP work while broker sandbox new
  capability development is paused.
- ShellAccounting manual transaction and cash movement MVP authorization test plan:
  [153_shell_accounting_manual_transaction_cash_movement_mvp_authorization_test_plan.md](153_shell_accounting_manual_transaction_cash_movement_mvp_authorization_test_plan.md)
  defines TASK-177 static probes for no production implementation, no schema
  change, no broker SDK, no network, no credentials, no order placement, and
  retained broker safety gates.
- ShellAccounting manual transaction and cash movement DTO validation scaffold:
  [154_shell_accounting_manual_transaction_cash_movement_dto_validation_scaffold.md](154_shell_accounting_manual_transaction_cash_movement_dto_validation_scaffold.md)
  records TASK-178 in-memory DTO and validation scaffold for manual buy/sell
  and deposit/withdraw inputs.
- ShellAccounting manual transaction and cash movement DTO validation test plan:
  [155_shell_accounting_manual_transaction_cash_movement_dto_validation_test_plan.md](155_shell_accounting_manual_transaction_cash_movement_dto_validation_test_plan.md)
  defines TASK-178 direct validation and static boundary probes proving no
  DataService action, repository, schema, QML, broker, network, credential, or
  order-placement expansion.
- ShellAccounting manual entry DataService action authorization gate:
  [156_shell_accounting_manual_entry_dataservice_action_authorization_gate.md](156_shell_accounting_manual_entry_dataservice_action_authorization_gate.md)
  records TASK-179 gate-only policy for future DataService manual buy/sell and
  deposit/withdraw actions without implementing actions or writes.
- ShellAccounting manual entry DataService action authorization test plan:
  [157_shell_accounting_manual_entry_dataservice_action_authorization_test_plan.md](157_shell_accounting_manual_entry_dataservice_action_authorization_test_plan.md)
  defines TASK-179 static probes and the TASK-180 scaffold-only evolution:
  action names may be registered as disabled scaffolds, while repositories, DB
  writes, schema/QML/startup drift, and broker changes remain forbidden.
- ShellAccounting manual entry DataService action scaffold:
  [158_shell_accounting_manual_entry_dataservice_action_scaffold.md](158_shell_accounting_manual_entry_dataservice_action_scaffold.md)
  records TASK-180 disabled DataService action registration for manual
  transaction and cash movement entries without any write implementation.
- ShellAccounting manual entry DataService action scaffold test plan:
  [159_shell_accounting_manual_entry_dataservice_action_scaffold_test_plan.md](159_shell_accounting_manual_entry_dataservice_action_scaffold_test_plan.md)
  defines TASK-180 probes for dispatcher registration, disabled response
  semantics, no repository, no DB writes, no QML/startup drift, and broker gate
  retention.
- ShellAccounting manual entry DataService action implementation authorization gate:
  [160_shell_accounting_manual_entry_dataservice_action_implementation_authorization_gate.md](160_shell_accounting_manual_entry_dataservice_action_implementation_authorization_gate.md)
  records TASK-181 gate-only policy for future manual entry DataService action
  implementation without modifying action handlers, repositories, schema, QML,
  replay, broker, network, credentials, or automatic trading.
- ShellAccounting manual entry DataService action implementation authorization test plan:
  [161_shell_accounting_manual_entry_dataservice_action_implementation_authorization_test_plan.md](161_shell_accounting_manual_entry_dataservice_action_implementation_authorization_test_plan.md)
  defines TASK-181 static probes that keep TASK-180 disabled scaffold semantics
  and require later payload parsing, validation wiring, repositories, writes,
  replay, and UI work to remain separately authorized.
- ShellAccounting manual entry DataService action validation wiring:
  [162_shell_accounting_manual_entry_dataservice_action_validation_wiring.md](162_shell_accounting_manual_entry_dataservice_action_validation_wiring.md)
  records TASK-182 payload parsing plus TASK-178 validator wiring for the
  existing manual entry actions while keeping persistence unavailable.
- ShellAccounting manual entry DataService action validation wiring test plan:
  [163_shell_accounting_manual_entry_dataservice_action_validation_wiring_test_plan.md](163_shell_accounting_manual_entry_dataservice_action_validation_wiring_test_plan.md)
  defines TASK-182 dispatcher, validation-only response, no-write, no-UI, and
  broker-gate regression probes.
- ShellAccounting manual entry persistence authorization gate:
  [164_shell_accounting_manual_entry_persistence_authorization_gate.md](164_shell_accounting_manual_entry_persistence_authorization_gate.md)
  records TASK-183 gate-only policy for future manual transaction and cash
  movement persistence while keeping TASK-182 validation-only responses.
- ShellAccounting manual entry persistence authorization test plan:
  [165_shell_accounting_manual_entry_persistence_authorization_test_plan.md](165_shell_accounting_manual_entry_persistence_authorization_test_plan.md)
  defines TASK-183 static probes for no repository, no DB writes, no schema,
  no QML/startup drift, no broker, and future persistence task splitting.
- ShellAccounting manual entry repository scaffold authorization gate:
  [166_shell_accounting_manual_entry_repository_scaffold_authorization_gate.md](166_shell_accounting_manual_entry_repository_scaffold_authorization_gate.md)
  records TASK-184 gate-only policy for a future manual entry DataAccess
  repository scaffold while keeping repository headers/sources, DataAccess
  code, DataServiceActions, DB writes, schema, QML/startup, broker, and
  automatic trading unchanged.
- ShellAccounting manual entry repository scaffold authorization test plan:
  [167_shell_accounting_manual_entry_repository_scaffold_authorization_test_plan.md](167_shell_accounting_manual_entry_repository_scaffold_authorization_test_plan.md)
  defines TASK-184 static probes for no repository scaffold, no repository
  header/source, no SQL/SQLite writes, no DataAccess CMake drift, retained
  TASK-182 validation-only semantics, and retained broker gates.
- ShellAccounting manual entry repository scaffold:
  [168_shell_accounting_manual_entry_repository_scaffold.md](168_shell_accounting_manual_entry_repository_scaffold.md)
  records TASK-185 disabled DataAccess repository scaffold types and methods
  for future manual transaction and cash movement persistence without enabling
  SQL, SQLite writes, DataService action behavior, QML, broker, network, or
  automatic trading.
- ShellAccounting manual entry repository scaffold test plan:
  [169_shell_accounting_manual_entry_repository_scaffold_test_plan.md](169_shell_accounting_manual_entry_repository_scaffold_test_plan.md)
  defines TASK-185 tests for command DTOs, result DTOs, disabled scaffold
  methods, no persistent ids, no writes, no DataService action drift, and
  retained broker gates.
- ShellAccounting manual entry repository implementation authorization gate:
  [170_shell_accounting_manual_entry_repository_implementation_authorization_gate.md](170_shell_accounting_manual_entry_repository_implementation_authorization_gate.md)
  records TASK-186 gate-only policy for future manual entry DataAccess
  repository implementation while keeping the TASK-185 scaffold disabled and
  no-write.
- ShellAccounting manual entry repository implementation authorization test plan:
  [171_shell_accounting_manual_entry_repository_implementation_authorization_test_plan.md](171_shell_accounting_manual_entry_repository_implementation_authorization_test_plan.md)
  defines TASK-186 static probes for no repository implementation, no SQL,
  no SQLite writes, no schema drift, no DataService action drift, and retained
  broker gates.
- ShellAccounting manual entry schema adequacy review gate:
  [172_shell_accounting_manual_entry_schema_adequacy_review_gate.md](172_shell_accounting_manual_entry_schema_adequacy_review_gate.md)
  records TASK-187 review-only schema adequacy findings against
  `migrations/001_initial_schema.sql` for future manual transaction and manual
  cash movement repository implementation.
- ShellAccounting manual entry schema adequacy review test plan:
  [173_shell_accounting_manual_entry_schema_adequacy_review_test_plan.md](173_shell_accounting_manual_entry_schema_adequacy_review_test_plan.md)
  defines TASK-187 static probes for schema source-of-truth review, manual
  transaction and cash movement adequacy evidence/gaps, no schema drift, no
  repository implementation, and retained broker gates.
- ShellAccounting manual entry schema gap authorization gate:
  [174_shell_accounting_manual_entry_schema_gap_authorization_gate.md](174_shell_accounting_manual_entry_schema_gap_authorization_gate.md)
  records TASK-188 gate-only policy for future manual entry schema gap
  resolution while keeping schema, migrations, repository writes, and broker
  capabilities unchanged.
- ShellAccounting manual entry schema gap authorization test plan:
  [175_shell_accounting_manual_entry_schema_gap_authorization_test_plan.md](175_shell_accounting_manual_entry_schema_gap_authorization_test_plan.md)
  defines TASK-188 static probes for future schema implementation boundaries,
  no schema drift, no DataService action drift, no repository implementation,
  no SQL/SQLite writes, and retained broker gates.
- ShellAccounting manual entry schema implementation authorization gate:
  [176_shell_accounting_manual_entry_schema_implementation_authorization_gate.md](176_shell_accounting_manual_entry_schema_implementation_authorization_gate.md)
  records TASK-189 gate-only policy for future manual entry schema
  implementation while keeping schema, migrations, repository writes, and
  broker capabilities unchanged.
- ShellAccounting manual entry schema implementation authorization test plan:
  [177_shell_accounting_manual_entry_schema_implementation_authorization_test_plan.md](177_shell_accounting_manual_entry_schema_implementation_authorization_test_plan.md)
  defines TASK-189 static probes for independent migration authorization,
  migration id / rollback / compatibility policy, no schema drift, no
  DataService action drift, no repository implementation, no SQL/SQLite
  writes, and retained broker gates.
- ShellAccounting manual entry schema migration implementation:
  [178_shell_accounting_manual_entry_schema_migration_implementation.md](178_shell_accounting_manual_entry_schema_migration_implementation.md)
  records TASK-190 schema-only implementation of the independent
  `migrations/002_shell_accounting_manual_entry_schema.sql` migration while
  keeping `migrations/001_initial_schema.sql`, repository writes, DataService
  writes, QML, replay, and broker capabilities unchanged.
- ShellAccounting manual entry schema migration implementation test plan:
  [179_shell_accounting_manual_entry_schema_migration_implementation_test_plan.md](179_shell_accounting_manual_entry_schema_migration_implementation_test_plan.md)
  defines TASK-190 probes for DDL-only migration content, SQLite application,
  `PRAGMA table_info`, `sqlite_master` index checks, no runtime write path, and
  retained broker gates.
- ShellAccounting manual entry repository implementation post-migration authorization gate:
  [180_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_gate.md](180_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_gate.md)
  records TASK-191 gate-only policy for future repository implementation after
  the TASK-190 migration exists, while keeping migrations, repository scaffold,
  DataServiceActions, runtime SQL/SQLite writes, QML, replay, and broker
  capabilities unchanged.
- ShellAccounting manual entry repository implementation post-migration authorization test plan:
  [181_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_test_plan.md](181_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_test_plan.md)
  defines TASK-191 static probes for repository mapping boundaries,
  transaction / rollback / idempotency policy, no runtime write path, no
  migration drift, and retained broker gates.
- ShellAccounting manual transaction repository write implementation:
  [182_shell_accounting_manual_transaction_repository_write_implementation.md](182_shell_accounting_manual_transaction_repository_write_implementation.md)
  records TASK-192 DataAccess-only manual BUY / SELL repository write
  implementation while keeping manual cash movement, DataService write wiring,
  UI, replay, broker, network, credentials, real order, and automatic trading
  out of scope.
- ShellAccounting manual transaction repository write implementation test plan:
  [183_shell_accounting_manual_transaction_repository_write_implementation_test_plan.md](183_shell_accounting_manual_transaction_repository_write_implementation_test_plan.md)
  defines TASK-192 direct SQLite repository write tests for BUY / SELL,
  TASK-190 field mapping, idempotency, rollback, sanitized memo, no audit /
  cash movement write, and retained broker gates.
- ShellAccounting manual cash movement repository write authorization gate:
  [184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md](184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md)
  records TASK-194 gate-only policy for future DataAccess-only manual cash
  movement repository write implementation while keeping DataServiceActions,
  runtime SQL, migrations, QML, replay, broker, network, credentials, real
  order, and automatic trading unchanged.
- ShellAccounting manual cash movement repository write authorization test plan:
  [185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md](185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md)
  defines TASK-194 static probes for cash_adjustment mapping, required
  trade_log cash fact linkage under the current schema contract, movement type
  policy, transaction / rollback / idempotency, privacy, no runtime write path,
  and retained broker gates.
- ShellAccounting manual cash movement schema contract alignment gate:
  [186_shell_accounting_manual_cash_movement_schema_contract_alignment_gate.md](186_shell_accounting_manual_cash_movement_schema_contract_alignment_gate.md)
  records TASK-195 gate-only alignment that current `cash_adjustment` schema
  requires a valid `trade_log_id`, so cash_adjustment-only repository writes
  are blocked unless a future schema migration separately authorizes them.
- ShellAccounting manual cash movement schema contract alignment test plan:
  [187_shell_accounting_manual_cash_movement_schema_contract_alignment_test_plan.md](187_shell_accounting_manual_cash_movement_schema_contract_alignment_test_plan.md)
  defines TASK-195 static probes for the NOT NULL foreign-key contract,
  required future dual-write policy, no schema drift, no repository
  implementation, no runtime write path, and retained broker gates.
- ShellAccounting manual cash movement repository dual-write implementation:
  [188_shell_accounting_manual_cash_movement_repository_dual_write_implementation.md](188_shell_accounting_manual_cash_movement_repository_dual_write_implementation.md)
  records TASK-196 DataAccess-only Deposit / Withdrawal repository dual-write
  for `trade_log` + `cash_adjustment`, while keeping DataServiceActions, QML,
  replay, audit, ledger, broker, network, credentials, real order, and
  automatic trading unchanged.
- ShellAccounting manual cash movement repository dual-write implementation test plan:
  [189_shell_accounting_manual_cash_movement_repository_dual_write_implementation_test_plan.md](189_shell_accounting_manual_cash_movement_repository_dual_write_implementation_test_plan.md)
  defines TASK-196 direct SQLite repository tests for 001 + 002 migrations,
  linkage, idempotency, rollback, sanitization, DataAccess-only boundaries, and
  retained broker gates.
- ShellAccounting manual entry DataService write wiring authorization gate:
  [190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md](190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md)
  records TASK-197 gate-only policy for future DataService runtime write wiring
  through TASK-192 / TASK-196 repositories while keeping current actions
  validation-only.
- ShellAccounting manual entry DataService write wiring authorization test plan:
  [191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md](191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md)
  defines TASK-197 static probes for no DataServiceActions drift, no runtime
  SQL/SQLite writes, no persistent ids, retained validation-only semantics, and
  retained broker gates.
- ShellAccounting manual entry DataService write wiring implementation:
  [192_shell_accounting_manual_entry_dataservice_write_wiring_implementation.md](192_shell_accounting_manual_entry_dataservice_write_wiring_implementation.md)
  records TASK-198 validation-first DataService wiring into the authorized
  TASK-192 manual transaction repository and TASK-196 manual cash movement
  repository while keeping SQL in DataAccess only.
- ShellAccounting manual entry DataService write wiring implementation test plan:
  [193_shell_accounting_manual_entry_dataservice_write_wiring_implementation_test_plan.md](193_shell_accounting_manual_entry_dataservice_write_wiring_implementation_test_plan.md)
  defines TASK-198 runtime and static probes for repository writes,
  idempotency, safe errors, no migrations, no QML/replay, and retained broker
  gates.
- ShellAccounting manual entry QML Presenter authorization gate:
  [194_shell_accounting_manual_entry_qml_presenter_authorization_gate.md](194_shell_accounting_manual_entry_qml_presenter_authorization_gate.md)
  records TASK-199 gate-only policy for future manual entry QML / Presenter
  integration through DataService actions only.
- ShellAccounting manual entry QML Presenter authorization test plan:
  [195_shell_accounting_manual_entry_qml_presenter_authorization_test_plan.md](195_shell_accounting_manual_entry_qml_presenter_authorization_test_plan.md)
  defines TASK-199 static probes for no production QML/startup/Presenter drift,
  no direct SQLite/DataAccess access, no broker/network/credentials, and
  retained TASK-198 / repository / broker regressions.
- ShellAccounting manual entry QML Presenter implementation:
  [196_shell_accounting_manual_entry_qml_presenter_implementation.md](196_shell_accounting_manual_entry_qml_presenter_implementation.md)
  records TASK-200 production ShellAccounting manual entry UI and Presenter /
  Controller / adapter wiring through DataService actions only.
- ShellAccounting manual entry QML Presenter implementation test plan:
  [197_shell_accounting_manual_entry_qml_presenter_implementation_test_plan.md](197_shell_accounting_manual_entry_qml_presenter_implementation_test_plan.md)
  defines TASK-200 static probes for QML bindings, Presenter invokables,
  Controller / adapter boundary, no DataServiceActions or repository drift, no
  direct DB access, and retained broker regressions.
- ShellAccounting manual entry post-write readback refresh authorization gate:
  [198_shell_accounting_manual_entry_post_write_readback_refresh_authorization_gate.md](198_shell_accounting_manual_entry_post_write_readback_refresh_authorization_gate.md)
  records TASK-201 gate-only policy for future manual entry post-write readback,
  refresh, replay, read model refresh, or UI auto-refresh integration.
- ShellAccounting manual entry post-write readback refresh authorization test plan:
  [199_shell_accounting_manual_entry_post_write_readback_refresh_authorization_test_plan.md](199_shell_accounting_manual_entry_post_write_readback_refresh_authorization_test_plan.md)
  defines TASK-201 static probes for no production drift, no direct SQLite or
  DataAccess readback, no replay/read model implementation, no audit/ledger
  write, and retained broker regressions.
- ShellAccounting manual entry post-write readback refresh implementation:
  [200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md](200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md)
  records TASK-202 minimal post-write readback refresh through existing
  DataService read actions after successful manual entry writes.
- ShellAccounting manual entry post-write readback refresh implementation test plan:
  [201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md](201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md)
  defines TASK-202 static probes for Presenter/Controller read-boundary refresh,
  QML refresh status fields, no DataServiceActions or repository drift, no
  replay/snapshot/audit/ledger/broker behavior, and retained regressions.
- ShellAccounting manual entry MVP E2E acceptance authorization gate:
  [202_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_gate.md](202_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_gate.md)
  records TASK-203 gate-only policy for manual entry MVP end-to-end acceptance
  without adding runtime implementation, production drift, replay, audit,
  ledger, broker, real order, or automatic trading behavior.
- ShellAccounting manual entry MVP E2E acceptance authorization test plan:
  [203_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_test_plan.md](203_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_test_plan.md)
  defines TASK-203 documentation, changed-path, boundary, no-drift, and
  retained regression probes for the manual entry MVP acceptance gate.
- ShellAccounting manual entry MVP runtime E2E acceptance:
  [204_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance.md](204_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance.md)
  records TASK-204 runtime E2E acceptance tests over temporary SQLite databases
  and existing DataService action boundaries without production behavior drift.
- ShellAccounting manual entry MVP runtime E2E acceptance test plan:
  [205_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance_test_plan.md](205_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance_test_plan.md)
  defines TASK-204 runtime probes for valid writes, duplicate idempotency,
  invalid no-write behavior, sanitized responses, post-write readback boundary,
  no direct UI DB access, and retained broker gates.
- ShellAccounting manual entry readback replay adequacy review gate:
  [206_shell_accounting_manual_entry_readback_replay_adequacy_review_gate.md](206_shell_accounting_manual_entry_readback_replay_adequacy_review_gate.md)
  records TASK-205 review-gate-only adequacy findings for manual entry facts
  entering readback, future AccountingEngine replay, and daily-use accounting
  pages without production implementation.
- ShellAccounting manual entry readback replay adequacy review test plan:
  [207_shell_accounting_manual_entry_readback_replay_adequacy_review_test_plan.md](207_shell_accounting_manual_entry_readback_replay_adequacy_review_test_plan.md)
  defines TASK-205 static probes for the adequacy review documents, formal gap
  matrix, next task recommendation, no production drift, and retained broker
  gates.
- ShellAccounting manual entry readback mapping authorization gate:
  [208_shell_accounting_manual_entry_readback_mapping_authorization_gate.md](208_shell_accounting_manual_entry_readback_mapping_authorization_gate.md)
  records TASK-206 authorization-only policy for future DataService readback
  mapping into `position.list`, `cash.summary`, and `portfolio.pnl.summary`.
- ShellAccounting manual entry readback mapping authorization test plan:
  [209_shell_accounting_manual_entry_readback_mapping_authorization_test_plan.md](209_shell_accounting_manual_entry_readback_mapping_authorization_test_plan.md)
  defines TASK-206 static probes for docs registration, readback mapping policy,
  production drift, no replay, no broker, and retained regression gates.
- ShellAccounting manual entry readback mapping implementation:
  [210_shell_accounting_manual_entry_readback_mapping_implementation.md](210_shell_accounting_manual_entry_readback_mapping_implementation.md)
  records TASK-207 DataService readback mapping for manual entry facts into
  `position.list`, `cash.summary`, and safe partial `portfolio.pnl.summary`
  without replay, audit / ledger, broker, real order, or automatic trading.
- ShellAccounting manual entry readback mapping implementation test plan:
  [211_shell_accounting_manual_entry_readback_mapping_implementation_test_plan.md](211_shell_accounting_manual_entry_readback_mapping_implementation_test_plan.md)
  defines TASK-207 runtime and static probes for manual entry readback mapping,
  duplicate idempotency, safe PnL unavailability, response sanitization, and
  retained regression gates.
- ShellAccounting manual entry readback daily-use acceptance authorization gate:
  [212_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_gate.md](212_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_gate.md)
  records TASK-208 gate-only daily-use acceptance policy for `position.list`,
  `cash.summary`, and safe partial `portfolio.pnl.summary` after TASK-207.
- ShellAccounting manual entry readback daily-use acceptance authorization test plan:
  [213_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_test_plan.md](213_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_test_plan.md)
  defines TASK-208 static probes for docs registration, MVP ready / not-ready
  conclusion, no production drift, no replay/audit/ledger/broker behavior, and
  retained regression gates.
- ShellAccounting manual entry readback daily-use runtime acceptance:
  [214_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance.md](214_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance.md)
  records TASK-209 runtime acceptance coverage for the BUY + Deposit manual
  entry daily-use baseline.
- ShellAccounting manual entry readback daily-use runtime acceptance test plan:
  [215_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance_test_plan.md](215_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance_test_plan.md)
  defines TASK-209 runtime probes for `position.list`, `cash.summary`, safe
  partial `portfolio.pnl.summary`, readback no-write behavior, privacy, and
  broker / production boundary retention.
- ShellAccounting manual entry SELL / Withdrawal daily-use acceptance authorization gate:
  [216_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.md](216_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.md)
  records TASK-210 gate-only SELL / Withdrawal acceptance policy after the
  TASK-209 BUY + Deposit baseline.
- ShellAccounting manual entry SELL / Withdrawal daily-use acceptance authorization test plan:
  [217_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_test_plan.md](217_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_test_plan.md)
  defines TASK-210 static probes for SELL reduction, Withdrawal cash outflow,
  no fabricated PnL, no replay, no production drift, and retained broker gates.
- ShellAccounting manual entry SELL / Withdrawal daily-use runtime acceptance:
  [218_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance.md](218_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance.md)
  records TASK-211 runtime acceptance coverage for manual SELL reduction,
  sell-exceeds-position safety, Withdrawal cash outflow, insufficient-cash /
  negative-cash safety, no fabricated PnL, and retained no-replay / no-broker
  boundaries.
- ShellAccounting manual entry SELL / Withdrawal daily-use runtime acceptance test plan:
  [219_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance_test_plan.md](219_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance_test_plan.md)
  defines TASK-211 runtime probes for temporary SQLite fixtures, DataService
  manual entry writes, readback no-write behavior, response privacy, production
  boundary retention, and broker / real order gate retention.
- ShellAccounting manual entry replay audit ledger adequacy review gate:
  [220_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_gate.md](220_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_gate.md)
  records TASK-212 review-gate-only adequacy conclusions for future replay,
  audit, ledger, and backup/export/restore productionization.
- ShellAccounting manual entry replay audit ledger adequacy review test plan:
  [221_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_test_plan.md](221_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_test_plan.md)
  defines TASK-212 static probes for docs registration, replay/audit/ledger
  adequacy coverage, backup/export/restore adequacy, gap matrix, formal
  conclusion, no production drift, and retained broker gates.
- ShellAccounting manual entry replay policy authorization gate:
  [222_shell_accounting_manual_entry_replay_policy_authorization_gate.md](222_shell_accounting_manual_entry_replay_policy_authorization_gate.md)
  records TASK-213 policy-only boundaries for future manual entry replay input,
  ordering, position, cash, PnL, output, unsupported states, privacy, and
  integration.
- ShellAccounting manual entry replay policy authorization test plan:
  [223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md](223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md)
  defines TASK-213 static probes for docs registration, gate-only scope, replay
  policy coverage, no production drift, retained manual entry regressions, and
  broker / transport gates.
- ShellAccounting manual entry replay fixture matrix authorization gate:
  [224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md](224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md)
  records TASK-214 gate-only fixture matrix design authorization for manual
  replay fixture naming, source facts, baseline, ordering, position, cash, PnL,
  multi-scope, failure, expected output, privacy, and test boundaries.
- ShellAccounting manual entry replay fixture matrix authorization test plan:
  [225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md](225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md)
  defines TASK-214 static probes for fixture matrix coverage, no fixture-file
  creation, no replay implementation, no production drift, retained manual
  entry regressions, and broker / transport gates.
- ShellAccounting manual entry replay fixture files authorization gate:
  [226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md](226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md)
  records TASK-215 gate-only replay fixture files contract authorization
  without creating fixture files.
- ShellAccounting manual entry replay fixture files authorization test plan:
  [227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md](227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md)
  defines TASK-215 static probes for future fixture directory, naming, index,
  JSON schema, content, privacy, validation, runtime boundary, no fixture
  creation, and no-production-drift checks.
- ShellAccounting manual entry replay fixture files scaffold authorization gate:
  [228_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_gate.md](228_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_gate.md)
  records TASK-216 gate-only scaffold policy for future replay fixture
  directory, file set, fixture index, placeholders, JSON shape, static
  validation, privacy, and runtime boundaries without creating fixture files.
- ShellAccounting manual entry replay fixture files scaffold authorization test plan:
  [229_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_test_plan.md](229_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_test_plan.md)
  defines TASK-216 static probes for documentation registration, no fixture
  creation, no parser / validator / replay implementation, no production drift,
  retained replay and manual entry regressions, broker gates, and transport
  regression.
- ShellAccounting manual entry replay fixture files scaffold:
  [230_shell_accounting_manual_entry_replay_fixture_files_scaffold.md](230_shell_accounting_manual_entry_replay_fixture_files_scaffold.md)
  records TASK-217 test-only replay fixture scaffold files for MRF001-MRF006
  without replay implementation.
- ShellAccounting manual entry replay fixture files scaffold test plan:
  [231_shell_accounting_manual_entry_replay_fixture_files_scaffold_test_plan.md](231_shell_accounting_manual_entry_replay_fixture_files_scaffold_test_plan.md)
  defines TASK-217 static probes for fixture index, JSON parse,
  scaffold-only flags, synthetic-only privacy, no parser / validator, no
  replay, and no-production-drift checks.
- ShellAccounting manual entry replay fixture scaffold static validator authorization gate:
  [232_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_gate.md](232_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_gate.md)
  records TASK-218 gate-only authorization for a future test-only static
  validator over the TASK-217 scaffold files.
- ShellAccounting manual entry replay fixture scaffold static validator authorization test plan:
  [233_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_test_plan.md](233_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_test_plan.md)
  defines TASK-218 static probes for future validator input, output, check
  matrix, fixture index rules, fixture JSON rules, privacy, no-replay,
  no-runtime-dependency, and no-production-drift boundaries.
- ShellAccounting manual entry replay fixture scaffold static validator:
  [234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md](234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md)
  records TASK-219 test-only static validator behavior for the TASK-217
  manual entry replay fixture scaffold files.
- ShellAccounting manual entry replay fixture scaffold static validator test plan:
  [235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md](235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md)
  defines TASK-219 probes for exact fixture file set, index validation,
  fixture JSON validation, forbidden token validation, no-replay, and
  no-production-runtime boundaries.
- ShellAccounting manual entry replay fixture static validator negative fixtures authorization gate:
  [236_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_gate.md](236_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_gate.md)
  records TASK-220 gate-only policy for future negative fixture coverage
  without creating negative fixture files.
- ShellAccounting manual entry replay fixture static validator negative fixtures authorization test plan:
  [237_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_test_plan.md](237_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_test_plan.md)
  defines TASK-220 static probes for negative fixture naming, schema, issue
  codes, changed-path allowlist, no negative fixture files, no replay, and no
  production-runtime boundaries.
- ShellAccounting manual entry replay negative fixtures scaffold authorization gate:
  [238_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_gate.md](238_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_gate.md)
  records TASK-221 gate-only policy for future negative fixture scaffold
  directory, index, file set, metadata, mutation categories, issue codes, and
  no-runtime boundaries without creating negative fixture files.
- ShellAccounting manual entry replay negative fixtures scaffold authorization test plan:
  [239_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_test_plan.md](239_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_test_plan.md)
  defines TASK-221 static probes for docs/238, docs/239, changed-path
  allowlist, no negative fixture directory, no NEG_MRF JSON, no replay, and no
  production-runtime boundaries.
- ShellAccounting manual entry replay negative fixture scaffold files gate:
  [240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md](240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md)
  records TASK-222 test-only creation of the negative fixture scaffold
  directory, index, and ten controlled NEG_MRF JSON files.
- ShellAccounting manual entry replay negative fixture scaffold files test plan:
  [241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md](241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md)
  defines TASK-222 static probes for exact file set, JSON contracts, safe
  scaffold flags, issue-code mapping, positive fixture references, no replay,
  and no production-runtime boundaries.
- ShellAccounting manual entry replay negative fixture static validator authorization gate:
  [242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md](242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md)
  records TASK-223 gate-only policy for future negative fixture static
  validator scope, test-only inputs, sanitized output contract, issue-code
  mapping, failure semantics, privacy, no replay, and no runtime dependency.
- ShellAccounting manual entry replay negative fixture static validator authorization test plan:
  [243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md](243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md)
  defines TASK-223 static probes for docs/242, docs/243, changed-path
  allowlist, unchanged fixture JSON, no TASK-219 validator drift, no future
  validator implementation, and no production-runtime boundaries.
- ShellAccounting manual entry replay negative fixture static validator implementation gate:
  [244_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_gate.md](244_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_gate.md)
  records TASK-224 test-only negative fixture static validator scope, allowed
  fixture inputs, issue-code mapping, sanitized output, no replay, and
  no-production-runtime boundaries.
- ShellAccounting manual entry replay negative fixture static validator implementation test plan:
  [245_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_test_plan.md](245_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_test_plan.md)
  defines TASK-224 probes for docs/244, docs/245, CTest registration, exact
  negative fixture file-set checks, positive fixture source mapping,
  sanitized summary output, unchanged fixtures, and full regression.
- ShellAccounting manual entry replay negative fixture static validator regression matrix gate:
  [246_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.md](246_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.md)
  records TASK-225 test-only regression matrix policy for the TASK-224
  validator summary output, ten NEG_MRF mappings, no replay, and no production
  runtime boundary.
- ShellAccounting manual entry replay negative fixture static validator regression matrix test plan:
  [247_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_test_plan.md](247_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_test_plan.md)
  defines TASK-225 probes for docs/246, docs/247, summary JSON schema,
  issue-count and issue-code mapping, sanitized output, unchanged fixtures,
  no production drift, and full regression.
- ShellAccounting manual entry replay negative fixture static validator failure-mode hardening gate:
  [248_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.md](248_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.md)
  records TASK-226 test-only failure-mode hardening policy for temporary
  mutated negative fixture copies, sanitized failures, no replay, and no
  production runtime boundary.
- ShellAccounting manual entry replay negative fixture static validator failure-mode hardening test plan:
  [249_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_test_plan.md](249_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_test_plan.md)
  defines TASK-226 probes for bad input non-zero exits, sanitized failure JSON,
  unchanged fixtures, no production drift, and full regression.
- ShellAccounting manual entry replay negative fixture validator CI closeout gate:
  [250_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_gate.md](250_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_gate.md)
  records TASK-227 static CI closeout policy for TASK-220 through TASK-226,
  clean-main invariants, no recursive full CTest inside the gate, unchanged
  fixtures, unchanged validator validation behavior, exact-path TASK-224
  validator changed-set self-consistency, and no production drift.
- ShellAccounting manual entry replay negative fixture validator CI closeout test plan:
  [251_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_test_plan.md](251_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_test_plan.md)
  defines TASK-227 probes for docs/250, docs/251, CTest registration,
  critical gate registration, changed-path fail-closed behavior, fixture
  immutability, validator immutability, and external full CTest regression.
- ShellAccounting manual entry replay negative fixture validator phase closeout and handoff gate:
  [252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md](252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md)
  records TASK-228 phase closeout and handoff policy for TASK-220 through
  TASK-227 artifact inventory, validation inventory, CTest inventory,
  boundary inventory, clean-main invariants, and next-phase entry conditions.
- ShellAccounting manual entry replay negative fixture validator phase closeout and handoff test plan:
  [253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md](253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md)
  defines TASK-228 probes for docs/252, docs/253, CTest registration,
  artifact inventory, fixture immutability, validator behavior boundaries,
  production no-drift, and external full CTest regression.
- ShellAccounting manual entry replay next-phase authorization planning gate:
  [254_shell_accounting_manual_entry_replay_next_phase_authorization_planning_gate.md](254_shell_accounting_manual_entry_replay_next_phase_authorization_planning_gate.md)
  records TASK-229 next-phase planning boundaries, future candidate tracks,
  required preconditions, risk register, explicit non-authorization list, and
  Go / No-Go handoff rules after TASK-220 through TASK-228.
- ShellAccounting manual entry replay next-phase authorization planning test plan:
  [255_shell_accounting_manual_entry_replay_next_phase_authorization_planning_test_plan.md](255_shell_accounting_manual_entry_replay_next_phase_authorization_planning_test_plan.md)
  defines TASK-229 probes for docs/254, docs/255, CTest registration,
  TASK-228 phase closeout retention, fixture immutability, validator behavior
  immutability, production no-drift, and external full CTest regression.
- ShellAccounting manual entry replay test-only dry-run harness authorization gate:
  [256_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_gate.md](256_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_gate.md)
  records TASK-230 future harness directory, CTest name, input and output
  boundary, no-write, no-replay, no-AccountingEngine, no-runtime SQL, and
  no-production authorization constraints.
- ShellAccounting manual entry replay test-only dry-run harness authorization test plan:
  [257_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_test_plan.md](257_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_test_plan.md)
  defines TASK-230 probes for docs/256, docs/257, CTest registration, absent
  harness implementation, fixture immutability, validator behavior immutability,
  production no-drift, and external full CTest regression.
- ShellAccounting manual entry replay test-only dry-run harness implementation gate:
  [258_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_gate.md](258_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_gate.md)
  records TASK-231 test-only dry-run harness scope, fixture input boundary,
  sanitized summary output, no-write, no-replay, no-AccountingEngine,
  no-runtime SQL, and no-production constraints.
- ShellAccounting manual entry replay test-only dry-run harness implementation test plan:
  [259_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_test_plan.md](259_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_test_plan.md)
  defines TASK-231 probes for the test-only harness CTest, fixture
  immutability, sanitized output, validator behavior immutability, production
  no-drift, and external full CTest regression.
- ShellAccounting manual entry replay test-only dry-run harness regression matrix gate:
  [260_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.md](260_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.md)
  records TASK-232 regression matrix policy for the TASK-231 dry-run harness
  summary JSON, positive and negative fixture coverage, fixture immutability,
  no replay, no AccountingEngine, no runtime writes, and no production drift.
- ShellAccounting manual entry replay test-only dry-run harness regression matrix test plan:
  [261_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_test_plan.md](261_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_test_plan.md)
  defines TASK-232 probes for docs/260, docs/261, CTest registration, harness
  invocation, sanitized summary JSON, fixture coverage, no writes, no replay,
  no AccountingEngine, and external full CTest regression.
- ShellAccounting manual entry replay implementation authorization gate:
  [262_shell_accounting_manual_entry_replay_implementation_authorization_gate.md](262_shell_accounting_manual_entry_replay_implementation_authorization_gate.md)
  records TASK-233 future replay implementation directory, CTest name, input
  and output boundary, AccountingEngine boundary, no-write boundary, runtime
  SQL / ledger boundary, production integration boundary, and broker/network
  non-authorization constraints.
- ShellAccounting manual entry replay implementation authorization test plan:
  [263_shell_accounting_manual_entry_replay_implementation_authorization_test_plan.md](263_shell_accounting_manual_entry_replay_implementation_authorization_test_plan.md)
  defines TASK-233 probes for docs/262, docs/263, CTest registration, absent
  replay implementation, fixture immutability, harness behavior immutability,
  validator behavior immutability, production no-drift, and external full
  CTest regression.
- ShellAccounting manual entry replay test-only implementation gate:
  [264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md](264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md)
  records TASK-234 test-only replay implementation shell scope, dry-run harness
  summary input, sanitized report output, no AccountingEngine replay,
  no-runtime-write, no-production, and no-broker constraints.
- ShellAccounting manual entry replay test-only implementation test plan:
  [265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md](265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md)
  defines TASK-234 probes for the implementation CTest, TASK-233 authorization
  gate, TASK-231/TASK-232 dry-run harness gates, fixture immutability, sanitized
  output, production no-drift, and external full CTest regression.
- ShellAccounting manual entry replay test-only implementation regression matrix gate:
  [266_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_gate.md](266_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_gate.md)
  records TASK-235 regression matrix policy for the TASK-234 test-only
  implementation summary JSON, fixture coverage, fixture immutability, no
  AccountingEngine replay, no runtime writes, no real position/cash/PnL, and no
  production drift.
- ShellAccounting manual entry replay test-only implementation regression matrix test plan:
  [267_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_test_plan.md](267_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_test_plan.md)
  defines TASK-235 probes for the regression matrix CTest, TASK-234
  implementation, TASK-233 authorization gate, TASK-231/TASK-232 harness
  regression, sanitized output, fixture immutability, production no-drift, and
  external full CTest regression.
- ShellAccounting manual entry replay test-only implementation failure-mode hardening gate:
  [268_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_gate.md](268_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_gate.md)
  records TASK-236 failure-mode policy for temporary bad dry-run summary inputs,
  sanitized failure JSON, fixture immutability, no AccountingEngine replay, no
  runtime writes, no production drift, and no broker/network behavior.
- ShellAccounting manual entry replay test-only implementation failure-mode hardening test plan:
  [269_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_test_plan.md](269_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_test_plan.md)
  defines TASK-236 probes for malformed dry-run summaries, sanitized failure
  output, TASK-235 regression matrix, TASK-234 implementation, fixture
  immutability, production no-drift, and external full CTest regression.
- ShellAccounting manual entry replay test-only implementation CI closeout gate:
  [270_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_gate.md](270_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_gate.md)
  records TASK-237 CI closeout policy for the TASK-233 through TASK-236
  test-only implementation chain, clean-main safety, CTest registration,
  fixture immutability, no recursive full CTest, and no production drift.
- ShellAccounting manual entry replay test-only implementation CI closeout test plan:
  [271_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_test_plan.md](271_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_test_plan.md)
  defines TASK-237 probes for required artifacts, critical CTest registration,
  clean-main invariants, no AccountingEngine replay, no runtime writes, broker
  boundaries, and external full CTest regression.
- ShellAccounting manual entry replay test-only implementation phase closeout and handoff gate:
  [272_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_gate.md](272_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_gate.md)
  records TASK-238 phase artifact inventory, validation inventory, boundary
  inventory, clean-main invariants, and next-phase handoff conditions.
- ShellAccounting manual entry replay test-only implementation phase closeout and handoff test plan:
  [273_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_test_plan.md](273_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_test_plan.md)
  defines TASK-238 probes for required artifacts, critical CTest registration,
  changed-path exact allowlists, fixture immutability, implementation behavior,
  harness behavior, validator behavior, and production no-drift boundaries.
- ShellAccounting manual entry replay AccountingEngine adequacy review authorization gate:
  [274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md](274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md)
  records TASK-239 planning-only authorization for a future test-only
  AccountingEngine adequacy review without creating the future review directory,
  registering the future review CTest, calling AccountingEngine replay, or
  modifying production code.
- ShellAccounting manual entry replay AccountingEngine adequacy review authorization test plan:
  [275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md](275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md)
  defines TASK-239 probes for docs, indexes, CTest registration, clean-main
  changed-path behavior, fixture immutability, AccountingEngine no-call
  boundaries, no runtime writes, no read model refresh, and no broker drift.
- ShellAccounting manual entry replay AccountingEngine adequacy review implementation gate:
  [276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md](276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md)
  records TASK-240 review gate implementation scope, static adequacy matrix,
  forbidden side effects, formal conclusion, and next-phase handoff without
  creating an AccountingEngine bridge or calling AccountingEngine replay.
- ShellAccounting manual entry replay AccountingEngine adequacy review implementation test plan:
  [277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md](277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md)
  defines TASK-240 probes for docs, indexes, CTest registration, clean-main
  changed-path behavior, exact allowlists, fixture immutability, no
  AccountingEngine code drift, no runtime writes, no read model refresh, and
  no broker drift.
- ShellAccounting manual entry replay AccountingEngine adequacy review regression matrix gate:
  [278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md](278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md)
  records TASK-241 review-only regression matrix scope, review input matrix,
  no-bridge, no-replay, no-write, AccountingEngine code, production integration,
  Go / No-Go, and next-phase handoff boundaries.
- ShellAccounting manual entry replay AccountingEngine adequacy review regression matrix test plan:
  [279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md](279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md)
  defines TASK-241 matrix checks, exact changed-path allowlist, regression
  probes, failure modes, acceptance criteria, and formal conclusion without
  authorizing AccountingEngine replay or bridge implementation.
- ShellAccounting manual entry replay AccountingEngine adequacy review failure-mode hardening gate:
  [280_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.md](280_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.md)
  records TASK-242 failure-mode hardening for review-only AccountingEngine
  adequacy gates, temporary bad input probes, sanitized output, no-bridge,
  no-replay, no-write, and no production integration boundaries.
- ShellAccounting manual entry replay AccountingEngine adequacy review failure-mode hardening test plan:
  [281_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_test_plan.md](281_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_test_plan.md)
  defines TASK-242 temporary input probes, failure matrix, changed-path
  allowlist, regression gates, acceptance criteria, and formal conclusion
- ShellAccounting manual entry replay AccountingEngine adequacy review CI closeout gate:
  [282_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.md](282_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.md)
  records TASK-243 review-only CI closeout boundaries for TASK-239 through
  TASK-242, clean-main invariants, no recursive full CTest, no bridge,
  no replay, no write, and no production integration.
- ShellAccounting manual entry replay AccountingEngine adequacy review CI closeout test plan:
  [283_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_test_plan.md](283_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_test_plan.md)
  defines TASK-243 CI checks, clean-main checks, exact changed-path allowlist,
  regression gates, failure modes, acceptance criteria, and formal conclusion.
  without authorizing AccountingEngine replay or bridge implementation.
- ShellAccounting manual entry replay AccountingEngine adequacy review phase closeout handoff gate:
  [284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md](284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md)
  records TASK-244 review-only phase closeout and handoff boundaries for
  TASK-239 through TASK-243, no bridge, no replay, no write, no read model
  refresh, no production integration, and no broker/network drift.
- ShellAccounting manual entry replay AccountingEngine adequacy review phase closeout handoff test plan:
  [285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md](285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md)
  defines TASK-244 static checks, regression gates, exact changed-path
  allowlist, phase closeout evidence, handoff checks, failure modes,
  acceptance criteria, and formal conclusion.
- ShellAccounting manual entry replay AccountingEngine bridge authorization gate:
  [286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md](286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md)
  records TASK-245 authorization-only bridge policy, future bridge directory
  candidate, input/output contract boundaries, no replay, no write, no
  production integration, and no broker/network constraints.
- ShellAccounting manual entry replay AccountingEngine bridge authorization test plan:
  [287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md](287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md)
  defines TASK-245 static checks, regression gates, exact changed-path
  allowlist, authorization checks, future bridge contract checks, failure
  modes, acceptance criteria, and formal conclusion.
- ShellAccounting manual entry replay AccountingEngine bridge test-only implementation gate:
  [288_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_gate.md](288_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_gate.md)
  records TASK-246 test-only bridge contract shell scope, sanitized request /
  response contract boundaries, sanitized failure behavior, no AccountingEngine
  replay, no runtime write, no production integration, and no broker/network
  constraints.
- ShellAccounting manual entry replay AccountingEngine bridge test-only implementation test plan:
  [289_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_test_plan.md](289_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_test_plan.md)
  defines TASK-246 static checks, bridge CLI probes, sanitized output checks,
  exact changed-path allowlist, regression gates, failure modes, acceptance
  criteria, and formal conclusion.
- ShellAccounting manual entry replay AccountingEngine bridge regression matrix gate:
  [290_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_gate.md](290_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_gate.md)
  records TASK-247 test-only bridge regression matrix scope, positive matrix,
  failure matrix, sanitized output matrix, no AccountingEngine call, no replay,
  no write, no production integration, and no broker/network constraints.
- ShellAccounting manual entry replay AccountingEngine bridge regression matrix test plan:
  [291_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_test_plan.md](291_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_test_plan.md)
  defines TASK-247 static checks, bridge matrix cases, failure modes, exact
  changed-path allowlist, regression gates, acceptance criteria, and formal
  conclusion.
- ShellAccounting manual entry replay AccountingEngine bridge failure-mode hardening gate:
  [292_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.md](292_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.md)
  records TASK-248 test-only bridge failure-mode hardening scope, stable issue
  codes, sanitized output, fail-closed matrix, no AccountingEngine call, no
  replay, no write, no production integration, and no broker/network
  constraints.
- ShellAccounting manual entry replay AccountingEngine bridge failure-mode hardening test plan:
  [293_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_test_plan.md](293_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_test_plan.md)
  defines TASK-248 static checks, regression gates, changed-path allowlist,
  failure-mode cases, sanitization checks, acceptance criteria, and formal
  conclusion.
- ShellAccounting manual entry replay AccountingEngine bridge CI closeout gate:
  [294_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_gate.md](294_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_gate.md)
  records TASK-249 CI closeout scope for the test-only bridge phase, required
  bridge gate set, regression evidence, clean-main invariants, no AccountingEngine
  call, no replay, no write, no production integration, and no broker/network
  constraints.
- ShellAccounting manual entry replay AccountingEngine bridge CI closeout test plan:
  [295_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_test_plan.md](295_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_test_plan.md)
  defines TASK-249 static checks, required regression gates, changed-path
  allowlist, clean-main checks, failure modes, acceptance criteria, and formal
  conclusion.
- ShellAccounting manual entry replay AccountingEngine bridge phase closeout and handoff gate:
  [296_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_gate.md](296_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_gate.md)
  records TASK-250 phase closeout scope for the test-only bridge phase, CI
  evidence, bridge contract evidence, failure-mode evidence, handoff
  preconditions, no AccountingEngine call, no replay, no write, no production
  integration, and no broker/network constraints.
- ShellAccounting manual entry replay AccountingEngine bridge phase closeout and handoff test plan:
  [297_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_test_plan.md](297_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_test_plan.md)
  defines TASK-250 static checks, required regression gates, changed-path
  allowlist, phase closeout evidence, handoff checks, failure modes, acceptance
  criteria, and formal conclusion.
- ShellAccounting manual entry replay AccountingEngine runtime integration authorization gate:
  [298_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_gate.md](298_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_gate.md)
  records TASK-251 authorization-only runtime integration policy, future entry
  points, AccountingEngine call authorization, replay boundary, runtime write
  boundary, production code boundary, fixture boundary, and broker/network
  constraints.
- ShellAccounting manual entry replay AccountingEngine runtime integration authorization test plan:
  [299_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_test_plan.md](299_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_test_plan.md)
  defines TASK-251 static checks, required regression gates, changed-path
  allowlist, authorization checks, runtime integration boundary checks, failure
  modes, acceptance criteria, and formal conclusion.
- ShellAccounting manual entry replay read-only runtime integration vertical slice gate:
  [300_shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.md](300_shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.md)
  records TASK-252 test-only read-only vertical slice scope, AccountingEngine
  read-only replay evidence, in-memory projection summary, no-write boundary,
  and no broker/network constraints.
- ShellAccounting manual entry replay read-only runtime integration vertical slice test plan:
  [301_shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice_test_plan.md](301_shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice_test_plan.md)
  defines TASK-252 runtime evidence checks, no-write checks, no-trading checks,
  production boundary checks, failure modes, changed-path allowlist, and
  regression requirements. The next handoff is TASK-253 ShellAccounting manual entry replay fixture-backed VBA parity read-only vertical slice.
- ShellAccounting manual entry replay fixture-backed VBA parity read-only vertical slice:
  [302_shell_accounting_manual_entry_replay_fixture_backed_vba_parity_readonly_vertical_slice.md](302_shell_accounting_manual_entry_replay_fixture_backed_vba_parity_readonly_vertical_slice.md)
  records TASK-253 fixture-backed read-only replay scope, TASK-253-only fixture
  boundary, AccountingEngine read-only replay evidence, exact projection parity,
  no-write boundary, and no broker/network constraints.
- ShellAccounting manual entry replay fixture-backed VBA parity test plan:
  [303_shell_accounting_manual_entry_replay_fixture_backed_vba_parity_test_plan.md](303_shell_accounting_manual_entry_replay_fixture_backed_vba_parity_test_plan.md)
  defines TASK-253 fixture checks, runtime evidence checks, no-write checks,
  no-trading checks, production boundary checks, required commands, and formal
  conclusion.
- ShellAccounting manual entry replay fixture parity matrix read-only vertical slice:
  [304_shell_accounting_manual_entry_replay_fixture_parity_matrix_readonly_vertical_slice.md](304_shell_accounting_manual_entry_replay_fixture_parity_matrix_readonly_vertical_slice.md)
  records TASK-254 fixture matrix scope, TASK-254-only fixture boundary,
  AccountingEngine read-only replay evidence, exact projection / sanitized issue
  parity, no-write boundary, and no broker/network constraints.
- ShellAccounting manual entry replay fixture parity matrix test plan:
  [305_shell_accounting_manual_entry_replay_fixture_parity_matrix_test_plan.md](305_shell_accounting_manual_entry_replay_fixture_parity_matrix_test_plan.md)
  defines TASK-254 matrix checks, runtime evidence checks, per-fixture parity
  checks, no-write checks, no-trading checks, production boundary checks, and
  required commands.
- ShellAccounting manual entry replay Excel/VBA export sample import read-only vertical slice:
  [306_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice.md](306_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice.md)
  records TASK-255 sanitized sample import scope, test-only importer boundary,
  AccountingEngine read-only replay evidence, parity checks, no-write boundary,
  and no broker/network constraints.
- ShellAccounting manual entry replay Excel/VBA export sample import test plan:
  [307_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_test_plan.md](307_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_test_plan.md)
  defines TASK-255 sample loading, importer, replay evidence, parity,
  no-write, no-trading, production boundary, and required command checks.
- ShellAccounting manual entry replay Excel/VBA export sample import matrix diagnostics read-only vertical slice:
  [308_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_readonly_vertical_slice.md](308_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_readonly_vertical_slice.md)
  records TASK-256 sanitized sample matrix scope, normalized diagnostics,
  test-only importer boundary, AccountingEngine read-only replay evidence,
  parity checks, no-write boundary, and no broker/network constraints.
- ShellAccounting manual entry replay Excel/VBA export sample import matrix diagnostics test plan:
  [309_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_test_plan.md](309_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_test_plan.md)
  defines TASK-256 matrix loading, importer diagnostics, replay evidence,
  parity / diagnostic checks, no-write, no-trading, production boundary, and
  required command checks.
- ShellAccounting manual entry replay Excel/VBA import read-only production parser boundary slice:
  [310_shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary.md](310_shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary.md)
  records TASK-257 production read-only parser boundary scope, sanitized
  diagnostics, replay fact DTO construction, test-only AccountingEngine replay
  evidence, no-write boundary, and no broker/network constraints.
- ShellAccounting manual entry replay Excel/VBA import read-only production parser boundary test plan:
  [311_shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary_test_plan.md](311_shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary_test_plan.md)
  defines TASK-257 parser boundary checks, sample diagnostics, read-only replay
  evidence, parity assertions, no-write, no-trading, production boundary, and
  required command checks.
- ShellAccounting Excel/VBA import read-only DataService preview action:
  [312_shell_accounting_excel_vba_import_readonly_dataservice_preview_action.md](312_shell_accounting_excel_vba_import_readonly_dataservice_preview_action.md)
  records TASK-258 read-only DataService preview action scope, TASK-257 parser
  boundary usage, normalized diagnostics, replay fact summary response, no-write
  boundary, and no broker/network constraints.
- ShellAccounting Excel/VBA import read-only DataService preview action test plan:
  [313_shell_accounting_excel_vba_import_readonly_dataservice_preview_action_test_plan.md](313_shell_accounting_excel_vba_import_readonly_dataservice_preview_action_test_plan.md)
  defines TASK-258 action registration, parser boundary, preview diagnostics,
  fact summary, no-write, no-trading, production boundary, and required command
  checks.
- ShellAccounting Excel/VBA import read-only preview client adapter:
  [314_shell_accounting_excel_vba_import_readonly_preview_client_adapter.md](314_shell_accounting_excel_vba_import_readonly_preview_client_adapter.md)
  records TASK-259 typed client adapter scope, TASK-258 read-only action usage,
  normalized diagnostic / replay fact summary mapping, no-write boundary, and
  no broker/network constraints.
- ShellAccounting Excel/VBA import read-only preview client adapter test plan:
  [315_shell_accounting_excel_vba_import_readonly_preview_client_adapter_test_plan.md](315_shell_accounting_excel_vba_import_readonly_preview_client_adapter_test_plan.md)
  defines TASK-259 client adapter mapping, DataService action call, sanitized
  diagnostics, fact summary, no-write, no-trading, production boundary, and
  required command checks.
- ShellAccounting Excel/VBA import read-only preview ShellServices Presenter contract:
  [316_shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract.md](316_shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract.md)
  records TASK-260 ShellServices / Controller / Presenter preview contract,
  typed client boundary usage, diagnostics and fact summary mapping, no-write
  boundary, and no broker/network constraints.
- ShellAccounting Excel/VBA import read-only preview ShellServices Presenter contract test plan:
  [317_shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract_test_plan.md](317_shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract_test_plan.md)
  defines TASK-260 Presenter state, Controller delegation, ShellServices
  mapping, typed client call boundary, local fail-closed input checks, no-write,
  no-trading, production boundary, and required command checks.
- ShellAccounting Excel/VBA import read-only preview QML panel wiring:
  [318_shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring.md](318_shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring.md)
  records TASK-261 read-only QML panel scope, Presenter preview method usage,
  visible status / diagnostics / fact count bindings, no direct service access,
  and no-write / no-broker constraints.
- ShellAccounting Excel/VBA import read-only preview QML panel wiring test plan:
  [319_shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring_test_plan.md](319_shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring_test_plan.md)
  defines TASK-261 QML object contract, Presenter invocation, property binding,
  reset wiring, FileDialog and direct dependency bans, production boundary, and
  required command checks.
- ShellAccounting Excel/VBA import read-only local export JSON file loader preview:
  [320_shell_accounting_excel_vba_import_readonly_local_export_json_file_loader_preview.md](320_shell_accounting_excel_vba_import_readonly_local_export_json_file_loader_preview.md)
  records TASK-262 local-file-only loader scope, QML FileDialog handoff,
  ShellServices file reading boundary, sanitized basename display, existing
  Presenter preview path reuse, and no-write / no-broker constraints.
- ShellAccounting Excel/VBA import read-only local export JSON file loader preview test plan:
  [321_shell_accounting_excel_vba_import_readonly_local_export_json_file_loader_preview_test_plan.md](321_shell_accounting_excel_vba_import_readonly_local_export_json_file_loader_preview_test_plan.md)
  defines TASK-262 loader validation, Presenter file preview invocation, QML
  file-control wiring, sanitized error handling, no direct QML file read, and
  required command checks.
- ShellAccounting Excel/VBA import read-only preview acceptance UX and export format contract:
  [322_shell_accounting_excel_vba_import_readonly_preview_acceptance_ux_export_format_contract.md](322_shell_accounting_excel_vba_import_readonly_preview_acceptance_ux_export_format_contract.md)
  records TASK-263 acceptance state visibility, compact sanitized export format
  contract using `schemaVersion=excel-vba-export/v1` and
  `source=sanitized-excel-vba-export`, sample preview payload behavior,
  continued pasted/local preview support, and no-write / no-broker constraints.
- ShellAccounting Excel/VBA import read-only preview acceptance UX and export format contract test plan:
  [323_shell_accounting_excel_vba_import_readonly_preview_acceptance_ux_export_format_contract_test_plan.md](323_shell_accounting_excel_vba_import_readonly_preview_acceptance_ux_export_format_contract_test_plan.md)
  defines TASK-263 QML object contract, acceptance summary checks, sample
  payload preview path reuse, sanitized issue checks, no import/persist
  controls, and required command checks.
- ShellAccounting Excel/VBA import preview to manual entry persistence authorization gate:
  [324_shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_gate.md](324_shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_gate.md)
  records TASK-264 `ACCEPTED` preview requirement, DataService-only future
  write boundary, transaction / rollback / idempotency / duplicate import
  policy, minimal persistence request DTO, and no-write / no-broker constraints.
- ShellAccounting Excel/VBA import preview to manual entry persistence authorization test plan:
  [325_shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_test_plan.md](325_shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_test_plan.md)
  defines TASK-264 docs-only authorization gate checks, forbidden production
  paths, sanitized error mapping assertions, no Import UI checks, and required
  command checks.
- ShellAccounting Excel/VBA import accepted preview manual entry persistence implementation:
  [326_shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence_implementation.md](326_shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence_implementation.md)
  records TASK-265 DataService write action registration, parser-boundary
  reuse, DataAccess single-transaction composition, idempotency handling,
  rollback behavior, and no UI / no broker constraints.
- ShellAccounting Excel/VBA import accepted preview manual entry persistence test plan:
  [327_shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence_test_plan.md](327_shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence_test_plan.md)
  defines TASK-265 temporary SQLite persistence checks, active transaction
  method checks, duplicate / conflict / rollback assertions, no direct
  DataService `trade_log` SQL checks, and required command checks.
- ShellAccounting Excel/VBA import persist manual entry client adapter:
  [328_shell_accounting_excel_vba_import_persist_manual_entry_client_adapter.md](328_shell_accounting_excel_vba_import_persist_manual_entry_client_adapter.md)
  records TASK-266 typed DataServiceClient adapter scope, request / response
  mapping, sanitized issue mapping, and no UI / no broker constraints.
- ShellAccounting Excel/VBA import persist manual entry client adapter test plan:
  [329_shell_accounting_excel_vba_import_persist_manual_entry_client_adapter_test_plan.md](329_shell_accounting_excel_vba_import_persist_manual_entry_client_adapter_test_plan.md)
  defines TASK-266 local DataService harness checks, duplicate / conflict
  mapping assertions, client boundary scans, and required command checks.
- ShellAccounting Excel/VBA import persist manual entry ShellServices Presenter contract:
  [330_shell_accounting_excel_vba_import_persist_manual_entry_shellservices_presenter_contract.md](330_shell_accounting_excel_vba_import_persist_manual_entry_shellservices_presenter_contract.md)
  records TASK-267 ShellServices / Controller / Presenter C++ contract scope,
  accepted preview fail-closed mapping, typed client port boundary, and no QML
  / no broker constraints.
- ShellAccounting Excel/VBA import persist manual entry ShellServices Presenter contract test plan:
  [331_shell_accounting_excel_vba_import_persist_manual_entry_shellservices_presenter_contract_test_plan.md](331_shell_accounting_excel_vba_import_persist_manual_entry_shellservices_presenter_contract_test_plan.md)
  defines TASK-267 fake-port Presenter mapping checks, duplicate / conflict
  view-model assertions, static boundary scans, and required command checks.
- ShellAccounting Excel/VBA import accepted preview persist QML wiring:
  [332_shell_accounting_excel_vba_import_accepted_preview_persist_qml_wiring.md](332_shell_accounting_excel_vba_import_accepted_preview_persist_qml_wiring.md)
  records TASK-268 QML Persist accepted preview control wiring, explicit user
  confirmation, Presenter-only invocation, accepted preview gating, and no
  direct DataService / SQLite / AccountingEngine access.
- ShellAccounting Excel/VBA import accepted preview persist QML wiring test plan:
  [333_shell_accounting_excel_vba_import_accepted_preview_persist_qml_wiring_test_plan.md](333_shell_accounting_excel_vba_import_accepted_preview_persist_qml_wiring_test_plan.md)
  defines TASK-268 QML object checks, Presenter invokable checks, fail-closed
  mapping assertions, duplicate / conflict visibility checks, and required
  command checks.
- ShellAccounting Excel/VBA import persist supported fact types alignment:
  [334_shell_accounting_excel_vba_import_persist_supported_fact_types_alignment.md](334_shell_accounting_excel_vba_import_persist_supported_fact_types_alignment.md)
  records TASK-269 trade and cash persistence support, market / FX rejection,
  and aligned QML / Presenter / DataService / DataAccess boundaries.
- ShellAccounting Excel/VBA import persist supported fact types alignment test plan:
  [335_shell_accounting_excel_vba_import_persist_supported_fact_types_alignment_test_plan.md](335_shell_accounting_excel_vba_import_persist_supported_fact_types_alignment_test_plan.md)
  defines TASK-269 cash-only, trade+cash, unsupported market / FX, duplicate,
  rollback, and old row-count regression checks.
- ShellAccounting Excel/VBA import persist post-write readback refresh:
  [336_shell_accounting_excel_vba_import_persist_post_write_readback_refresh.md](336_shell_accounting_excel_vba_import_persist_post_write_readback_refresh.md)
  records TASK-270 successful persistence to existing read-only readback
  refresh wiring, persisted row-count visibility, duplicate skip, conflict
  skip, and sanitized refresh failure handling.
- ShellAccounting Excel/VBA import persist post-write readback refresh test plan:
  [337_shell_accounting_excel_vba_import_persist_post_write_readback_refresh_test_plan.md](337_shell_accounting_excel_vba_import_persist_post_write_readback_refresh_test_plan.md)
  defines TASK-270 fake-port Presenter/QML checks for refresh success,
  duplicate no-new-row behavior, conflict no-refresh behavior, and refresh
  failure evidence.
- ShellAccounting Excel/VBA import MVP local service E2E acceptance:
  [338_shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance.md](338_shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance.md)
  records TASK-271 local service host, real DataServiceClient, Presenter
  preview/persist, temporary SQLite row-count, duplicate, conflict, and
  readback refresh acceptance scope.
- ShellAccounting Excel/VBA import MVP local service E2E acceptance test plan:
  [339_shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance_test_plan.md](339_shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance_test_plan.md)
  defines TASK-271 end-to-end local-service checks, exact temp DB row-count
  assertions, no-residual-listener checks, and required command checks.
- AccountingEngine skeleton module: [libs/AccountingEngine](../libs/AccountingEngine)
- AccountingEngine DTO parser boundary: [AccountingEngine public headers](../libs/AccountingEngine/include/AccountingEngine)
- AccountingEngine empty ledger replay skeleton: [AccountingReplayEngine.h](../libs/AccountingEngine/include/AccountingEngine/AccountingReplayEngine.h)
- AccountingEngine single buy replay skeleton: [AccountingReplayEngine.h](../libs/AccountingEngine/include/AccountingEngine/AccountingReplayEngine.h)
- AccountingEngine partial sell replay skeleton: [AccountingReplayEngine.h](../libs/AccountingEngine/include/AccountingEngine/AccountingReplayEngine.h)
- AccountingEngine sell exceeds position scenario: [AccountingReplayEngine.h](../libs/AccountingEngine/include/AccountingEngine/AccountingReplayEngine.h)
- AccountingEngine missing fee scenario: [AccountingReplayEngine.h](../libs/AccountingEngine/include/AccountingEngine/AccountingReplayEngine.h)
- AccountingEngine negative cash scenario: [AccountingReplayEngine.h](../libs/AccountingEngine/include/AccountingEngine/AccountingReplayEngine.h)
- AccountingEngine multi-instrument BUY scenario: [AccountingReplayEngine.h](../libs/AccountingEngine/include/AccountingEngine/AccountingReplayEngine.h)
- AccountingEngine multi-account BUY scenario: [AccountingReplayEngine.h](../libs/AccountingEngine/include/AccountingEngine/AccountingReplayEngine.h)
- AccountingEngine multi-currency unsupported scenario: [AccountingReplayEngine.h](../libs/AccountingEngine/include/AccountingEngine/AccountingReplayEngine.h)
- AccountingEngine missing market price scenario: [AccountingReplayEngine.h](../libs/AccountingEngine/include/AccountingEngine/AccountingReplayEngine.h)
- [Release Notes - v0.1 ReadOnly Shell Demo](release_notes/v0_1_readonly_shell_demo.md)
- [Release Notes - v0.2 ReadOnly Business Pages](release_notes/v0_2_readonly_business_pages.md)
- [Release Notes - v0.3 Accounting Replay Test-only Coverage](release_notes/v0_3_accounting_replay_testonly_coverage.md)
- [Release Notes - v0.4 AccountingEngine Replay Skeleton](release_notes/v0_4_accounting_engine_replay_skeleton.md)
- [Release Notes - v0.5 DataService Accounting Guard Suite](release_notes/v0_5_dataservice_accounting_guard_suite.md)

## TASK-110 ShellAccountingDataServiceAdapter Live-call Skeleton

TASK-110 adds `ShellAccountingDataServiceClientPort` as a ShellServices-owned
abstract client port plus test-only spy port coverage for
`ShellAccountingDataServiceAdapter`. The adapter can map requests, responses,
protocol errors, timeout, transport errors, domain issues, no-write, no-trade,
and controller refresh behavior through the abstract port.

This remains a skeleton: real DataServiceClient integration, QML integration,
SQLite access, AccountingEngine calls, database writes, TradeDraft generation,
and trade suggestions remain unimplemented.
