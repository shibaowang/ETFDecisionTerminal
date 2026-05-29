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
