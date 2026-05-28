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
