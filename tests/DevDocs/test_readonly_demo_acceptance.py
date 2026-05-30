from pathlib import Path


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def extract_between(text: str, start: str, end: str) -> str:
    start_index = text.find(start)
    require(start_index >= 0, f"source contains {start}")
    end_index = text.find(end, start_index)
    require(end_index >= 0, f"source contains {end} after {start}")
    return text[start_index:end_index]


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    script_path = root / "tools" / "dev" / "run_readonly_demo.ps1"
    stop_script_path = root / "tools" / "dev" / "stop_readonly_demo.ps1"
    doc_path = root / "docs" / "13_manual_acceptance_readonly_shell.md"
    milestone_path = root / "docs" / "14_milestone_v0_1_readonly_shell_demo.md"
    quick_start_path = root / "docs" / "15_quick_start_readonly_shell_demo.md"
    business_acceptance_path = root / "docs" / "16_manual_acceptance_readonly_business_pages.md"
    ui_readability_path = root / "docs" / "17_ui_readability_checklist.md"
    ui_design_path = root / "docs" / "07_ui_design.md"
    milestone_v02_path = root / "docs" / "18_milestone_v0_2_readonly_business_pages.md"
    business_summary_path = root / "docs" / "19_readonly_business_pages_summary.md"
    position_boundary_path = root / "docs" / "20_position_accounting_boundary.md"
    position_contract_path = root / "docs" / "21_position_readonly_data_contract_draft.md"
    position_stable_contract_path = root / "docs" / "22_position_accounting_data_contract.md"
    position_fixture_design_path = root / "docs" / "23_position_accounting_test_fixture_design.md"
    accounting_fixture_samples_path = root / "docs" / "24_accounting_replay_fixture_samples.md"
    position_viewmodel_design_path = root / "docs" / "25_position_shell_viewmodel_design.md"
    position_mapping_path = root / "docs" / "26_position_dto_viewmodel_mapping.md"
    codex_prompt_template_path = root / "docs" / "12_codex_prompt_template.md"
    accounting_replay_incremental_plan_path = root / "docs" / "30_accounting_replay_incremental_implementation_plan.md"
    accounting_replay_milestone_path = root / "docs" / "31_accounting_replay_testonly_coverage_milestone.md"
    accounting_replay_readiness_path = root / "docs" / "32_production_accounting_replay_readiness_review.md"
    accounting_replay_architecture_path = root / "docs" / "33_production_accounting_replay_architecture.md"
    accounting_engine_candidate_path = root / "docs" / "34_accounting_engine_module_candidate.md"
    accounting_engine_replay_milestone_path = root / "docs" / "35_accounting_engine_replay_skeleton_milestone.md"
    accounting_engine_next_phase_review_path = root / "docs" / "36_accounting_engine_next_phase_boundary_review.md"
    dataservice_readonly_accounting_contracts_path = (
        root / "docs" / "37_dataservice_readonly_accounting_action_contracts.md"
    )
    dataservice_accounting_no_write_plan_path = root / "docs" / "38_dataservice_accounting_no_write_test_plan.md"
    sqlite_readonly_facts_query_boundary_path = root / "docs" / "39_sqlite_readonly_facts_query_boundary.md"
    accounting_facts_source_mapping_path = root / "docs" / "40_accounting_facts_source_mapping.md"
    cash_facts_source_boundary_path = root / "docs" / "41_cash_facts_source_boundary.md"
    cash_facts_query_decision_path = root / "docs" / "42_cash_facts_query_decision.md"
    dataservice_guard_suite_milestone_path = (
        root / "docs" / "43_dataservice_accounting_guard_suite_milestone.md"
    )
    dataservice_guard_no_write_readiness_path = (
        root / "docs" / "44_dataservice_accounting_guard_no_write_readiness_review.md"
    )
    position_list_real_boundary_path = root / "docs" / "45_position_list_real_implementation_boundary.md"
    position_list_readiness_checklist_path = (
        root / "docs" / "46_position_list_real_implementation_readiness_checklist.md"
    )
    position_list_readiness_hardening_path = root / "docs" / "47_position_list_readiness_hardening.md"
    position_list_first_stage_scope_path = root / "docs" / "48_position_list_first_stage_real_action_scope.md"
    shellservices_accounting_controller_contract_path = (
        root / "docs" / "49_shellservices_accounting_controller_contract.md"
    )
    shell_accounting_viewmodel_state_contract_path = (
        root / "docs" / "50_shell_accounting_viewmodel_state_contract.md"
    )
    shell_accounting_service_adapter_boundary_path = (
        root / "docs" / "51_shell_accounting_service_adapter_boundary.md"
    )
    shell_accounting_service_integration_readiness_path = (
        root / "docs" / "52_shell_accounting_service_integration_readiness.md"
    )
    shell_accounting_dataservice_adapter_boundary_path = (
        root / "docs" / "53_shell_accounting_dataservice_adapter_boundary.md"
    )
    shell_accounting_dataservice_adapter_test_plan_path = (
        root / "docs" / "54_shell_accounting_dataservice_adapter_test_plan.md"
    )
    shell_accounting_dataservice_adapter_live_call_gate_path = (
        root / "docs" / "55_shell_accounting_dataservice_adapter_live_call_gate.md"
    )
    shell_accounting_dataservice_adapter_live_call_checklist_path = (
        root / "docs" / "56_shell_accounting_dataservice_adapter_live_call_acceptance_checklist.md"
    )
    shell_accounting_presenter_contract_path = (
        root / "docs" / "57_shell_accounting_presenter_contract.md"
    )
    shell_accounting_qml_binding_readiness_path = (
        root / "docs" / "58_shell_accounting_qml_binding_readiness_plan.md"
    )
    shell_accounting_qml_binding_smoke_plan_path = (
        root / "docs" / "59_shell_accounting_qml_binding_smoke_plan.md"
    )
    shell_accounting_qml_static_gate_path = (
        root / "docs" / "60_shell_accounting_qml_static_gate.md"
    )
    shell_accounting_readonly_ui_milestone_path = (
        root / "docs" / "61_shell_accounting_readonly_ui_readiness_milestone.md"
    )
    shell_accounting_next_phase_review_path = (
        root / "docs" / "62_shell_accounting_next_phase_review.md"
    )
    shell_accounting_production_qml_boundary_path = (
        root / "docs" / "63_shell_accounting_production_qml_binding_boundary.md"
    )
    shell_accounting_first_stage_qml_checklist_path = (
        root / "docs" / "64_shell_accounting_first_stage_qml_binding_checklist.md"
    )
    shell_accounting_qml_type_registration_boundary_path = (
        root / "docs" / "65_shell_accounting_qml_type_registration_boundary.md"
    )
    shell_accounting_qml_type_registration_test_plan_path = (
        root / "docs" / "66_shell_accounting_qml_type_registration_test_plan.md"
    )
    shell_accounting_qml_type_registration_implementation_gate_path = (
        root / "docs" / "67_shell_accounting_qml_type_registration_implementation_gate.md"
    )
    shell_accounting_qml_type_registration_rollback_strategy_path = (
        root / "docs" / "68_shell_accounting_qml_type_registration_rollback_strategy.md"
    )
    shell_accounting_qml_startup_registration_gate_path = (
        root / "docs" / "69_shell_accounting_qml_startup_registration_gate.md"
    )
    shell_accounting_qml_startup_registration_test_plan_path = (
        root / "docs" / "70_shell_accounting_qml_startup_registration_test_plan.md"
    )
    shell_accounting_qml_startup_registration_wiring_path = (
        root / "docs" / "71_shell_accounting_qml_startup_registration_wiring.md"
    )
    shell_accounting_production_qml_binding_gate_path = (
        root / "docs" / "72_shell_accounting_production_qml_binding_gate.md"
    )
    shell_accounting_production_qml_binding_test_plan_path = (
        root / "docs" / "73_shell_accounting_production_qml_binding_test_plan.md"
    )
    shell_accounting_production_qml_binding_implementation_path = (
        root / "docs" / "74_shell_accounting_production_qml_binding_implementation.md"
    )
    shell_accounting_presenter_lifecycle_gate_path = (
        root / "docs" / "75_shell_accounting_presenter_lifecycle_gate.md"
    )
    shell_accounting_presenter_lifecycle_test_plan_path = (
        root / "docs" / "76_shell_accounting_presenter_lifecycle_test_plan.md"
    )
    shell_accounting_presenter_lifecycle_implementation_path = (
        root / "docs" / "77_shell_accounting_presenter_lifecycle_implementation.md"
    )
    shell_accounting_real_data_adapter_gate_path = (
        root / "docs" / "78_shell_accounting_real_data_adapter_gate.md"
    )
    shell_accounting_real_data_adapter_test_plan_path = (
        root / "docs" / "79_shell_accounting_real_data_adapter_test_plan.md"
    )
    shell_accounting_real_data_adapter_implementation_path = (
        root / "docs" / "80_shell_accounting_real_data_adapter_implementation.md"
    )
    shell_accounting_dataservice_facts_query_gate_path = (
        root / "docs" / "81_shell_accounting_dataservice_readonly_facts_query_gate.md"
    )
    shell_accounting_dataservice_facts_query_test_plan_path = (
        root / "docs" / "82_shell_accounting_dataservice_readonly_facts_query_test_plan.md"
    )
    shell_accounting_dataservice_facts_query_implementation_path = (
        root / "docs" / "83_shell_accounting_dataservice_readonly_facts_query_implementation.md"
    )
    shell_accounting_replay_snapshot_rebuild_gate_path = (
        root / "docs" / "84_shell_accounting_replay_snapshot_rebuild_gate.md"
    )
    shell_accounting_replay_snapshot_rebuild_test_plan_path = (
        root / "docs" / "85_shell_accounting_replay_snapshot_rebuild_test_plan.md"
    )
    shell_accounting_readonly_replay_implementation_path = (
        root / "docs" / "86_shell_accounting_readonly_replay_implementation.md"
    )
    shell_accounting_qml_static_gate_cmake_path = (
        root / "tests" / "ShellAccountingQmlStaticGate" / "CMakeLists.txt"
    )
    shell_accounting_qml_binding_smoke_cmake_path = (
        root / "tests" / "ShellAccountingQmlBindingSmoke" / "CMakeLists.txt"
    )
    shell_accounting_qml_smoke_runtime_cmake_path = (
        root / "tests" / "ShellAccountingQmlBindingSmokeRuntime" / "CMakeLists.txt"
    )
    shell_accounting_qml_type_registration_cmake_path = (
        root / "tests" / "ShellAccountingQmlTypeRegistrationScaffold" / "CMakeLists.txt"
    )
    shell_accounting_qml_type_registration_gate_cmake_path = (
        root / "tests" / "ShellAccountingQmlTypeRegistrationGate" / "CMakeLists.txt"
    )
    shell_accounting_qml_type_registration_implementation_cmake_path = (
        root / "tests" / "ShellAccountingQmlTypeRegistrationImplementation" / "CMakeLists.txt"
    )
    shell_accounting_qml_startup_registration_gate_cmake_path = (
        root / "tests" / "ShellAccountingQmlStartupRegistrationGate" / "CMakeLists.txt"
    )
    shell_accounting_qml_startup_registration_wiring_cmake_path = (
        root / "tests" / "ShellAccountingQmlStartupRegistrationWiring" / "CMakeLists.txt"
    )
    shell_accounting_production_qml_binding_gate_cmake_path = (
        root / "tests" / "ShellAccountingProductionQmlBindingGate" / "CMakeLists.txt"
    )
    shell_accounting_production_qml_binding_implementation_cmake_path = (
        root / "tests" / "ShellAccountingProductionQmlBindingImplementation" / "CMakeLists.txt"
    )
    shell_accounting_presenter_lifecycle_gate_cmake_path = (
        root / "tests" / "ShellAccountingPresenterLifecycleGate" / "CMakeLists.txt"
    )
    shell_accounting_presenter_lifecycle_implementation_cmake_path = (
        root / "tests" / "ShellAccountingPresenterLifecycleImplementation" / "CMakeLists.txt"
    )
    shell_accounting_real_data_adapter_gate_cmake_path = (
        root / "tests" / "ShellAccountingRealDataAdapterGate" / "CMakeLists.txt"
    )
    shell_accounting_real_data_adapter_implementation_cmake_path = (
        root / "tests" / "ShellAccountingRealDataAdapterImplementation" / "CMakeLists.txt"
    )
    shell_accounting_dataservice_facts_query_gate_cmake_path = (
        root / "tests" / "ShellAccountingDataServiceFactsQueryGate" / "CMakeLists.txt"
    )
    shell_accounting_dataservice_facts_query_implementation_cmake_path = (
        root / "tests" / "ShellAccountingDataServiceFactsQueryImplementation" / "CMakeLists.txt"
    )
    shell_accounting_replay_snapshot_rebuild_gate_cmake_path = (
        root / "tests" / "ShellAccountingReplaySnapshotRebuildGate" / "CMakeLists.txt"
    )
    shell_accounting_readonly_replay_implementation_cmake_path = (
        root / "tests" / "ShellAccountingReadOnlyReplayImplementation" / "CMakeLists.txt"
    )
    shell_accounting_qml_registration_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingQmlRegistration.h"
    )
    shell_accounting_qml_registration_source_path = (
        root / "libs" / "ShellServices" / "src" / "ShellAccountingQmlRegistration.cpp"
    )
    shell_accounting_presenter_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h"
    )
    shell_accounting_presenter_source_path = (
        root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp"
    )
    shell_accounting_presenter_cmake_path = root / "tests" / "ShellAccountingPresenter" / "CMakeLists.txt"
    shell_accounting_dataservice_adapter_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceAdapter.h"
    )
    shell_accounting_dataservice_adapter_source_path = (
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceAdapter.cpp"
    )
    shell_accounting_dataservice_client_port_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceClientPort.h"
    )
    shell_accounting_dataservice_client_port_adapter_header_path = (
        root
        / "libs"
        / "ShellServices"
        / "include"
        / "ShellServices"
        / "ShellAccountingDataServiceClientPortAdapter.h"
    )
    shell_accounting_dataservice_client_port_adapter_source_path = (
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceClientPortAdapter.cpp"
    )
    shell_accounting_status_object_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingStatusObject.h"
    )
    shell_accounting_status_object_source_path = (
        root / "libs" / "ShellServices" / "src" / "ShellAccountingStatusObject.cpp"
    )
    shell_accounting_issue_list_model_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingIssueListModel.h"
    )
    shell_accounting_issue_list_model_source_path = (
        root / "libs" / "ShellServices" / "src" / "ShellAccountingIssueListModel.cpp"
    )
    shell_position_list_model_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellPositionListModel.h"
    )
    shell_position_list_model_source_path = (
        root / "libs" / "ShellServices" / "src" / "ShellPositionListModel.cpp"
    )
    shell_accounting_viewmodel_cmake_path = root / "tests" / "ShellAccountingViewModel" / "CMakeLists.txt"
    shell_accounting_dataservice_adapter_skeleton_cmake_path = (
        root / "tests" / "ShellAccountingDataServiceAdapter" / "CMakeLists.txt"
    )
    spy_shell_accounting_dataservice_client_port_header_path = (
        root / "tests" / "ShellAccountingDataServiceAdapter" / "SpyShellAccountingDataServiceClientPort.h"
    )
    spy_shell_accounting_dataservice_client_port_source_path = (
        root / "tests" / "ShellAccountingDataServiceAdapter" / "SpyShellAccountingDataServiceClientPort.cpp"
    )
    shell_accounting_dataservice_adapter_live_call_gate_cmake_path = (
        root / "tests" / "ShellAccountingDataServiceAdapterLiveCallGate" / "CMakeLists.txt"
    )
    shell_accounting_dataservice_adapter_scaffolding_cmake_path = (
        root / "tests" / "ShellAccountingDataServiceAdapterScaffolding" / "CMakeLists.txt"
    )
    spy_accounting_dataservice_client_header_path = (
        root / "tests" / "ShellAccountingDataServiceAdapterScaffolding" / "SpyAccountingDataServiceClient.h"
    )
    spy_accounting_dataservice_client_source_path = (
        root / "tests" / "ShellAccountingDataServiceAdapterScaffolding" / "SpyAccountingDataServiceClient.cpp"
    )
    shell_accounting_dataservice_expected_call_header_path = (
        root / "tests" / "ShellAccountingDataServiceAdapterScaffolding" / "ShellAccountingDataServiceAdapterExpectedCall.h"
    )
    shell_accounting_dataservice_expected_call_source_path = (
        root / "tests" / "ShellAccountingDataServiceAdapterScaffolding" / "ShellAccountingDataServiceAdapterExpectedCall.cpp"
    )
    shell_accounting_service_adapter_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingServiceAdapter.h"
    )
    shell_accounting_service_types_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingServiceTypes.h"
    )
    shell_accounting_service_types_source_path = (
        root / "libs" / "ShellServices" / "src" / "ShellAccountingServiceTypes.cpp"
    )
    shell_accounting_service_adapter_cmake_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "CMakeLists.txt"
    )
    fake_shell_accounting_service_adapter_header_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "FakeShellAccountingServiceAdapter.h"
    )
    fake_shell_accounting_service_adapter_source_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "FakeShellAccountingServiceAdapter.cpp"
    )
    shell_accounting_service_adapter_interface_test_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "test_shell_accounting_service_adapter_interface.cpp"
    )
    shell_accounting_service_result_contract_test_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "test_shell_accounting_service_result_contract.cpp"
    )
    shell_accounting_service_adapter_no_dependency_test_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "test_shell_accounting_service_adapter_no_real_dependency.cpp"
    )
    shell_accounting_controller_adapter_injection_test_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "test_shell_accounting_controller_adapter_injection.cpp"
    )
    shell_accounting_fake_adapter_refresh_success_test_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "test_shell_accounting_controller_fake_adapter_refresh_success.cpp"
    )
    shell_accounting_fake_adapter_unavailable_test_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "test_shell_accounting_controller_fake_adapter_unavailable.cpp"
    )
    shell_accounting_fake_adapter_protocol_error_test_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "test_shell_accounting_controller_fake_adapter_protocol_error.cpp"
    )
    shell_accounting_fake_adapter_issue_mapping_test_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "test_shell_accounting_controller_fake_adapter_issue_mapping.cpp"
    )
    shell_accounting_fake_adapter_privacy_no_trade_test_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "test_shell_accounting_controller_fake_adapter_privacy_no_trade.cpp"
    )
    shell_accounting_fake_adapter_no_dependency_test_path = (
        root / "tests" / "ShellAccountingServiceAdapter" / "test_shell_accounting_controller_fake_adapter_no_real_dependency.cpp"
    )
    shell_accounting_scaffolding_cmake_path = (
        root / "tests" / "ShellAccountingControllerScaffolding" / "CMakeLists.txt"
    )
    shell_accounting_scenario_header_path = (
        root / "tests" / "ShellAccountingControllerScaffolding" / "ShellAccountingControllerScenario.h"
    )
    shell_accounting_scenario_source_path = (
        root / "tests" / "ShellAccountingControllerScaffolding" / "ShellAccountingControllerScenario.cpp"
    )
    shell_accounting_state_utils_header_path = (
        root / "tests" / "ShellAccountingControllerScaffolding" / "ShellAccountingStateMappingTestUtils.h"
    )
    shell_accounting_state_utils_source_path = (
        root / "tests" / "ShellAccountingControllerScaffolding" / "ShellAccountingStateMappingTestUtils.cpp"
    )
    shell_accounting_state_test_path = (
        root / "tests" / "ShellAccountingControllerScaffolding" / "test_shell_accounting_controller_state_contract.cpp"
    )
    shell_accounting_issue_test_path = (
        root / "tests" / "ShellAccountingControllerScaffolding" / "test_shell_accounting_controller_issue_contract.cpp"
    )
    shell_accounting_privacy_test_path = (
        root / "tests" / "ShellAccountingControllerScaffolding" / "test_shell_accounting_controller_privacy_contract.cpp"
    )
    shell_accounting_no_trade_test_path = (
        root
        / "tests"
        / "ShellAccountingControllerScaffolding"
        / "test_shell_accounting_controller_no_trade_action_contract.cpp"
    )
    shell_accounting_state_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingState.h"
    )
    shell_accounting_state_source_path = root / "libs" / "ShellServices" / "src" / "ShellAccountingState.cpp"
    shell_accounting_issue_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingIssue.h"
    )
    shell_accounting_issue_source_path = root / "libs" / "ShellServices" / "src" / "ShellAccountingIssue.cpp"
    shell_accounting_display_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDisplayText.h"
    )
    shell_accounting_display_source_path = (
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDisplayText.cpp"
    )
    shell_accounting_controller_header_path = (
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h"
    )
    shell_accounting_controller_source_path = (
        root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp"
    )
    shell_accounting_controller_state_test_path = (
        root / "tests" / "ShellServices" / "test_shell_accounting_readonly_controller_skeleton_state.cpp"
    )
    shell_accounting_controller_issue_test_path = (
        root / "tests" / "ShellServices" / "test_shell_accounting_readonly_controller_skeleton_issue.cpp"
    )
    shell_accounting_controller_privacy_test_path = (
        root / "tests" / "ShellServices" / "test_shell_accounting_readonly_controller_skeleton_privacy.cpp"
    )
    shell_accounting_controller_no_trade_test_path = (
        root / "tests" / "ShellServices" / "test_shell_accounting_readonly_controller_skeleton_no_trade_action.cpp"
    )
    shell_accounting_controller_no_dependency_test_path = (
        root / "tests" / "ShellServices" / "test_shell_accounting_readonly_controller_skeleton_no_service_dependency.cpp"
    )
    shell_accounting_fake_client_cmake_path = (
        root / "tests" / "ShellAccountingFakeClientBoundary" / "CMakeLists.txt"
    )
    fake_accounting_client_payload_header_path = (
        root / "tests" / "ShellAccountingFakeClientBoundary" / "FakeAccountingClientPayload.h"
    )
    fake_accounting_client_payload_source_path = (
        root / "tests" / "ShellAccountingFakeClientBoundary" / "FakeAccountingClientPayload.cpp"
    )
    fake_accounting_client_scenario_header_path = (
        root / "tests" / "ShellAccountingFakeClientBoundary" / "FakeAccountingClientScenario.h"
    )
    fake_accounting_client_scenario_source_path = (
        root / "tests" / "ShellAccountingFakeClientBoundary" / "FakeAccountingClientScenario.cpp"
    )
    shell_accounting_fake_guard_payload_test_path = (
        root / "tests" / "ShellAccountingFakeClientBoundary" / "test_shell_accounting_fake_client_guard_payloads.cpp"
    )
    shell_accounting_fake_state_transition_test_path = (
        root / "tests" / "ShellAccountingFakeClientBoundary" / "test_shell_accounting_fake_client_state_transitions.cpp"
    )
    shell_accounting_fake_issue_mapping_test_path = (
        root / "tests" / "ShellAccountingFakeClientBoundary" / "test_shell_accounting_fake_client_issue_mapping.cpp"
    )
    shell_accounting_fake_privacy_no_trade_test_path = (
        root / "tests" / "ShellAccountingFakeClientBoundary" / "test_shell_accounting_fake_client_privacy_and_no_trade.cpp"
    )
    shell_accounting_fake_no_real_dependency_test_path = (
        root / "tests" / "ShellAccountingFakeClientBoundary" / "test_shell_accounting_fake_client_no_real_dependency.cpp"
    )
    shellservices_cmake_path = root / "libs" / "ShellServices" / "CMakeLists.txt"
    shellservices_public_header_path = root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellServices.h"
    shellservices_tests_cmake_path = root / "tests" / "ShellServices" / "CMakeLists.txt"
    qml_dir = root / "apps" / "ETFDecisionShell" / "qml"
    position_list_first_stage_cmake_path = (
        root / "tests" / "PositionListFirstStageScaffolding" / "CMakeLists.txt"
    )
    position_list_first_stage_scenario_header_path = (
        root / "tests" / "PositionListFirstStageScaffolding" / "PositionListFirstStageScenario.h"
    )
    position_list_first_stage_scenario_source_path = (
        root / "tests" / "PositionListFirstStageScaffolding" / "PositionListFirstStageScenario.cpp"
    )
    position_list_first_stage_scope_test_path = (
        root / "tests" / "PositionListFirstStageScaffolding" / "test_position_list_first_stage_scope_contract.cpp"
    )
    position_list_first_stage_transition_test_path = (
        root
        / "tests"
        / "PositionListFirstStageScaffolding"
        / "test_position_list_first_stage_guard_transition_contract.cpp"
    )
    position_list_first_stage_failure_test_path = (
        root
        / "tests"
        / "PositionListFirstStageScaffolding"
        / "test_position_list_first_stage_failure_matrix_contract.cpp"
    )
    position_list_mapping_cmake_path = root / "tests" / "PositionListMapping" / "CMakeLists.txt"
    position_list_mapping_utils_header_path = (
        root / "tests" / "PositionListMapping" / "PositionListMappingContractTestUtils.h"
    )
    position_list_mapping_utils_source_path = (
        root / "tests" / "PositionListMapping" / "PositionListMappingContractTestUtils.cpp"
    )
    position_list_request_mapping_test_path = (
        root / "tests" / "PositionListMapping" / "test_position_list_request_mapping.cpp"
    )
    position_list_trade_fact_mapping_test_path = (
        root / "tests" / "PositionListMapping" / "test_position_list_trade_fact_mapping.cpp"
    )
    position_list_response_mapping_test_path = (
        root / "tests" / "PositionListMapping" / "test_position_list_response_mapping.cpp"
    )
    position_list_no_write_cmake_path = root / "tests" / "PositionListRealActionNoWrite" / "CMakeLists.txt"
    position_list_no_write_skeleton_header_path = (
        root / "tests" / "PositionListRealActionNoWrite" / "PositionListRealActionNoWriteSkeleton.h"
    )
    position_list_no_write_skeleton_source_path = (
        root / "tests" / "PositionListRealActionNoWrite" / "PositionListRealActionNoWriteSkeleton.cpp"
    )
    position_list_no_write_skeleton_test_path = (
        root / "tests" / "PositionListRealActionNoWrite" / "test_position_list_real_action_no_write_skeleton.cpp"
    )
    position_list_failure_no_write_skeleton_test_path = (
        root
        / "tests"
        / "PositionListRealActionNoWrite"
        / "test_position_list_real_action_failure_no_write_skeleton.cpp"
    )
    root_cmake_path = root / "CMakeLists.txt"
    tests_cmake_path = root / "tests" / "CMakeLists.txt"
    accounting_engine_dir = root / "libs" / "AccountingEngine"
    accounting_engine_cmake_path = accounting_engine_dir / "CMakeLists.txt"
    accounting_issue_header_path = accounting_engine_dir / "include" / "AccountingEngine" / "AccountingIssue.h"
    accounting_replay_dtos_header_path = accounting_engine_dir / "include" / "AccountingEngine" / "AccountingReplayDtos.h"
    accounting_replay_engine_header_path = accounting_engine_dir / "include" / "AccountingEngine" / "AccountingReplayEngine.h"
    accounting_replay_result_header_path_prod = (
        accounting_engine_dir / "include" / "AccountingEngine" / "AccountingReplayResult.h"
    )
    accounting_replay_validation_header_path = (
        accounting_engine_dir / "include" / "AccountingEngine" / "AccountingReplayValidation.h"
    )
    accounting_replay_parser_header_path = accounting_engine_dir / "include" / "AccountingEngine" / "AccountingReplayParser.h"
    accounting_engine_boundary_test_path = root / "tests" / "AccountingEngine" / "test_accounting_engine_boundary.cpp"
    accounting_replay_engine_empty_ledger_test_path = (
        root / "tests" / "AccountingEngine" / "test_accounting_replay_engine_empty_ledger.cpp"
    )
    accounting_replay_engine_single_buy_test_path = (
        root / "tests" / "AccountingEngine" / "test_accounting_replay_engine_single_buy.cpp"
    )
    accounting_replay_engine_buy_sell_partial_test_path = (
        root / "tests" / "AccountingEngine" / "test_accounting_replay_engine_buy_sell_partial.cpp"
    )
    accounting_replay_engine_sell_exceeds_position_test_path = (
        root / "tests" / "AccountingEngine" / "test_accounting_replay_engine_sell_exceeds_position.cpp"
    )
    accounting_replay_engine_missing_fee_test_path = (
        root / "tests" / "AccountingEngine" / "test_accounting_replay_engine_missing_fee.cpp"
    )
    accounting_replay_engine_negative_cash_test_path = (
        root / "tests" / "AccountingEngine" / "test_accounting_replay_engine_negative_cash.cpp"
    )
    accounting_replay_engine_multi_instrument_buy_test_path = (
        root / "tests" / "AccountingEngine" / "test_accounting_replay_engine_multi_instrument_buy.cpp"
    )
    accounting_replay_engine_multi_account_buy_test_path = (
        root / "tests" / "AccountingEngine" / "test_accounting_replay_engine_multi_account_buy.cpp"
    )
    accounting_replay_engine_multi_currency_unsupported_test_path = (
        root / "tests" / "AccountingEngine" / "test_accounting_replay_engine_multi_currency_unsupported.cpp"
    )
    accounting_replay_engine_missing_market_price_test_path = (
        root / "tests" / "AccountingEngine" / "test_accounting_replay_engine_missing_market_price.cpp"
    )
    accounting_replay_dto_parser_test_path = (
        root / "tests" / "AccountingEngine" / "test_accounting_replay_dto_parser.cpp"
    )
    accounting_engine_test_cmake_path = root / "tests" / "AccountingEngine" / "CMakeLists.txt"
    accounting_fixture_dir = root / "tests" / "fixtures" / "accounting_replay"
    accounting_fixture_index_path = accounting_fixture_dir / "fixtures_index.json"
    accounting_fixture_validator_path = (
        root / "tests" / "AccountingFixtures" / "validate_accounting_replay_fixtures.py"
    )
    accounting_fixture_loader_header_path = root / "tests" / "AccountingFixtures" / "AccountingFixtureLoader.h"
    accounting_fixture_loader_source_path = root / "tests" / "AccountingFixtures" / "AccountingFixtureLoader.cpp"
    accounting_fixture_loader_test_path = root / "tests" / "AccountingFixtures" / "test_accounting_fixture_loader.cpp"
    accounting_replay_stub_header_path = root / "tests" / "AccountingFixtures" / "AccountingReplayStubEngine.h"
    accounting_replay_stub_source_path = root / "tests" / "AccountingFixtures" / "AccountingReplayStubEngine.cpp"
    accounting_replay_result_header_path = root / "tests" / "AccountingFixtures" / "AccountingReplayResult.h"
    accounting_replay_result_source_path = root / "tests" / "AccountingFixtures" / "AccountingReplayResult.cpp"
    accounting_replay_mapper_header_path = root / "tests" / "AccountingFixtures" / "AccountingReplayResultMapper.h"
    accounting_replay_mapper_source_path = root / "tests" / "AccountingFixtures" / "AccountingReplayResultMapper.cpp"
    accounting_replay_result_test_path = root / "tests" / "AccountingFixtures" / "test_accounting_replay_result.cpp"
    accounting_replay_harness_header_path = root / "tests" / "AccountingFixtures" / "AccountingReplayTestHarness.h"
    accounting_replay_harness_source_path = root / "tests" / "AccountingFixtures" / "AccountingReplayTestHarness.cpp"
    accounting_replay_harness_test_path = root / "tests" / "AccountingFixtures" / "test_accounting_replay_harness.cpp"
    accounting_expected_inspector_header_path = (
        root / "tests" / "AccountingFixtures" / "AccountingExpectedOutputInspector.h"
    )
    accounting_expected_inspector_source_path = (
        root / "tests" / "AccountingFixtures" / "AccountingExpectedOutputInspector.cpp"
    )
    accounting_replay_assertion_header_path = (
        root / "tests" / "AccountingFixtures" / "AccountingReplayAssertionSkeleton.h"
    )
    accounting_replay_assertion_source_path = (
        root / "tests" / "AccountingFixtures" / "AccountingReplayAssertionSkeleton.cpp"
    )
    accounting_replay_assertion_test_path = (
        root / "tests" / "AccountingFixtures" / "test_accounting_replay_assertions.cpp"
    )
    accounting_replay_minimal_header_path = root / "tests" / "AccountingFixtures" / "AccountingReplayMinimalEngine.h"
    accounting_replay_minimal_source_path = root / "tests" / "AccountingFixtures" / "AccountingReplayMinimalEngine.cpp"
    accounting_replay_minimal_test_path = root / "tests" / "AccountingFixtures" / "test_accounting_replay_minimal_engine.cpp"
    release_notes_path = root / "docs" / "release_notes" / "v0_1_readonly_shell_demo.md"
    release_notes_v02_path = root / "docs" / "release_notes" / "v0_2_readonly_business_pages.md"
    release_notes_v03_path = root / "docs" / "release_notes" / "v0_3_accounting_replay_testonly_coverage.md"
    release_notes_v04_path = root / "docs" / "release_notes" / "v0_4_accounting_engine_replay_skeleton.md"
    release_notes_v05_path = root / "docs" / "release_notes" / "v0_5_dataservice_accounting_guard_suite.md"
    release_notes_v06_path = (
        root / "docs" / "release_notes" / "v0_6_shell_accounting_readonly_ui_readiness.md"
    )
    docs_index_path = root / "docs" / "README.md"
    protocol_path = root / "docs" / "04_protocol.md"
    accounting_rules_path = root / "docs" / "06_accounting_rules.md"
    readme_path = root / "README.md"
    gitignore_path = root / ".gitignore"
    dataservice_actions_header_path = root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" / "DataServiceActions.h"
    dataservice_actions_source_path = root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp"
    dataservice_action_registrar_path = root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp"
    dataservice_client_header_path = root / "libs" / "DataServiceClient" / "include" / "DataServiceClient" / "DataServiceClient.h"
    dataservice_client_source_path = root / "libs" / "DataServiceClient" / "src" / "DataServiceClient.cpp"
    dataservice_readonly_test_path = root / "tests" / "DataService" / "test_dataservice_readonly_actions.cpp"
    dataservice_test_cmake_path = root / "tests" / "DataService" / "CMakeLists.txt"
    dataservice_client_test_path = root / "tests" / "DataServiceClient" / "test_dataservice_client.cpp"
    dataservice_client_test_cmake_path = root / "tests" / "DataServiceClient" / "CMakeLists.txt"
    dataaccess_cmake_path = root / "libs" / "DataAccess" / "CMakeLists.txt"
    dataaccess_repositories_header_path = root / "libs" / "DataAccess" / "include" / "DataAccess" / "Repositories.h"
    accounting_trade_fact_reader_header_path = (
        root / "libs" / "DataAccess" / "include" / "DataAccess" / "AccountingTradeFactReader.h"
    )
    accounting_trade_fact_reader_source_path = root / "libs" / "DataAccess" / "src" / "AccountingTradeFactReader.cpp"
    dataaccess_test_cmake_path = root / "tests" / "DataAccess" / "CMakeLists.txt"
    accounting_trade_fact_reader_test_path = root / "tests" / "DataAccess" / "test_accounting_trade_fact_reader.cpp"
    accounting_no_write_dir = root / "tests" / "AccountingNoWrite"
    accounting_no_write_cmake_path = accounting_no_write_dir / "CMakeLists.txt"
    accounting_no_write_utils_header_path = accounting_no_write_dir / "AccountingNoWriteTestUtils.h"
    accounting_no_write_utils_source_path = accounting_no_write_dir / "AccountingNoWriteTestUtils.cpp"
    forbidden_sql_scanner_header_path = accounting_no_write_dir / "ForbiddenSqlScanner.h"
    forbidden_sql_scanner_source_path = accounting_no_write_dir / "ForbiddenSqlScanner.cpp"
    accounting_no_write_harness_test_path = accounting_no_write_dir / "test_accounting_no_write_harness.cpp"
    forbidden_sql_scanner_test_path = accounting_no_write_dir / "test_forbidden_sql_scanner.cpp"

    require(script_path.exists(), "run_readonly_demo.ps1 exists")
    require(stop_script_path.exists(), "stop_readonly_demo.ps1 exists")
    require(doc_path.exists(), "manual acceptance doc exists")
    require(milestone_path.exists(), "v0.1 milestone doc exists")
    require(quick_start_path.exists(), "quick start doc exists")
    require(business_acceptance_path.exists(), "read-only business pages acceptance doc exists")
    require(ui_readability_path.exists(), "UI readability checklist exists")
    require(ui_design_path.exists(), "UI design doc exists")
    require(milestone_v02_path.exists(), "v0.2 milestone doc exists")
    require(business_summary_path.exists(), "read-only business pages summary exists")
    require(position_boundary_path.exists(), "position accounting boundary doc exists")
    require(position_contract_path.exists(), "position read-only data contract draft exists")
    require(position_stable_contract_path.exists(), "position accounting data contract doc exists")
    require(position_fixture_design_path.exists(), "position accounting test fixture design exists")
    require(accounting_fixture_samples_path.exists(), "accounting replay fixture samples doc exists")
    require(position_viewmodel_design_path.exists(), "position Shell ViewModel design doc exists")
    require(position_mapping_path.exists(), "position DTO ViewModel mapping doc exists")
    require(codex_prompt_template_path.exists(), "Codex prompt template doc exists")
    require(accounting_replay_incremental_plan_path.exists(), "accounting replay incremental plan doc exists")
    require(accounting_replay_milestone_path.exists(), "accounting replay test-only coverage milestone doc exists")
    require(accounting_replay_readiness_path.exists(), "production accounting replay readiness review doc exists")
    require(accounting_replay_architecture_path.exists(), "production accounting replay architecture doc exists")
    require(accounting_engine_candidate_path.exists(), "AccountingEngine module candidate doc exists")
    require(accounting_engine_replay_milestone_path.exists(), "AccountingEngine replay skeleton milestone doc exists")
    require(accounting_engine_next_phase_review_path.exists(), "AccountingEngine next phase boundary review doc exists")
    require(
        dataservice_readonly_accounting_contracts_path.exists(),
        "DataService read-only accounting action contracts doc exists",
    )
    require(
        dataservice_accounting_no_write_plan_path.exists(),
        "DataService accounting no-write test plan doc exists",
    )
    require(sqlite_readonly_facts_query_boundary_path.exists(), "SQLite read-only facts query boundary doc exists")
    require(accounting_facts_source_mapping_path.exists(), "Accounting facts source mapping doc exists")
    require(cash_facts_source_boundary_path.exists(), "Cash facts source boundary doc exists")
    require(cash_facts_query_decision_path.exists(), "Cash facts query decision doc exists")
    require(dataservice_guard_suite_milestone_path.exists(), "DataService accounting guard suite milestone doc exists")
    require(
        dataservice_guard_no_write_readiness_path.exists(),
        "DataService accounting guard no-write readiness review doc exists",
    )
    require(position_list_real_boundary_path.exists(), "position.list real implementation boundary doc exists")
    require(
        position_list_readiness_checklist_path.exists(),
        "position.list real implementation readiness checklist doc exists",
    )
    require(position_list_readiness_hardening_path.exists(), "position.list readiness hardening doc exists")
    require(position_list_first_stage_scope_path.exists(), "position.list first-stage real action scope doc exists")
    require(
        shellservices_accounting_controller_contract_path.exists(),
        "ShellServices accounting controller contract doc exists",
    )
    require(
        shell_accounting_viewmodel_state_contract_path.exists(),
        "Shell accounting ViewModel state contract doc exists",
    )
    require(
        shell_accounting_service_adapter_boundary_path.exists(),
        "Shell accounting service adapter boundary doc exists",
    )
    require(
        shell_accounting_service_integration_readiness_path.exists(),
        "Shell accounting service integration readiness doc exists",
    )
    require(
        shell_accounting_dataservice_adapter_boundary_path.exists(),
        "Shell accounting DataService adapter boundary doc exists",
    )
    require(
        shell_accounting_dataservice_adapter_test_plan_path.exists(),
        "Shell accounting DataService adapter test plan doc exists",
    )
    require(
        shell_accounting_dataservice_adapter_live_call_gate_path.exists(),
        "Shell accounting DataService adapter live-call gate doc exists",
    )
    require(
        shell_accounting_dataservice_adapter_live_call_checklist_path.exists(),
        "Shell accounting DataService adapter live-call acceptance checklist exists",
    )
    require(shell_accounting_presenter_contract_path.exists(), "Shell accounting presenter contract doc exists")
    require(shell_accounting_qml_binding_readiness_path.exists(), "Shell accounting QML binding readiness plan exists")
    require(shell_accounting_presenter_header_path.exists(), "ShellAccountingPresenter header exists")
    require(shell_accounting_presenter_source_path.exists(), "ShellAccountingPresenter source exists")
    require(shell_accounting_presenter_cmake_path.exists(), "ShellAccountingPresenter tests CMake exists")
    require(
        shell_accounting_dataservice_adapter_header_path.exists(),
        "ShellAccountingDataServiceAdapter production skeleton header exists",
    )
    require(
        shell_accounting_dataservice_adapter_source_path.exists(),
        "ShellAccountingDataServiceAdapter production skeleton source exists",
    )
    require(
        shell_accounting_dataservice_client_port_header_path.exists(),
        "ShellAccountingDataServiceClientPort live-call skeleton header exists",
    )
    require(
        shell_accounting_dataservice_client_port_adapter_header_path.exists(),
        "ShellAccountingDataServiceClientPortAdapter concrete port header exists",
    )
    require(
        shell_accounting_dataservice_client_port_adapter_source_path.exists(),
        "ShellAccountingDataServiceClientPortAdapter concrete port source exists",
    )
    require(shell_accounting_status_object_header_path.exists(), "ShellAccountingStatusObject header exists")
    require(shell_accounting_status_object_source_path.exists(), "ShellAccountingStatusObject source exists")
    require(shell_accounting_issue_list_model_header_path.exists(), "ShellAccountingIssueListModel header exists")
    require(shell_accounting_issue_list_model_source_path.exists(), "ShellAccountingIssueListModel source exists")
    require(shell_position_list_model_header_path.exists(), "ShellPositionListModel header exists")
    require(shell_position_list_model_source_path.exists(), "ShellPositionListModel source exists")
    require(shell_accounting_viewmodel_cmake_path.exists(), "ShellAccounting ViewModel tests CMake exists")
    require(
        shell_accounting_dataservice_adapter_skeleton_cmake_path.exists(),
        "Shell accounting DataService adapter skeleton CMake exists",
    )
    require(
        spy_shell_accounting_dataservice_client_port_header_path.exists(),
        "Shell accounting DataService adapter spy port header exists",
    )
    require(
        spy_shell_accounting_dataservice_client_port_source_path.exists(),
        "Shell accounting DataService adapter spy port source exists",
    )
    require(
        shell_accounting_dataservice_adapter_live_call_gate_cmake_path.exists(),
        "Shell accounting DataService adapter live-call gate CMake exists",
    )
    require(
        shell_accounting_dataservice_adapter_scaffolding_cmake_path.exists(),
        "Shell accounting DataService adapter scaffolding CMake exists",
    )
    require(spy_accounting_dataservice_client_header_path.exists(), "spy DataService client wrapper header exists")
    require(spy_accounting_dataservice_client_source_path.exists(), "spy DataService client wrapper source exists")
    require(
        shell_accounting_dataservice_expected_call_header_path.exists(),
        "DataService adapter expected call header exists",
    )
    require(
        shell_accounting_dataservice_expected_call_source_path.exists(),
        "DataService adapter expected call source exists",
    )
    require(shell_accounting_service_adapter_header_path.exists(), "ShellAccountingServiceAdapter header exists")
    require(shell_accounting_service_types_header_path.exists(), "ShellAccountingServiceTypes header exists")
    require(shell_accounting_service_types_source_path.exists(), "ShellAccountingServiceTypes source exists")
    require(shell_accounting_service_adapter_cmake_path.exists(), "ShellAccountingServiceAdapter test CMake exists")
    require(fake_shell_accounting_service_adapter_header_path.exists(), "fake Shell accounting service adapter header exists")
    require(fake_shell_accounting_service_adapter_source_path.exists(), "fake Shell accounting service adapter source exists")
    require(shell_accounting_service_adapter_interface_test_path.exists(), "Shell accounting service adapter interface test exists")
    require(shell_accounting_service_result_contract_test_path.exists(), "Shell accounting service result contract test exists")
    require(
        shell_accounting_service_adapter_no_dependency_test_path.exists(),
        "Shell accounting service adapter no real dependency test exists",
    )
    require(
        shell_accounting_controller_adapter_injection_test_path.exists(),
        "Shell accounting controller adapter injection test exists",
    )
    require(
        shell_accounting_fake_adapter_refresh_success_test_path.exists(),
        "Shell accounting fake adapter refresh success test exists",
    )
    require(
        shell_accounting_fake_adapter_unavailable_test_path.exists(),
        "Shell accounting fake adapter unavailable test exists",
    )
    require(
        shell_accounting_fake_adapter_protocol_error_test_path.exists(),
        "Shell accounting fake adapter protocol error test exists",
    )
    require(
        shell_accounting_fake_adapter_issue_mapping_test_path.exists(),
        "Shell accounting fake adapter issue mapping test exists",
    )
    require(
        shell_accounting_fake_adapter_privacy_no_trade_test_path.exists(),
        "Shell accounting fake adapter privacy no trade test exists",
    )
    require(
        shell_accounting_fake_adapter_no_dependency_test_path.exists(),
        "Shell accounting fake adapter no real dependency test exists",
    )
    require(shell_accounting_scaffolding_cmake_path.exists(), "Shell accounting scaffolding CMake exists")
    require(shell_accounting_scenario_header_path.exists(), "Shell accounting scenario header exists")
    require(shell_accounting_scenario_source_path.exists(), "Shell accounting scenario source exists")
    require(shell_accounting_state_utils_header_path.exists(), "Shell accounting state utils header exists")
    require(shell_accounting_state_utils_source_path.exists(), "Shell accounting state utils source exists")
    require(shell_accounting_state_test_path.exists(), "Shell accounting state contract test exists")
    require(shell_accounting_issue_test_path.exists(), "Shell accounting issue contract test exists")
    require(shell_accounting_privacy_test_path.exists(), "Shell accounting privacy contract test exists")
    require(shell_accounting_no_trade_test_path.exists(), "Shell accounting no trade action contract test exists")
    require(shell_accounting_state_header_path.exists(), "Shell accounting state header exists")
    require(shell_accounting_state_source_path.exists(), "Shell accounting state source exists")
    require(shell_accounting_issue_header_path.exists(), "Shell accounting issue header exists")
    require(shell_accounting_issue_source_path.exists(), "Shell accounting issue source exists")
    require(shell_accounting_display_header_path.exists(), "Shell accounting display header exists")
    require(shell_accounting_display_source_path.exists(), "Shell accounting display source exists")
    require(shell_accounting_controller_header_path.exists(), "ShellAccountingReadOnlyController header exists")
    require(shell_accounting_controller_source_path.exists(), "ShellAccountingReadOnlyController source exists")
    require(shell_accounting_controller_state_test_path.exists(), "Shell accounting controller skeleton state test exists")
    require(shell_accounting_controller_issue_test_path.exists(), "Shell accounting controller skeleton issue test exists")
    require(shell_accounting_controller_privacy_test_path.exists(), "Shell accounting controller skeleton privacy test exists")
    require(shell_accounting_controller_no_trade_test_path.exists(), "Shell accounting controller skeleton no trade test exists")
    require(
        shell_accounting_controller_no_dependency_test_path.exists(),
        "Shell accounting controller skeleton no dependency test exists",
    )
    require(shell_accounting_fake_client_cmake_path.exists(), "Shell accounting fake client CMake exists")
    require(fake_accounting_client_payload_header_path.exists(), "Fake accounting client payload header exists")
    require(fake_accounting_client_payload_source_path.exists(), "Fake accounting client payload source exists")
    require(fake_accounting_client_scenario_header_path.exists(), "Fake accounting client scenario header exists")
    require(fake_accounting_client_scenario_source_path.exists(), "Fake accounting client scenario source exists")
    require(shell_accounting_fake_guard_payload_test_path.exists(), "Shell accounting fake guard payload test exists")
    require(
        shell_accounting_fake_state_transition_test_path.exists(),
        "Shell accounting fake state transition test exists",
    )
    require(shell_accounting_fake_issue_mapping_test_path.exists(), "Shell accounting fake issue mapping test exists")
    require(
        shell_accounting_fake_privacy_no_trade_test_path.exists(),
        "Shell accounting fake privacy and no trade test exists",
    )
    require(
        shell_accounting_fake_no_real_dependency_test_path.exists(),
        "Shell accounting fake no real dependency test exists",
    )
    require(shellservices_cmake_path.exists(), "ShellServices CMake exists")
    require(shellservices_public_header_path.exists(), "ShellServices public header exists")
    require(shellservices_tests_cmake_path.exists(), "ShellServices tests CMake exists")
    require(qml_dir.exists(), "ETFDecisionShell QML directory exists")
    require(position_list_first_stage_cmake_path.exists(), "position.list first-stage scaffolding CMake exists")
    require(position_list_first_stage_scenario_header_path.exists(), "position.list first-stage scenario header exists")
    require(position_list_first_stage_scenario_source_path.exists(), "position.list first-stage scenario source exists")
    require(position_list_first_stage_scope_test_path.exists(), "position.list first-stage scope contract test exists")
    require(
        position_list_first_stage_transition_test_path.exists(),
        "position.list first-stage guard transition contract test exists",
    )
    require(
        position_list_first_stage_failure_test_path.exists(),
        "position.list first-stage failure matrix contract test exists",
    )
    require(position_list_mapping_cmake_path.exists(), "position.list mapping contract CMake exists")
    require(position_list_mapping_utils_header_path.exists(), "position.list mapping contract helper header exists")
    require(position_list_mapping_utils_source_path.exists(), "position.list mapping contract helper source exists")
    require(position_list_request_mapping_test_path.exists(), "position.list request mapping contract test exists")
    require(position_list_trade_fact_mapping_test_path.exists(), "position.list trade fact mapping contract test exists")
    require(position_list_response_mapping_test_path.exists(), "position.list response mapping contract test exists")
    require(position_list_no_write_cmake_path.exists(), "position.list real action no-write CMake exists")
    require(
        position_list_no_write_skeleton_header_path.exists(),
        "position.list real action no-write skeleton header exists",
    )
    require(
        position_list_no_write_skeleton_source_path.exists(),
        "position.list real action no-write skeleton source exists",
    )
    require(
        position_list_no_write_skeleton_test_path.exists(),
        "position.list real action no-write skeleton test exists",
    )
    require(
        position_list_failure_no_write_skeleton_test_path.exists(),
        "position.list real action failure no-write skeleton test exists",
    )
    require(root_cmake_path.exists(), "root CMakeLists exists")
    require(tests_cmake_path.exists(), "tests CMakeLists exists")
    require(accounting_engine_dir.exists(), "AccountingEngine module directory exists")
    require(accounting_engine_cmake_path.exists(), "AccountingEngine CMake exists")
    require(accounting_issue_header_path.exists(), "AccountingIssue header exists")
    require(accounting_replay_dtos_header_path.exists(), "AccountingReplayDtos header exists")
    require(accounting_replay_engine_header_path.exists(), "AccountingReplayEngine header exists")
    require(accounting_replay_result_header_path_prod.exists(), "production AccountingReplayResult header exists")
    require(accounting_replay_validation_header_path.exists(), "AccountingReplayValidation header exists")
    require(accounting_replay_parser_header_path.exists(), "AccountingReplayParser header exists")
    require(accounting_engine_boundary_test_path.exists(), "AccountingEngine boundary test exists")
    require(accounting_replay_engine_empty_ledger_test_path.exists(), "Accounting replay empty ledger test exists")
    require(accounting_replay_engine_single_buy_test_path.exists(), "Accounting replay single buy test exists")
    require(accounting_replay_engine_buy_sell_partial_test_path.exists(), "Accounting replay buy-sell partial test exists")
    require(
        accounting_replay_engine_sell_exceeds_position_test_path.exists(),
        "Accounting replay sell exceeds position test exists",
    )
    require(accounting_replay_engine_missing_fee_test_path.exists(), "Accounting replay missing fee test exists")
    require(accounting_replay_engine_negative_cash_test_path.exists(), "Accounting replay negative cash test exists")
    require(
        accounting_replay_engine_multi_instrument_buy_test_path.exists(),
        "Accounting replay multi-instrument buy test exists",
    )
    require(accounting_replay_engine_multi_account_buy_test_path.exists(), "Accounting replay multi-account buy test exists")
    require(
        accounting_replay_engine_multi_currency_unsupported_test_path.exists(),
        "Accounting replay multi-currency unsupported test exists",
    )
    require(
        accounting_replay_engine_missing_market_price_test_path.exists(),
        "Accounting replay missing market price test exists",
    )
    require(accounting_replay_dto_parser_test_path.exists(), "Accounting replay DTO parser test exists")
    require(accounting_engine_test_cmake_path.exists(), "AccountingEngine test CMake exists")
    require(accounting_fixture_dir.exists(), "accounting replay fixture directory exists")
    require(accounting_fixture_index_path.exists(), "accounting replay fixture index exists")
    require(accounting_fixture_validator_path.exists(), "accounting replay fixture validator exists")
    require(accounting_fixture_loader_header_path.exists(), "accounting fixture loader header exists")
    require(accounting_fixture_loader_source_path.exists(), "accounting fixture loader source exists")
    require(accounting_fixture_loader_test_path.exists(), "accounting fixture loader test exists")
    require(accounting_replay_stub_header_path.exists(), "accounting replay stub header exists")
    require(accounting_replay_stub_source_path.exists(), "accounting replay stub source exists")
    require(accounting_replay_result_header_path.exists(), "accounting replay result header exists")
    require(accounting_replay_result_source_path.exists(), "accounting replay result source exists")
    require(accounting_replay_mapper_header_path.exists(), "accounting replay result mapper header exists")
    require(accounting_replay_mapper_source_path.exists(), "accounting replay result mapper source exists")
    require(accounting_replay_result_test_path.exists(), "accounting replay result test exists")
    require(accounting_replay_harness_header_path.exists(), "accounting replay harness header exists")
    require(accounting_replay_harness_source_path.exists(), "accounting replay harness source exists")
    require(accounting_replay_harness_test_path.exists(), "accounting replay harness test exists")
    require(accounting_expected_inspector_header_path.exists(), "accounting expected output inspector header exists")
    require(accounting_expected_inspector_source_path.exists(), "accounting expected output inspector source exists")
    require(accounting_replay_assertion_header_path.exists(), "accounting replay assertion skeleton header exists")
    require(accounting_replay_assertion_source_path.exists(), "accounting replay assertion skeleton source exists")
    require(accounting_replay_assertion_test_path.exists(), "accounting replay assertion skeleton test exists")
    require(accounting_replay_minimal_header_path.exists(), "accounting replay minimal engine header exists")
    require(accounting_replay_minimal_source_path.exists(), "accounting replay minimal engine source exists")
    require(accounting_replay_minimal_test_path.exists(), "accounting replay minimal engine test exists")
    require(release_notes_path.exists(), "release notes doc exists")
    require(release_notes_v02_path.exists(), "v0.2 release notes doc exists")
    require(release_notes_v03_path.exists(), "v0.3 accounting replay release notes doc exists")
    require(release_notes_v04_path.exists(), "v0.4 AccountingEngine replay skeleton release notes doc exists")
    require(release_notes_v05_path.exists(), "v0.5 DataService accounting guard suite release notes doc exists")
    require(shell_accounting_readonly_ui_milestone_path.exists(), "docs/61 ShellAccounting readiness milestone exists")
    require(shell_accounting_next_phase_review_path.exists(), "docs/62 ShellAccounting next phase review exists")
    require(release_notes_v06_path.exists(), "v0.6 ShellAccounting readiness release notes doc exists")
    require(docs_index_path.exists(), "docs index exists")
    require(protocol_path.exists(), "protocol doc exists")
    require(accounting_rules_path.exists(), "accounting rules doc exists")
    require(dataservice_actions_header_path.exists(), "DataServiceActions header exists")
    require(dataservice_actions_source_path.exists(), "DataServiceActions source exists")
    require(dataservice_action_registrar_path.exists(), "DataService action registrar source exists")
    require(dataservice_client_header_path.exists(), "DataServiceClient header exists")
    require(dataservice_client_source_path.exists(), "DataServiceClient source exists")
    require(dataservice_readonly_test_path.exists(), "DataService read-only action test exists")
    require(dataservice_test_cmake_path.exists(), "DataService test CMake exists")
    require(dataservice_client_test_path.exists(), "DataServiceClient test exists")
    require(dataservice_client_test_cmake_path.exists(), "DataServiceClient test CMake exists")
    require(dataaccess_cmake_path.exists(), "DataAccess CMake exists")
    require(dataaccess_repositories_header_path.exists(), "DataAccess repositories header exists")
    require(accounting_trade_fact_reader_header_path.exists(), "Accounting trade fact reader header exists")
    require(accounting_trade_fact_reader_source_path.exists(), "Accounting trade fact reader source exists")
    require(dataaccess_test_cmake_path.exists(), "DataAccess test CMake exists")
    require(accounting_trade_fact_reader_test_path.exists(), "Accounting trade fact reader test exists")
    require(accounting_no_write_dir.exists(), "Accounting no-write test-only directory exists")
    require(accounting_no_write_cmake_path.exists(), "Accounting no-write CMake exists")
    require(accounting_no_write_utils_header_path.exists(), "Accounting no-write helper header exists")
    require(accounting_no_write_utils_source_path.exists(), "Accounting no-write helper source exists")
    require(forbidden_sql_scanner_header_path.exists(), "Forbidden SQL scanner header exists")
    require(forbidden_sql_scanner_source_path.exists(), "Forbidden SQL scanner source exists")
    require(accounting_no_write_harness_test_path.exists(), "Accounting no-write harness test exists")
    require(forbidden_sql_scanner_test_path.exists(), "Forbidden SQL scanner test exists")

    script = script_path.read_text(encoding="utf-8")
    stop_script = stop_script_path.read_text(encoding="utf-8")
    doc = doc_path.read_text(encoding="utf-8")
    milestone = milestone_path.read_text(encoding="utf-8")
    quick_start = quick_start_path.read_text(encoding="utf-8")
    business_acceptance = business_acceptance_path.read_text(encoding="utf-8")
    ui_readability = ui_readability_path.read_text(encoding="utf-8")
    ui_design = ui_design_path.read_text(encoding="utf-8")
    milestone_v02 = milestone_v02_path.read_text(encoding="utf-8")
    business_summary = business_summary_path.read_text(encoding="utf-8")
    position_boundary = position_boundary_path.read_text(encoding="utf-8")
    position_contract = position_contract_path.read_text(encoding="utf-8")
    position_stable_contract = position_stable_contract_path.read_text(encoding="utf-8")
    position_fixture_design = position_fixture_design_path.read_text(encoding="utf-8")
    accounting_fixture_samples = accounting_fixture_samples_path.read_text(encoding="utf-8")
    position_viewmodel_design = position_viewmodel_design_path.read_text(encoding="utf-8")
    position_mapping = position_mapping_path.read_text(encoding="utf-8")
    codex_prompt_template = codex_prompt_template_path.read_text(encoding="utf-8")
    accounting_replay_incremental_plan = accounting_replay_incremental_plan_path.read_text(encoding="utf-8")
    accounting_replay_milestone = accounting_replay_milestone_path.read_text(encoding="utf-8")
    accounting_replay_readiness = accounting_replay_readiness_path.read_text(encoding="utf-8")
    accounting_replay_architecture = accounting_replay_architecture_path.read_text(encoding="utf-8")
    accounting_engine_candidate = accounting_engine_candidate_path.read_text(encoding="utf-8")
    accounting_engine_replay_milestone = accounting_engine_replay_milestone_path.read_text(encoding="utf-8")
    accounting_engine_next_phase_review = accounting_engine_next_phase_review_path.read_text(encoding="utf-8")
    dataservice_readonly_accounting_contracts = dataservice_readonly_accounting_contracts_path.read_text(encoding="utf-8")
    dataservice_accounting_no_write_plan = dataservice_accounting_no_write_plan_path.read_text(encoding="utf-8")
    sqlite_readonly_facts_query_boundary = sqlite_readonly_facts_query_boundary_path.read_text(encoding="utf-8")
    accounting_facts_source_mapping = accounting_facts_source_mapping_path.read_text(encoding="utf-8")
    cash_facts_source_boundary = cash_facts_source_boundary_path.read_text(encoding="utf-8")
    cash_facts_query_decision = cash_facts_query_decision_path.read_text(encoding="utf-8")
    dataservice_guard_suite_milestone = dataservice_guard_suite_milestone_path.read_text(encoding="utf-8")
    dataservice_guard_no_write_readiness = dataservice_guard_no_write_readiness_path.read_text(encoding="utf-8")
    position_list_real_boundary = position_list_real_boundary_path.read_text(encoding="utf-8")
    position_list_readiness_checklist = position_list_readiness_checklist_path.read_text(encoding="utf-8")
    position_list_readiness_hardening = position_list_readiness_hardening_path.read_text(encoding="utf-8")
    position_list_first_stage_scope = position_list_first_stage_scope_path.read_text(encoding="utf-8")
    shellservices_accounting_controller_contract = shellservices_accounting_controller_contract_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_viewmodel_state_contract = shell_accounting_viewmodel_state_contract_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_scaffolding_cmake = shell_accounting_scaffolding_cmake_path.read_text(encoding="utf-8")
    shell_accounting_scenario_header = shell_accounting_scenario_header_path.read_text(encoding="utf-8")
    shell_accounting_scenario_source = shell_accounting_scenario_source_path.read_text(encoding="utf-8")
    shell_accounting_state_utils_header = shell_accounting_state_utils_header_path.read_text(encoding="utf-8")
    shell_accounting_state_utils_source = shell_accounting_state_utils_source_path.read_text(encoding="utf-8")
    shell_accounting_state_test = shell_accounting_state_test_path.read_text(encoding="utf-8")
    shell_accounting_issue_test = shell_accounting_issue_test_path.read_text(encoding="utf-8")
    shell_accounting_privacy_test = shell_accounting_privacy_test_path.read_text(encoding="utf-8")
    shell_accounting_no_trade_test = shell_accounting_no_trade_test_path.read_text(encoding="utf-8")
    shell_accounting_state_header = shell_accounting_state_header_path.read_text(encoding="utf-8")
    shell_accounting_state_source = shell_accounting_state_source_path.read_text(encoding="utf-8")
    shell_accounting_issue_header = shell_accounting_issue_header_path.read_text(encoding="utf-8")
    shell_accounting_issue_source = shell_accounting_issue_source_path.read_text(encoding="utf-8")
    shell_accounting_display_header = shell_accounting_display_header_path.read_text(encoding="utf-8")
    shell_accounting_display_source = shell_accounting_display_source_path.read_text(encoding="utf-8")
    shell_accounting_controller_header = shell_accounting_controller_header_path.read_text(encoding="utf-8")
    shell_accounting_controller_source = shell_accounting_controller_source_path.read_text(encoding="utf-8")
    shell_accounting_controller_state_test = shell_accounting_controller_state_test_path.read_text(encoding="utf-8")
    shell_accounting_controller_issue_test = shell_accounting_controller_issue_test_path.read_text(encoding="utf-8")
    shell_accounting_controller_privacy_test = shell_accounting_controller_privacy_test_path.read_text(encoding="utf-8")
    shell_accounting_controller_no_trade_test = shell_accounting_controller_no_trade_test_path.read_text(encoding="utf-8")
    shell_accounting_controller_no_dependency_test = shell_accounting_controller_no_dependency_test_path.read_text(
        encoding="utf-8"
    )
    shellservices_cmake = shellservices_cmake_path.read_text(encoding="utf-8")
    shellservices_public_header = shellservices_public_header_path.read_text(encoding="utf-8")
    shellservices_tests_cmake = shellservices_tests_cmake_path.read_text(encoding="utf-8")
    qml_sources = "\n".join(path.read_text(encoding="utf-8") for path in qml_dir.rglob("*.qml"))
    shell_accounting_readonly_page_path = qml_dir / "pages" / "ShellAccountingReadOnlyPage.qml"
    shell_accounting_readonly_page = (
        shell_accounting_readonly_page_path.read_text(encoding="utf-8")
        if shell_accounting_readonly_page_path.exists()
        else ""
    )
    authorized_shell_accounting_import_count = qml_sources.count("import ETFDecisionTerminal.ShellAccounting")
    authorized_shell_accounting_presenter_property_count = qml_sources.count(
        "property ShellAccountingPresenter accountingPresenter: null"
    )
    position_list_first_stage_cmake = position_list_first_stage_cmake_path.read_text(encoding="utf-8")
    position_list_first_stage_scenario_header = position_list_first_stage_scenario_header_path.read_text(encoding="utf-8")
    position_list_first_stage_scenario_source = position_list_first_stage_scenario_source_path.read_text(encoding="utf-8")
    position_list_first_stage_scope_test = position_list_first_stage_scope_test_path.read_text(encoding="utf-8")
    position_list_first_stage_transition_test = position_list_first_stage_transition_test_path.read_text(encoding="utf-8")
    position_list_first_stage_failure_test = position_list_first_stage_failure_test_path.read_text(encoding="utf-8")
    position_list_mapping_cmake = position_list_mapping_cmake_path.read_text(encoding="utf-8")
    position_list_mapping_utils_header = position_list_mapping_utils_header_path.read_text(encoding="utf-8")
    position_list_mapping_utils_source = position_list_mapping_utils_source_path.read_text(encoding="utf-8")
    position_list_request_mapping_test = position_list_request_mapping_test_path.read_text(encoding="utf-8")
    position_list_trade_fact_mapping_test = position_list_trade_fact_mapping_test_path.read_text(encoding="utf-8")
    position_list_response_mapping_test = position_list_response_mapping_test_path.read_text(encoding="utf-8")
    position_list_no_write_cmake = position_list_no_write_cmake_path.read_text(encoding="utf-8")
    position_list_no_write_skeleton_header = position_list_no_write_skeleton_header_path.read_text(encoding="utf-8")
    position_list_no_write_skeleton_source = position_list_no_write_skeleton_source_path.read_text(encoding="utf-8")
    position_list_no_write_skeleton_test = position_list_no_write_skeleton_test_path.read_text(encoding="utf-8")
    position_list_failure_no_write_skeleton_test = position_list_failure_no_write_skeleton_test_path.read_text(
        encoding="utf-8"
    )
    root_cmake = root_cmake_path.read_text(encoding="utf-8")
    tests_cmake = tests_cmake_path.read_text(encoding="utf-8")
    accounting_engine_cmake = accounting_engine_cmake_path.read_text(encoding="utf-8")
    accounting_replay_dtos_header = accounting_replay_dtos_header_path.read_text(encoding="utf-8")
    accounting_replay_engine_header = accounting_replay_engine_header_path.read_text(encoding="utf-8")
    accounting_replay_result_header_prod = accounting_replay_result_header_path_prod.read_text(encoding="utf-8")
    accounting_replay_validation_header = accounting_replay_validation_header_path.read_text(encoding="utf-8")
    accounting_replay_parser_header = accounting_replay_parser_header_path.read_text(encoding="utf-8")
    accounting_engine_boundary_test = accounting_engine_boundary_test_path.read_text(encoding="utf-8")
    accounting_replay_engine_empty_ledger_test = accounting_replay_engine_empty_ledger_test_path.read_text(encoding="utf-8")
    accounting_replay_engine_single_buy_test = accounting_replay_engine_single_buy_test_path.read_text(encoding="utf-8")
    accounting_replay_engine_buy_sell_partial_test = accounting_replay_engine_buy_sell_partial_test_path.read_text(
        encoding="utf-8"
    )
    accounting_replay_engine_sell_exceeds_position_test = (
        accounting_replay_engine_sell_exceeds_position_test_path.read_text(encoding="utf-8")
    )
    accounting_replay_engine_missing_fee_test = accounting_replay_engine_missing_fee_test_path.read_text(encoding="utf-8")
    accounting_replay_engine_negative_cash_test = accounting_replay_engine_negative_cash_test_path.read_text(encoding="utf-8")
    accounting_replay_engine_multi_instrument_buy_test = accounting_replay_engine_multi_instrument_buy_test_path.read_text(
        encoding="utf-8"
    )
    accounting_replay_engine_multi_account_buy_test = accounting_replay_engine_multi_account_buy_test_path.read_text(
        encoding="utf-8"
    )
    accounting_replay_engine_multi_currency_unsupported_test = (
        accounting_replay_engine_multi_currency_unsupported_test_path.read_text(encoding="utf-8")
    )
    accounting_replay_engine_missing_market_price_test = (
        accounting_replay_engine_missing_market_price_test_path.read_text(encoding="utf-8")
    )
    accounting_replay_dto_parser_test = accounting_replay_dto_parser_test_path.read_text(encoding="utf-8")
    accounting_engine_test_cmake = accounting_engine_test_cmake_path.read_text(encoding="utf-8")
    accounting_engine_sources = "\n".join(
        path.read_text(encoding="utf-8")
        for path in sorted(accounting_engine_dir.rglob("*"))
        if path.is_file() and path.suffix in {".h", ".hpp", ".cpp", ".cmake", ".txt"}
    )
    accounting_fixture_index = accounting_fixture_index_path.read_text(encoding="utf-8")
    accounting_fixture_validator = accounting_fixture_validator_path.read_text(encoding="utf-8")
    accounting_fixture_loader_header = accounting_fixture_loader_header_path.read_text(encoding="utf-8")
    accounting_fixture_loader_source = accounting_fixture_loader_source_path.read_text(encoding="utf-8")
    accounting_fixture_loader_test = accounting_fixture_loader_test_path.read_text(encoding="utf-8")
    accounting_replay_stub_header = accounting_replay_stub_header_path.read_text(encoding="utf-8")
    accounting_replay_stub_source = accounting_replay_stub_source_path.read_text(encoding="utf-8")
    accounting_replay_result_header = accounting_replay_result_header_path.read_text(encoding="utf-8")
    accounting_replay_result_source = accounting_replay_result_source_path.read_text(encoding="utf-8")
    accounting_replay_mapper_header = accounting_replay_mapper_header_path.read_text(encoding="utf-8")
    accounting_replay_mapper_source = accounting_replay_mapper_source_path.read_text(encoding="utf-8")
    accounting_replay_result_test = accounting_replay_result_test_path.read_text(encoding="utf-8")
    accounting_replay_harness_header = accounting_replay_harness_header_path.read_text(encoding="utf-8")
    accounting_replay_harness_source = accounting_replay_harness_source_path.read_text(encoding="utf-8")
    accounting_replay_harness_test = accounting_replay_harness_test_path.read_text(encoding="utf-8")
    accounting_expected_inspector_header = accounting_expected_inspector_header_path.read_text(encoding="utf-8")
    accounting_expected_inspector_source = accounting_expected_inspector_source_path.read_text(encoding="utf-8")
    accounting_replay_assertion_header = accounting_replay_assertion_header_path.read_text(encoding="utf-8")
    accounting_replay_assertion_source = accounting_replay_assertion_source_path.read_text(encoding="utf-8")
    accounting_replay_assertion_test = accounting_replay_assertion_test_path.read_text(encoding="utf-8")
    accounting_replay_minimal_header = accounting_replay_minimal_header_path.read_text(encoding="utf-8")
    accounting_replay_minimal_source = accounting_replay_minimal_source_path.read_text(encoding="utf-8")
    accounting_replay_minimal_test = accounting_replay_minimal_test_path.read_text(encoding="utf-8")
    shell_accounting_service_adapter_boundary = shell_accounting_service_adapter_boundary_path.read_text(encoding="utf-8")
    shell_accounting_service_integration_readiness = shell_accounting_service_integration_readiness_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_dataservice_adapter_boundary = shell_accounting_dataservice_adapter_boundary_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_dataservice_adapter_test_plan = shell_accounting_dataservice_adapter_test_plan_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_dataservice_adapter_live_call_gate = (
        shell_accounting_dataservice_adapter_live_call_gate_path.read_text(encoding="utf-8")
    )
    shell_accounting_dataservice_adapter_live_call_checklist = (
        shell_accounting_dataservice_adapter_live_call_checklist_path.read_text(encoding="utf-8")
    )
    shell_accounting_presenter_contract = shell_accounting_presenter_contract_path.read_text(encoding="utf-8")
    shell_accounting_qml_binding_readiness = shell_accounting_qml_binding_readiness_path.read_text(encoding="utf-8")
    shell_accounting_qml_binding_smoke_plan = shell_accounting_qml_binding_smoke_plan_path.read_text(encoding="utf-8")
    shell_accounting_qml_static_gate = shell_accounting_qml_static_gate_path.read_text(encoding="utf-8")
    shell_accounting_readonly_ui_milestone = shell_accounting_readonly_ui_milestone_path.read_text(encoding="utf-8")
    shell_accounting_next_phase_review = shell_accounting_next_phase_review_path.read_text(encoding="utf-8")
    shell_accounting_production_qml_boundary = shell_accounting_production_qml_boundary_path.read_text(encoding="utf-8")
    shell_accounting_first_stage_qml_checklist = shell_accounting_first_stage_qml_checklist_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_qml_type_registration_boundary = (
        shell_accounting_qml_type_registration_boundary_path.read_text(encoding="utf-8")
    )
    shell_accounting_qml_type_registration_test_plan = (
        shell_accounting_qml_type_registration_test_plan_path.read_text(encoding="utf-8")
    )
    shell_accounting_qml_type_registration_implementation_gate = (
        shell_accounting_qml_type_registration_implementation_gate_path.read_text(encoding="utf-8")
    )
    shell_accounting_qml_type_registration_rollback_strategy = (
        shell_accounting_qml_type_registration_rollback_strategy_path.read_text(encoding="utf-8")
    )
    shell_accounting_qml_startup_registration_gate = (
        shell_accounting_qml_startup_registration_gate_path.read_text(encoding="utf-8")
    )
    shell_accounting_qml_startup_registration_test_plan = (
        shell_accounting_qml_startup_registration_test_plan_path.read_text(encoding="utf-8")
    )
    shell_accounting_qml_startup_registration_wiring = (
        shell_accounting_qml_startup_registration_wiring_path.read_text(encoding="utf-8")
    )
    shell_accounting_production_qml_binding_gate = (
        shell_accounting_production_qml_binding_gate_path.read_text(encoding="utf-8")
    )
    shell_accounting_production_qml_binding_test_plan = (
        shell_accounting_production_qml_binding_test_plan_path.read_text(encoding="utf-8")
    )
    shell_accounting_production_qml_binding_implementation = (
        shell_accounting_production_qml_binding_implementation_path.read_text(encoding="utf-8")
    )
    shell_accounting_presenter_lifecycle_gate = shell_accounting_presenter_lifecycle_gate_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_presenter_lifecycle_test_plan = (
        shell_accounting_presenter_lifecycle_test_plan_path.read_text(encoding="utf-8")
    )
    shell_accounting_presenter_lifecycle_implementation = (
        shell_accounting_presenter_lifecycle_implementation_path.read_text(encoding="utf-8")
    )
    shell_accounting_real_data_adapter_gate = shell_accounting_real_data_adapter_gate_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_real_data_adapter_test_plan = shell_accounting_real_data_adapter_test_plan_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_real_data_adapter_implementation = (
        shell_accounting_real_data_adapter_implementation_path.read_text(encoding="utf-8")
    )
    shell_accounting_dataservice_facts_query_gate = shell_accounting_dataservice_facts_query_gate_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_dataservice_facts_query_test_plan = (
        shell_accounting_dataservice_facts_query_test_plan_path.read_text(encoding="utf-8")
    )
    shell_accounting_dataservice_facts_query_implementation = (
        shell_accounting_dataservice_facts_query_implementation_path.read_text(encoding="utf-8")
    )
    shell_accounting_replay_snapshot_rebuild_gate = (
        shell_accounting_replay_snapshot_rebuild_gate_path.read_text(encoding="utf-8")
    )
    shell_accounting_replay_snapshot_rebuild_test_plan = (
        shell_accounting_replay_snapshot_rebuild_test_plan_path.read_text(encoding="utf-8")
    )
    shell_accounting_readonly_replay_implementation = (
        shell_accounting_readonly_replay_implementation_path.read_text(encoding="utf-8")
    )
    shell_accounting_qml_static_gate_cmake = shell_accounting_qml_static_gate_cmake_path.read_text(encoding="utf-8")
    shell_accounting_qml_binding_smoke_cmake = shell_accounting_qml_binding_smoke_cmake_path.read_text(encoding="utf-8")
    shell_accounting_qml_smoke_runtime_cmake = shell_accounting_qml_smoke_runtime_cmake_path.read_text(encoding="utf-8")
    shell_accounting_qml_type_registration_cmake = shell_accounting_qml_type_registration_cmake_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_qml_type_registration_gate_cmake = shell_accounting_qml_type_registration_gate_cmake_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_qml_type_registration_implementation_cmake = (
        shell_accounting_qml_type_registration_implementation_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_qml_startup_registration_gate_cmake = (
        shell_accounting_qml_startup_registration_gate_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_qml_startup_registration_wiring_cmake = (
        shell_accounting_qml_startup_registration_wiring_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_production_qml_binding_gate_cmake = (
        shell_accounting_production_qml_binding_gate_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_production_qml_binding_implementation_cmake = (
        shell_accounting_production_qml_binding_implementation_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_presenter_lifecycle_gate_cmake = shell_accounting_presenter_lifecycle_gate_cmake_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_presenter_lifecycle_implementation_cmake = (
        shell_accounting_presenter_lifecycle_implementation_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_real_data_adapter_gate_cmake = (
        shell_accounting_real_data_adapter_gate_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_real_data_adapter_implementation_cmake = (
        shell_accounting_real_data_adapter_implementation_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_dataservice_facts_query_gate_cmake = (
        shell_accounting_dataservice_facts_query_gate_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_dataservice_facts_query_implementation_cmake = (
        shell_accounting_dataservice_facts_query_implementation_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_replay_snapshot_rebuild_gate_cmake = (
        shell_accounting_replay_snapshot_rebuild_gate_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_readonly_replay_implementation_cmake = (
        shell_accounting_readonly_replay_implementation_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_qml_registration_header = shell_accounting_qml_registration_header_path.read_text(encoding="utf-8")
    shell_accounting_qml_registration_source = shell_accounting_qml_registration_source_path.read_text(encoding="utf-8")
    shell_accounting_presenter_header = shell_accounting_presenter_header_path.read_text(encoding="utf-8")
    shell_accounting_presenter_source = shell_accounting_presenter_source_path.read_text(encoding="utf-8")
    shell_accounting_presenter_cmake = shell_accounting_presenter_cmake_path.read_text(encoding="utf-8")
    shell_accounting_dataservice_adapter_header = shell_accounting_dataservice_adapter_header_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_dataservice_adapter_source = shell_accounting_dataservice_adapter_source_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_dataservice_client_port_header = shell_accounting_dataservice_client_port_header_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_dataservice_client_port_adapter_header = (
        shell_accounting_dataservice_client_port_adapter_header_path.read_text(encoding="utf-8")
    )
    shell_accounting_dataservice_client_port_adapter_source = (
        shell_accounting_dataservice_client_port_adapter_source_path.read_text(encoding="utf-8")
    )
    shell_accounting_status_object_header = shell_accounting_status_object_header_path.read_text(encoding="utf-8")
    shell_accounting_status_object_source = shell_accounting_status_object_source_path.read_text(encoding="utf-8")
    shell_accounting_issue_list_model_header = shell_accounting_issue_list_model_header_path.read_text(encoding="utf-8")
    shell_accounting_issue_list_model_source = shell_accounting_issue_list_model_source_path.read_text(encoding="utf-8")
    shell_position_list_model_header = shell_position_list_model_header_path.read_text(encoding="utf-8")
    shell_position_list_model_source = shell_position_list_model_source_path.read_text(encoding="utf-8")
    shell_accounting_viewmodel_cmake = shell_accounting_viewmodel_cmake_path.read_text(encoding="utf-8")
    shell_accounting_dataservice_adapter_skeleton_cmake = (
        shell_accounting_dataservice_adapter_skeleton_cmake_path.read_text(encoding="utf-8")
    )
    spy_shell_accounting_dataservice_client_port_header = (
        spy_shell_accounting_dataservice_client_port_header_path.read_text(encoding="utf-8")
    )
    spy_shell_accounting_dataservice_client_port_source = (
        spy_shell_accounting_dataservice_client_port_source_path.read_text(encoding="utf-8")
    )
    shell_accounting_dataservice_adapter_live_call_gate_cmake = (
        shell_accounting_dataservice_adapter_live_call_gate_cmake_path.read_text(encoding="utf-8")
    )
    shell_accounting_dataservice_adapter_scaffolding_cmake = (
        shell_accounting_dataservice_adapter_scaffolding_cmake_path.read_text(encoding="utf-8")
    )
    spy_accounting_dataservice_client_header = spy_accounting_dataservice_client_header_path.read_text(encoding="utf-8")
    spy_accounting_dataservice_client_source = spy_accounting_dataservice_client_source_path.read_text(encoding="utf-8")
    shell_accounting_dataservice_expected_call_header = shell_accounting_dataservice_expected_call_header_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_dataservice_expected_call_source = shell_accounting_dataservice_expected_call_source_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_service_adapter_header = shell_accounting_service_adapter_header_path.read_text(encoding="utf-8")
    shell_accounting_service_types_header = shell_accounting_service_types_header_path.read_text(encoding="utf-8")
    shell_accounting_service_types_source = shell_accounting_service_types_source_path.read_text(encoding="utf-8")
    shell_accounting_service_adapter_cmake = shell_accounting_service_adapter_cmake_path.read_text(encoding="utf-8")
    fake_shell_accounting_service_adapter_header = fake_shell_accounting_service_adapter_header_path.read_text(encoding="utf-8")
    fake_shell_accounting_service_adapter_source = fake_shell_accounting_service_adapter_source_path.read_text(encoding="utf-8")
    shell_accounting_service_adapter_interface_test = shell_accounting_service_adapter_interface_test_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_service_result_contract_test = shell_accounting_service_result_contract_test_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_service_adapter_no_dependency_test = shell_accounting_service_adapter_no_dependency_test_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_controller_adapter_injection_test = shell_accounting_controller_adapter_injection_test_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_fake_adapter_refresh_success_test = (
        shell_accounting_fake_adapter_refresh_success_test_path.read_text(encoding="utf-8")
    )
    shell_accounting_fake_adapter_unavailable_test = shell_accounting_fake_adapter_unavailable_test_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_fake_adapter_protocol_error_test = shell_accounting_fake_adapter_protocol_error_test_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_fake_adapter_issue_mapping_test = shell_accounting_fake_adapter_issue_mapping_test_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_fake_adapter_privacy_no_trade_test = (
        shell_accounting_fake_adapter_privacy_no_trade_test_path.read_text(encoding="utf-8")
    )
    shell_accounting_fake_adapter_no_dependency_test = shell_accounting_fake_adapter_no_dependency_test_path.read_text(
        encoding="utf-8"
    )
    release_notes = release_notes_path.read_text(encoding="utf-8")
    release_notes_v02 = release_notes_v02_path.read_text(encoding="utf-8")
    release_notes_v03 = release_notes_v03_path.read_text(encoding="utf-8")
    release_notes_v04 = release_notes_v04_path.read_text(encoding="utf-8")
    release_notes_v05 = release_notes_v05_path.read_text(encoding="utf-8")
    release_notes_v06 = release_notes_v06_path.read_text(encoding="utf-8")
    docs_index = docs_index_path.read_text(encoding="utf-8")
    protocol_doc = protocol_path.read_text(encoding="utf-8")
    accounting_rules = accounting_rules_path.read_text(encoding="utf-8")
    readme = readme_path.read_text(encoding="utf-8")
    gitignore = gitignore_path.read_text(encoding="utf-8")
    dataservice_actions_header = dataservice_actions_header_path.read_text(encoding="utf-8")
    dataservice_actions_source = dataservice_actions_source_path.read_text(encoding="utf-8")
    dataservice_action_registrar = dataservice_action_registrar_path.read_text(encoding="utf-8")
    dataservice_client_header = dataservice_client_header_path.read_text(encoding="utf-8")
    dataservice_client_source = dataservice_client_source_path.read_text(encoding="utf-8")
    dataservice_readonly_test = dataservice_readonly_test_path.read_text(encoding="utf-8")
    dataservice_test_cmake = dataservice_test_cmake_path.read_text(encoding="utf-8")
    dataservice_client_test = dataservice_client_test_path.read_text(encoding="utf-8")
    dataservice_client_test_cmake = dataservice_client_test_cmake_path.read_text(encoding="utf-8")
    dataaccess_cmake = dataaccess_cmake_path.read_text(encoding="utf-8")
    dataaccess_repositories_header = dataaccess_repositories_header_path.read_text(encoding="utf-8")
    accounting_trade_fact_reader_header = accounting_trade_fact_reader_header_path.read_text(encoding="utf-8")
    accounting_trade_fact_reader_source = accounting_trade_fact_reader_source_path.read_text(encoding="utf-8")
    dataaccess_test_cmake = dataaccess_test_cmake_path.read_text(encoding="utf-8")
    accounting_trade_fact_reader_test = accounting_trade_fact_reader_test_path.read_text(encoding="utf-8")
    accounting_no_write_cmake = accounting_no_write_cmake_path.read_text(encoding="utf-8")
    accounting_no_write_utils_header = accounting_no_write_utils_header_path.read_text(encoding="utf-8")
    accounting_no_write_utils_source = accounting_no_write_utils_source_path.read_text(encoding="utf-8")
    forbidden_sql_scanner_header = forbidden_sql_scanner_header_path.read_text(encoding="utf-8")
    forbidden_sql_scanner_source = forbidden_sql_scanner_source_path.read_text(encoding="utf-8")
    accounting_no_write_harness_test = accounting_no_write_harness_test_path.read_text(encoding="utf-8")
    forbidden_sql_scanner_test = forbidden_sql_scanner_test_path.read_text(encoding="utf-8")
    shell_accounting_fake_client_cmake = shell_accounting_fake_client_cmake_path.read_text(encoding="utf-8")
    fake_accounting_client_payload_header = fake_accounting_client_payload_header_path.read_text(encoding="utf-8")
    fake_accounting_client_payload_source = fake_accounting_client_payload_source_path.read_text(encoding="utf-8")
    fake_accounting_client_scenario_header = fake_accounting_client_scenario_header_path.read_text(encoding="utf-8")
    fake_accounting_client_scenario_source = fake_accounting_client_scenario_source_path.read_text(encoding="utf-8")
    shell_accounting_fake_guard_payload_test = shell_accounting_fake_guard_payload_test_path.read_text(encoding="utf-8")
    shell_accounting_fake_state_transition_test = shell_accounting_fake_state_transition_test_path.read_text(encoding="utf-8")
    shell_accounting_fake_issue_mapping_test = shell_accounting_fake_issue_mapping_test_path.read_text(encoding="utf-8")
    shell_accounting_fake_privacy_no_trade_test = shell_accounting_fake_privacy_no_trade_test_path.read_text(
        encoding="utf-8"
    )
    shell_accounting_fake_no_real_dependency_test = shell_accounting_fake_no_real_dependency_test_path.read_text(
        encoding="utf-8"
    )
    position_list_guard_source = extract_between(
        dataservice_actions_source,
        "etfdt::protocol::ProtocolResponse handlePositionList",
        "etfdt::protocol::ProtocolResponse handleCashSummary",
    )
    cash_summary_guard_source = extract_between(
        dataservice_actions_source,
        "etfdt::protocol::ProtocolResponse handleCashSummary",
        "etfdt::protocol::ProtocolResponse handlePortfolioPnlSummary",
    )
    portfolio_pnl_summary_guard_source = extract_between(
        dataservice_actions_source,
        "etfdt::protocol::ProtocolResponse handlePortfolioPnlSummary",
        "etfdt::protocol::ProtocolResponse handleBasePositionSummary",
    )
    base_position_summary_guard_source = extract_between(
        dataservice_actions_source,
        "etfdt::protocol::ProtocolResponse handleBasePositionSummary",
        "etfdt::protocol::ProtocolResponse handleSniperPoolSummary",
    )
    sniper_pool_summary_guard_source = extract_between(
        dataservice_actions_source,
        "etfdt::protocol::ProtocolResponse handleSniperPoolSummary",
        "}  // namespace etfdt::data_service_api",
    )
    gitignore_lines = {line.strip() for line in gitignore.splitlines()}

    require("runtime/" in gitignore_lines, ".gitignore ignores runtime/")
    require("*.db-wal" in gitignore_lines, ".gitignore ignores sqlite wal files")
    require("*.db-shm" in gitignore_lines, ".gitignore ignores sqlite shm files")
    require("*.pid" in gitignore_lines, ".gitignore ignores pid files")
    require("migrations/" not in gitignore_lines, ".gitignore does not ignore migrations directory")
    require("*.sql" not in gitignore_lines, ".gitignore does not ignore migration sql files")

    require("run_readonly_demo.ps1" in readme, "README references run_readonly_demo.ps1")
    require("stop_readonly_demo.ps1" in readme, "README references stop_readonly_demo.ps1")
    require("13_manual_acceptance_readonly_shell.md" in readme, "README references manual acceptance doc")
    require("v0.1 ReadOnly Shell Demo" in readme, "README names current milestone")
    require("15_quick_start_readonly_shell_demo.md" in readme, "README links quick start doc")
    require("14_milestone_v0_1_readonly_shell_demo.md" in readme, "README links milestone doc")
    require("release_notes/v0_1_readonly_shell_demo.md" in readme, "README links release notes")
    require(
        "16_manual_acceptance_readonly_business_pages.md" in readme,
        "README links read-only business acceptance doc",
    )
    require("17_ui_readability_checklist.md" in readme, "README links UI readability checklist")
    require("v0.2 ReadOnly Business Pages" in readme, "README documents v0.2 read-only business pages")
    require(
        "18_milestone_v0_2_readonly_business_pages.md" in readme,
        "README links v0.2 milestone doc",
    )
    require("v0_2_readonly_business_pages.md" in readme, "README links v0.2 release notes")
    require("19_readonly_business_pages_summary.md" in readme, "README links read-only business summary")
    require("position_accounting_boundary" in readme, "README links position accounting boundary")
    require(
        "21_position_readonly_data_contract_draft.md" in readme,
        "README links position read-only data contract draft",
    )
    require("position_accounting_data_contract" in readme, "README links position accounting data contract")
    require(
        "23_position_accounting_test_fixture_design.md" in readme,
        "README links position accounting fixture design",
    )
    require(
        "24_accounting_replay_fixture_samples.md" in readme,
        "README links accounting replay fixture samples",
    )
    require("tests/fixtures/accounting_replay" in readme, "README links accounting replay fixture files")
    require("AccountingFixtureLoader" in readme, "README documents accounting fixture loader")
    require("AccountingReplayTestHarness" in readme, "README documents replay harness")
    require("AccountingReplayStubEngine" in readme, "README documents replay stub engine")
    require("AccountingReplayResult" in readme, "README documents replay result")
    require("AccountingReplayResultMapper" in readme, "README documents replay result mapper")
    require("AccountingExpectedOutputInspector" in readme, "README documents expected output inspector")
    require("AccountingReplayAssertionSkeleton" in readme, "README documents assertion skeleton")
    require("SKIPPED_BY_DESIGN" in readme, "README documents skipped assertion state")
    require("AccountingReplayMinimalEngine" in readme, "README documents minimal replay engine")
    require("FX001_EMPTY_LEDGER" in readme, "README documents FX001 minimal replay")
    require("FX002_SINGLE_BUY" in readme, "README documents FX002 minimal replay")
    require("FX003_BUY_SELL_PARTIAL" in readme, "README documents FX003 minimal replay")
    require("FX004_SELL_EXCEEDS_POSITION" in readme, "README documents FX004 minimal replay")
    require("FX005_MISSING_FEE" in readme, "README documents FX005 minimal replay")
    require("FX006_NEGATIVE_CASH" in readme, "README documents FX006 minimal replay")
    require("FX007_MULTI_INSTRUMENT" in readme, "README documents FX007 minimal replay")
    require("FX008_MULTI_ACCOUNT" in readme, "README documents FX008 minimal replay")
    require("FX009_BASE_POSITION_LOCKED" in readme, "README documents FX009 minimal replay")
    require("FX010_SNIPER_TIER_COMPLETED" in readme, "README documents FX010 minimal replay")
    require("FX011_STALE_SNAPSHOT" in readme, "README documents FX011 minimal replay")
    require("FX012_MISSING_MARKET_PRICE" in readme, "README documents FX012 minimal replay")
    require("FX013_MULTI_CURRENCY_UNSUPPORTED" in readme, "README documents FX013 minimal replay")
    require("v0.3 Accounting Replay Test-only Coverage" in readme, "README documents v0.3 accounting replay milestone")
    require(
        "31_accounting_replay_testonly_coverage_milestone.md" in readme,
        "README links accounting replay test-only milestone",
    )
    require(
        "32_production_accounting_replay_readiness_review.md" in readme,
        "README links production accounting replay readiness review",
    )
    require(
        "v0_3_accounting_replay_testonly_coverage.md" in readme,
        "README links v0.3 accounting replay release notes",
    )
    require(
        "33_production_accounting_replay_architecture.md" in readme,
        "README links production accounting replay architecture",
    )
    require(
        "34_accounting_engine_module_candidate.md" in readme,
        "README links AccountingEngine module candidate",
    )
    require(
        "35_accounting_engine_replay_skeleton_milestone" in readme,
        "README links AccountingEngine replay skeleton milestone",
    )
    require(
        "36_accounting_engine_next_phase_boundary_review" in readme,
        "README links AccountingEngine next phase boundary review",
    )
    require(
        "37_dataservice_readonly_accounting_action_contracts" in readme,
        "README links DataService read-only accounting action contracts",
    )
    require(
        "38_dataservice_accounting_no_write_test_plan" in readme,
        "README links DataService accounting no-write test plan",
    )
    require(
        "39_sqlite_readonly_facts_query_boundary" in readme,
        "README links SQLite read-only facts query boundary",
    )
    require(
        "40_accounting_facts_source_mapping" in readme,
        "README links accounting facts source mapping",
    )
    require(
        "41_cash_facts_source_boundary" in readme,
        "README links cash facts source boundary",
    )
    require(
        "42_cash_facts_query_decision" in readme,
        "README links cash facts query decision",
    )
    require(
        "v0.5 DataService Accounting Guard Suite" in readme,
        "README documents v0.5 DataService Accounting Guard Suite",
    )
    require(
        "43_dataservice_accounting_guard_suite_milestone" in readme,
        "README links DataService accounting guard suite milestone",
    )
    require(
        "44_dataservice_accounting_guard_no_write_readiness_review" in readme,
        "README links DataService accounting guard no-write readiness review",
    )
    require(
        "45_position_list_real_implementation_boundary" in readme,
        "README links position.list real implementation boundary",
    )
    require(
        "46_position_list_real_implementation_readiness_checklist" in readme,
        "README links position.list real implementation readiness checklist",
    )
    require(
        "v0_5_dataservice_accounting_guard_suite" in readme,
        "README links v0.5 DataService accounting guard suite release notes",
    )
    require(
        "v0_4_accounting_engine_replay_skeleton" in readme,
        "README links v0.4 AccountingEngine replay skeleton release notes",
    )
    require("FX001-FX013 test-only coverage is complete" in readme, "README states FX001-FX013 test-only coverage complete")
    require("not production accounting replay" in readme, "README states test-only coverage is not production replay")
    require("not direct code migration" in readme, "README states next phase is not direct code migration")
    require("accounting_replay_minimal_fx001" in readme, "README documents minimal FX001 CTest")
    require("accounting_replay_minimal_fx002" in readme, "README documents minimal FX002 CTest")
    require("accounting_replay_minimal_fx003" in readme, "README documents minimal FX003 CTest")
    require("accounting_replay_minimal_fx004" in readme, "README documents minimal FX004 CTest")
    require("accounting_replay_minimal_fx005" in readme, "README documents minimal FX005 CTest")
    require("accounting_replay_minimal_fx006" in readme, "README documents minimal FX006 CTest")
    require("accounting_replay_minimal_fx007" in readme, "README documents minimal FX007 CTest")
    require("accounting_replay_minimal_fx008" in readme, "README documents minimal FX008 CTest")
    require("accounting_replay_minimal_fx009" in readme, "README documents minimal FX009 CTest")
    require("accounting_replay_minimal_fx010" in readme, "README documents minimal FX010 CTest")
    require("accounting_replay_minimal_fx011" in readme, "README documents minimal FX011 CTest")
    require("accounting_replay_minimal_fx012" in readme, "README documents minimal FX012 CTest")
    require("accounting_replay_minimal_fx013" in readme, "README documents minimal FX013 CTest")
    require("NOT_IMPLEMENTED" in readme, "README documents replay stub status")
    require(
        "25_position_shell_viewmodel_design.md" in readme,
        "README links position Shell ViewModel design",
    )
    require("26_position_dto_viewmodel_mapping.md" in readme, "README links position DTO mapping")
    require("accounting.health" in readme, "README documents accounting.health")
    require("accounting.replay.preview" in readme, "README documents accounting replay preview guard")
    require("REPLAY_NOT_AVAILABLE" in readme, "README documents replay preview unavailable status")
    require("runtime/" in readme, "README documents runtime output directory")
    require("ForceRecreateDb" in readme, "README documents ForceRecreateDb")

    require(
        "只读数据预览" in doc or "鍙鏁版嵁棰勮" in doc,
        "manual doc mentions readonly page",
    )
    require("ETFDataServiceReadonly" in doc, "manual doc mentions default socket")
    require(
        "不得调用 `data.audit.append`" in doc
        or "涓嶅緱璋冪敤 `data.audit.append`" in doc,
        "manual doc prohibits audit append",
    )
    require("runtime/" in doc, "manual doc documents runtime output directory")
    require("ForceRecreateDb" in doc, "manual doc documents ForceRecreateDb")
    require("KeepDataService" in doc, "manual doc documents KeepDataService")
    require("NoShell" in doc, "manual doc documents NoShell")

    require("data.audit.append" not in script, "script must not call audit append")
    require("--serve-dev-audit" not in script, "script must not start audit dev service")
    require("--serve-readonly" in script, "script starts read-only service")
    require("--init-db" in script, "script initializes demo database")
    require("--check-db" in script, "script checks demo database")
    require("KeepDataService" in script, "script supports KeepDataService")
    require("ForceRecreateDb" in script, "script supports ForceRecreateDb")
    require("WritePidFile" in script, "script supports WritePidFile")
    require(".pid" in script, "script writes a pid file")
    require("Stop-StartedDataService" in script, "script has scoped cleanup function")
    require("ETFDecisionShell" in script, "script can launch ETFDecisionShell")
    require("runtime\\dev\\readonly_demo\\ETFDecision.db" in script, "script uses default demo database")

    require("ForceAll" in stop_script, "stop script requires explicit ForceAll option")
    require(
        'Get-Process -Name "ETFDataService"' in stop_script,
        "stop script can find DataService only for ForceAll",
    )
    require(
        stop_script.index("if (!$ForceAll)") < stop_script.index('Get-Process -Name "ETFDataService"'),
        "stop script checks ForceAll before scanning all ETFDataService processes",
    )

    require("v0.1 ReadOnly Shell Demo" in milestone, "milestone doc has milestone name")
    require("不支持自动交易" in milestone, "milestone doc states automatic trading is unsupported")
    require("不写 TradeLog" in milestone, "milestone doc states TradeLog is not written")
    require("ETFDataService --serve-readonly" in milestone, "milestone doc lists readonly service path")
    require("runtime/" in milestone, "milestone doc explains runtime output")

    require("run_readonly_demo.ps1" in quick_start, "quick start documents demo script")
    require("Qt `6.9.3`" in quick_start, "quick start documents Qt version")
    require("C:\\Qt\\6.9.3\\msvc2022_64" in quick_start, "quick start documents Qt path")
    require("stop_readonly_demo.ps1" in quick_start, "quick start documents cleanup script")

    require("账户与组合" in business_acceptance, "business acceptance doc mentions account portfolio page")
    require("标的与策略" in business_acceptance, "business acceptance doc mentions instrument strategy page")
    require("筛选" in business_acceptance, "business acceptance doc mentions filtering")
    require("排序" in business_acceptance, "business acceptance doc mentions sorting")
    require("列显示" in business_acceptance, "business acceptance doc mentions column visibility")
    require("密度" in business_acceptance, "business acceptance doc mentions density")
    require(
        "不得调用 data.audit.append" in business_acceptance,
        "business acceptance doc prohibits data.audit.append",
    )
    require("不写 trade_log" in business_acceptance, "business acceptance doc prohibits trade_log writes")
    require("QML must not directly call `DataServiceClient`" in business_acceptance, "business acceptance doc prohibits direct DataServiceClient use")
    require("QML must not directly access SQLite" in business_acceptance, "business acceptance doc prohibits direct SQLite access")
    require("runtime/" in business_acceptance, "business acceptance doc mentions runtime cleanup")

    require("禁止写入按钮" in ui_readability, "UI readability checklist includes prohibited write button rule")
    require("QML 不做业务逻辑" in ui_readability, "UI readability checklist includes QML business logic rule")
    require("Mock / Placeholder / ReadOnly" in ui_readability, "UI readability checklist includes state labels")
    require("Column Visibility Standards" in ui_readability, "UI readability checklist includes column standards")
    require("Density Setting Standards" in ui_readability, "UI readability checklist includes density standards")

    require("v0.2 ReadOnly Business Pages" in milestone_v02, "v0.2 milestone doc has milestone name")
    require("账户与组合" in milestone_v02, "v0.2 milestone doc mentions account portfolio")
    require("标的与策略" in milestone_v02, "v0.2 milestone doc mentions instrument strategy")
    require("不支持自动交易" in milestone_v02, "v0.2 milestone doc states automatic trading is unsupported")
    require("不写 TradeLog" in milestone_v02, "v0.2 milestone doc states TradeLog is not written")
    require("QML still must not directly access SQLite" in milestone_v02, "v0.2 milestone doc prohibits direct SQLite access")
    require("must not directly call `DataServiceClient`" in milestone_v02, "v0.2 milestone doc prohibits direct DataServiceClient use")
    require("v0.2.0-readonly-business-pages" in milestone_v02, "v0.2 milestone doc includes suggested tag")

    require("Release Notes - v0.2 ReadOnly Business Pages" in release_notes_v02, "v0.2 release notes title")
    require("Suggested Tag After Merge" in release_notes_v02, "v0.2 release notes include suggested tag section")
    require("v0.2.0-readonly-business-pages" in release_notes_v02, "v0.2 release notes include suggested tag")
    require("Auto trading" in release_notes_v02, "v0.2 release notes list excluded auto trading")
    require("Writing to `trade_log`" in release_notes_v02, "v0.2 release notes list excluded trade_log writing")
    require("Release Notes - v0.3 Accounting Replay Test-only Coverage" in release_notes_v03, "v0.3 release notes title")
    require("FX001-FX013 test-only minimal replay coverage complete" in release_notes_v03, "v0.3 release notes highlight full fixture coverage")
    require("Production readiness review" in release_notes_v03, "v0.3 release notes mention readiness review")
    require("Production accounting replay" in release_notes_v03, "v0.3 release notes list excluded production replay")
    require("DataService `position.list`" in release_notes_v03, "v0.3 release notes list excluded position action")
    require("v0.3.0-accounting-replay-testonly-coverage" in release_notes_v03, "v0.3 release notes include suggested tag")

    require("ReadOnlyTable" in business_summary, "business summary mentions ReadOnlyTable")
    require("ReadOnlyColumnChooser" in business_summary, "business summary mentions ReadOnlyColumnChooser")
    require("禁止" in business_summary or "Not supported" in business_summary, "business summary lists prohibited operations")
    require("execute strategy" in business_summary, "business summary excludes strategy execution")
    require("generate TradeDraft" in business_summary, "business summary excludes TradeDraft generation")

    require("Release Notes - v0.1 ReadOnly Shell Demo" in release_notes, "release notes title")
    require("Suggested Tag After Merge" in release_notes, "release notes include suggested tag section")
    require("v0.1.0-readonly-shell-demo" in release_notes, "release notes include suggested tag")
    require("Automatic trading" in release_notes, "release notes list excluded scope")

    require("14_milestone_v0_1_readonly_shell_demo.md" in docs_index, "docs index links milestone")
    require("15_quick_start_readonly_shell_demo.md" in docs_index, "docs index links quick start")
    require(
        "16_manual_acceptance_readonly_business_pages.md" in docs_index,
        "docs index links read-only business acceptance doc",
    )
    require("17_ui_readability_checklist.md" in docs_index, "docs index links UI readability checklist")
    require("18_milestone_v0_2_readonly_business_pages.md" in docs_index, "docs index links v0.2 milestone")
    require("19_readonly_business_pages_summary.md" in docs_index, "docs index links business summary")
    require("release_notes/v0_1_readonly_shell_demo.md" in docs_index, "docs index links release notes")
    require("release_notes/v0_2_readonly_business_pages.md" in docs_index, "docs index links v0.2 release notes")
    require("30_accounting_replay_incremental_implementation_plan.md" in docs_index, "docs index links accounting replay incremental plan")
    require("31_accounting_replay_testonly_coverage_milestone.md" in docs_index, "docs index links accounting replay test-only milestone")
    require("32_production_accounting_replay_readiness_review.md" in docs_index, "docs index links production accounting replay readiness review")
    require("33_production_accounting_replay_architecture.md" in docs_index, "docs index links production accounting replay architecture")
    require("34_accounting_engine_module_candidate.md" in docs_index, "docs index links AccountingEngine module candidate")
    require("35_accounting_engine_replay_skeleton_milestone.md" in docs_index, "docs index links AccountingEngine replay skeleton milestone")
    require("36_accounting_engine_next_phase_boundary_review.md" in docs_index, "docs index links AccountingEngine next phase boundary review")
    require(
        "37_dataservice_readonly_accounting_action_contracts.md" in docs_index,
        "docs index links DataService read-only accounting action contracts",
    )
    require(
        "38_dataservice_accounting_no_write_test_plan.md" in docs_index,
        "docs index links DataService accounting no-write test plan",
    )
    require(
        "39_sqlite_readonly_facts_query_boundary.md" in docs_index,
        "docs index links SQLite read-only facts query boundary",
    )
    require(
        "40_accounting_facts_source_mapping.md" in docs_index,
        "docs index links accounting facts source mapping",
    )
    require(
        "41_cash_facts_source_boundary.md" in docs_index,
        "docs index links cash facts source boundary",
    )
    require(
        "42_cash_facts_query_decision.md" in docs_index,
        "docs index links cash facts query decision",
    )
    require(
        "43_dataservice_accounting_guard_suite_milestone.md" in docs_index,
        "docs index links DataService accounting guard suite milestone",
    )
    require(
        "44_dataservice_accounting_guard_no_write_readiness_review.md" in docs_index,
        "docs index links DataService accounting guard no-write readiness review",
    )
    require(
        "45_position_list_real_implementation_boundary.md" in docs_index,
        "docs index links position.list real implementation boundary",
    )
    require(
        "46_position_list_real_implementation_readiness_checklist.md" in docs_index,
        "docs index links position.list real implementation readiness checklist",
    )
    require("../libs/AccountingEngine" in docs_index, "docs index links AccountingEngine skeleton module")
    require("AccountingEngine public headers" in docs_index, "docs index links AccountingEngine DTO parser boundary")
    require("release_notes/v0_3_accounting_replay_testonly_coverage.md" in docs_index, "docs index links v0.3 accounting replay release notes")
    require("release_notes/v0_4_accounting_engine_replay_skeleton.md" in docs_index, "docs index links v0.4 AccountingEngine release notes")
    require(
        "release_notes/v0_5_dataservice_accounting_guard_suite.md" in docs_index,
        "docs index links v0.5 DataService accounting guard suite release notes",
    )
    require("20_position_accounting_boundary.md" in docs_index, "docs index links position boundary")
    require("21_position_readonly_data_contract_draft.md" in docs_index, "docs index links position data contract")
    require("22_position_accounting_data_contract.md" in docs_index, "docs index links stable position contract")
    require("23_position_accounting_test_fixture_design.md" in docs_index, "docs index links position fixture design")
    require("24_accounting_replay_fixture_samples.md" in docs_index, "docs index links accounting fixture samples")
    require("tests/fixtures/accounting_replay" in docs_index, "docs index links accounting replay fixture files")
    require("tests/AccountingFixtures" in docs_index, "docs index links accounting fixture loader")
    require("25_position_shell_viewmodel_design.md" in docs_index, "docs index links position ViewModel design")
    require("26_position_dto_viewmodel_mapping.md" in docs_index, "docs index links position DTO mapping")

    require("FX001-FX013 test-only minimal engine coverage is complete" in accounting_replay_incremental_plan, "incremental plan states test-only coverage complete")
    require("not mean production accounting replay is complete" in accounting_replay_incremental_plan, "incremental plan distinguishes production replay")
    require("Do not directly migrate `AccountingReplayMinimalEngine`" in accounting_replay_incremental_plan, "incremental plan blocks direct migration")
    require("DataService read-only action boundary" in accounting_replay_incremental_plan, "incremental plan calls out DataService boundary")

    require("Accounting Replay Test-only Coverage Milestone" in accounting_replay_milestone, "milestone doc title exists")
    require("FX001_EMPTY_LEDGER" in accounting_replay_milestone, "milestone doc includes FX001")
    require("FX013_MULTI_CURRENCY_UNSUPPORTED" in accounting_replay_milestone, "milestone doc includes FX013")
    require("test-only" in accounting_replay_milestone, "milestone doc states test-only")
    require("production accounting replay is not implemented" in accounting_replay_milestone, "milestone doc states production replay not implemented")
    require("AccountingReplayMinimalEngine" in accounting_replay_milestone, "milestone doc documents minimal engine")
    require("tests/AccountingFixtures" in accounting_replay_milestone, "milestone doc names test-only directory")
    require("must not be directly copied into production" in accounting_replay_milestone, "milestone doc blocks direct production copy")
    require("DataService read-only action boundary" in accounting_replay_milestone, "milestone doc recommends DataService boundary")
    require("v0.3.0-accounting-replay-testonly-coverage" in accounting_replay_milestone, "milestone doc includes suggested v0.3 tag")
    require("TASK-068 does not create a tag" in accounting_replay_milestone, "milestone doc states task does not create tag")
    require("33_production_accounting_replay_architecture.md" in accounting_replay_milestone, "milestone doc links architecture boundary")
    require("34_accounting_engine_module_candidate.md" in accounting_replay_milestone, "milestone doc links AccountingEngine candidate")

    require("Production Accounting Replay Readiness Review" in accounting_replay_readiness, "readiness doc title exists")
    require("test-only fixture engine" in accounting_replay_readiness, "readiness doc mentions test-only engine")
    require("must not be moved directly into `libs`" in accounting_replay_readiness, "readiness doc blocks direct migration")
    require("DataService read-only action" in accounting_replay_readiness, "readiness doc mentions DataService read-only action")
    require("33_production_accounting_replay_architecture.md" in accounting_replay_readiness, "readiness doc links architecture boundary")
    require("34_accounting_engine_module_candidate.md" in accounting_replay_readiness, "readiness doc links AccountingEngine candidate")
    require("no snapshot writes" in accounting_replay_readiness, "readiness doc checklist includes no snapshot writes")
    require("no TradeLog writes" in accounting_replay_readiness, "readiness doc checklist includes no TradeLog writes")
    require("no QML accounting calculation" in accounting_replay_readiness, "readiness doc checklist includes no QML accounting")
    require("TASK-069" in accounting_replay_readiness, "readiness doc suggests TASK-069")
    require("After FX001-FX013 test-only coverage, do not start production replay without readiness review" in codex_prompt_template, "prompt template requires readiness review after coverage")
    require("Do not move the test-only minimal engine into production code" in codex_prompt_template, "prompt template blocks direct test-only engine migration")
    require("Production replay tasks must explicitly declare whether they are read-only or write-capable" in codex_prompt_template, "prompt template requires read/write declaration")
    require("DataAccess dependency boundaries" in codex_prompt_template, "prompt template requires DataAccess boundary")
    require("DataService action scope" in codex_prompt_template, "prompt template requires DataService action scope")
    require("snapshot behavior" in codex_prompt_template, "prompt template requires snapshot behavior")
    require("TradeLog behavior" in codex_prompt_template, "prompt template requires TradeLog behavior")
    require("QML boundary" in codex_prompt_template, "prompt template requires QML boundary")
    require("no writes, no QML accounting calculation, no TradeDraft, and no auto-trading" in codex_prompt_template, "prompt template keeps default no-write boundary")
    require("docs/33_production_accounting_replay_architecture.md" in codex_prompt_template, "prompt template requires docs/33")
    require("docs/34_accounting_engine_module_candidate.md" in codex_prompt_template, "prompt template requires docs/34")
    require("DataService action implementation requires architecture boundary approval" in codex_prompt_template, "prompt template gates DataService actions")
    require("AccountingEngine dependency on DataAccess requires explicit authorization" in codex_prompt_template, "prompt template gates AccountingEngine DataAccess dependency")
    require("default production replay phase is read-only and has no snapshot writes" in codex_prompt_template, "prompt template keeps read-only no-snapshot default")
    require("AccountingEngine DTO parser tasks must not implement replay" in codex_prompt_template, "prompt template blocks replay in DTO parser tasks")
    require("DTO validation must not calculate positions" in codex_prompt_template, "prompt template blocks accounting calculation in DTO validation")

    require("Production Accounting Replay Architecture Boundary" in accounting_replay_architecture, "architecture doc title exists")
    require("test-only `AccountingReplayMinimalEngine`" in accounting_replay_architecture, "architecture doc names test-only engine")
    require("must not be directly migrated to production" in accounting_replay_architecture, "architecture doc blocks direct migration")
    require("Production replay is read-only by default" in accounting_replay_architecture, "architecture doc states read-only default")
    require("DataServiceApi does not carry complex replay algorithms" in accounting_replay_architecture, "architecture doc keeps algorithms out of DataServiceApi")
    require("DataAccess does not carry replay algorithms" in accounting_replay_architecture, "architecture doc keeps algorithms out of DataAccess")
    require("QML does not calculate accounting" in accounting_replay_architecture, "architecture doc keeps accounting out of QML")
    require("ShellServices does not calculate accounting" in accounting_replay_architecture, "architecture doc keeps accounting out of ShellServices")
    require("Snapshot writes require separate authorization" in accounting_replay_architecture, "architecture doc requires snapshot authorization")
    require("TradeLog writes are not authorized" in accounting_replay_architecture, "architecture doc requires TradeLog authorization")
    require("Production libs -> tests/AccountingFixtures" in accounting_replay_architecture, "architecture doc forbids production libs to test fixture dependency")
    require("DataServiceApi -> AccountingEngine" in accounting_replay_architecture, "architecture doc lists DataServiceApi to AccountingEngine direction")
    require("AccountingEngine -> SQLiteConnection" in accounting_replay_architecture, "architecture doc forbids AccountingEngine to SQLiteConnection")
    require("position.list" in accounting_replay_architecture, "architecture doc lists position.list")
    require("cash.summary" in accounting_replay_architecture, "architecture doc lists cash.summary")
    require("portfolio.pnl.summary" in accounting_replay_architecture, "architecture doc lists portfolio.pnl.summary")
    require("base_position.summary" in accounting_replay_architecture, "architecture doc lists base_position.summary")
    require("sniper_pool.summary" in accounting_replay_architecture, "architecture doc lists sniper_pool.summary")
    require("SELL_EXCEEDS_POSITION" in accounting_replay_architecture, "architecture doc lists oversell error")
    require("DATA_VERSION_MISMATCH" in accounting_replay_architecture, "architecture doc lists data version issue")
    require("UNSUPPORTED_ACCOUNTING_MODE" in accounting_replay_architecture, "architecture doc lists unsupported mode issue")
    require("TASK-070" in accounting_replay_architecture, "architecture doc records TASK-070 skeleton")
    require("TASK-071" in accounting_replay_architecture, "architecture doc records TASK-071 DTO parser boundary")
    require("TASK-072" in accounting_replay_architecture, "architecture doc records TASK-072 empty ledger skeleton")
    require("TASK-073" in accounting_replay_architecture, "architecture doc records TASK-073 single buy skeleton")
    require("single BUY replay skeleton" in accounting_replay_architecture, "architecture doc documents single BUY skeleton")
    require("TASK-074" in accounting_replay_architecture, "architecture doc records TASK-074 partial sell skeleton")
    require("partial sell replay" in accounting_replay_architecture, "architecture doc documents partial sell skeleton")
    require("TASK-075" in accounting_replay_architecture, "architecture doc records TASK-075 sell exceeds scenario")
    require("SELL_EXCEEDS_POSITION" in accounting_replay_architecture, "architecture doc documents sell exceeds issue")
    require("TASK-076" in accounting_replay_architecture, "architecture doc records TASK-076 missing fee scenario")
    require("MISSING_FEE" in accounting_replay_architecture, "architecture doc documents missing fee issue")
    require("TASK-077" in accounting_replay_architecture, "architecture doc records TASK-077 negative cash scenario")
    require("NEGATIVE_CASH" in accounting_replay_architecture, "architecture doc documents negative cash issue")
    require("TASK-078" in accounting_replay_architecture, "architecture doc records TASK-078 multi-instrument scenario")
    require("instrumentCode" in accounting_replay_architecture, "architecture doc documents instrumentCode grouping")
    require("TASK-079" in accounting_replay_architecture, "architecture doc records TASK-079 multi-account scenario")
    require("accountId + portfolioId + instrumentCode" in accounting_replay_architecture, "architecture doc documents multi-account grouping")
    require("TASK-080" in accounting_replay_architecture, "architecture doc records TASK-080 multi-currency scenario")
    require("supportsFxRate=false" in accounting_replay_architecture, "architecture doc records FX rate unsupported")
    require("supportsMultiCurrency=false" in accounting_replay_architecture, "architecture doc records multi-currency unsupported")
    require("Only empty ledger input is supported" in accounting_replay_architecture, "architecture doc states only empty ledger is supported")
    require("DTO validation is not replay" in accounting_replay_architecture, "architecture doc states DTO validation is not replay")
    require("No replay algorithm" in accounting_replay_architecture, "architecture doc states skeleton has no replay")
    require("No DataAccess dependency" in accounting_replay_architecture, "architecture doc states skeleton has no DataAccess")
    require("No DataService action" in accounting_replay_architecture, "architecture doc states skeleton has no DataService action")

    require("AccountingEngine Module Candidate" in accounting_engine_candidate, "AccountingEngine candidate doc title exists")
    require("libs/AccountingEngine" in accounting_engine_candidate, "AccountingEngine candidate doc names module")
    require("Replay facts" in accounting_engine_candidate, "AccountingEngine candidate doc lists replay responsibility")
    require("SQLite connection management" in accounting_engine_candidate, "AccountingEngine candidate doc excludes SQLite management")
    require("TradeLog writes" in accounting_engine_candidate, "AccountingEngine candidate doc excludes TradeLog writes")
    require("Snapshot writes" in accounting_engine_candidate, "AccountingEngine candidate doc excludes snapshot writes")
    require("Market data fetching" in accounting_engine_candidate, "AccountingEngine candidate doc excludes market fetching")
    require("FX rate fetching" in accounting_engine_candidate, "AccountingEngine candidate doc excludes FX fetching")
    require("TradeFactDto" in accounting_engine_candidate, "AccountingEngine candidate doc lists TradeFactDto")
    require("FxRateFactDto" in accounting_engine_candidate, "AccountingEngine candidate doc lists FxRateFactDto")
    require("PositionListResponse" in accounting_engine_candidate, "AccountingEngine candidate doc lists PositionListResponse")
    require("does not directly depend on DataAccess" in accounting_engine_candidate, "AccountingEngine candidate doc avoids direct DataAccess dependency")
    require("does not directly depend on SQLite" in accounting_engine_candidate, "AccountingEngine candidate doc avoids direct SQLite dependency")
    require("FX001-FX013" in accounting_engine_candidate, "AccountingEngine candidate doc keeps fixture coverage")
    require("must not be directly migrated to" in accounting_engine_candidate, "AccountingEngine candidate doc blocks direct minimal engine migration")
    require("TASK-070" in accounting_engine_candidate, "AccountingEngine candidate doc records TASK-070 skeleton")
    require("Candidate Skeleton Created" in accounting_engine_candidate, "AccountingEngine candidate doc records skeleton creation")
    require("TASK-071" in accounting_engine_candidate, "AccountingEngine candidate doc records TASK-071")
    require("DTO / Parser / Validation Boundary" in accounting_engine_candidate, "AccountingEngine candidate doc records DTO parser boundary")
    require("TASK-072" in accounting_engine_candidate, "AccountingEngine candidate doc records TASK-072")
    require("Empty Ledger Replay Entry" in accounting_engine_candidate, "AccountingEngine candidate doc records empty ledger entry")
    require("TASK-073" in accounting_engine_candidate, "AccountingEngine candidate doc records TASK-073")
    require("Single Buy Replay Entry" in accounting_engine_candidate, "AccountingEngine candidate doc records single buy entry")
    require("TASK-074" in accounting_engine_candidate, "AccountingEngine candidate doc records TASK-074")
    require("Partial Sell Replay Entry" in accounting_engine_candidate, "AccountingEngine candidate doc records partial sell entry")
    require("TASK-075" in accounting_engine_candidate, "AccountingEngine candidate doc records TASK-075")
    require("Sell Exceeds Position Detection" in accounting_engine_candidate, "AccountingEngine candidate doc records sell exceeds detection")
    require("TASK-076" in accounting_engine_candidate, "AccountingEngine candidate doc records TASK-076")
    require("Missing Fee Detection" in accounting_engine_candidate, "AccountingEngine candidate doc records missing fee detection")
    require("TASK-077" in accounting_engine_candidate, "AccountingEngine candidate doc records TASK-077")
    require("Negative Cash Detection" in accounting_engine_candidate, "AccountingEngine candidate doc records negative cash detection")
    require("TASK-078" in accounting_engine_candidate, "AccountingEngine candidate doc records TASK-078")
    require("Multi-instrument BUY Scenario" in accounting_engine_candidate, "AccountingEngine candidate doc records multi-instrument buy")
    require("TASK-079" in accounting_engine_candidate, "AccountingEngine candidate doc records TASK-079")
    require("Multi-account BUY Scenario" in accounting_engine_candidate, "AccountingEngine candidate doc records multi-account buy")
    require("TASK-080" in accounting_engine_candidate, "AccountingEngine candidate doc records TASK-080")
    require("Multi-currency Unsupported Detection" in accounting_engine_candidate, "AccountingEngine candidate doc records multi-currency unsupported")
    require("replayImplemented=false" in accounting_engine_candidate, "AccountingEngine candidate doc records replay false")
    require("snapshotWriteEnabled=false" in accounting_engine_candidate, "AccountingEngine candidate doc records snapshot write false")
    require("tradeLogWriteEnabled=false" in accounting_engine_candidate, "AccountingEngine candidate doc records trade log write false")
    require("No DataAccess dependency" in accounting_engine_candidate, "AccountingEngine candidate doc keeps DataAccess forbidden")

    require("AccountingEngine skeleton" in readme, "README documents AccountingEngine skeleton")
    require("AccountingEngine replay DTO parser boundary" in readme, "README documents DTO parser boundary")
    require("AccountingEngine empty ledger replay skeleton" in readme, "README documents empty ledger replay skeleton")
    require("AccountingEngine single buy replay skeleton" in readme, "README documents single buy replay skeleton")
    require("AccountingEngine partial sell replay skeleton" in readme, "README documents partial sell replay skeleton")
    require("AccountingEngine sell exceeds position scenario" in readme, "README documents sell exceeds scenario")
    require("AccountingEngine missing fee scenario" in readme, "README documents missing fee scenario")
    require("AccountingEngine negative cash scenario" in readme, "README documents negative cash scenario")
    require("AccountingEngine multi-instrument BUY scenario" in readme, "README documents multi-instrument buy scenario")
    require("AccountingEngine multi-account BUY scenario" in readme, "README documents multi-account buy scenario")
    require("AccountingEngine multi-currency unsupported scenario" in readme, "README documents multi-currency unsupported scenario")
    require("AccountingEngine missing market price scenario" in readme, "README documents missing market price scenario")
    require("v0.4 AccountingEngine Replay Skeleton" in readme, "README documents v0.4 AccountingEngine Replay Skeleton")
    require("position.list DataService action guard" in readme, "README documents position.list guard")
    require("POSITION_LIST_NOT_AVAILABLE" in readme, "README documents position.list guard status")
    require("dataservice_position_list_guard" in readme, "README documents position.list guard test")
    require("dataservice_position_list_no_write" in readme, "README documents position.list no-write test")
    require("SQLite read-only no-write harness skeleton" in readme, "README documents TASK-086 no-write harness")
    require("tests/AccountingNoWrite" in readme, "README documents no-write harness test-only location")
    require("accounting_forbidden_sql_scanner" in readme, "README documents forbidden SQL scanner test")
    require("accounting_no_write_harness" in readme, "README documents no-write harness test")
    require("ReplayRequestDto" in readme, "README documents ReplayRequestDto")
    require("TradeFactDto" in readme, "README documents TradeFactDto")
    require("accounting_replay_dto_parser_boundary" in readme, "README documents DTO parser boundary test")
    require("accounting_replay_engine_empty_ledger" in readme, "README documents empty ledger replay test")
    require("accounting_replay_engine_single_buy" in readme, "README documents single buy replay test")
    require("accounting_replay_engine_buy_sell_partial" in readme, "README documents partial sell replay test")
    require("accounting_replay_engine_sell_exceeds_position" in readme, "README documents sell exceeds position test")
    require("accounting_replay_engine_missing_fee" in readme, "README documents missing fee test")
    require("accounting_replay_engine_negative_cash" in readme, "README documents negative cash test")
    require("accounting_replay_engine_multi_instrument_buy" in readme, "README documents multi-instrument buy test")
    require("accounting_replay_engine_multi_account_buy" in readme, "README documents multi-account buy test")
    require("accounting_replay_engine_multi_currency_unsupported" in readme, "README documents multi-currency unsupported test")
    require("accounting_replay_engine_missing_market_price" in readme, "README documents missing market price test")
    require("supportsMarketPrice=false" in readme, "README records market price unsupported")
    require("supportsMultiCurrency=false" in readme, "README records multi-currency unsupported")
    require("supportsFxRate=false" in readme, "README records FX rate unsupported")
    require("replayImplemented=false" in readme, "README records AccountingEngine replay false")
    require("productionReady=false" in readme, "README records AccountingEngine productionReady false")
    require("writeEnabled=false" in readme, "README records AccountingEngine write false")
    require("accounting_engine_boundary" in readme, "README documents AccountingEngine boundary test")

    require("empty ledger" in accounting_engine_replay_milestone, "milestone doc covers empty ledger")
    require("single BUY" in accounting_engine_replay_milestone, "milestone doc covers single BUY")
    require("BUY + SELL partial sell" in accounting_engine_replay_milestone, "milestone doc covers partial sell")
    require("SELL_EXCEEDS_POSITION" in accounting_engine_replay_milestone, "milestone doc covers sell exceeds position")
    require("MISSING_FEE" in accounting_engine_replay_milestone, "milestone doc covers missing fee")
    require("NEGATIVE_CASH" in accounting_engine_replay_milestone, "milestone doc covers negative cash")
    require("Multi-instrument BUY" in accounting_engine_replay_milestone, "milestone doc covers multi-instrument BUY")
    require("Multi-account BUY" in accounting_engine_replay_milestone, "milestone doc covers multi-account BUY")
    require("Multi-currency unsupported" in accounting_engine_replay_milestone, "milestone doc covers multi-currency unsupported")
    require("Missing market price" in accounting_engine_replay_milestone, "milestone doc covers missing market price")
    require("replayImplemented=false" in accounting_engine_replay_milestone, "milestone doc records replayImplemented false")
    require("productionReady=false" in accounting_engine_replay_milestone, "milestone doc records productionReady false")
    require("no SQLite" in accounting_engine_replay_milestone, "milestone doc records no SQLite")
    require("no DataService action" in accounting_engine_replay_milestone, "milestone doc records no DataService action")
    require("no snapshot writes" in accounting_engine_replay_milestone, "milestone doc records no snapshot writes")
    require("no TradeLog writes" in accounting_engine_replay_milestone, "milestone doc records no TradeLog writes")
    require(
        "v0.4.0-accounting-engine-replay-skeleton" in accounting_engine_replay_milestone,
        "milestone doc includes suggested v0.4 tag",
    )

    require(
        "DataService read-only action contract" in accounting_engine_next_phase_review,
        "next phase review covers DataService read-only action contract",
    )
    require(
        "SQLite read-only facts query" in accounting_engine_next_phase_review,
        "next phase review covers SQLite read-only facts query",
    )
    require("no-write" in accounting_engine_next_phase_review, "next phase review covers no-write boundary")
    require("no DataAccess dependency unless authorized" in accounting_engine_next_phase_review, "next phase review keeps DataAccess authorization boundary")
    require("no DataService action unless authorized" in accounting_engine_next_phase_review, "next phase review keeps DataService authorization boundary")
    require(
        "docs/37_dataservice_readonly_accounting_action_contracts.md" in accounting_engine_next_phase_review,
        "next phase review links DataService accounting action contracts",
    )
    require(
        "docs/38_dataservice_accounting_no_write_test_plan.md" in accounting_engine_next_phase_review,
        "next phase review links DataService accounting no-write test plan",
    )
    require(
        "docs/39_sqlite_readonly_facts_query_boundary.md" in accounting_engine_next_phase_review,
        "next phase review links SQLite read-only facts query boundary",
    )
    require(
        "docs/40_accounting_facts_source_mapping.md" in accounting_engine_next_phase_review,
        "next phase review links accounting facts source mapping",
    )

    require("position.list" in dataservice_readonly_accounting_contracts, "DataService contract doc defines position.list")
    require("cash.summary" in dataservice_readonly_accounting_contracts, "DataService contract doc defines cash.summary")
    require(
        "portfolio.pnl.summary" in dataservice_readonly_accounting_contracts,
        "DataService contract doc defines portfolio.pnl.summary",
    )
    require(
        "base_position.summary" in dataservice_readonly_accounting_contracts,
        "DataService contract doc defines base_position.summary",
    )
    require(
        "sniper_pool.summary" in dataservice_readonly_accounting_contracts,
        "DataService contract doc defines sniper_pool.summary",
    )
    require("read-only" in dataservice_readonly_accounting_contracts, "DataService contract doc states read-only")
    require(
        "Forbidden writes" in dataservice_readonly_accounting_contracts
        or "forbidden writes" in dataservice_readonly_accounting_contracts,
        "DataService contract doc lists forbidden writes",
    )
    require(
        "ProtocolResponse" in dataservice_readonly_accounting_contracts,
        "DataService contract doc defines ProtocolResponse semantics",
    )
    require(
        "QML does not calculate accounting" in dataservice_readonly_accounting_contracts,
        "DataService contract doc keeps QML out of accounting calculation",
    )
    require(
        "docs/39_sqlite_readonly_facts_query_boundary.md" in dataservice_readonly_accounting_contracts,
        "DataService contract doc references SQLite read-only facts query boundary",
    )
    require("TASK-085" in dataservice_readonly_accounting_contracts, "DataService contract doc records TASK-085")
    require("position.list" in dataservice_readonly_accounting_contracts, "DataService contract doc covers position.list")
    require(
        "implemented=false" in dataservice_readonly_accounting_contracts,
        "DataService contract doc states position.list guard implemented=false",
    )
    require(
        "POSITION_LIST_NOT_AVAILABLE" in dataservice_readonly_accounting_contracts,
        "DataService contract doc documents position.list not available status",
    )
    require("trade_log" in dataservice_accounting_no_write_plan, "no-write plan protects trade_log")
    require("cash_snapshot" in dataservice_accounting_no_write_plan, "no-write plan protects cash_snapshot")
    require("position_snapshot" in dataservice_accounting_no_write_plan, "no-write plan protects position_snapshot")
    require("portfolio_summary" in dataservice_accounting_no_write_plan, "no-write plan protects portfolio_summary")
    require("INSERT" in dataservice_accounting_no_write_plan, "no-write plan scans INSERT")
    require("UPDATE" in dataservice_accounting_no_write_plan, "no-write plan scans UPDATE")
    require("DELETE" in dataservice_accounting_no_write_plan, "no-write plan scans DELETE")
    require(
        "docs/39_sqlite_readonly_facts_query_boundary.md" in dataservice_accounting_no_write_plan,
        "no-write plan references SQLite read-only facts query boundary",
    )
    require(
        "position.list` guard has no-write table count coverage" in dataservice_accounting_no_write_plan,
        "no-write plan documents position.list guard no-write coverage",
    )
    require("TASK-086" in dataservice_accounting_no_write_plan, "no-write plan records TASK-086")
    require("tests/AccountingNoWrite" in dataservice_accounting_no_write_plan, "no-write plan documents test-only harness directory")
    require("protected table row-count helper" in dataservice_accounting_no_write_plan, "no-write plan documents row-count helper")
    require("forbidden SQL scanner" in dataservice_accounting_no_write_plan, "no-write plan documents forbidden SQL scanner")
    require("accounting_forbidden_sql_scanner" in dataservice_accounting_no_write_plan, "no-write plan documents scanner CTest")
    require("accounting_no_write_harness" in dataservice_accounting_no_write_plan, "no-write plan documents harness CTest")
    require("position.list` guard" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc documents position.list guard")
    require(
        "does not use SQLite facts query" in sqlite_readonly_facts_query_boundary,
        "SQLite facts query doc states position.list guard avoids SQLite facts query",
    )
    require("TASK-086" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc records TASK-086")
    require("no-write harness skeleton" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc documents no-write harness")
    require("does not implement a SQLite facts query" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc says harness is not facts query")
    require("trade_log" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc covers trade_log")
    require("read-only" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc states read-only")
    require("SELECT" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc allows SELECT")
    require("INSERT" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc forbids INSERT")
    require("UPDATE" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc forbids UPDATE")
    require("DELETE" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc forbids DELETE")
    require("snapshot 是派生数据" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc states snapshot is derived data")
    require(
        "AccountingFactsReadOnlyRepository" in sqlite_readonly_facts_query_boundary,
        "SQLite facts query doc drafts read-only repository boundary",
    )
    require("no-write table count" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc requires no-write table count")
    require("TradeFactDto" in accounting_facts_source_mapping, "facts source mapping covers TradeFactDto")
    require("CashFactDto" in accounting_facts_source_mapping, "facts source mapping covers CashFactDto")
    require("MarketPriceFactDto" in accounting_facts_source_mapping, "facts source mapping covers MarketPriceFactDto")
    require("FxRateFactDto" in accounting_facts_source_mapping, "facts source mapping covers FxRateFactDto")
    require("cash_snapshot 是派生数据" in accounting_facts_source_mapping, "facts source mapping states cash_snapshot is derived data")
    require("position.list` guard" in accounting_facts_source_mapping, "facts source mapping documents position.list guard")
    require(
        "does not use this facts mapping" in accounting_facts_source_mapping,
        "facts source mapping states position.list guard does not use facts mapping",
    )
    require("TASK-086" in accounting_facts_source_mapping, "facts source mapping records TASK-086")
    require("does not read facts" in accounting_facts_source_mapping, "facts source mapping says no-write harness does not read facts")
    require(
        "docs/37_dataservice_readonly_accounting_action_contracts.md" in codex_prompt_template,
        "prompt template references DataService action contracts",
    )
    require(
        "docs/38_dataservice_accounting_no_write_test_plan.md" in codex_prompt_template,
        "prompt template references DataService no-write plan",
    )
    require(
        "docs/39_sqlite_readonly_facts_query_boundary.md" in codex_prompt_template,
        "prompt template references SQLite read-only facts query boundary",
    )
    require(
        "docs/40_accounting_facts_source_mapping.md" in codex_prompt_template,
        "prompt template references accounting facts source mapping",
    )
    require(
        "`position.list` guard does not equal a real position query" in codex_prompt_template,
        "prompt template states position.list guard is not real query",
    )
    require("SQLite facts query tasks must use the test-only no-write harness skeleton" in codex_prompt_template, "prompt template requires no-write harness for SQLite query tasks")
    require("Do not calculate replay in the SQLite query layer" in codex_prompt_template, "prompt template forbids query-layer replay")
    require("Do not bypass the forbidden SQL scanner" in codex_prompt_template, "prompt template forbids bypassing SQL scanner")
    require("Do not place the no-write harness in production libs" in codex_prompt_template, "prompt template keeps harness out of production libs")
    require("kActionPositionList" in dataservice_actions_header, "DataServiceActions exposes position.list action constant")
    require("handlePositionList" in dataservice_actions_header, "DataServiceActions declares position.list handler")
    require("kActionCashSummary" in dataservice_actions_header, "DataServiceActions exposes cash.summary action constant")
    require("handleCashSummary" in dataservice_actions_header, "DataServiceActions declares cash.summary handler")
    require("kActionPortfolioPnlSummary" in dataservice_actions_header, "DataServiceActions exposes portfolio.pnl.summary action constant")
    require("handlePortfolioPnlSummary" in dataservice_actions_header, "DataServiceActions declares portfolio.pnl.summary handler")
    require("kActionBasePositionSummary" in dataservice_actions_header, "DataServiceActions exposes base_position.summary action constant")
    require("handleBasePositionSummary" in dataservice_actions_header, "DataServiceActions declares base_position.summary handler")
    require("kActionSniperPoolSummary" in dataservice_actions_header, "DataServiceActions exposes sniper_pool.summary action constant")
    require("handleSniperPoolSummary" in dataservice_actions_header, "DataServiceActions declares sniper_pool.summary handler")
    require("kActionPositionList" in dataservice_action_registrar, "DataService registrar registers position.list")
    require("handlePositionList" in dataservice_action_registrar, "DataService registrar wires position.list handler")
    require("kActionCashSummary" in dataservice_action_registrar, "DataService registrar registers cash.summary")
    require("handleCashSummary" in dataservice_action_registrar, "DataService registrar wires cash.summary handler")
    require("kActionPortfolioPnlSummary" in dataservice_action_registrar, "DataService registrar registers portfolio.pnl.summary")
    require("handlePortfolioPnlSummary" in dataservice_action_registrar, "DataService registrar wires portfolio.pnl.summary handler")
    require("kActionBasePositionSummary" in dataservice_action_registrar, "DataService registrar registers base_position.summary")
    require("handleBasePositionSummary" in dataservice_action_registrar, "DataService registrar wires base_position.summary handler")
    require("kActionSniperPoolSummary" in dataservice_action_registrar, "DataService registrar registers sniper_pool.summary")
    require("handleSniperPoolSummary" in dataservice_action_registrar, "DataService registrar wires sniper_pool.summary handler")
    require("positionList(" in dataservice_client_header, "DataServiceClient exposes positionList wrapper")
    require("cashSummary(" in dataservice_client_header, "DataServiceClient exposes cashSummary wrapper")
    require("portfolioPnlSummary(" in dataservice_client_header, "DataServiceClient exposes portfolioPnlSummary wrapper")
    require("basePositionSummary(" in dataservice_client_header, "DataServiceClient exposes basePositionSummary wrapper")
    require("sniperPoolSummary(" in dataservice_client_header, "DataServiceClient exposes sniperPoolSummary wrapper")
    require("kActionPositionList" in dataservice_client_source, "DataServiceClient has position.list action constant")
    require("sendAction(kActionPositionList" in dataservice_client_source, "DataServiceClient wrapper sends position.list")
    require("kActionCashSummary" in dataservice_client_source, "DataServiceClient has cash.summary action constant")
    require("sendAction(kActionCashSummary" in dataservice_client_source, "DataServiceClient wrapper sends cash.summary")
    require("kActionPortfolioPnlSummary" in dataservice_client_source, "DataServiceClient has portfolio.pnl.summary action constant")
    require("sendAction(kActionPortfolioPnlSummary" in dataservice_client_source, "DataServiceClient wrapper sends portfolio.pnl.summary")
    require("kActionBasePositionSummary" in dataservice_client_source, "DataServiceClient has base_position.summary action constant")
    require("sendAction(kActionBasePositionSummary" in dataservice_client_source, "DataServiceClient wrapper sends base_position.summary")
    require("kActionSniperPoolSummary" in dataservice_client_source, "DataServiceClient has sniper_pool.summary action constant")
    require("sendAction(kActionSniperPoolSummary" in dataservice_client_source, "DataServiceClient wrapper sends sniper_pool.summary")
    for action_source, label in [
        (position_list_guard_source, "position.list"),
        (cash_summary_guard_source, "cash.summary"),
        (portfolio_pnl_summary_guard_source, "portfolio.pnl.summary"),
        (base_position_summary_guard_source, "base_position.summary"),
        (sniper_pool_summary_guard_source, "sniper_pool.summary"),
    ]:
        require("ShellAccountingReadOnlyFactsQuery" in action_source, f"{label} uses read-only facts query")
        require("shellAccountingPayloadPrefix" in action_source, f"{label} uses shared read-only payload")
        require("data.audit.append" not in action_source, f"{label} source does not call data.audit.append")
        require("AccountingReplayEngine" not in action_source, f"{label} source does not call replay engine")
    for payload_token in [
        '\\"implemented\\":true',
        '\\"readOnly\\":true',
        '\\"writeEnabled\\":false',
        '\\"sqliteAccessed\\":true',
        '\\"snapshotRebuilt\\":false',
        '\\"accountingEngineCalled\\":',
        '\\"tradeDraftGenerated\\":false',
    ]:
        require(payload_token in dataservice_actions_source, f"DataService read-only payload includes {payload_token}")
    for forbidden_sql in ["INSERT ", "UPDATE ", "DELETE ", "CREATE TABLE", "DROP ", "ALTER ", "REPLACE ", "VACUUM"]:
        require(forbidden_sql not in dataservice_actions_source, f"DataService accounting actions do not contain {forbidden_sql}")
    require("dataservice_position_list_guard" in dataservice_test_cmake, "DataService CMake registers position.list guard test")
    require("dataservice_position_list_no_write" in dataservice_test_cmake, "DataService CMake registers position.list no-write test")
    require("dataservice_cash_summary_guard" in dataservice_test_cmake, "DataService CMake registers cash.summary guard test")
    require("dataservice_cash_summary_no_write" in dataservice_test_cmake, "DataService CMake registers cash.summary no-write test")
    require(
        "dataservice_portfolio_pnl_summary_guard" in dataservice_test_cmake,
        "DataService CMake registers portfolio.pnl.summary guard test",
    )
    require(
        "dataservice_portfolio_pnl_summary_no_write" in dataservice_test_cmake,
        "DataService CMake registers portfolio.pnl.summary no-write test",
    )
    require(
        "dataservice_base_position_summary_guard" in dataservice_test_cmake,
        "DataService CMake registers base_position.summary guard test",
    )
    require(
        "dataservice_base_position_summary_no_write" in dataservice_test_cmake,
        "DataService CMake registers base_position.summary no-write test",
    )
    require(
        "dataservice_sniper_pool_summary_guard" in dataservice_test_cmake,
        "DataService CMake registers sniper_pool.summary guard test",
    )
    require(
        "dataservice_sniper_pool_summary_no_write" in dataservice_test_cmake,
        "DataService CMake registers sniper_pool.summary no-write test",
    )
    require(
        "dataservice_client_position_list_guard" in dataservice_client_test_cmake,
        "DataServiceClient CMake registers position.list client guard test",
    )
    require(
        "dataservice_client_cash_summary_guard" in dataservice_client_test_cmake,
        "DataServiceClient CMake registers cash.summary client guard test",
    )
    require(
        "dataservice_client_portfolio_pnl_summary_guard" in dataservice_client_test_cmake,
        "DataServiceClient CMake registers portfolio.pnl.summary client guard test",
    )
    require(
        "dataservice_client_base_position_summary_guard" in dataservice_client_test_cmake,
        "DataServiceClient CMake registers base_position.summary client guard test",
    )
    require(
        "dataservice_client_sniper_pool_summary_guard" in dataservice_client_test_cmake,
        "DataServiceClient CMake registers sniper_pool.summary client guard test",
    )
    require("kActionPositionList" in dataservice_readonly_test, "DataService test calls position.list action")
    require("POSITION_LIST_EMPTY" in dataservice_readonly_test, "DataService test checks position.list empty status")
    require("position.list returns empty positions" in dataservice_readonly_test, "DataService test checks empty positions")
    require("expectProtectedTableCountsUnchanged" in dataservice_readonly_test, "DataService test checks position.list no-write")
    require("kActionCashSummary" in dataservice_readonly_test, "DataService test calls cash.summary action")
    require("CASH_SUMMARY_EMPTY" in dataservice_readonly_test, "DataService test checks cash.summary empty status")
    require("cash.summary returns empty accountCashSummaries" in dataservice_readonly_test, "DataService test checks empty cash summaries")
    require("kActionPortfolioPnlSummary" in dataservice_readonly_test, "DataService test calls portfolio.pnl.summary action")
    require(
        "PORTFOLIO_PNL_SUMMARY_EMPTY" in dataservice_readonly_test,
        "DataService test checks portfolio.pnl.summary empty status",
    )
    require("portfolio.pnl.summary returns empty portfolioPnlSummaries" in dataservice_readonly_test, "DataService test checks empty PnL summaries")
    require("kActionBasePositionSummary" in dataservice_readonly_test, "DataService test calls base_position.summary action")
    require(
        "BASE_POSITION_SUMMARY_EMPTY" in dataservice_readonly_test,
        "DataService test checks base_position.summary empty status",
    )
    require("base_position.summary returns empty basePositions" in dataservice_readonly_test, "DataService test checks empty base positions")
    require("kActionSniperPoolSummary" in dataservice_readonly_test, "DataService test calls sniper_pool.summary action")
    require(
        "SNIPER_POOL_SUMMARY_EMPTY" in dataservice_readonly_test,
        "DataService test checks sniper_pool.summary empty status",
    )
    require("sniper_pool.summary returns empty tierSummary" in dataservice_readonly_test, "DataService test checks empty tier summary")
    require("implemented=true" in dataservice_readonly_test, "DataService test checks read-only facts implemented")
    require("sqliteAccessed=true" in dataservice_readonly_test, "DataService test checks SQLite access is DataService-owned")
    require("accountingEngineCalled=false" in dataservice_readonly_test, "DataService test checks no AccountingEngine")
    require("tradeDraftGenerated=false" in dataservice_readonly_test, "DataService test checks no TradeDraft")
    require("client.positionList" in dataservice_client_test, "DataServiceClient test calls positionList wrapper")
    require("client.cashSummary" in dataservice_client_test, "DataServiceClient test calls cashSummary wrapper")
    require("client.portfolioPnlSummary" in dataservice_client_test, "DataServiceClient test calls portfolioPnlSummary wrapper")
    require("client.basePositionSummary" in dataservice_client_test, "DataServiceClient test calls basePositionSummary wrapper")
    require("client.sniperPoolSummary" in dataservice_client_test, "DataServiceClient test calls sniperPoolSummary wrapper")

    require("add_subdirectory(AccountingNoWrite)" in tests_cmake, "tests CMake adds AccountingNoWrite tests")
    require("accounting_forbidden_sql_scanner" in accounting_no_write_cmake, "AccountingNoWrite CMake registers scanner CTest")
    require("accounting_no_write_harness" in accounting_no_write_cmake, "AccountingNoWrite CMake registers no-write harness CTest")
    require("ETFDecisionTerminal::DataAccess" in accounting_no_write_cmake, "AccountingNoWrite test utils link DataAccess for test DB only")
    require("protectedAccountingTables" in accounting_no_write_utils_header, "no-write helper exposes protected tables")
    require("captureProtectedTableRowCounts" in accounting_no_write_utils_header, "no-write helper exposes row-count capture")
    require("assertNoWritesAround" in accounting_no_write_utils_header, "no-write helper exposes no-write assertion")
    require("trade_log" in accounting_no_write_utils_source, "no-write helper protects trade_log")
    require("trade_draft" in accounting_no_write_utils_source, "no-write helper protects trade_draft")
    require("cash_snapshot" in accounting_no_write_utils_source, "no-write helper protects cash_snapshot")
    require("position_snapshot" in accounting_no_write_utils_source, "no-write helper protects position_snapshot")
    require("portfolio_summary" in accounting_no_write_utils_source, "no-write helper protects portfolio_summary")
    require("audit_log" in accounting_no_write_utils_source, "no-write helper protects audit_log")
    require("findForbiddenWriteKeywords" in forbidden_sql_scanner_header, "forbidden SQL scanner exposes keyword scan")
    require("containsForbiddenWriteSql" in forbidden_sql_scanner_header, "forbidden SQL scanner exposes boolean scan")
    for forbidden_sql in ["INSERT", "UPDATE", "DELETE", "CREATE", "DROP", "ALTER", "REPLACE", "UPSERT", "VACUUM"]:
        require(forbidden_sql in forbidden_sql_scanner_source, f"forbidden SQL scanner detects {forbidden_sql}")
        require(forbidden_sql in forbidden_sql_scanner_test, f"forbidden SQL scanner test covers {forbidden_sql}")
    require("PRAGMA writable_schema" in forbidden_sql_scanner_source, "forbidden SQL scanner detects writable_schema")
    require("BEGIN IMMEDIATE" in forbidden_sql_scanner_source, "forbidden SQL scanner detects BEGIN IMMEDIATE")
    require("BEGIN EXCLUSIVE" in forbidden_sql_scanner_source, "forbidden SQL scanner detects BEGIN EXCLUSIVE")
    require("SELECT * FROM trade_log" in forbidden_sql_scanner_test, "forbidden SQL scanner test allows SELECT")
    require("WITH x AS" in forbidden_sql_scanner_test, "forbidden SQL scanner test allows WITH SELECT")
    require("assertNoWritesAround" in accounting_no_write_harness_test, "no-write harness test exercises no-write assertion")
    require("SELECT COUNT(*) FROM trade_log" in accounting_no_write_harness_test, "no-write harness test exercises SELECT-only callback")
    require("compareProtectedTableRowCounts" in accounting_no_write_harness_test, "no-write harness test checks row-count change reporting")
    require("AccountingEngine" not in accounting_no_write_utils_source, "no-write helper does not call AccountingEngine")
    require("DataService" not in accounting_no_write_utils_source, "no-write helper does not call DataService")
    require("AccountingReplayMinimalEngine" not in accounting_no_write_utils_source, "no-write helper does not reference test-only replay engine")
    require("AccountingFactsReadOnlyRepository" not in accounting_no_write_utils_source, "no-write helper does not add production facts repository")
    require("AccountingEngine" not in forbidden_sql_scanner_source, "SQL scanner does not call AccountingEngine")
    require("DataService" not in forbidden_sql_scanner_source, "SQL scanner does not call DataService")

    require("SQLite read-only trade facts query skeleton" in readme, "README documents trade facts query skeleton")
    require("dataaccess_accounting_trade_facts_readonly_query" in readme, "README documents trade facts query CTest")
    require("dataaccess_accounting_trade_facts_no_write" in readme, "README documents trade facts no-write CTest")
    require("dataaccess_accounting_trade_facts_sql_scan" in readme, "README documents trade facts SQL scan CTest")
    require("TASK-087" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc records TASK-087")
    require("trade_log read-only facts query skeleton" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc documents trade_log skeleton")
    require("cash facts query" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc keeps cash facts out of scope")
    require("market price facts query" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc keeps market price out of scope")
    require("FX rate facts query" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc keeps FX rate out of scope")
    require("TASK-087" in accounting_facts_source_mapping, "facts source mapping records TASK-087")
    require("trade_log to TradeFactRow" in accounting_facts_source_mapping, "facts source mapping documents trade_log mapping")
    require("fee_cents` is `NOT NULL DEFAULT 0" in accounting_facts_source_mapping, "facts source mapping records fee gap")
    require("TASK-087" in dataservice_accounting_no_write_plan, "no-write plan records TASK-087")
    require("Trade Facts Reader No-write Coverage" in dataservice_accounting_no_write_plan, "no-write plan documents trade facts reader")
    require("TASK-087" in dataservice_readonly_accounting_contracts, "DataService contract doc records TASK-087")
    require("position.list` still does not use it" in dataservice_readonly_accounting_contracts, "DataService doc says position.list does not use reader")
    require("DataAccess read-only query skeletons do not equal replay" in codex_prompt_template, "prompt template says DataAccess query is not replay")
    require("Query layer must not call AccountingEngine" in codex_prompt_template, "prompt template forbids query calling AccountingEngine")
    require("AccountingTradeFactReader.cpp" in dataaccess_cmake, "DataAccess CMake includes trade fact reader source")
    require("AccountingEngine" not in dataaccess_cmake, "DataAccess CMake does not link AccountingEngine")
    require("DataServiceApi" not in dataaccess_cmake, "DataAccess CMake does not link DataServiceApi")
    require("TradeFactQueryRequest" in accounting_trade_fact_reader_header, "trade fact reader header declares query request")
    require("TradeFactRow" in accounting_trade_fact_reader_header, "trade fact reader header declares row")
    require("TradeFactQueryResult" in accounting_trade_fact_reader_header, "trade fact reader header declares result")
    require("AccountingTradeFactReader" in accounting_trade_fact_reader_header, "trade fact reader header declares reader")
    require("AccountingEngine" not in accounting_trade_fact_reader_header, "trade fact reader header does not include AccountingEngine")
    require("DataServiceApi" not in accounting_trade_fact_reader_header, "trade fact reader header does not include DataServiceApi")
    require("AccountingReplayMinimalEngine" not in accounting_trade_fact_reader_header, "trade fact reader header does not reference minimal engine")
    require("SELECT" in accounting_trade_fact_reader_source, "trade fact reader source contains SELECT")
    for forbidden_sql in ["INSERT", "UPDATE", "DELETE", "CREATE", "DROP", "ALTER", "REPLACE", "VACUUM"]:
        require(forbidden_sql not in accounting_trade_fact_reader_source, f"trade fact reader source does not contain {forbidden_sql}")
    require("AccountingEngine" not in accounting_trade_fact_reader_source, "trade fact reader source does not call AccountingEngine")
    require("DataServiceApi" not in accounting_trade_fact_reader_source, "trade fact reader source does not include DataServiceApi")
    require("QtQuick" not in accounting_trade_fact_reader_source, "trade fact reader source does not include QtQuick")
    require("AccountingReplayEngine" not in accounting_trade_fact_reader_source, "trade fact reader source does not call AccountingReplayEngine")
    require("AccountingReplayMinimalEngine" not in accounting_trade_fact_reader_source, "trade fact reader source does not call minimal engine")
    require("dataaccess_accounting_trade_facts_readonly_query" in dataaccess_test_cmake, "DataAccess CMake registers trade facts query test")
    require("dataaccess_accounting_trade_facts_no_write" in dataaccess_test_cmake, "DataAccess CMake registers trade facts no-write test")
    require("dataaccess_accounting_trade_facts_sql_scan" in dataaccess_test_cmake, "DataAccess CMake registers trade facts SQL scan test")
    require("AccountingNoWriteTestUtils" in dataaccess_test_cmake, "DataAccess trade facts test links no-write harness")
    require("accountingTradeFactsReadOnlySql" in accounting_trade_fact_reader_test, "trade facts test scans reader SQL")
    require("assertNoWritesAround" in accounting_trade_fact_reader_test, "trade facts test uses no-write harness")
    require("containsForbiddenWriteSql" in accounting_trade_fact_reader_test, "trade facts test uses ForbiddenSqlScanner")

    require("Cash facts source boundary" in readme, "README documents cash facts source boundary")
    require("Cash facts query decision" in readme, "README documents cash facts query decision")
    require("TASK-088" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc records TASK-088")
    require(
        "docs/41_cash_facts_source_boundary.md" in sqlite_readonly_facts_query_boundary,
        "SQLite facts query doc references cash facts source boundary",
    )
    require(
        "cash_snapshot" in sqlite_readonly_facts_query_boundary and "portfolio_summary" in sqlite_readonly_facts_query_boundary,
        "SQLite facts query doc rejects derived cash sources",
    )
    require("TASK-088" in accounting_facts_source_mapping, "facts source mapping records TASK-088")
    require(
        "docs/41_cash_facts_source_boundary.md" in accounting_facts_source_mapping,
        "facts source mapping references cash facts source boundary",
    )
    require(
        "trade_log.net_cash_impact_cents" in accounting_facts_source_mapping,
        "facts source mapping distinguishes trade cash impact",
    )
    require("TASK-088" in dataservice_readonly_accounting_contracts, "DataService contract doc records TASK-088")
    require(
        "docs/41_cash_facts_source_boundary.md" in dataservice_readonly_accounting_contracts,
        "DataService contract doc references cash facts source boundary",
    )
    require("TASK-088" in dataservice_accounting_no_write_plan, "no-write plan records TASK-088")
    require(
        "docs/41_cash_facts_source_boundary.md" in dataservice_accounting_no_write_plan,
        "no-write plan references cash facts source boundary",
    )
    require("CashFactDto" in cash_facts_source_boundary, "cash facts source boundary documents CashFactDto")
    require("INITIAL_CASH" in cash_facts_source_boundary, "cash facts source boundary documents INITIAL_CASH")
    require("DEPOSIT" in cash_facts_source_boundary, "cash facts source boundary documents DEPOSIT")
    require("WITHDRAW" in cash_facts_source_boundary, "cash facts source boundary documents WITHDRAW")
    require("ADJUSTMENT" in cash_facts_source_boundary, "cash facts source boundary documents ADJUSTMENT")
    require("cash_snapshot is derived data" in cash_facts_source_boundary, "cash facts source boundary says cash_snapshot is derived")
    require(
        "portfolio_summary` is derived data" in cash_facts_source_boundary
        or "portfolio_summary is derived data" in cash_facts_source_boundary,
        "cash facts source boundary says portfolio_summary is derived",
    )
    require(
        "must not reverse-engineer cash facts from snapshot" in cash_facts_source_boundary,
        "cash facts source boundary forbids snapshot reverse inference",
    )
    require("Go / No-Go Checklist" in cash_facts_query_decision, "cash facts decision includes Go / No-Go Checklist")
    require(
        "schema is insufficient" in cash_facts_query_decision or "schema gap" in cash_facts_query_decision,
        "cash facts decision documents schema insufficiency",
    )
    require(
        "docs/41_cash_facts_source_boundary.md" in codex_prompt_template,
        "prompt template references cash facts source boundary",
    )
    require(
        "docs/42_cash_facts_query_decision.md" in codex_prompt_template,
        "prompt template references cash facts query decision",
    )
    require(
        "Never derive `CashFactDto` from `cash_snapshot` or `portfolio_summary`" in codex_prompt_template,
        "prompt template forbids deriving CashFactDto from snapshots",
    )
    require("Query layer must not compute cash balance" in codex_prompt_template, "prompt template forbids cash balance calculation in query layer")
    require("cash.summary DataService action guard" in readme, "README documents cash.summary guard")
    require("CASH_SUMMARY_NOT_AVAILABLE" in readme, "README documents cash.summary guard status")
    require("dataservice_cash_summary_guard" in readme, "README documents cash.summary guard test")
    require("dataservice_cash_summary_no_write" in readme, "README documents cash.summary no-write test")
    require("TASK-089" in dataservice_readonly_accounting_contracts, "DataService contract doc records TASK-089")
    require("cash.summary` as a DataService read-only action guard" in dataservice_readonly_accounting_contracts, "DataService contract doc documents cash.summary guard")
    require("implemented=false" in dataservice_readonly_accounting_contracts, "DataService contract doc states cash.summary guard implemented=false")
    require("CASH_SUMMARY_NOT_AVAILABLE" in dataservice_readonly_accounting_contracts, "DataService contract doc documents cash.summary status")
    require("CashSummaryResponse" in dataservice_readonly_accounting_contracts, "DataService contract doc documents CashSummaryResponse")
    require("docs/41_cash_facts_source_boundary.md" in dataservice_readonly_accounting_contracts, "DataService contract doc references cash facts boundary")
    require("TASK-089" in dataservice_accounting_no_write_plan, "no-write plan records TASK-089")
    require(
        "cash.summary` guard has no-write table count coverage" in dataservice_accounting_no_write_plan,
        "no-write plan documents cash.summary guard no-write coverage",
    )
    require("cash_snapshot" in dataservice_accounting_no_write_plan, "no-write plan protects cash_snapshot for cash.summary")
    require("portfolio_summary" in dataservice_accounting_no_write_plan, "no-write plan protects portfolio_summary for cash.summary")
    require("TASK-089" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc records TASK-089")
    require("cash.summary` guard does not use SQLite facts query" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc says cash.summary guard avoids SQLite")
    require("TASK-089" in accounting_facts_source_mapping, "facts source mapping records TASK-089")
    require("cash.summary` guard does not use this facts mapping" in accounting_facts_source_mapping, "facts source mapping says cash.summary guard avoids mapping")
    require("TASK-089" in cash_facts_source_boundary, "cash facts boundary records TASK-089")
    require("not a cash facts query" in cash_facts_source_boundary, "cash facts boundary says guard is not query")
    require("TASK-089" in cash_facts_query_decision, "cash facts decision records TASK-089")
    require("keeps `cash.summary` as a guard" in cash_facts_query_decision, "cash facts decision says cash.summary remains guard")
    require(
        "DataService `cash.summary` guard tasks must not pretend to be real" in codex_prompt_template,
        "prompt template says cash.summary guard is not real implementation",
    )
    require(
        "must not return real cash" in codex_prompt_template,
        "prompt template forbids real cash balance from guard",
    )
    require("portfolio.pnl.summary DataService action guard" in readme, "README documents portfolio.pnl.summary guard")
    require(
        "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE" in readme,
        "README documents portfolio.pnl.summary guard status",
    )
    require(
        "dataservice_portfolio_pnl_summary_guard" in readme,
        "README documents portfolio.pnl.summary guard test",
    )
    require(
        "dataservice_portfolio_pnl_summary_no_write" in readme,
        "README documents portfolio.pnl.summary no-write test",
    )
    require("TASK-090" in dataservice_readonly_accounting_contracts, "DataService contract doc records TASK-090")
    require(
        "portfolio.pnl.summary` as a DataService read-only action" in dataservice_readonly_accounting_contracts,
        "DataService contract doc documents portfolio.pnl.summary guard",
    )
    require(
        "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE" in dataservice_readonly_accounting_contracts,
        "DataService contract doc documents portfolio.pnl.summary status",
    )
    require(
        "PortfolioPnlSummaryResponse" in dataservice_readonly_accounting_contracts,
        "DataService contract doc documents PortfolioPnlSummaryResponse",
    )
    require("TASK-090" in dataservice_accounting_no_write_plan, "no-write plan records TASK-090")
    require(
        "portfolio.pnl.summary` guard has no-write table count coverage" in dataservice_accounting_no_write_plan,
        "no-write plan documents portfolio.pnl.summary guard no-write coverage",
    )
    require("TASK-090" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc records TASK-090")
    require(
        "portfolio.pnl.summary` guard does not use SQLite facts query" in sqlite_readonly_facts_query_boundary,
        "SQLite facts query doc says portfolio.pnl.summary guard avoids SQLite",
    )
    require("TASK-090" in accounting_facts_source_mapping, "facts source mapping records TASK-090")
    require(
        "portfolio.pnl.summary` guard does not use this facts mapping" in accounting_facts_source_mapping,
        "facts source mapping says portfolio.pnl.summary guard avoids mapping",
    )
    require(
        "DataService `portfolio.pnl.summary` guard tasks must not pretend to be real" in codex_prompt_template,
        "prompt template says portfolio.pnl.summary guard is not real implementation",
    )
    require(
        "must not return real" in codex_prompt_template and "totalAssets" in codex_prompt_template,
        "prompt template forbids real totalAssets from PnL guard",
    )
    require("base_position.summary DataService action guard" in readme, "README documents base_position.summary guard")
    require(
        "BASE_POSITION_SUMMARY_NOT_AVAILABLE" in readme,
        "README documents base_position.summary guard status",
    )
    require(
        "dataservice_base_position_summary_guard" in readme,
        "README documents base_position.summary guard test",
    )
    require(
        "dataservice_base_position_summary_no_write" in readme,
        "README documents base_position.summary no-write test",
    )
    require("TASK-091" in dataservice_readonly_accounting_contracts, "DataService contract doc records TASK-091")
    require(
        "base_position.summary` as a DataService read-only action" in dataservice_readonly_accounting_contracts,
        "DataService contract doc documents base_position.summary guard",
    )
    require(
        "BASE_POSITION_SUMMARY_NOT_AVAILABLE" in dataservice_readonly_accounting_contracts,
        "DataService contract doc documents base_position.summary status",
    )
    require(
        "BasePositionSummaryResponse" in dataservice_readonly_accounting_contracts,
        "DataService contract doc documents BasePositionSummaryResponse",
    )
    require(
        "sellableAboveBaseAmountText` is not a trade suggestion" in dataservice_readonly_accounting_contracts,
        "DataService contract doc says sellableAboveBaseAmountText is not trade suggestion",
    )
    require("TASK-091" in dataservice_accounting_no_write_plan, "no-write plan records TASK-091")
    require(
        "base_position.summary` guard has no-write table count coverage" in dataservice_accounting_no_write_plan,
        "no-write plan documents base_position.summary guard no-write coverage",
    )
    require("TASK-091" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc records TASK-091")
    require(
        "base_position.summary` guard does not use SQLite facts query" in sqlite_readonly_facts_query_boundary,
        "SQLite facts query doc says base_position.summary guard avoids SQLite",
    )
    require("TASK-091" in accounting_facts_source_mapping, "facts source mapping records TASK-091")
    require(
        "base_position.summary` guard does not use this facts mapping" in accounting_facts_source_mapping,
        "facts source mapping says base_position.summary guard avoids mapping",
    )
    require(
        "DataService `base_position.summary` guard tasks must not pretend to be real" in codex_prompt_template,
        "prompt template says base_position.summary guard is not real implementation",
    )
    require(
        "sellableAboveBaseAmountText` is not a sell suggestion" in codex_prompt_template,
        "prompt template says sellableAboveBaseAmountText is not sell suggestion",
    )
    require(
        "must not generate TradeDraft" in codex_prompt_template,
        "prompt template forbids TradeDraft from base position guard",
    )
    require("sniper_pool.summary DataService action guard" in readme, "README documents sniper_pool.summary guard")
    require(
        "SNIPER_POOL_SUMMARY_NOT_AVAILABLE" in readme,
        "README documents sniper_pool.summary guard status",
    )
    require(
        "dataservice_sniper_pool_summary_guard" in readme,
        "README documents sniper_pool.summary guard test",
    )
    require(
        "dataservice_sniper_pool_summary_no_write" in readme,
        "README documents sniper_pool.summary no-write test",
    )
    require("TASK-092" in dataservice_readonly_accounting_contracts, "DataService contract doc records TASK-092")
    require(
        "sniper_pool.summary` as a DataService read-only action" in dataservice_readonly_accounting_contracts,
        "DataService contract doc documents sniper_pool.summary guard",
    )
    require(
        "SNIPER_POOL_SUMMARY_NOT_AVAILABLE" in dataservice_readonly_accounting_contracts,
        "DataService contract doc documents sniper_pool.summary status",
    )
    require(
        "SniperPoolSummaryResponse" in dataservice_readonly_accounting_contracts,
        "DataService contract doc documents SniperPoolSummaryResponse",
    )
    require(
        "remainingAmountText` is not a trade suggestion" in dataservice_readonly_accounting_contracts,
        "DataService contract doc says remainingAmountText is not trade suggestion",
    )
    require(
        "completed` is not derived from current market value" in dataservice_readonly_accounting_contracts,
        "DataService contract doc says completed is not market-value derived",
    )
    require("TASK-092" in dataservice_accounting_no_write_plan, "no-write plan records TASK-092")
    require(
        "sniper_pool.summary` guard has no-write table count coverage" in dataservice_accounting_no_write_plan,
        "no-write plan documents sniper_pool.summary guard no-write coverage",
    )
    require("TASK-092" in sqlite_readonly_facts_query_boundary, "SQLite facts query doc records TASK-092")
    require(
        "sniper_pool.summary` guard does not use SQLite facts query" in sqlite_readonly_facts_query_boundary,
        "SQLite facts query doc says sniper_pool.summary guard avoids SQLite",
    )
    require("TASK-092" in accounting_facts_source_mapping, "facts source mapping records TASK-092")
    require(
        "sniper_pool.summary` guard does not use this facts mapping" in accounting_facts_source_mapping,
        "facts source mapping says sniper_pool.summary guard avoids mapping",
    )
    require(
        "DataService `sniper_pool.summary` guard tasks must not pretend to be real" in codex_prompt_template,
        "prompt template says sniper_pool.summary guard is not real implementation",
    )
    require(
        "remainingAmountText` is not a trade suggestion" in codex_prompt_template,
        "prompt template says remainingAmountText is not trade suggestion",
    )
    require(
        "completed` is not derived from current market value" in codex_prompt_template,
        "prompt template says completed is not derived from market value",
    )
    require("accounting.replay.preview" in dataservice_guard_suite_milestone, "guard suite milestone lists replay preview")
    require("position.list" in dataservice_guard_suite_milestone, "guard suite milestone lists position.list")
    require("cash.summary" in dataservice_guard_suite_milestone, "guard suite milestone lists cash.summary")
    require("portfolio.pnl.summary" in dataservice_guard_suite_milestone, "guard suite milestone lists portfolio.pnl.summary")
    require("base_position.summary" in dataservice_guard_suite_milestone, "guard suite milestone lists base_position.summary")
    require("sniper_pool.summary" in dataservice_guard_suite_milestone, "guard suite milestone lists sniper_pool.summary")
    require("implemented=false" in dataservice_guard_suite_milestone, "guard suite milestone states implemented=false")
    require("readOnly=true" in dataservice_guard_suite_milestone, "guard suite milestone states readOnly=true")
    require("writeEnabled=false" in dataservice_guard_suite_milestone, "guard suite milestone states writeEnabled=false")
    require("no-write" in dataservice_guard_suite_milestone, "guard suite milestone documents no-write coverage")
    require("Go / No-Go Checklist" in dataservice_guard_no_write_readiness, "guard readiness has Go / No-Go Checklist")
    require(
        "AccountingEngine has not been called by DataService" in dataservice_guard_no_write_readiness,
        "guard readiness says AccountingEngine is not called by DataService",
    )
    require(
        "SQLite facts query has not been called by DataService" in dataservice_guard_no_write_readiness,
        "guard readiness says SQLite facts query is not called by DataService",
    )
    require(
        "v0.5.0-dataservice-accounting-guard-suite" in release_notes_v05,
        "v0.5 release notes include suggested tag",
    )
    require("DataService Accounting Guard Suite" in release_notes_v05, "v0.5 release notes title exists")
    require("No-write guard tests" in release_notes_v05, "v0.5 release notes list no-write guard tests")
    require(
        "docs/43_dataservice_accounting_guard_suite_milestone.md" in dataservice_readonly_accounting_contracts,
        "DataService contract doc links docs/43",
    )
    require(
        "docs/43_dataservice_accounting_guard_suite_milestone.md" in dataservice_accounting_no_write_plan,
        "no-write plan links docs/43",
    )
    require(
        "DataService accounting guard suite milestone does not equal real action implementation"
        in codex_prompt_template,
        "prompt template says guard suite milestone is not real action implementation",
    )
    require("DataAccess `TradeFactRow`" in position_list_real_boundary, "position.list boundary mentions DataAccess TradeFactRow")
    require("AccountingEngine `TradeFactDto`" in position_list_real_boundary, "position.list boundary mentions AccountingEngine TradeFactDto")
    require("no-write" in position_list_real_boundary, "position.list boundary documents no-write")
    require("cash facts" in position_list_real_boundary, "position.list boundary documents cash facts source")
    require("MARKET_PRICE_MISSING" in position_list_real_boundary, "position.list boundary documents missing market price")
    require("MULTI_CURRENCY_UNSUPPORTED" in position_list_real_boundary, "position.list boundary documents multi-currency unsupported")
    require("Guard -> Real Action" in position_list_real_boundary, "position.list boundary documents guard to real action switch")
    require("Go / No-Go Checklist" in position_list_readiness_checklist, "position.list readiness has Go / No-Go Checklist")
    require("User explicitly authorized" in position_list_readiness_checklist, "position.list readiness requires user authorization")
    require("docs/45_position_list_real_implementation_boundary.md" in dataservice_readonly_accounting_contracts, "DataService contract doc links docs/45")
    require("docs/45_position_list_real_implementation_boundary.md" in dataservice_accounting_no_write_plan, "no-write plan links docs/45")
    require("docs/45_position_list_real_implementation_boundary.md" in sqlite_readonly_facts_query_boundary, "SQLite boundary links docs/45")
    require("docs/45_position_list_real_implementation_boundary.md" in accounting_facts_source_mapping, "facts mapping links docs/45")
    require("docs/45_position_list_real_implementation_boundary.md" in codex_prompt_template, "prompt template links docs/45")
    require("docs/46_position_list_real_implementation_readiness_checklist.md" in codex_prompt_template, "prompt template links docs/46")
    require("TASK-095" in position_list_real_boundary, "docs/45 records TASK-095 mapping contract tests")
    require("mapping contract tests" in position_list_real_boundary, "docs/45 documents mapping contract tests")
    require("mapping contract tests" in position_list_readiness_checklist, "docs/46 documents mapping contract tests")
    require(
        "TradeFactRow` -> `TradeFactDto` mapping contract" in accounting_facts_source_mapping,
        "docs/40 documents TradeFactRow to TradeFactDto mapping contract tests",
    )
    require(
        "position.list` mapping contract tests do not equal real action" in codex_prompt_template,
        "prompt template says mapping contract tests are not real action",
    )
    require(
        "add_subdirectory(PositionListMapping)" in tests_cmake,
        "tests CMake adds PositionListMapping tests",
    )
    require(
        "position_list_request_mapping_contract" in position_list_mapping_cmake,
        "PositionListMapping CMake registers request mapping contract test",
    )
    require(
        "position_list_trade_fact_mapping_contract" in position_list_mapping_cmake,
        "PositionListMapping CMake registers trade fact mapping contract test",
    )
    require(
        "position_list_response_mapping_contract" in position_list_mapping_cmake,
        "PositionListMapping CMake registers response mapping contract test",
    )
    require(
        "mapPositionListRequestToReplayRequestForTest" in position_list_mapping_utils_header,
        "mapping helper exposes request mapping",
    )
    require(
        "mapTradeFactRowToTradeFactDtoForTest" in position_list_mapping_utils_header,
        "mapping helper exposes trade fact mapping",
    )
    require(
        "mapReplayResultToPositionListResponseForTest" in position_list_mapping_utils_header,
        "mapping helper exposes response mapping",
    )
    require("SQLite" not in position_list_mapping_utils_source, "mapping helper source does not access SQLite")
    require("DataServiceActions" not in position_list_mapping_utils_source, "mapping helper source does not call DataService action")
    require("AccountingReplayEngine" not in position_list_mapping_utils_source, "mapping helper does not call AccountingReplayEngine")
    require(
        "position.list guard remains `implemented=false`" in position_list_readiness_checklist
        or "do not authorize changing the guard to a real" in position_list_readiness_checklist,
        "readiness checklist keeps position.list guard as guard",
    )
    require("validateReplayRequest" in position_list_request_mapping_test, "request mapping test validates replay request")
    require("validateTradeFact" in position_list_trade_fact_mapping_test, "trade fact mapping test validates trade facts")
    require("makeSingleBuyReplayResult" in position_list_response_mapping_test, "response mapping test covers single BUY")
    require("makeMissingMarketPriceReplayResult" in position_list_response_mapping_test, "response mapping test covers missing market price")
    require(
        "makeMultiCurrencyUnsupportedReplayResult" in position_list_response_mapping_test,
        "response mapping test covers multi-currency unsupported",
    )
    require("TASK-096" in position_list_real_boundary, "docs/45 records TASK-096 no-write skeleton")
    require(
        "real action no-write skeleton" in position_list_real_boundary,
        "docs/45 documents real action no-write skeleton",
    )
    require(
        "no-write skeleton" in position_list_readiness_checklist,
        "docs/46 documents no-write skeleton",
    )
    require(
        "position.list Real Action No-write Skeleton" in dataservice_accounting_no_write_plan,
        "docs/38 documents position.list real action no-write skeleton",
    )
    require(
        "no-write skeleton does not equal real action implementation" in codex_prompt_template,
        "prompt template says no-write skeleton is not real action",
    )
    require(
        "add_subdirectory(PositionListRealActionNoWrite)" in tests_cmake,
        "tests CMake adds PositionListRealActionNoWrite tests",
    )
    require(
        "position_list_real_action_no_write_skeleton" in position_list_no_write_cmake,
        "PositionListRealActionNoWrite CMake registers valid-path skeleton test",
    )
    require(
        "position_list_real_action_failure_no_write_skeleton" in position_list_no_write_cmake,
        "PositionListRealActionNoWrite CMake registers failure-path skeleton test",
    )
    require(
        "PositionListRealActionNoWriteScenario" in position_list_no_write_skeleton_header,
        "position.list no-write skeleton helper declares scenario model",
    )
    require(
        "runFakePositionListRealActionCallbackForTest" in position_list_no_write_skeleton_header,
        "position.list no-write skeleton helper exposes fake callback",
    )
    require("valid_request_guard_path" in position_list_no_write_skeleton_source, "skeleton covers valid request path")
    require(
        "invalid_request_protocol_error_path" in position_list_no_write_skeleton_source,
        "skeleton covers invalid request path",
    )
    require("replay_unavailable_path" in position_list_no_write_skeleton_source, "skeleton covers replay unavailable")
    require(
        "facts_query_unavailable_path" in position_list_no_write_skeleton_source,
        "skeleton covers facts query unavailable",
    )
    require("mapping_failure_path" in position_list_no_write_skeleton_source, "skeleton covers mapping failure")
    require(
        "missing_market_price_path" in position_list_no_write_skeleton_source,
        "skeleton covers missing market price",
    )
    require(
        "multi_currency_unsupported_path" in position_list_no_write_skeleton_source,
        "skeleton covers multi-currency unsupported",
    )
    require("FX_RATE_MISSING" in position_list_no_write_skeleton_source, "skeleton covers missing FX rate")
    require(
        "SQLITE_READONLY_OPEN_FAILED" in position_list_no_write_skeleton_source,
        "skeleton covers SQLite read-only open failure",
    )
    require("SQLITE_QUERY_FAILED" in position_list_no_write_skeleton_source, "skeleton covers SQLite query failure")
    require(
        "assertNoWritesAround" in position_list_no_write_skeleton_test,
        "no-write skeleton test uses no-write harness",
    )
    require(
        "assertNoWritesAround" in position_list_failure_no_write_skeleton_test,
        "failure no-write skeleton test uses no-write harness",
    )
    require("AccountingReplayEngine" not in position_list_no_write_skeleton_source, "skeleton helper does not call AccountingEngine replay")
    require("DataServiceActions" not in position_list_no_write_skeleton_source, "skeleton helper does not call DataService action")
    require("AccountingTradeFactReader" not in position_list_no_write_skeleton_source, "skeleton helper does not call DataAccess trade facts reader")
    require(
        "implemented=false" in position_list_real_boundary
        or "implemented=false" in position_list_readiness_checklist,
        "docs keep position.list guard implemented=false",
    )
    require(
        "47_position_list_readiness_hardening" in readme,
        "README links position.list readiness hardening doc",
    )
    require(
        "48_position_list_first_stage_real_action_scope" in readme,
        "README links position.list first-stage scope doc",
    )
    require(
        "47_position_list_readiness_hardening" in docs_index,
        "docs index links position.list readiness hardening doc",
    )
    require(
        "48_position_list_first_stage_real_action_scope" in docs_index,
        "docs index links position.list first-stage scope doc",
    )
    require("cash facts gap" in position_list_readiness_hardening, "docs/47 documents cash facts gap")
    require("MARKET_PRICE_MISSING" in position_list_readiness_hardening, "docs/47 documents missing market price")
    require(
        "MULTI_CURRENCY_UNSUPPORTED" in position_list_readiness_hardening,
        "docs/47 documents multi-currency unsupported",
    )
    require("Guard Fallback" in position_list_readiness_hardening, "docs/47 documents Guard fallback")
    require(
        "Failure Behavior Matrix" in position_list_readiness_hardening,
        "docs/47 documents Failure behavior matrix",
    )
    require(
        "Allowed First-stage Scope" in position_list_first_stage_scope,
        "docs/48 documents Allowed first-stage scope",
    )
    require("Explicitly Excluded" in position_list_first_stage_scope, "docs/48 documents explicitly excluded scope")
    require("QML integration" in position_list_first_stage_scope, "docs/48 excludes QML integration")
    require(
        "docs/47_position_list_readiness_hardening.md" in position_list_real_boundary,
        "docs/45 links docs/47",
    )
    require(
        "docs/48_position_list_first_stage_real_action_scope.md" in position_list_readiness_checklist,
        "docs/46 links docs/48",
    )
    require(
        "docs/47_position_list_readiness_hardening.md" in codex_prompt_template,
        "prompt template links docs/47",
    )
    require(
        "docs/48_position_list_first_stage_real_action_scope.md" in codex_prompt_template,
        "prompt template links docs/48",
    )
    require(
        "docs/47_position_list_readiness_hardening.md" in dataservice_accounting_no_write_plan,
        "no-write plan links docs/47",
    )
    require(
        "docs/48_position_list_first_stage_real_action_scope.md" in dataservice_readonly_accounting_contracts,
        "DataService contracts link docs/48",
    )
    require(
        "add_subdirectory(PositionListFirstStageScaffolding)" in tests_cmake,
        "tests CMake adds PositionListFirstStageScaffolding tests",
    )
    require(
        "position_list_first_stage_scope_contract" in position_list_first_stage_cmake,
        "first-stage CMake registers scope contract test",
    )
    require(
        "position_list_first_stage_guard_transition_contract" in position_list_first_stage_cmake,
        "first-stage CMake registers guard transition contract test",
    )
    require(
        "position_list_first_stage_failure_matrix_contract" in position_list_first_stage_cmake,
        "first-stage CMake registers failure matrix contract test",
    )
    require(
        "PositionListFirstStageScenario" in position_list_first_stage_scenario_header,
        "first-stage scenario helper declares scenario model",
    )
    require("empty_ledger_ok" in position_list_first_stage_scenario_source, "first-stage scenarios include empty ledger")
    require("single_buy_ok" in position_list_first_stage_scenario_source, "first-stage scenarios include single BUY")
    require(
        "missing_market_price_warning" in position_list_first_stage_scenario_source,
        "first-stage scenarios include missing market price",
    )
    require(
        "multi_currency_unsupported_error" in position_list_first_stage_scenario_source,
        "first-stage scenarios include multi-currency unsupported",
    )
    require(
        "cash_facts_missing_degraded_or_unavailable" in position_list_first_stage_scenario_source,
        "first-stage scenarios include cash facts missing",
    )
    require("mapping_failure_error" in position_list_first_stage_scenario_source, "first-stage scenarios include mapping failure")
    require("replay_unavailable_error" in position_list_first_stage_scenario_source, "first-stage scenarios include replay unavailable")
    require("invalid_request_protocol_error" in position_list_first_stage_scenario_source, "first-stage scenarios include invalid request")
    require("facts_query_unavailable_error" in position_list_first_stage_scenario_source, "first-stage scenarios include facts query unavailable")
    require("unsupported_scenario_error" in position_list_first_stage_scenario_source, "first-stage scenarios include unsupported scenario")
    require("position_list_first_stage_scope_contract" in position_list_first_stage_cmake, "CTest name for scope contract exists")
    require("TradeDraft excluded" in position_list_first_stage_scope_test, "scope contract excludes TradeDraft")
    require("QML integration excluded" in position_list_first_stage_scope_test, "scope contract excludes QML")
    require("implemented=false" in position_list_first_stage_transition_test, "guard transition contract keeps guard implemented false")
    require("protected tables include trade_log" in position_list_first_stage_transition_test, "guard transition contract checks protected table list")
    require("INVALID_REQUEST" in position_list_first_stage_failure_test, "failure matrix test checks INVALID_REQUEST")
    require("REPLAY_NOT_AVAILABLE" in position_list_first_stage_failure_test, "failure matrix test checks REPLAY_NOT_AVAILABLE")
    require("DATA_SOURCE_MISSING" in position_list_first_stage_failure_test, "failure matrix test checks DATA_SOURCE_MISSING")
    require("POSITION_LIST_MAPPING_FAILED" in position_list_first_stage_failure_test, "failure matrix test checks mapping failure")
    require("MARKET_PRICE_MISSING" in position_list_first_stage_failure_test, "failure matrix test checks market price missing")
    require("MULTI_CURRENCY_UNSUPPORTED" in position_list_first_stage_failure_test, "failure matrix test checks multi-currency unsupported")
    require("UNSUPPORTED_SCENARIO" in position_list_first_stage_failure_test, "failure matrix test checks unsupported scenario")
    require("TASK-098" in position_list_real_boundary, "docs/45 records TASK-098 first-stage scaffolding")
    require("first-stage implementation scaffolding" in position_list_real_boundary, "docs/45 documents first-stage scaffolding")
    require("first-stage scaffolding" in position_list_readiness_checklist, "docs/46 documents first-stage scaffolding")
    require("failure matrix" in position_list_readiness_hardening, "docs/47 documents failure matrix")
    require("scope contract tests" in position_list_first_stage_scope, "docs/48 documents scope contract tests")
    require("TASK-098" in codex_prompt_template, "prompt template mentions TASK-098")
    require(
        "Scaffolding does not equal real action implementation" in codex_prompt_template,
        "prompt template says scaffolding is not real action",
    )
    require("DataServiceActions" not in position_list_first_stage_scenario_source, "scaffolding helper does not call DataService action")
    require("AccountingReplayEngine" not in position_list_first_stage_scenario_source, "scaffolding helper does not call AccountingReplayEngine")
    require("AccountingTradeFactReader" not in position_list_first_stage_scenario_source, "scaffolding helper does not call DataAccess trade facts reader")
    require(
        "49_shellservices_accounting_controller_contract" in readme,
        "README links ShellServices accounting controller contract",
    )
    require(
        "50_shell_accounting_viewmodel_state_contract" in readme,
        "README links Shell accounting ViewModel state contract",
    )
    require(
        "49_shellservices_accounting_controller_contract" in docs_index,
        "docs index links ShellServices accounting controller contract",
    )
    require(
        "50_shell_accounting_viewmodel_state_contract" in docs_index,
        "docs index links Shell accounting ViewModel state contract",
    )
    require(
        "QML 不得直接调用 DataServiceClient" in shellservices_accounting_controller_contract,
        "docs/49 forbids direct QML DataServiceClient calls",
    )
    require(
        "ShellAccountingReadOnlyController" in shellservices_accounting_controller_contract,
        "docs/49 defines ShellAccountingReadOnlyController",
    )
    require(
        "ShellPositionListModel" in shellservices_accounting_controller_contract,
        "docs/49 defines ShellPositionListModel",
    )
    require(
        "ShellAccountingIssueListModel" in shellservices_accounting_controller_contract,
        "docs/49 defines ShellAccountingIssueListModel",
    )
    require("不生成 TradeDraft" in shellservices_accounting_controller_contract, "docs/49 forbids TradeDraft generation")
    require("不生成交易建议" in shellservices_accounting_controller_contract, "docs/49 forbids trade suggestions")
    require(
        "Empty vs Unavailable" in shell_accounting_viewmodel_state_contract,
        "docs/50 defines Empty vs Unavailable",
    )
    require(
        "implemented=false -> Unavailable" in shell_accounting_viewmodel_state_contract,
        "docs/50 maps implemented false to unavailable",
    )
    require("privacyMode" in shell_accounting_viewmodel_state_contract, "docs/50 defines privacyMode")
    require("no trade suggestion" in shell_accounting_viewmodel_state_contract, "docs/50 defines no trade suggestion")
    require(
        "docs/49_shellservices_accounting_controller_contract.md" in codex_prompt_template,
        "prompt template links docs/49",
    )
    require(
        "docs/50_shell_accounting_viewmodel_state_contract.md" in codex_prompt_template,
        "prompt template links docs/50",
    )
    require(
        "docs/49_shellservices_accounting_controller_contract.md" in dataservice_readonly_accounting_contracts,
        "DataService contracts link docs/49",
    )
    require("ShellServices controller" in position_list_real_boundary, "docs/45 mentions ShellServices controller")
    require(
        "49_shellservices_accounting_controller_contract.md" in ui_design
        or "ShellServices Accounting Controller Contract" in ui_design,
        "UI design links ShellServices accounting controller contract",
    )
    require(
        "docs/49_shellservices_accounting_controller_contract.md" in position_viewmodel_design,
        "position ViewModel design links docs/49",
    )
    require(
        "docs/50_shell_accounting_viewmodel_state_contract.md" in position_mapping,
        "position DTO ViewModel mapping links docs/50",
    )
    require(
        "shell_accounting_controller_state_contract" in shell_accounting_scaffolding_cmake,
        "Shell accounting scaffolding registers state contract test",
    )
    require(
        "shell_accounting_controller_issue_contract" in shell_accounting_scaffolding_cmake,
        "Shell accounting scaffolding registers issue contract test",
    )
    require(
        "shell_accounting_controller_privacy_contract" in shell_accounting_scaffolding_cmake,
        "Shell accounting scaffolding registers privacy contract test",
    )
    require(
        "shell_accounting_controller_no_trade_action_contract" in shell_accounting_scaffolding_cmake,
        "Shell accounting scaffolding registers no trade action contract test",
    )
    require(
        "ShellAccountingControllerScenario" in shell_accounting_scenario_header,
        "Shell accounting scenario helper exists",
    )
    require(
        "ShellAccountingControllerScenario" in shell_accounting_scenario_source,
        "Shell accounting scenario source implements helper",
    )
    require(
        "mapScenarioToViewState" in shell_accounting_state_utils_header,
        "Shell accounting state utils expose state mapping",
    )
    require(
        "implemented=false -> Unavailable" in shell_accounting_viewmodel_state_contract,
        "docs/50 still states implemented=false maps to Unavailable",
    )
    require(
        "TASK-100" in shellservices_accounting_controller_contract
        and "test scaffolding" in shellservices_accounting_controller_contract,
        "docs/49 mentions TASK-100 test scaffolding",
    )
    require("TASK-100" in shell_accounting_viewmodel_state_contract, "docs/50 mentions TASK-100")
    require(
        "ShellServices accounting implementation tasks must pass TASK-100 scaffolding" in codex_prompt_template,
        "prompt template requires TASK-100 scaffolding",
    )
    require(
        "QML must not directly call DataServiceClient" in codex_prompt_template,
        "prompt template forbids direct QML DataServiceClient calls",
    )
    require(
        "Accounting UI states must distinguish Empty / Unavailable / Warning / Error /" in ui_design,
        "UI design defines accounting UI states",
    )
    require(
        "shell_accounting_controller_state_contract" in shell_accounting_state_test,
        "state contract test name is present",
    )
    require("implemented=false" in shell_accounting_state_test, "state test checks implemented=false")
    require("Unavailable" in shell_accounting_state_test, "state test checks Unavailable")
    require("Empty" in shell_accounting_state_test, "state test checks Empty")
    require("Warning" in shell_accounting_state_test, "state test checks Warning")
    require("Error" in shell_accounting_state_test, "state test checks Error")
    require("Stale" in shell_accounting_state_test, "state test checks Stale")
    require("Loading" in shell_accounting_state_test, "state test checks Loading")
    require("MARKET_PRICE_MISSING" in shell_accounting_issue_test, "issue test checks MARKET_PRICE_MISSING")
    require("NEGATIVE_CASH" in shell_accounting_issue_test, "issue test checks NEGATIVE_CASH")
    require(
        "MULTI_CURRENCY_UNSUPPORTED" in shell_accounting_issue_test,
        "issue test checks MULTI_CURRENCY_UNSUPPORTED",
    )
    require("privacyMode=true" in shell_accounting_privacy_test, "privacy test checks privacyMode")
    require("display text" in shell_accounting_privacy_test, "privacy test checks display text")
    require("Unavailable does not fabricate masked amount" in shell_accounting_privacy_test, "privacy test checks unavailable amount")
    require("Empty does not fabricate amount" in shell_accounting_privacy_test, "privacy test checks empty amount")
    require("TradeDraft" in shell_accounting_no_trade_test, "no trade test checks TradeDraft")
    require("trade suggestion" in shell_accounting_no_trade_test, "no trade test checks trade suggestion")
    require("broker order" in shell_accounting_no_trade_test, "no trade test checks broker order")
    require("strategy" in shell_accounting_no_trade_test, "no trade test checks strategy execution")
    require("DataServiceClient" not in shell_accounting_scenario_source, "scaffolding helper does not call real DataServiceClient")
    require("SQLite" not in shell_accounting_scenario_source, "scaffolding helper does not access SQLite")
    require("AccountingEngine" not in shell_accounting_scenario_source, "scaffolding helper does not call AccountingEngine")
    require("ShellAccountingControllerScaffolding" not in root_cmake, "scaffolding is not added as production root target")
    require(
        "add_subdirectory(ShellAccountingControllerScaffolding)" in tests_cmake,
        "tests CMake adds Shell accounting scaffolding",
    )
    require(
        "ShellAccountingReadOnlyController production skeleton" in readme,
        "README mentions ShellAccountingReadOnlyController skeleton",
    )
    require(
        "TASK-101" in shellservices_accounting_controller_contract
        and "Production Skeleton" in shellservices_accounting_controller_contract,
        "docs/49 mentions TASK-101 production skeleton",
    )
    require(
        "TASK-101" in shell_accounting_viewmodel_state_contract
        and "Production Skeleton" in shell_accounting_viewmodel_state_contract,
        "docs/50 mentions TASK-101 production skeleton",
    )
    require("TASK-101 ShellAccountingReadOnlyController skeleton" in codex_prompt_template, "prompt template mentions TASK-101")
    require(
        "ShellAccountingReadOnlyController Skeleton" in ui_design,
        "UI design mentions ShellAccountingReadOnlyController skeleton",
    )
    require(
        "ShellAccountingReadOnlyController" in shellservices_public_header,
        "ShellServices public header includes ShellAccountingReadOnlyController",
    )
    require(
        "ShellAccountingReadOnlyController" in shellservices_cmake,
        "ShellServices CMake includes ShellAccountingReadOnlyController",
    )
    require(
        "ShellAccountingViewState" in shell_accounting_state_header,
        "Shell accounting state header declares ShellAccountingViewState",
    )
    require("Idle" in shell_accounting_state_header, "Shell accounting state includes Idle")
    require("Loading" in shell_accounting_state_header, "Shell accounting state includes Loading")
    require("Loaded" in shell_accounting_state_header, "Shell accounting state includes Loaded")
    require("Empty" in shell_accounting_state_header, "Shell accounting state includes Empty")
    require("Unavailable" in shell_accounting_state_header, "Shell accounting state includes Unavailable")
    require("Warning" in shell_accounting_state_header, "Shell accounting state includes Warning")
    require("Error" in shell_accounting_state_header, "Shell accounting state includes Error")
    require("Stale" in shell_accounting_state_header, "Shell accounting state includes Stale")
    require("implemented = false" in shell_accounting_controller_header, "state snapshot defaults implemented false")
    require("readOnly = true" in shell_accounting_controller_header, "state snapshot defaults readOnly true")
    require("writeEnabled = false" in shell_accounting_controller_header, "state snapshot defaults writeEnabled false")
    require("tradeDraftGenerationEnabled" in shell_accounting_controller_header, "controller exposes TradeDraft boundary")
    require("tradeSuggestionEnabled" in shell_accounting_controller_header, "controller exposes trade suggestion boundary")
    require("strategyExecutionEnabled" in shell_accounting_controller_header, "controller exposes strategy execution boundary")
    require("brokerOrderEnabled" in shell_accounting_controller_header, "controller exposes broker order boundary")
    require("applyStateSnapshot" in shell_accounting_controller_header, "controller accepts in-memory state snapshot")
    require("beginRefresh" in shell_accounting_controller_header, "controller exposes beginRefresh skeleton")
    require("markUnavailable" in shell_accounting_controller_header, "controller exposes unavailable marker")
    require("shellAccountingDisplayText" in shell_accounting_display_header, "display helper is declared")
    require("privacyMode" in shell_accounting_display_source, "display helper handles privacyMode")
    require("MARKET_PRICE_MISSING" in shell_accounting_issue_source, "issue source maps MARKET_PRICE_MISSING")
    require("MISSING_FEE" in shell_accounting_issue_source, "issue source maps MISSING_FEE")
    require("NEGATIVE_CASH" in shell_accounting_issue_source, "issue source maps NEGATIVE_CASH")
    require("SELL_EXCEEDS_POSITION" in shell_accounting_issue_source, "issue source maps SELL_EXCEEDS_POSITION")
    require("MULTI_CURRENCY_UNSUPPORTED" in shell_accounting_issue_source, "issue source maps MULTI_CURRENCY_UNSUPPORTED")
    require("FX_RATE_MISSING" in shell_accounting_issue_source, "issue source maps FX_RATE_MISSING")
    require("REPLAY_NOT_AVAILABLE" in shell_accounting_issue_source, "issue source maps REPLAY_NOT_AVAILABLE")
    require(
        "shell_accounting_readonly_controller_skeleton_state" in shellservices_tests_cmake,
        "ShellServices tests register skeleton state test",
    )
    require(
        "shell_accounting_readonly_controller_skeleton_issue" in shellservices_tests_cmake,
        "ShellServices tests register skeleton issue test",
    )
    require(
        "shell_accounting_readonly_controller_skeleton_privacy" in shellservices_tests_cmake,
        "ShellServices tests register skeleton privacy test",
    )
    require(
        "shell_accounting_readonly_controller_skeleton_no_trade_action" in shellservices_tests_cmake,
        "ShellServices tests register skeleton no trade action test",
    )
    require(
        "shell_accounting_readonly_controller_skeleton_no_service_dependency" in shellservices_tests_cmake,
        "ShellServices tests register skeleton no service dependency test",
    )
    require("implemented=false maps to Unavailable" in shell_accounting_controller_state_test, "skeleton state test checks implemented false")
    require("Empty and Unavailable are distinct" in shell_accounting_controller_state_test, "skeleton state test checks Empty vs Unavailable")
    require("MARKET_PRICE_MISSING" in shell_accounting_controller_issue_test, "skeleton issue test checks MARKET_PRICE_MISSING")
    require("MISSING_FEE" in shell_accounting_controller_issue_test, "skeleton issue test checks MISSING_FEE")
    require("NEGATIVE_CASH" in shell_accounting_controller_issue_test, "skeleton issue test checks NEGATIVE_CASH")
    require("SELL_EXCEEDS_POSITION" in shell_accounting_controller_issue_test, "skeleton issue test checks SELL_EXCEEDS_POSITION")
    require(
        "MULTI_CURRENCY_UNSUPPORTED" in shell_accounting_controller_issue_test,
        "skeleton issue test checks MULTI_CURRENCY_UNSUPPORTED",
    )
    require("FX_RATE_MISSING" in shell_accounting_controller_issue_test, "skeleton issue test checks FX_RATE_MISSING")
    require("privacyMode=false" in shell_accounting_controller_privacy_test, "skeleton privacy test checks privacyMode false")
    require("privacyMode=true" in shell_accounting_controller_privacy_test, "skeleton privacy test checks privacyMode true")
    require("Empty state does not fabricate amount" in shell_accounting_controller_privacy_test, "skeleton privacy test checks Empty")
    require("Unavailable state does not fabricate amount" in shell_accounting_controller_privacy_test, "skeleton privacy test checks Unavailable")
    require("TradeDraft" in shell_accounting_controller_no_trade_test, "skeleton no trade test checks TradeDraft")
    require("trade suggestions" in shell_accounting_controller_no_trade_test, "skeleton no trade test checks trade suggestions")
    require("strategy" in shell_accounting_controller_no_trade_test, "skeleton no trade test checks strategy")
    require("broker order" in shell_accounting_controller_no_trade_test, "skeleton no trade test checks broker order")
    require("DataServiceClient" in shell_accounting_controller_no_dependency_test, "no dependency test scans DataServiceClient")
    require("DataServiceApi" in shell_accounting_controller_no_dependency_test, "no dependency test scans DataServiceApi")
    require("DataAccess" in shell_accounting_controller_no_dependency_test, "no dependency test scans DataAccess")
    require("AccountingEngine" in shell_accounting_controller_no_dependency_test, "no dependency test scans AccountingEngine")
    require("SQLite" in shell_accounting_controller_no_dependency_test, "no dependency test scans SQLite")
    require("QtQuick" in shell_accounting_controller_no_dependency_test, "no dependency test scans QtQuick")
    shell_accounting_controller_sources = "\n".join(
        [
            shell_accounting_controller_header,
            shell_accounting_controller_source,
            shell_accounting_state_header,
            shell_accounting_state_source,
            shell_accounting_issue_header,
            shell_accounting_issue_source,
            shell_accounting_display_header,
            shell_accounting_display_source,
        ]
    )
    for forbidden in ["DataServiceClient", "DataServiceApi", "DataAccess", "AccountingEngine", "SQLite", "QtQuick"]:
        require(
            forbidden not in shell_accounting_controller_sources,
            f"ShellAccountingReadOnlyController skeleton does not include {forbidden}",
        )
    require("ShellAccountingReadOnlyController" not in qml_sources, "QML does not reference ShellAccountingReadOnlyController")
    require("position.list" not in qml_sources, "QML does not call accounting action names directly")
    require("cash.summary" not in qml_sources, "QML does not call cash accounting action names directly")
    require(
        "ShellAccountingReadOnlyController fake/client boundary tests" in readme,
        "README mentions ShellAccountingReadOnlyController fake client boundary tests",
    )
    require(
        "shell_accounting_fake_client_guard_payloads" in shell_accounting_fake_client_cmake,
        "Shell accounting fake client CMake registers guard payload test",
    )
    require(
        "shell_accounting_fake_client_state_transitions" in shell_accounting_fake_client_cmake,
        "Shell accounting fake client CMake registers state transitions test",
    )
    require(
        "shell_accounting_fake_client_issue_mapping" in shell_accounting_fake_client_cmake,
        "Shell accounting fake client CMake registers issue mapping test",
    )
    require(
        "shell_accounting_fake_client_privacy_and_no_trade" in shell_accounting_fake_client_cmake,
        "Shell accounting fake client CMake registers privacy and no trade test",
    )
    require(
        "shell_accounting_fake_client_no_real_dependency" in shell_accounting_fake_client_cmake,
        "Shell accounting fake client CMake registers no real dependency test",
    )
    require(
        "TASK-102" in shellservices_accounting_controller_contract
        and "fake/client boundary" in shellservices_accounting_controller_contract,
        "docs/49 mentions TASK-102 fake/client boundary",
    )
    require(
        "fake payload" in shell_accounting_viewmodel_state_contract
        or "fake/client" in shell_accounting_viewmodel_state_contract,
        "docs/50 mentions fake payload boundary",
    )
    require("TASK-102" in codex_prompt_template, "prompt template mentions TASK-102")
    require(
        "Fake client must remain test-only" in codex_prompt_template
        or "fake client must remain test-only" in codex_prompt_template,
        "prompt template keeps fake client test-only",
    )
    require(
        "Do not call real DataServiceClient unless explicitly authorized" in codex_prompt_template,
        "prompt template forbids real DataServiceClient without authorization",
    )
    require(
        "fake/client tests do not imply QML integration" in ui_design,
        "UI design states fake/client tests do not imply QML integration",
    )
    require(
        "add_subdirectory(ShellAccountingFakeClientBoundary)" in tests_cmake,
        "tests CMake adds Shell accounting fake client boundary tests",
    )
    require(
        "ShellAccountingFakeClientBoundary" not in root_cmake,
        "fake client helper is not added as production root target",
    )
    fake_accounting_client_helper_sources = "\n".join(
        [
            fake_accounting_client_payload_header,
            fake_accounting_client_payload_source,
            fake_accounting_client_scenario_header,
            fake_accounting_client_scenario_source,
        ]
    )
    for forbidden in ["DataServiceClient", "DataServiceApi", "DataAccess", "AccountingEngine", "SQLite"]:
        require(
            forbidden not in fake_accounting_client_helper_sources,
            f"fake accounting client helper does not include {forbidden}",
        )
    for forbidden in ["socket", "ipc"]:
        require(
            forbidden not in fake_accounting_client_helper_sources.lower(),
            f"fake accounting client helper does not mention {forbidden}",
        )
    require(
        "position.list" in shell_accounting_fake_guard_payload_test
        and "cash.summary" in shell_accounting_fake_guard_payload_test
        and "portfolio.pnl.summary" in shell_accounting_fake_guard_payload_test
        and "base_position.summary" in shell_accounting_fake_guard_payload_test
        and "sniper_pool.summary" in shell_accounting_fake_guard_payload_test,
        "fake guard payload test covers all accounting guard actions",
    )
    require(
        "Idle" in shell_accounting_fake_state_transition_test
        and "Loading" in shell_accounting_fake_state_transition_test,
        "fake state transition test covers Idle to Loading",
    )
    require(
        "MARKET_PRICE_MISSING" in shell_accounting_fake_issue_mapping_test
        and "NEGATIVE_CASH" in shell_accounting_fake_issue_mapping_test
        and "MULTI_CURRENCY_UNSUPPORTED" in shell_accounting_fake_issue_mapping_test,
        "fake issue mapping test covers accounting issues",
    )
    require(
        "privacyMode" in shell_accounting_fake_privacy_no_trade_test
        and (
            "TradeDraft" in shell_accounting_fake_privacy_no_trade_test
            or "tradeDraftGenerationEnabled" in shell_accounting_fake_privacy_no_trade_test
        ),
        "fake privacy and no trade test covers privacy and TradeDraft boundary",
    )
    require(
        "DataServiceClient" in shell_accounting_fake_no_real_dependency_test
        and "AccountingEngine" in shell_accounting_fake_no_real_dependency_test,
        "fake no real dependency test scans service and engine dependencies",
    )
    require(
        "51_shell_accounting_service_adapter_boundary" in readme,
        "README links Shell accounting service adapter boundary",
    )
    require(
        "52_shell_accounting_service_integration_readiness" in readme,
        "README links Shell accounting service integration readiness",
    )
    require(
        "51_shell_accounting_service_adapter_boundary" in docs_index,
        "docs index links Shell accounting service adapter boundary",
    )
    require(
        "52_shell_accounting_service_integration_readiness" in docs_index,
        "docs index links Shell accounting service integration readiness",
    )
    require(
        "ShellAccountingServiceAdapter" in shell_accounting_service_adapter_boundary,
        "docs/51 defines ShellAccountingServiceAdapter",
    )
    require(
        "controller must not directly hard-depend on `DataServiceClient`" in shell_accounting_service_adapter_boundary
        or "controller must not directly depend on `DataServiceClient`" in shell_accounting_service_adapter_boundary,
        "docs/51 says controller does not directly depend on DataServiceClient",
    )
    require(
        "Fake adapter" in shell_accounting_service_adapter_boundary
        or "Fake Adapter" in shell_accounting_service_adapter_boundary,
        "docs/51 defines fake adapter boundary",
    )
    require(
        "Real adapter" in shell_accounting_service_adapter_boundary
        or "Real Adapter" in shell_accounting_service_adapter_boundary,
        "docs/51 defines real adapter boundary",
    )
    require("protocol error" in shell_accounting_service_adapter_boundary, "docs/51 handles protocol error")
    require("domain issue" in shell_accounting_service_adapter_boundary, "docs/51 handles domain issue")
    require(
        "Go / No-Go Checklist" in shell_accounting_service_integration_readiness,
        "docs/52 defines Go / No-Go Checklist",
    )
    require(
        "User explicitly authorized real adapter implementation" in shell_accounting_service_integration_readiness,
        "docs/52 requires user authorization",
    )
    require(
        "51_shell_accounting_service_adapter_boundary.md" in shellservices_accounting_controller_contract,
        "docs/49 links docs/51",
    )
    require(
        "protocol error" in shell_accounting_viewmodel_state_contract,
        "docs/50 mentions protocol error",
    )
    require(
        "docs/51_shell_accounting_service_adapter_boundary.md" in codex_prompt_template,
        "prompt template links docs/51",
    )
    require(
        "docs/52_shell_accounting_service_integration_readiness.md" in codex_prompt_template,
        "prompt template links docs/52",
    )
    require(
        "ShellAccountingServiceAdapter" in codex_prompt_template,
        "prompt template prefers ShellAccountingServiceAdapter",
    )
    require(
        "ShellAccountingServiceAdapter" in position_list_real_boundary,
        "docs/45 mentions ShellAccountingServiceAdapter",
    )
    require(
        "ShellAccountingServiceAdapter interface skeleton" in readme,
        "README mentions ShellAccountingServiceAdapter interface skeleton",
    )
    require(
        "TASK-104" in shell_accounting_service_adapter_boundary
        and "interface skeleton" in shell_accounting_service_adapter_boundary,
        "docs/51 mentions TASK-104 interface skeleton",
    )
    require(
        "TASK-104" in shell_accounting_service_integration_readiness,
        "docs/52 mentions TASK-104",
    )
    require("TASK-104" in codex_prompt_template, "prompt template mentions TASK-104")
    require(
        "ShellAccountingServiceAdapter interface skeleton does not equal a" in codex_prompt_template,
        "prompt template states adapter skeleton is not real adapter",
    )
    require(
        "Do not implement ShellAccountingDataServiceAdapter without explicit" in codex_prompt_template,
        "prompt template forbids real adapter without authorization",
    )
    require(
        "ShellAccountingServiceAdapter" in shellservices_public_header,
        "ShellServices public header includes ShellAccountingServiceAdapter",
    )
    require(
        "ShellAccountingServiceAdapter" in shellservices_cmake,
        "ShellServices CMake includes ShellAccountingServiceAdapter",
    )
    require(
        "ShellAccountingServiceTypes" in shellservices_cmake,
        "ShellServices CMake includes ShellAccountingServiceTypes",
    )
    require(
        "class ShellAccountingServiceAdapter" in shell_accounting_service_adapter_header,
        "adapter header declares ShellAccountingServiceAdapter",
    )
    require("fetchPositionList" in shell_accounting_service_adapter_header, "adapter exposes fetchPositionList")
    require("fetchCashSummary" in shell_accounting_service_adapter_header, "adapter exposes fetchCashSummary")
    require("fetchPortfolioPnlSummary" in shell_accounting_service_adapter_header, "adapter exposes fetchPortfolioPnlSummary")
    require("fetchBasePositionSummary" in shell_accounting_service_adapter_header, "adapter exposes fetchBasePositionSummary")
    require("fetchSniperPoolSummary" in shell_accounting_service_adapter_header, "adapter exposes fetchSniperPoolSummary")
    require(
        "struct ShellAccountingServiceRequest" in shell_accounting_service_types_header,
        "service types header declares request",
    )
    require(
        "struct ShellAccountingServiceResult" in shell_accounting_service_types_header,
        "service types header declares result",
    )
    require("protocolSuccess" in shell_accounting_service_types_header, "result exposes protocolSuccess")
    require("domainError" in shell_accounting_service_types_header, "result exposes domainError")
    require("readOnly = true" in shell_accounting_service_types_header, "result defaults readOnly true")
    require("writeEnabled = false" in shell_accounting_service_types_header, "result defaults writeEnabled false")
    require("generatedTradeDraft = false" in shell_accounting_service_types_header, "result defaults no TradeDraft")
    require("generatedTradeSuggestion = false" in shell_accounting_service_types_header, "result defaults no trade suggestion")
    require("isShellAccountingServiceResultReadOnly" in shell_accounting_service_types_source, "service result source checks read-only")
    require(
        "setServiceAdapter" in shell_accounting_controller_header
        and "hasServiceAdapter" in shell_accounting_controller_header
        and "clearServiceAdapter" in shell_accounting_controller_header,
        "controller exposes adapter injection skeleton",
    )
    require(
        "serviceAdapter_" in shell_accounting_controller_header,
        "controller stores optional service adapter",
    )
    require("refreshPositionList" in shell_accounting_controller_header, "controller exposes refreshPositionList")
    require("refreshCashSummary" in shell_accounting_controller_header, "controller exposes refreshCashSummary")
    require(
        "refreshPortfolioPnlSummary" in shell_accounting_controller_header,
        "controller exposes refreshPortfolioPnlSummary",
    )
    require(
        "refreshBasePositionSummary" in shell_accounting_controller_header,
        "controller exposes refreshBasePositionSummary",
    )
    require(
        "refreshSniperPoolSummary" in shell_accounting_controller_header,
        "controller exposes refreshSniperPoolSummary",
    )
    require(
        "SERVICE_ADAPTER_NOT_CONFIGURED" in shell_accounting_controller_source,
        "controller handles missing adapter without real service calls",
    )
    require(
        "shell_accounting_service_adapter_interface" in shell_accounting_service_adapter_cmake,
        "adapter tests register interface test",
    )
    require(
        "shell_accounting_service_result_contract" in shell_accounting_service_adapter_cmake,
        "adapter tests register result contract test",
    )
    require(
        "shell_accounting_service_adapter_no_real_dependency" in shell_accounting_service_adapter_cmake,
        "adapter tests register no real dependency test",
    )
    require(
        "shell_accounting_controller_adapter_injection" in shell_accounting_service_adapter_cmake,
        "adapter tests register controller injection test",
    )
    require(
        "shell_accounting_controller_fake_adapter_refresh_success" in shell_accounting_service_adapter_cmake,
        "adapter tests register fake adapter refresh success test",
    )
    require(
        "shell_accounting_controller_fake_adapter_unavailable" in shell_accounting_service_adapter_cmake,
        "adapter tests register fake adapter unavailable test",
    )
    require(
        "shell_accounting_controller_fake_adapter_protocol_error" in shell_accounting_service_adapter_cmake,
        "adapter tests register fake adapter protocol error test",
    )
    require(
        "shell_accounting_controller_fake_adapter_issue_mapping" in shell_accounting_service_adapter_cmake,
        "adapter tests register fake adapter issue mapping test",
    )
    require(
        "shell_accounting_controller_fake_adapter_privacy_no_trade" in shell_accounting_service_adapter_cmake,
        "adapter tests register fake adapter privacy no trade test",
    )
    require(
        "shell_accounting_controller_fake_adapter_no_real_dependency" in shell_accounting_service_adapter_cmake,
        "adapter tests register fake adapter no real dependency test",
    )
    require(
        "add_subdirectory(ShellAccountingServiceAdapter)" in tests_cmake,
        "tests CMake adds Shell accounting service adapter tests",
    )
    require(
        "FakeShellAccountingServiceAdapter" in fake_shell_accounting_service_adapter_header
        and "public etfdt::shell_services::ShellAccountingServiceAdapter" in fake_shell_accounting_service_adapter_header,
        "fake adapter implements adapter interface",
    )
    require(
        "fetchPositionList" in shell_accounting_service_adapter_interface_test
        and "fetchCashSummary" in shell_accounting_service_adapter_interface_test
        and "fetchPortfolioPnlSummary" in shell_accounting_service_adapter_interface_test
        and "fetchBasePositionSummary" in shell_accounting_service_adapter_interface_test
        and "fetchSniperPoolSummary" in shell_accounting_service_adapter_interface_test,
        "interface test covers all fetch methods",
    )
    require(
        "protocolSuccess=false" in shell_accounting_service_result_contract_test
        or "protocolSuccess = false" in shell_accounting_service_result_contract_test,
        "result contract test covers protocol error",
    )
    require(
        "POSITION_LIST_NOT_AVAILABLE" in shell_accounting_service_result_contract_test,
        "result contract test covers guard status",
    )
    require(
        "DataServiceClient" in shell_accounting_service_adapter_no_dependency_test,
        "adapter no dependency test scans DataServiceClient",
    )
    require(
        "setting adapter does not trigger fetch call" in shell_accounting_controller_adapter_injection_test,
        "controller injection test verifies no automatic service call",
    )
    require(
        "refreshPositionList" in shell_accounting_fake_adapter_refresh_success_test
        and "refreshCashSummary" in shell_accounting_fake_adapter_refresh_success_test
        and "refreshPortfolioPnlSummary" in shell_accounting_fake_adapter_refresh_success_test
        and "refreshBasePositionSummary" in shell_accounting_fake_adapter_refresh_success_test
        and "refreshSniperPoolSummary" in shell_accounting_fake_adapter_refresh_success_test,
        "fake adapter refresh success test covers all action refresh methods",
    )
    require(
        "SERVICE_ADAPTER_NOT_CONFIGURED" in shell_accounting_fake_adapter_unavailable_test,
        "fake adapter unavailable test covers missing adapter",
    )
    require(
        "POSITION_LIST_NOT_AVAILABLE" in shell_accounting_fake_adapter_unavailable_test
        and "SNIPER_POOL_SUMMARY_NOT_AVAILABLE" in shell_accounting_fake_adapter_unavailable_test,
        "fake adapter unavailable test covers guard payloads",
    )
    require(
        "protocolSuccess = false" in shell_accounting_fake_adapter_protocol_error_test
        and "timeout" in shell_accounting_fake_adapter_protocol_error_test
        and "transportError" in shell_accounting_fake_adapter_protocol_error_test,
        "fake adapter protocol test covers protocol, timeout, and transport errors",
    )
    require(
        "MARKET_PRICE_MISSING" in shell_accounting_fake_adapter_issue_mapping_test
        and "MULTI_CURRENCY_UNSUPPORTED" in shell_accounting_fake_adapter_issue_mapping_test
        and "FX_RATE_MISSING" in shell_accounting_fake_adapter_issue_mapping_test,
        "fake adapter issue mapping test covers warning and blocking issues",
    )
    require(
        "privacyMode=true" in shell_accounting_fake_adapter_privacy_no_trade_test
        and "tradeDraftGenerationEnabled" in shell_accounting_fake_adapter_privacy_no_trade_test,
        "fake adapter privacy/no-trade test covers privacy and TradeDraft boundary",
    )
    require(
        "DataServiceClient" in shell_accounting_fake_adapter_no_dependency_test
        and "AccountingEngine" in shell_accounting_fake_adapter_no_dependency_test,
        "fake adapter no dependency test scans real service dependencies",
    )
    require("TASK-105" in shellservices_accounting_controller_contract, "docs/49 mentions TASK-105")
    require("fake adapter refresh" in shell_accounting_viewmodel_state_contract, "docs/50 mentions fake adapter refresh")
    require("TASK-105" in shell_accounting_service_adapter_boundary, "docs/51 mentions TASK-105")
    require("TASK-105" in shell_accounting_service_integration_readiness, "docs/52 mentions TASK-105")
    require("TASK-105" in codex_prompt_template, "prompt template mentions TASK-105")
    require("fake adapter injection behavior" in readme, "README mentions fake adapter injection behavior")
    require(
        "53_shell_accounting_dataservice_adapter_boundary" in readme,
        "README links Shell accounting DataService adapter boundary",
    )
    require(
        "54_shell_accounting_dataservice_adapter_test_plan" in readme,
        "README links Shell accounting DataService adapter test plan",
    )
    require(
        "53_shell_accounting_dataservice_adapter_boundary" in docs_index,
        "docs index links Shell accounting DataService adapter boundary",
    )
    require(
        "54_shell_accounting_dataservice_adapter_test_plan" in docs_index,
        "docs index links Shell accounting DataService adapter test plan",
    )
    require(
        "ShellAccountingDataServiceAdapter" in shell_accounting_dataservice_adapter_boundary,
        "docs/53 defines ShellAccountingDataServiceAdapter",
    )
    require(
        "DataServiceClient guard wrappers" in shell_accounting_dataservice_adapter_boundary
        or "DataServiceClient wrappers" in shell_accounting_dataservice_adapter_boundary,
        "docs/53 references DataServiceClient wrappers",
    )
    require("protocol error" in shell_accounting_dataservice_adapter_boundary, "docs/53 covers protocol error")
    require("domain issue" in shell_accounting_dataservice_adapter_boundary, "docs/53 covers domain issue")
    require("TradeDraft" in shell_accounting_dataservice_adapter_boundary, "docs/53 forbids TradeDraft")
    require(
        "trade suggestions" in shell_accounting_dataservice_adapter_boundary
        or "trade suggestion" in shell_accounting_dataservice_adapter_boundary,
        "docs/53 forbids trade suggestion",
    )
    require("Test Matrix" in shell_accounting_dataservice_adapter_test_plan, "docs/54 defines Test matrix")
    require(
        "Read-only Action Allowlist" in shell_accounting_dataservice_adapter_test_plan,
        "docs/54 defines read-only action allowlist",
    )
    require(
        "data.audit.append" in shell_accounting_dataservice_adapter_test_plan,
        "docs/54 forbids data.audit.append",
    )
    require(
        "53_shell_accounting_dataservice_adapter_boundary.md" in shell_accounting_service_adapter_boundary,
        "docs/51 links docs/53",
    )
    require(
        "54_shell_accounting_dataservice_adapter_test_plan.md" in shell_accounting_service_integration_readiness,
        "docs/52 links docs/54",
    )
    require(
        "docs/53_shell_accounting_dataservice_adapter_boundary.md" in codex_prompt_template,
        "prompt template links docs/53",
    )
    require(
        "docs/54_shell_accounting_dataservice_adapter_test_plan.md" in codex_prompt_template,
        "prompt template links docs/54",
    )
    for test_name in [
        "shell_accounting_dataservice_adapter_spy_allowlist",
        "shell_accounting_dataservice_adapter_spy_method_mapping",
        "shell_accounting_dataservice_adapter_spy_request_mapping",
        "shell_accounting_dataservice_adapter_spy_response_mapping",
        "shell_accounting_dataservice_adapter_spy_error_mapping",
        "shell_accounting_dataservice_adapter_spy_no_write_no_trade",
    ]:
        require(
            test_name in shell_accounting_dataservice_adapter_scaffolding_cmake,
            f"DataService adapter spy scaffolding registers {test_name}",
        )
    require(
        "shell_accounting_dataservice_adapter_spy_no_real_dependency"
        in shell_accounting_dataservice_adapter_scaffolding_cmake,
        "DataService adapter spy scaffolding registers no real dependency test",
    )
    require(
        "add_subdirectory(ShellAccountingDataServiceAdapterScaffolding)" in tests_cmake,
        "tests CMake adds Shell accounting DataService adapter scaffolding",
    )
    require(
        "TASK-107" in shell_accounting_dataservice_adapter_boundary
        or "spy/wrapper scaffolding" in shell_accounting_dataservice_adapter_boundary,
        "docs/53 mentions TASK-107 spy/wrapper scaffolding",
    )
    require(
        "spy wrapper" in shell_accounting_dataservice_adapter_test_plan,
        "docs/54 mentions spy wrapper",
    )
    require("TASK-107" in codex_prompt_template, "prompt template mentions TASK-107")
    require("TASK-108" in codex_prompt_template, "prompt template mentions TASK-108")
    require(
        "Real adapter may only call read-only accounting wrappers" in codex_prompt_template,
        "prompt template restricts real adapter to read-only accounting wrappers",
    )
    require(
        "ShellAccountingDataServiceAdapter skeleton is not real" in codex_prompt_template,
        "prompt template states skeleton is not real DataServiceClient integration",
    )
    require(
        "ShellAccountingDataServiceAdapter production skeleton" in readme,
        "README mentions ShellAccountingDataServiceAdapter production skeleton",
    )
    require(
        "ShellAccountingDataServiceAdapter production skeleton" in docs_index,
        "docs index mentions ShellAccountingDataServiceAdapter production skeleton",
    )
    require(
        "55_shell_accounting_dataservice_adapter_live_call_gate" in readme,
        "README links ShellAccountingDataServiceAdapter live-call gate",
    )
    require(
        "56_shell_accounting_dataservice_adapter_live_call_acceptance_checklist" in readme,
        "README links ShellAccountingDataServiceAdapter live-call acceptance checklist",
    )
    require(
        "55_shell_accounting_dataservice_adapter_live_call_gate" in docs_index,
        "docs index links ShellAccountingDataServiceAdapter live-call gate",
    )
    require(
        "56_shell_accounting_dataservice_adapter_live_call_acceptance_checklist" in docs_index,
        "docs index links ShellAccountingDataServiceAdapter live-call acceptance checklist",
    )
    require(
        "TASK-108" in shell_accounting_dataservice_adapter_boundary
        or "Production Skeleton Boundary" in shell_accounting_dataservice_adapter_boundary,
        "docs/53 mentions TASK-108 production skeleton",
    )
    require(
        "TASK-108" in shell_accounting_dataservice_adapter_test_plan,
        "docs/54 mentions TASK-108 skeleton tests",
    )
    require(
        "Read-only Allowlist" in shell_accounting_dataservice_adapter_live_call_gate
        or "read-only allowlist" in shell_accounting_dataservice_adapter_live_call_gate,
        "docs/55 defines read-only allowlist",
    )
    require(
        "Forbidden Call Denylist" in shell_accounting_dataservice_adapter_live_call_gate,
        "docs/55 defines forbidden call denylist",
    )
    require("rollback" in shell_accounting_dataservice_adapter_live_call_gate, "docs/55 defines rollback")
    require(
        "Go / No-Go Checklist" in shell_accounting_dataservice_adapter_live_call_checklist,
        "docs/56 defines Go / No-Go Checklist",
    )
    require(
        "用户明确授权" in shell_accounting_dataservice_adapter_live_call_checklist,
        "docs/56 requires explicit user authorization",
    )
    require(
        "TASK-109" in codex_prompt_template,
        "prompt template mentions TASK-109",
    )
    require(
        "TASK-109 live-call gate is not live implementation" in codex_prompt_template,
        "prompt template states live-call gate is not live implementation",
    )
    require(
        "docs/55_shell_accounting_dataservice_adapter_live_call_gate.md" in codex_prompt_template,
        "prompt template links docs/55",
    )
    require(
        "docs/56_shell_accounting_dataservice_adapter_live_call_acceptance_checklist.md" in codex_prompt_template,
        "prompt template links docs/56",
    )
    for test_name in [
        "shell_accounting_dataservice_adapter_live_call_allowlist_gate",
        "shell_accounting_dataservice_adapter_live_call_mapping_gate",
        "shell_accounting_dataservice_adapter_live_call_error_gate",
        "shell_accounting_dataservice_adapter_live_call_no_write_gate",
        "shell_accounting_dataservice_adapter_live_call_readiness_gate",
    ]:
        require(
            test_name in shell_accounting_dataservice_adapter_live_call_gate_cmake,
            f"DataService adapter live-call gate registers {test_name}",
        )
    require(
        "add_subdirectory(ShellAccountingDataServiceAdapterLiveCallGate)" in tests_cmake,
        "tests CMake adds Shell accounting DataService adapter live-call gate",
    )
    require(
        "docs/55_shell_accounting_dataservice_adapter_live_call_gate.md" in shell_accounting_dataservice_adapter_boundary,
        "docs/53 links docs/55",
    )
    require(
        "docs/56_shell_accounting_dataservice_adapter_live_call_acceptance_checklist.md" in shell_accounting_service_integration_readiness,
        "docs/52 links docs/56",
    )
    require(
        "SpyAccountingDataServiceClient" in spy_accounting_dataservice_client_header,
        "spy wrapper helper exists in tests",
    )
    require(
        "ShellAccountingDataServiceAdapter" in shellservices_cmake,
        "ShellServices CMake adds ShellAccountingDataServiceAdapter production skeleton",
    )
    require(
        "ShellAccountingDataServiceAdapter final" in shell_accounting_dataservice_adapter_header,
        "ShellAccountingDataServiceAdapter production skeleton class exists",
    )
    require(
        "SHELL_ACCOUNTING_DATASERVICE_ADAPTER_NOT_CONNECTED" in shell_accounting_dataservice_adapter_source,
        "ShellAccountingDataServiceAdapter skeleton returns not connected status",
    )
    for test_name in [
        "shell_accounting_dataservice_adapter_skeleton_interface",
        "shell_accounting_dataservice_adapter_skeleton_not_connected",
        "shell_accounting_dataservice_adapter_skeleton_no_live_calls",
        "shell_accounting_dataservice_adapter_skeleton_no_write_no_trade",
        "shell_accounting_dataservice_adapter_skeleton_controller_integration",
    ]:
        require(
            test_name in shell_accounting_dataservice_adapter_skeleton_cmake,
            f"DataService adapter skeleton registers {test_name}",
        )
    require(
        "add_subdirectory(ShellAccountingDataServiceAdapter)" in tests_cmake,
        "tests CMake adds Shell accounting DataService adapter skeleton tests",
    )
    spy_scaffolding_sources = "\n".join(
        [
            spy_accounting_dataservice_client_header,
            spy_accounting_dataservice_client_source,
            shell_accounting_dataservice_expected_call_header,
            shell_accounting_dataservice_expected_call_source,
        ]
    )
    for forbidden in ["DataServiceClient", "DataServiceApi", "DataAccess", "AccountingEngine", "SQLite", "QtQuick", "QML"]:
        require(
            f'#include "{forbidden}' not in spy_scaffolding_sources
            and f"#include <{forbidden}" not in spy_scaffolding_sources,
            f"spy scaffolding does not include {forbidden}",
        )
    require(
        "ShellAccountingDataServiceAdapter" not in qml_sources,
        "QML does not reference ShellAccountingDataServiceAdapter",
    )
    adapter_skeleton_sources = "\n".join(
        [
            shell_accounting_dataservice_adapter_header,
            shell_accounting_dataservice_adapter_source,
        ]
    )
    for forbidden in ["DataServiceClient", "DataServiceApi", "DataAccess", "AccountingEngine", "SQLite", "QtQuick", "QML"]:
        require(
            f'#include "{forbidden}' not in adapter_skeleton_sources
            and f"#include <{forbidden}" not in adapter_skeleton_sources,
            f"ShellAccountingDataServiceAdapter skeleton does not include {forbidden}",
        )
    for forbidden_call in [
        "DataServiceClient::positionList",
        "DataServiceClient::cashSummary",
        "DataServiceClient::portfolioPnlSummary",
        "DataServiceClient::basePositionSummary",
        "DataServiceClient::sniperPoolSummary",
        ".positionList(",
        ".cashSummary(",
        ".portfolioPnlSummary(",
        ".basePositionSummary(",
        ".sniperPoolSummary(",
        "->positionList(",
        "->cashSummary(",
        "->portfolioPnlSummary(",
        "->basePositionSummary(",
        "->sniperPoolSummary(",
    ]:
        require(
            forbidden_call not in adapter_skeleton_sources,
            f"ShellAccountingDataServiceAdapter skeleton does not call {forbidden_call}",
        )
    controller_adapter_sources = "\n".join([shell_accounting_controller_header, shell_accounting_controller_source])
    for forbidden in ["DataServiceClient", "DataServiceApi", "DataAccess", "AccountingEngine", "SQLite"]:
        require(
            forbidden not in controller_adapter_sources,
            f"ShellAccountingReadOnlyController fake adapter path does not include {forbidden}",
        )
    adapter_production_sources = "\n".join(
        [
            shell_accounting_service_adapter_header,
            shell_accounting_service_types_header,
            shell_accounting_service_types_source,
        ]
    )
    for forbidden in ["DataServiceClient", "DataServiceApi", "DataAccess", "AccountingEngine", "SQLite", "QtQuick", "QML"]:
        require(
            forbidden not in adapter_production_sources,
            f"ShellAccountingServiceAdapter interface skeleton does not include {forbidden}",
        )
    fake_adapter_sources = "\n".join(
        [
            fake_shell_accounting_service_adapter_header,
            fake_shell_accounting_service_adapter_source,
        ]
    )
    for forbidden in ["DataServiceClient", "SQLite"]:
        require(
            forbidden not in fake_adapter_sources,
            f"fake Shell accounting service adapter does not include {forbidden}",
        )
    require(
        "ShellAccountingServiceAdapter" not in qml_sources,
        "QML does not reference ShellAccountingServiceAdapter",
    )
    require(
        "ShellAccountingDataServiceAdapter" not in qml_sources,
        "QML does not reference ShellAccountingDataServiceAdapter",
    )

    require(
        "ShellAccountingDataServiceClientPort" in shell_accounting_dataservice_client_port_header,
        "ShellAccountingDataServiceClientPort interface exists",
    )
    require(
        "ShellAccountingDataServiceClientRequest" in shell_accounting_dataservice_client_port_header,
        "ShellAccountingDataServiceClientRequest exists",
    )
    require(
        "ShellAccountingDataServiceClientResponse" in shell_accounting_dataservice_client_port_header,
        "ShellAccountingDataServiceClientResponse exists",
    )
    require(
        "TASK-110" in codex_prompt_template,
        "prompt template mentions TASK-110",
    )
    require(
        "live-call skeleton" in readme,
        "README mentions live-call skeleton",
    )
    for doc_text, doc_name in [
        (shell_accounting_dataservice_adapter_boundary, "docs/53"),
        (shell_accounting_dataservice_adapter_test_plan, "docs/54"),
        (shell_accounting_dataservice_adapter_live_call_gate, "docs/55"),
        (shell_accounting_dataservice_adapter_live_call_checklist, "docs/56"),
    ]:
        require(
            "TASK-110" in doc_text or "live-call skeleton" in doc_text,
            f"{doc_name} mentions TASK-110 live-call skeleton",
        )
    for test_name in [
        "shell_accounting_dataservice_adapter_live_call_skeleton_default_not_connected",
        "shell_accounting_dataservice_adapter_live_call_skeleton_method_mapping",
        "shell_accounting_dataservice_adapter_live_call_skeleton_request_mapping",
        "shell_accounting_dataservice_adapter_live_call_skeleton_response_mapping",
        "shell_accounting_dataservice_adapter_live_call_skeleton_error_mapping",
        "shell_accounting_dataservice_adapter_live_call_skeleton_no_write_no_trade",
        "shell_accounting_dataservice_adapter_live_call_skeleton_no_real_dependency",
        "shell_accounting_controller_with_dataservice_adapter_spy_port",
    ]:
        require(
            test_name in shell_accounting_dataservice_adapter_skeleton_cmake,
            f"DataService adapter live-call skeleton registers {test_name}",
        )
    require(
        "ShellAccountingDataServiceClientPort" in shellservices_cmake,
        "ShellServices CMake adds ShellAccountingDataServiceClientPort",
    )
    require(
        "callPositionList" in shell_accounting_dataservice_adapter_source,
        "adapter calls abstract port position method",
    )
    require(
        "callCashSummary" in shell_accounting_dataservice_adapter_source,
        "adapter calls abstract port cash method",
    )
    require(
        "callPortfolioPnlSummary" in shell_accounting_dataservice_adapter_source,
        "adapter calls abstract port portfolio pnl method",
    )
    require(
        "callBasePositionSummary" in shell_accounting_dataservice_adapter_source,
        "adapter calls abstract port base position method",
    )
    require(
        "callSniperPoolSummary" in shell_accounting_dataservice_adapter_source,
        "adapter calls abstract port sniper pool method",
    )
    live_call_skeleton_sources = "\n".join(
        [
            shell_accounting_dataservice_adapter_header,
            shell_accounting_dataservice_adapter_source,
            shell_accounting_dataservice_client_port_header,
            spy_shell_accounting_dataservice_client_port_header,
            spy_shell_accounting_dataservice_client_port_source,
        ]
    )
    for forbidden in ["DataServiceClient", "DataServiceApi", "DataAccess", "AccountingEngine", "SQLite", "QtQuick", "QML"]:
        require(
            f'#include "{forbidden}' not in live_call_skeleton_sources
            and f"#include <{forbidden}" not in live_call_skeleton_sources,
            f"TASK-110 live-call skeleton does not include {forbidden}",
        )
    for forbidden_call in [
        "DataServiceClient::positionList",
        "DataServiceClient::cashSummary",
        "DataServiceClient::portfolioPnlSummary",
        "DataServiceClient::basePositionSummary",
        "DataServiceClient::sniperPoolSummary",
        ".positionList(",
        ".cashSummary(",
        ".portfolioPnlSummary(",
        ".basePositionSummary(",
        ".sniperPoolSummary(",
        "->positionList(",
        "->cashSummary(",
        "->portfolioPnlSummary(",
        "->basePositionSummary(",
        "->sniperPoolSummary(",
    ]:
        require(
            forbidden_call not in live_call_skeleton_sources,
            f"TASK-110 live-call skeleton does not call {forbidden_call}",
        )

    require(
        "TASK-111" in codex_prompt_template,
        "prompt template mentions TASK-111",
    )
    require(
        "concrete port for guard wrappers" in readme,
        "README mentions concrete port for guard wrappers",
    )
    for doc_text, doc_name in [
        (shell_accounting_dataservice_adapter_boundary, "docs/53"),
        (shell_accounting_dataservice_adapter_test_plan, "docs/54"),
        (shell_accounting_dataservice_adapter_live_call_gate, "docs/55"),
        (shell_accounting_dataservice_adapter_live_call_checklist, "docs/56"),
    ]:
        require("TASK-111" in doc_text, f"{doc_name} mentions TASK-111")
    for test_name in [
        "shell_accounting_dataservice_client_port_adapter_construction",
        "shell_accounting_dataservice_client_port_adapter_method_mapping",
        "shell_accounting_dataservice_client_port_adapter_request_mapping",
        "shell_accounting_dataservice_client_port_adapter_response_mapping",
        "shell_accounting_dataservice_client_port_adapter_error_mapping",
        "shell_accounting_dataservice_client_port_adapter_no_write_no_trade",
        "shell_accounting_dataservice_client_port_adapter_no_forbidden_dependency",
        "shell_accounting_dataservice_adapter_with_client_port_adapter_guard_payload",
    ]:
        require(
            test_name in shell_accounting_dataservice_adapter_skeleton_cmake,
            f"DataService client concrete port registers {test_name}",
        )
    require(
        "ShellAccountingDataServiceClientPortAdapter" in shellservices_cmake,
        "ShellServices CMake adds ShellAccountingDataServiceClientPortAdapter",
    )
    require(
        "ShellAccountingDataServiceClientPortAdapter" in shellservices_public_header,
        "ShellServices umbrella header exports ShellAccountingDataServiceClientPortAdapter",
    )
    require(
        "DataServiceClient/DataServiceClient.h" in shell_accounting_dataservice_client_port_adapter_source,
        "concrete port source includes DataServiceClient",
    )
    require(
        "DataServiceClient/DataServiceClient.h" not in shell_accounting_dataservice_client_port_adapter_header,
        "concrete port header does not include DataServiceClient",
    )
    for allowed_call in [
        "client_->positionList(",
        "client_->cashSummary(",
        "client_->portfolioPnlSummary(",
        "client_->basePositionSummary(",
        "client_->sniperPoolSummary(",
    ]:
        require(
            allowed_call in shell_accounting_dataservice_client_port_adapter_source,
            f"concrete port calls allowlisted wrapper {allowed_call}",
        )
    for forbidden_include in ["DataAccess", "AccountingEngine", "SQLite", "QtQuick"]:
        require(
            f'#include "{forbidden_include}' not in shell_accounting_dataservice_client_port_adapter_source
            and f"#include <{forbidden_include}" not in shell_accounting_dataservice_client_port_adapter_source
            and f'#include "{forbidden_include}' not in shell_accounting_dataservice_client_port_adapter_header
            and f"#include <{forbidden_include}" not in shell_accounting_dataservice_client_port_adapter_header,
            f"concrete port avoids forbidden include {forbidden_include}",
        )
    for forbidden_call in [
        "appendAuditDemo",
        "data.audit.append",
        "tradeDraft",
        "snapshotWrite",
        "strategyExecute",
        "brokerOrder",
    ]:
        require(
            forbidden_call not in shell_accounting_dataservice_client_port_adapter_source,
            f"concrete port avoids forbidden call {forbidden_call}",
        )
    require(
        "DataServiceClient/DataServiceClient.h" not in shell_accounting_dataservice_adapter_header
        and "DataServiceClient/DataServiceClient.h" not in shell_accounting_dataservice_adapter_source,
        "ShellAccountingDataServiceAdapter still does not include DataServiceClient",
    )
    require(
        "DataServiceClient/DataServiceClient.h" not in shell_accounting_controller_header
        and "DataServiceClient/DataServiceClient.h" not in shell_accounting_controller_source,
        "ShellAccountingReadOnlyController still does not include DataServiceClient",
    )
    require(
        "ShellAccountingDataServiceClientPortAdapter" not in qml_sources,
        "QML does not reference ShellAccountingDataServiceClientPortAdapter",
    )
    require(
        "TASK-112" in codex_prompt_template,
        "prompt template mentions TASK-112",
    )
    require(
        "concrete port integration hardening" in readme,
        "README mentions concrete port integration hardening",
    )
    for doc_text, doc_name in [
        (shellservices_accounting_controller_contract, "docs/49"),
        (shell_accounting_viewmodel_state_contract, "docs/50"),
        (shell_accounting_service_adapter_boundary, "docs/51"),
        (shell_accounting_service_integration_readiness, "docs/52"),
        (shell_accounting_dataservice_adapter_boundary, "docs/53"),
        (shell_accounting_dataservice_adapter_test_plan, "docs/54"),
        (shell_accounting_dataservice_adapter_live_call_gate, "docs/55"),
        (shell_accounting_dataservice_adapter_live_call_checklist, "docs/56"),
    ]:
        require("TASK-112" in doc_text, f"{doc_name} mentions TASK-112")
    for test_name in [
        "shell_accounting_controller_concrete_port_position_list_guard",
        "shell_accounting_controller_concrete_port_cash_summary_guard",
        "shell_accounting_controller_concrete_port_portfolio_pnl_summary_guard",
        "shell_accounting_controller_concrete_port_base_position_summary_guard",
        "shell_accounting_controller_concrete_port_sniper_pool_summary_guard",
        "shell_accounting_controller_concrete_port_no_write_no_trade",
        "shell_accounting_controller_concrete_port_no_qml_or_forbidden_dependency",
    ]:
        require(
            test_name in shell_accounting_dataservice_adapter_skeleton_cmake,
            f"controller concrete port integration registers {test_name}",
        )
    require(
        "DataServiceClient/DataServiceClient.h" not in shell_accounting_controller_header
        and "DataServiceClient/DataServiceClient.h" not in shell_accounting_controller_source,
        "TASK-112 controller still avoids DataServiceClient include",
    )
    require(
        "DataServiceClient/DataServiceClient.h" not in shell_accounting_dataservice_adapter_header
        and "DataServiceClient/DataServiceClient.h" not in shell_accounting_dataservice_adapter_source,
        "TASK-112 adapter body still avoids DataServiceClient include",
    )
    for accounting_action in [
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
        "base_position.summary",
        "sniper_pool.summary",
    ]:
        require(accounting_action not in qml_sources, f"QML does not call {accounting_action}")

    require("TASK-113" in codex_prompt_template, "prompt template mentions TASK-113")
    require(
        "ShellAccounting ViewModel / Model read-only boundary" in readme,
        "README mentions ShellAccounting ViewModel / Model read-only boundary",
    )
    for doc_text, doc_name in [
        (shellservices_accounting_controller_contract, "docs/49"),
        (shell_accounting_viewmodel_state_contract, "docs/50"),
        (ui_design, "docs/07"),
        (codex_prompt_template, "docs/12"),
    ]:
        require("TASK-113" in doc_text, f"{doc_name} mentions TASK-113")
    require("ShellAccountingStatusObject" in shellservices_cmake, "ShellServices CMake includes ShellAccountingStatusObject")
    require("ShellAccountingIssueListModel" in shellservices_cmake, "ShellServices CMake includes ShellAccountingIssueListModel")
    require("ShellPositionListModel" in shellservices_cmake, "ShellServices CMake includes ShellPositionListModel")
    require("ShellAccountingStatusObject" in shellservices_public_header, "ShellServices umbrella header exports ShellAccountingStatusObject")
    require("ShellAccountingIssueListModel" in shellservices_public_header, "ShellServices umbrella header exports ShellAccountingIssueListModel")
    require("ShellPositionListModel" in shellservices_public_header, "ShellServices umbrella header exports ShellPositionListModel")
    require("add_subdirectory(ShellAccountingViewModel)" in tests_cmake, "tests CMake adds ShellAccountingViewModel")
    for test_name in [
        "shell_accounting_status_object_readonly_boundary",
        "shell_accounting_issue_list_model_readonly_boundary",
        "shell_position_list_model_readonly_boundary",
        "shell_accounting_viewmodel_privacy_boundary",
        "shell_accounting_viewmodel_no_trade_action_boundary",
        "shell_accounting_viewmodel_no_service_dependency",
    ]:
        require(test_name in shell_accounting_viewmodel_cmake, f"ViewModel CMake registers {test_name}")
    viewmodel_sources = "\n".join(
        [
            shell_accounting_status_object_header,
            shell_accounting_status_object_source,
            shell_accounting_issue_list_model_header,
            shell_accounting_issue_list_model_source,
            shell_position_list_model_header,
            shell_position_list_model_source,
        ]
    )
    for forbidden_dependency in [
        "DataServiceClient",
        "DataServiceApi",
        "DataAccess",
        "AccountingEngine",
        "SQLite",
        "QtQuick",
    ]:
        require(forbidden_dependency not in viewmodel_sources, f"ViewModel / Model avoids {forbidden_dependency}")
    for forbidden_interface in [
        "createTradeDraft",
        "brokerOrder",
        "strategyExecute",
        "buyAction",
        "sellAction",
    ]:
        require(forbidden_interface not in viewmodel_sources, f"ViewModel / Model avoids {forbidden_interface}")
    require("ShellAccountingStatusObject" not in qml_sources, "QML does not bind ShellAccountingStatusObject")
    require("ShellAccountingIssueListModel" not in qml_sources, "QML does not bind ShellAccountingIssueListModel")
    require("ShellPositionListModel" not in qml_sources, "QML does not bind ShellPositionListModel")

    require("57_shell_accounting_presenter_contract" in readme, "README links docs/57")
    require("58_shell_accounting_qml_binding_readiness_plan" in readme, "README links docs/58")
    require("57_shell_accounting_presenter_contract" in docs_index, "docs README links docs/57")
    require("58_shell_accounting_qml_binding_readiness_plan" in docs_index, "docs README links docs/58")
    require("ShellAccountingPresenter" in shell_accounting_presenter_contract, "docs/57 names ShellAccountingPresenter")
    require(
        "QML -> DataServiceClient" in shell_accounting_presenter_contract
        or "QML must not call DataServiceClient" in shell_accounting_presenter_contract
        or "QML 不得直接调用 DataServiceClient" in shell_accounting_presenter_contract,
        "docs/57 forbids direct QML DataServiceClient calls",
    )
    require("TradeDraft" in shell_accounting_presenter_contract, "docs/57 mentions TradeDraft boundary")
    require(
        "trade suggestion" in shell_accounting_presenter_contract
        or "交易建议" in shell_accounting_presenter_contract,
        "docs/57 mentions trade suggestion boundary",
    )
    require(
        "Empty is not Unavailable" in shell_accounting_qml_binding_readiness
        or "Empty 不等于 Unavailable" in shell_accounting_qml_binding_readiness,
        "docs/58 distinguishes Empty and Unavailable",
    )
    require("no trading buttons" in shell_accounting_qml_binding_readiness, "docs/58 forbids trading buttons")
    require("用户明确授权 QML 接入" in shell_accounting_qml_binding_readiness, "docs/58 requires user authorization for QML integration")
    require(
        "58_shell_accounting_qml_binding_readiness_plan" in ui_design
        or "QML binding readiness" in ui_design,
        "docs/07 references QML binding readiness",
    )
    require("57_shell_accounting_presenter_contract" in codex_prompt_template, "docs/12 references docs/57")
    require("58_shell_accounting_qml_binding_readiness_plan" in codex_prompt_template, "docs/12 references docs/58")
    require("57_shell_accounting_presenter_contract" in shellservices_accounting_controller_contract, "docs/49 references docs/57")
    require("58_shell_accounting_qml_binding_readiness_plan" in shell_accounting_viewmodel_state_contract, "docs/50 references docs/58")
    for qml_binding in [
        "ShellAccountingStatusObject",
        "ShellAccountingIssueListModel",
        "ShellPositionListModel",
        "AccountingStatusBanner",
        "PositionListReadOnlyTable",
    ]:
        require(qml_binding not in qml_sources, f"QML has not added accounting binding {qml_binding}")
    require(
        qml_sources.count("ShellAccountingPresenter") == 1,
        "QML only has the authorized ShellAccountingPresenter property after TASK-131",
    )

    require("ShellAccountingPresenter skeleton" in readme, "README mentions ShellAccountingPresenter skeleton")
    require("TASK-115" in shell_accounting_presenter_contract, "docs/57 mentions TASK-115")
    require("TASK-115" in shell_accounting_qml_binding_readiness, "docs/58 mentions TASK-115")
    require("TASK-115" in shellservices_accounting_controller_contract, "docs/49 mentions TASK-115")
    require("TASK-115" in shell_accounting_viewmodel_state_contract, "docs/50 mentions TASK-115")
    require("TASK-115" in codex_prompt_template, "docs/12 mentions TASK-115")
    require("ShellAccountingPresenter" in shellservices_cmake, "ShellServices CMake includes ShellAccountingPresenter")
    require("ShellAccountingPresenter" in shellservices_public_header, "ShellServices umbrella header exports ShellAccountingPresenter")
    require("add_subdirectory(ShellAccountingPresenter)" in tests_cmake, "tests CMake adds ShellAccountingPresenter")
    for test_name in [
        "shell_accounting_presenter_skeleton_construction",
        "shell_accounting_presenter_skeleton_status_issue_models",
        "shell_accounting_presenter_skeleton_refresh_boundary",
        "shell_accounting_presenter_skeleton_privacy_boundary",
        "shell_accounting_presenter_skeleton_no_trade_action",
        "shell_accounting_presenter_skeleton_no_service_dependency",
    ]:
        require(test_name in shell_accounting_presenter_cmake, f"Presenter CMake registers {test_name}")
    presenter_sources = "\n".join([shell_accounting_presenter_header, shell_accounting_presenter_source])
    for forbidden_dependency in [
        "DataServiceClient",
        "DataServiceApi",
        "DataAccess",
        "AccountingEngine",
        "SQLite",
        "QtQuick",
    ]:
        require(forbidden_dependency not in presenter_sources, f"Presenter avoids {forbidden_dependency}")
    for forbidden_interface in [
        "createTradeDraft",
        "brokerOrder",
        "strategyExecute",
    ]:
        require(forbidden_interface not in presenter_sources, f"Presenter avoids {forbidden_interface}")
    require("ShellAccountingDataServiceClientPortAdapter" not in presenter_sources, "Presenter avoids concrete port adapter")
    require("ShellAccountingDataServiceAdapter" not in presenter_sources, "Presenter avoids data service adapter")
    require("qmlRegisterType" not in presenter_sources, "Presenter does not register QML type")
    require(
        qml_sources.count("ShellAccountingPresenter") == 1,
        "QML only has the authorized ShellAccountingPresenter property after TASK-131",
    )

    require(
        "presenter concrete-port integration" in readme
        or "ShellAccountingPresenter concrete port integration" in readme,
        "README mentions presenter concrete port integration",
    )
    for test_name in [
        "shell_accounting_presenter_concrete_port_position_list_guard",
        "shell_accounting_presenter_concrete_port_refresh_boundary",
        "shell_accounting_presenter_concrete_port_no_write_no_trade",
        "shell_accounting_presenter_concrete_port_no_qml_or_forbidden_dependency",
    ]:
        require(test_name in shell_accounting_presenter_cmake, f"Presenter CMake registers {test_name}")
    for doc_text, doc_name in [
        (shell_accounting_presenter_contract, "docs/57"),
        (shell_accounting_qml_binding_readiness, "docs/58"),
        (shellservices_accounting_controller_contract, "docs/49"),
        (shell_accounting_viewmodel_state_contract, "docs/50"),
        (shell_accounting_dataservice_adapter_boundary, "docs/53"),
        (shell_accounting_dataservice_adapter_test_plan, "docs/54"),
        (shell_accounting_dataservice_adapter_live_call_gate, "docs/55"),
        (shell_accounting_dataservice_adapter_live_call_checklist, "docs/56"),
        (dataservice_readonly_accounting_contracts, "docs/37"),
        (dataservice_accounting_no_write_plan, "docs/38"),
        (codex_prompt_template, "docs/12"),
    ]:
        require("TASK-116" in doc_text, f"{doc_name} mentions TASK-116")
    require(
        "DataServiceClient/DataServiceClient.h" not in presenter_sources,
        "Presenter still does not include DataServiceClient header",
    )
    require(
        "DataServiceClient/DataServiceClient.h" not in shell_accounting_controller_header
        and "DataServiceClient/DataServiceClient.h" not in shell_accounting_controller_source,
        "Controller still avoids direct DataServiceClient include",
    )
    require(
        "DataServiceClient/DataServiceClient.h" not in shell_accounting_dataservice_adapter_header
        and "DataServiceClient/DataServiceClient.h" not in shell_accounting_dataservice_adapter_source,
        "Adapter body still avoids direct DataServiceClient include",
    )
    require(
        "DataServiceClient/DataServiceClient.h" in shell_accounting_dataservice_client_port_adapter_header
        or "DataServiceClient/DataServiceClient.h" in shell_accounting_dataservice_client_port_adapter_source,
        "only concrete port adapter may include DataServiceClient",
    )
    require(
        qml_sources.count("ShellAccountingPresenter") == 1,
        "QML only has the authorized ShellAccountingPresenter property after TASK-131",
    )

    require(
        "presenter all guard actions refresh" in readme
        or "ShellAccountingPresenter all guard actions refresh" in readme,
        "README mentions presenter all guard actions refresh",
    )
    for method_name in [
        "refreshPositionList",
        "refreshCashSummary",
        "refreshPortfolioPnlSummary",
        "refreshBasePositionSummary",
        "refreshSniperPoolSummary",
        "refreshAllReadOnly",
    ]:
        require(method_name in shell_accounting_presenter_header, f"Presenter exposes {method_name}")
    for test_name in [
        "shell_accounting_presenter_all_guard_actions_position_list",
        "shell_accounting_presenter_all_guard_actions_cash_summary",
        "shell_accounting_presenter_all_guard_actions_portfolio_pnl",
        "shell_accounting_presenter_all_guard_actions_base_position",
        "shell_accounting_presenter_all_guard_actions_sniper_pool",
        "shell_accounting_presenter_all_guard_actions_refresh_all",
        "shell_accounting_presenter_all_guard_actions_no_write_no_trade",
        "shell_accounting_presenter_all_guard_actions_no_qml_or_forbidden_dependency",
    ]:
        require(test_name in shell_accounting_presenter_cmake, f"Presenter CMake registers {test_name}")
    for doc_text, doc_name in [
        (shell_accounting_presenter_contract, "docs/57"),
        (shell_accounting_qml_binding_readiness, "docs/58"),
        (shellservices_accounting_controller_contract, "docs/49"),
        (shell_accounting_viewmodel_state_contract, "docs/50"),
        (shell_accounting_dataservice_adapter_boundary, "docs/53"),
        (shell_accounting_dataservice_adapter_test_plan, "docs/54"),
        (shell_accounting_dataservice_adapter_live_call_gate, "docs/55"),
        (shell_accounting_dataservice_adapter_live_call_checklist, "docs/56"),
        (dataservice_readonly_accounting_contracts, "docs/37"),
        (dataservice_accounting_no_write_plan, "docs/38"),
        (codex_prompt_template, "docs/12"),
    ]:
        require("TASK-117" in doc_text, f"{doc_name} mentions TASK-117")
    require("TASK-117" in ui_design, "docs/07 mentions TASK-117")
    require(
        "DataServiceClient/DataServiceClient.h" not in presenter_sources,
        "Presenter still does not include DataServiceClient header after TASK-117",
    )
    require(
        "ShellAccountingDataServiceClientPortAdapter" not in presenter_sources,
        "Presenter still avoids concrete port adapter after TASK-117",
    )
    require(
        "ShellAccountingDataServiceAdapter" not in presenter_sources,
        "Presenter still avoids data service adapter after TASK-117",
    )
    require(
        qml_sources.count("ShellAccountingPresenter") == 1,
        "QML only has the authorized ShellAccountingPresenter property after TASK-131",
    )

    require(shell_accounting_qml_binding_smoke_plan_path.exists(), "docs/59 QML binding smoke plan exists")
    require(shell_accounting_qml_static_gate_path.exists(), "docs/60 QML static gate exists")
    require("59_shell_accounting_qml_binding_smoke_plan" in readme, "README references docs/59")
    require("60_shell_accounting_qml_static_gate" in readme, "README references docs/60")
    require("59_shell_accounting_qml_binding_smoke_plan" in docs_index, "docs/README references docs/59")
    require("60_shell_accounting_qml_static_gate" in docs_index, "docs/README references docs/60")
    require(
        "QML Binding Smoke Matrix" in shell_accounting_qml_binding_smoke_plan,
        "docs/59 contains QML binding smoke matrix",
    )
    require(
        "Empty is not Unavailable" in shell_accounting_qml_binding_smoke_plan,
        "docs/59 distinguishes Empty and Unavailable",
    )
    require(
        "no trading buttons" in shell_accounting_qml_binding_smoke_plan,
        "docs/59 includes no trading buttons rule",
    )
    require("privacyMode" in shell_accounting_qml_binding_smoke_plan, "docs/59 includes privacyMode")
    for forbidden_qml_token in [
        "DataServiceClient",
        "TradeDraft",
        "brokerOrder",
        "strategyExecute",
    ]:
        require(forbidden_qml_token in shell_accounting_qml_static_gate, f"docs/60 forbids {forbidden_qml_token}")
    require("Go / No-Go Checklist" in shell_accounting_qml_static_gate, "docs/60 contains Go / No-Go Checklist")
    require(
        "59_shell_accounting_qml_binding_smoke_plan" in ui_design
        or "QML binding smoke plan" in ui_design,
        "docs/07 references docs/59 or QML binding smoke plan",
    )
    require("59_shell_accounting_qml_binding_smoke_plan" in codex_prompt_template, "docs/12 references docs/59")
    require("60_shell_accounting_qml_static_gate" in codex_prompt_template, "docs/12 references docs/60")
    require("59_shell_accounting_qml_binding_smoke_plan" in shell_accounting_presenter_contract, "docs/57 references docs/59")
    require("60_shell_accounting_qml_static_gate" in shell_accounting_qml_binding_readiness, "docs/58 references docs/60")
    require(
        qml_sources.count("ShellAccountingPresenter") == 1,
        "QML only has the authorized ShellAccountingPresenter property after TASK-131",
    )

    require(
        "add_subdirectory(ShellAccountingQmlStaticGate)" in tests_cmake,
        "tests/CMakeLists includes ShellAccountingQmlStaticGate",
    )
    for qml_static_gate_test_name in [
        "shell_accounting_qml_static_gate_no_direct_service",
        "shell_accounting_qml_static_gate_no_accounting_binding_yet",
        "shell_accounting_qml_static_gate_no_write_tokens",
        "shell_accounting_qml_static_gate_accounting_scope_no_trade",
        "shell_accounting_qml_static_gate_docs_sync",
    ]:
        require(
            qml_static_gate_test_name in shell_accounting_qml_static_gate_cmake,
            f"TASK-119 CTest exists: {qml_static_gate_test_name}",
        )
    require("TASK-119" in shell_accounting_qml_binding_smoke_plan, "docs/59 mentions TASK-119")
    require(
        "QML static gate CTest" in shell_accounting_qml_binding_smoke_plan,
        "docs/59 says QML static gate CTest is automated",
    )
    require("TASK-119" in shell_accounting_qml_static_gate, "docs/60 mentions TASK-119")
    require(
        "CTest" in shell_accounting_qml_static_gate or "automated" in shell_accounting_qml_static_gate,
        "docs/60 describes automated static gate",
    )
    require("TASK-119" in codex_prompt_template, "docs/12 mentions TASK-119")
    require("ShellAccounting QML static gate CTest" in readme, "README mentions ShellAccounting QML static gate CTest")
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "QML only declares the authorized nullable accountingPresenter property after TASK-131",
    )

    require(
        "v0.6 ShellAccounting Read-only UI Readiness" in readme,
        "README mentions v0.6 ShellAccounting Read-only UI Readiness",
    )
    require(
        "61_shell_accounting_readonly_ui_readiness_milestone" in readme,
        "README references docs/61",
    )
    require("62_shell_accounting_next_phase_review" in readme, "README references docs/62")
    require(
        "61_shell_accounting_readonly_ui_readiness_milestone" in docs_index,
        "docs/README references docs/61",
    )
    require("62_shell_accounting_next_phase_review" in docs_index, "docs/README references docs/62")
    require(
        "v0_6_shell_accounting_readonly_ui_readiness" in docs_index,
        "docs/README references v0.6 release notes",
    )
    require(
        "ShellAccountingPresenter" in shell_accounting_readonly_ui_milestone,
        "docs/61 lists ShellAccountingPresenter",
    )
    require("QML 尚未接入" in shell_accounting_readonly_ui_milestone, "docs/61 states QML not wired")
    require(
        "Real accounting action 仍未实现" in shell_accounting_readonly_ui_milestone,
        "docs/61 states real accounting action not implemented",
    )
    require("no TradeDraft" in shell_accounting_readonly_ui_milestone, "docs/61 states no TradeDraft")
    require("no trade suggestion" in shell_accounting_readonly_ui_milestone, "docs/61 states no trade suggestion")
    require("Go / No-Go Checklist" in shell_accounting_next_phase_review, "docs/62 contains Go / No-Go Checklist")
    require(
        "用户明确授权 QML binding" in shell_accounting_next_phase_review,
        "docs/62 requires explicit QML binding authorization",
    )
    require(
        "v0.6.0-shell-accounting-readonly-ui-readiness" in release_notes_v06,
        "v0.6 release notes include suggested tag",
    )
    require("docs/61_shell_accounting_readonly_ui_readiness_milestone.md" in codex_prompt_template, "docs/12 references docs/61")
    require("docs/62_shell_accounting_next_phase_review.md" in codex_prompt_template, "docs/12 references docs/62")
    require(
        "docs/61_shell_accounting_readonly_ui_readiness_milestone.md" in shell_accounting_qml_binding_smoke_plan
        or "v0.6 readiness" in shell_accounting_qml_binding_smoke_plan,
        "docs/59 references docs/61 or v0.6 readiness",
    )
    require(
        "docs/61_shell_accounting_readonly_ui_readiness_milestone.md" in shell_accounting_qml_static_gate
        or "v0.6 readiness" in shell_accounting_qml_static_gate,
        "docs/60 references docs/61 or v0.6 readiness",
    )

    require(
        "add_subdirectory(ShellAccountingQmlBindingSmoke)" in tests_cmake,
        "tests/CMakeLists includes ShellAccountingQmlBindingSmoke",
    )
    for qml_binding_smoke_test_name in [
        "shell_accounting_qml_binding_smoke_object_contract",
        "shell_accounting_qml_binding_smoke_state_matrix",
        "shell_accounting_qml_binding_smoke_guard_payloads",
        "shell_accounting_qml_binding_smoke_issue_privacy",
        "shell_accounting_qml_binding_smoke_no_trade_ui",
        "shell_accounting_qml_binding_smoke_static_gate_dependency",
    ]:
        require(
            qml_binding_smoke_test_name in shell_accounting_qml_binding_smoke_cmake,
            f"TASK-121 CTest exists: {qml_binding_smoke_test_name}",
        )
    require("TASK-121" in shell_accounting_qml_binding_smoke_plan, "docs/59 mentions TASK-121")
    require(
        "smoke scaffold" in shell_accounting_qml_binding_smoke_plan,
        "docs/59 mentions smoke scaffold",
    )
    require("TASK-121" in shell_accounting_qml_static_gate, "docs/60 mentions TASK-121")
    require("TASK-121" in shell_accounting_readonly_ui_milestone, "docs/61 mentions TASK-121")
    require("TASK-121" in shell_accounting_next_phase_review, "docs/62 mentions TASK-121")
    require("TASK-121" in codex_prompt_template, "docs/12 mentions TASK-121")
    require(
        "ShellAccounting QML binding smoke scaffold" in readme,
        "README mentions ShellAccounting QML binding smoke scaffold",
    )
    require(
        qml_sources.count("ShellAccountingPresenter") == 1,
        "QML only has the authorized ShellAccountingPresenter property after TASK-131",
    )
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "QML only declares the authorized nullable accountingPresenter property after TASK-131",
    )

    require(
        "add_subdirectory(ShellAccountingQmlBindingSmokeRuntime)" in tests_cmake,
        "tests/CMakeLists includes ShellAccountingQmlBindingSmokeRuntime",
    )
    for qml_smoke_test_name in [
        "shell_accounting_qml_smoke_object_availability",
        "shell_accounting_qml_smoke_state_display",
        "shell_accounting_qml_smoke_guard_unavailable",
        "shell_accounting_qml_smoke_issue_visibility",
        "shell_accounting_qml_smoke_privacy_display",
        "shell_accounting_qml_smoke_no_trade_ui",
        "shell_accounting_qml_smoke_static_gate_regression",
    ]:
        require(
            qml_smoke_test_name in shell_accounting_qml_smoke_runtime_cmake,
            f"TASK-122 CTest exists: {qml_smoke_test_name}",
        )
    require("TASK-122" in shell_accounting_qml_binding_smoke_plan, "docs/59 mentions TASK-122")
    require("runtime smoke CTest" in shell_accounting_qml_binding_smoke_plan, "docs/59 mentions runtime smoke CTest")
    require("TASK-122" in shell_accounting_qml_static_gate, "docs/60 mentions TASK-122")
    require("TASK-122" in shell_accounting_readonly_ui_milestone, "docs/61 mentions TASK-122")
    require("TASK-122" in shell_accounting_next_phase_review, "docs/62 mentions TASK-122")
    require("TASK-122" in codex_prompt_template, "docs/12 mentions TASK-122")
    require("ShellAccounting QML smoke CTest" in readme, "README mentions ShellAccounting QML smoke CTest")
    require(
        qml_sources.count("ShellAccountingPresenter") == 1,
        "QML only has the authorized ShellAccountingPresenter property after TASK-131",
    )
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "QML only declares the authorized nullable accountingPresenter property after TASK-131",
    )

    require(shell_accounting_production_qml_boundary_path.exists(), "docs/63 production QML boundary exists")
    require(shell_accounting_first_stage_qml_checklist_path.exists(), "docs/64 first-stage QML checklist exists")
    require(
        "docs/63_shell_accounting_production_qml_binding_boundary.md" in readme,
        "README links docs/63",
    )
    require(
        "docs/64_shell_accounting_first_stage_qml_binding_checklist.md" in readme,
        "README links docs/64",
    )
    require(
        "63_shell_accounting_production_qml_binding_boundary.md" in docs_index,
        "docs/README links docs/63",
    )
    require(
        "64_shell_accounting_first_stage_qml_binding_checklist.md" in docs_index,
        "docs/README links docs/64",
    )
    require("DataServiceClient" in shell_accounting_production_qml_boundary, "docs/63 includes DataServiceClient deny item")
    require("SQLite" in shell_accounting_production_qml_boundary, "docs/63 includes SQLite deny item")
    require("AccountingEngine" in shell_accounting_production_qml_boundary, "docs/63 includes AccountingEngine deny item")
    require("no trading buttons" in shell_accounting_production_qml_boundary, "docs/63 includes no trading buttons")
    require("TASK-119" in shell_accounting_production_qml_boundary, "docs/63 includes TASK-119")
    require("TASK-121" in shell_accounting_production_qml_boundary, "docs/63 includes TASK-121")
    require("TASK-122" in shell_accounting_production_qml_boundary, "docs/63 includes TASK-122")
    require("Go / No-Go Checklist" in shell_accounting_first_stage_qml_checklist, "docs/64 includes checklist")
    require(
        "用户明确授权 production QML binding" in shell_accounting_first_stage_qml_checklist,
        "docs/64 requires explicit production QML binding authorization",
    )
    require("docs/63_shell_accounting_production_qml_binding_boundary.md" in codex_prompt_template, "docs/12 links docs/63")
    require("docs/64_shell_accounting_first_stage_qml_binding_checklist.md" in codex_prompt_template, "docs/12 links docs/64")
    require("docs/63_shell_accounting_production_qml_binding_boundary.md" in shell_accounting_qml_binding_smoke_plan, "docs/59 links docs/63")
    require("docs/64_shell_accounting_first_stage_qml_binding_checklist.md" in shell_accounting_qml_static_gate, "docs/60 links docs/64")
    require(
        qml_sources.count("ShellAccountingPresenter") == 1,
        "QML only has the authorized ShellAccountingPresenter property after TASK-131",
    )
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "QML only declares the authorized nullable accountingPresenter property after TASK-131",
    )

    require(
        shell_accounting_qml_type_registration_boundary_path.exists(),
        "docs/65 QML type registration boundary exists",
    )
    require(
        shell_accounting_qml_type_registration_test_plan_path.exists(),
        "docs/66 QML type registration test plan exists",
    )
    require(
        "docs/65_shell_accounting_qml_type_registration_boundary.md" in readme,
        "README links docs/65",
    )
    require(
        "docs/66_shell_accounting_qml_type_registration_test_plan.md" in readme,
        "README links docs/66",
    )
    require(
        "65_shell_accounting_qml_type_registration_boundary.md" in docs_index,
        "docs/README links docs/65",
    )
    require(
        "66_shell_accounting_qml_type_registration_test_plan.md" in docs_index,
        "docs/README links docs/66",
    )
    require(
        "ShellAccountingPresenter" in shell_accounting_qml_type_registration_boundary,
        "docs/65 includes ShellAccountingPresenter",
    )
    require(
        "DataServiceClient" in shell_accounting_qml_type_registration_boundary,
        "docs/65 includes DataServiceClient deny item",
    )
    require(
        "AccountingEngine" in shell_accounting_qml_type_registration_boundary,
        "docs/65 includes AccountingEngine deny item",
    )
    require(
        "createTradeDraft" in shell_accounting_qml_type_registration_boundary,
        "docs/65 includes createTradeDraft deny item",
    )
    require(
        "brokerOrder" in shell_accounting_qml_type_registration_boundary,
        "docs/65 includes brokerOrder deny item",
    )
    require("TASK-119" in shell_accounting_qml_type_registration_boundary, "docs/65 includes TASK-119")
    require("TASK-121" in shell_accounting_qml_type_registration_boundary, "docs/65 includes TASK-121")
    require("TASK-122" in shell_accounting_qml_type_registration_boundary, "docs/65 includes TASK-122")
    require("Test matrix" in shell_accounting_qml_type_registration_test_plan, "docs/66 includes Test matrix")
    require("Type allowlist" in shell_accounting_qml_type_registration_test_plan, "docs/66 includes Type allowlist")
    require("Type denylist" in shell_accounting_qml_type_registration_test_plan, "docs/66 includes Type denylist")
    require(
        "用户明确授权 QML type registration" in shell_accounting_qml_type_registration_test_plan,
        "docs/66 requires explicit QML type registration authorization",
    )
    require(
        "docs/65_shell_accounting_qml_type_registration_boundary.md" in codex_prompt_template,
        "docs/12 links docs/65",
    )
    require(
        "docs/66_shell_accounting_qml_type_registration_test_plan.md" in codex_prompt_template,
        "docs/12 links docs/66",
    )
    require(
        "docs/65_shell_accounting_qml_type_registration_boundary.md" in shell_accounting_qml_binding_smoke_plan,
        "docs/59 links docs/65",
    )
    require(
        "docs/66_shell_accounting_qml_type_registration_test_plan.md" in shell_accounting_qml_static_gate,
        "docs/60 links docs/66",
    )
    require(
        qml_sources.count("ShellAccountingPresenter") == 1,
        "QML only has the authorized ShellAccountingPresenter property after TASK-131",
    )
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "QML only declares the authorized nullable accountingPresenter property after TASK-131",
    )

    require("TASK-125" in shell_accounting_qml_type_registration_boundary, "docs/65 mentions TASK-125")
    require("TASK-125" in shell_accounting_qml_type_registration_test_plan, "docs/66 mentions TASK-125")
    require("TASK-125" in codex_prompt_template, "docs/12 mentions TASK-125")
    require(
        "ShellAccounting QML type registration test scaffold" in readme,
        "README mentions ShellAccounting QML type registration test scaffold",
    )
    require(
        "shell_accounting_qml_type_registration_allowlist_scaffold" in shell_accounting_qml_type_registration_cmake,
        "tests include type registration allowlist scaffold",
    )
    require(
        "shell_accounting_qml_type_registration_denylist_scaffold" in shell_accounting_qml_type_registration_cmake,
        "tests include type registration denylist scaffold",
    )
    require(
        "shell_accounting_qml_type_registration_method_denylist_scaffold"
        in shell_accounting_qml_type_registration_cmake,
        "tests include type registration method denylist scaffold",
    )
    require(
        "shell_accounting_qml_type_registration_module_import_scaffold"
        in shell_accounting_qml_type_registration_cmake,
        "tests include type registration module import scaffold",
    )
    require(
        "shell_accounting_qml_type_registration_gate_dependency" in shell_accounting_qml_type_registration_cmake,
        "tests include type registration gate dependency",
    )
    require(
        "shell_accounting_qml_type_registration_no_production_change" in shell_accounting_qml_type_registration_cmake,
        "tests include type registration no production change",
    )
    require(
        qml_sources.count("ShellAccountingPresenter") == 1,
        "QML only has the authorized ShellAccountingPresenter property after TASK-131",
    )
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "QML only declares the authorized nullable accountingPresenter property after TASK-131",
    )
    registration_sources = ""
    for path in (root / "apps" / "ETFDecisionShell").rglob("*"):
        if path.is_file() and path.suffix in {".cpp", ".h", ".hpp"}:
            registration_sources += path.read_text(encoding="utf-8")
    require(
        "qmlRegisterType<ShellAccountingPresenter>" not in registration_sources,
        "production registration has not registered ShellAccountingPresenter",
    )
    require(
        "qmlRegisterType<ShellAccountingStatusObject>" not in registration_sources,
        "production registration has not registered ShellAccountingStatusObject",
    )
    require(
        "qmlRegisterType<ShellAccountingIssueListModel>" not in registration_sources,
        "production registration has not registered ShellAccountingIssueListModel",
    )
    require(
        "qmlRegisterType<ShellPositionListModel>" not in registration_sources,
        "production registration has not registered ShellPositionListModel",
    )

    require(
        shell_accounting_qml_type_registration_implementation_gate_path.exists(),
        "docs/67 QML type registration implementation gate exists",
    )
    require(
        shell_accounting_qml_type_registration_rollback_strategy_path.exists(),
        "docs/68 QML type registration rollback strategy exists",
    )
    require(
        "docs/67_shell_accounting_qml_type_registration_implementation_gate.md" in readme,
        "README links docs/67",
    )
    require(
        "docs/68_shell_accounting_qml_type_registration_rollback_strategy.md" in readme,
        "README links docs/68",
    )
    require(
        "67_shell_accounting_qml_type_registration_implementation_gate.md" in docs_index,
        "docs/README links docs/67",
    )
    require(
        "68_shell_accounting_qml_type_registration_rollback_strategy.md" in docs_index,
        "docs/README links docs/68",
    )
    require(
        "TASK-126" in shell_accounting_qml_type_registration_implementation_gate,
        "docs/67 mentions TASK-126",
    )
    require(
        "rollback" in shell_accounting_qml_type_registration_rollback_strategy.lower(),
        "docs/68 covers rollback",
    )
    require("TASK-126" in codex_prompt_template, "docs/12 mentions TASK-126")
    require(
        "shell_accounting_qml_type_registration_implementation_gate"
        in shell_accounting_qml_type_registration_gate_cmake,
        "tests include type registration implementation gate",
    )
    require(
        "shell_accounting_qml_type_registration_location_gate" in shell_accounting_qml_type_registration_gate_cmake,
        "tests include type registration location gate",
    )
    require(
        "shell_accounting_qml_type_registration_allowlist_gate" in shell_accounting_qml_type_registration_gate_cmake,
        "tests include type registration allowlist gate",
    )
    require(
        "shell_accounting_qml_type_registration_forbidden_exposure_gate"
        in shell_accounting_qml_type_registration_gate_cmake,
        "tests include type registration forbidden exposure gate",
    )
    require(
        "shell_accounting_qml_type_registration_rollback_gate" in shell_accounting_qml_type_registration_gate_cmake,
        "tests include type registration rollback gate",
    )
    require(
        "shell_accounting_qml_type_registration_no_production_registration"
        in shell_accounting_qml_type_registration_gate_cmake,
        "tests include type registration no production registration gate",
    )
    require(
        qml_sources.count("ShellAccountingPresenter") == 1,
        "QML only has the authorized ShellAccountingPresenter property after TASK-131",
    )
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "QML only declares the authorized nullable accountingPresenter property after TASK-131",
    )
    require(
        "qmlRegisterType<ShellAccountingPresenter>" not in registration_sources,
        "TASK-126 production registration has not registered ShellAccountingPresenter",
    )
    require(
        "qmlRegisterType<ShellAccountingStatusObject>" not in registration_sources,
        "TASK-126 production registration has not registered ShellAccountingStatusObject",
    )
    require(
        "qmlRegisterType<ShellAccountingIssueListModel>" not in registration_sources,
        "TASK-126 production registration has not registered ShellAccountingIssueListModel",
    )
    require(
        "qmlRegisterType<ShellPositionListModel>" not in registration_sources,
        "TASK-126 production registration has not registered ShellPositionListModel",
    )

    require(
        shell_accounting_qml_registration_header_path.exists(),
        "ShellAccountingQmlRegistration header exists",
    )
    require(
        shell_accounting_qml_registration_source_path.exists(),
        "ShellAccountingQmlRegistration source exists",
    )
    require("TASK-127" in shell_accounting_qml_type_registration_boundary, "docs/65 mentions TASK-127")
    require("TASK-127" in shell_accounting_qml_type_registration_test_plan, "docs/66 mentions TASK-127")
    require("TASK-127" in shell_accounting_qml_type_registration_implementation_gate, "docs/67 mentions TASK-127")
    require("TASK-127" in shell_accounting_qml_type_registration_rollback_strategy, "docs/68 mentions TASK-127")
    require("TASK-127" in codex_prompt_template, "docs/12 mentions TASK-127")
    require(
        "ShellAccounting QML type registration implementation" in readme,
        "README mentions ShellAccounting QML type registration implementation",
    )
    require(
        "shell_accounting_qml_type_registration_module_import"
        in shell_accounting_qml_type_registration_implementation_cmake,
        "tests include type registration module import implementation",
    )
    require(
        "shell_accounting_qml_type_registration_allowlist_registered"
        in shell_accounting_qml_type_registration_implementation_cmake,
        "tests include type registration allowlist registered",
    )
    require(
        "shell_accounting_qml_type_registration_forbidden_types_absent"
        in shell_accounting_qml_type_registration_implementation_cmake,
        "tests include type registration forbidden types absent",
    )
    require(
        "shell_accounting_qml_type_registration_forbidden_methods_absent"
        in shell_accounting_qml_type_registration_implementation_cmake,
        "tests include type registration forbidden methods absent",
    )
    require(
        "shell_accounting_qml_type_registration_uncreatable_readonly"
        in shell_accounting_qml_type_registration_implementation_cmake,
        "tests include type registration uncreatable readonly",
    )
    require(
        "shell_accounting_qml_type_registration_no_production_qml_binding"
        in shell_accounting_qml_type_registration_implementation_cmake,
        "tests include no production QML binding registration test",
    )
    require(
        "shell_accounting_qml_type_registration_gate_regression"
        in shell_accounting_qml_type_registration_implementation_cmake,
        "tests include type registration gate regression",
    )
    for allowed_type in [
        "ShellAccountingPresenter",
        "ShellAccountingStatusObject",
        "ShellAccountingIssueListModel",
        "ShellPositionListModel",
    ]:
        require(allowed_type in shell_accounting_qml_registration_source, f"registration helper registers {allowed_type}")
    for forbidden_type in [
        "DataServiceClient",
        "DataServiceApi",
        "ShellAccountingDataServiceAdapter",
        "ShellAccountingDataServiceClientPortAdapter",
        "DataAccess",
        "SQLite",
        "AccountingEngine",
        "ShellCashSummaryObject",
        "ShellPortfolioPnlObject",
        "ShellBasePositionObject",
        "ShellSniperPoolObject",
        "ShellSniperTierListModel",
        "createTradeDraft",
        "brokerOrder",
        "strategyExecute",
    ]:
        require(
            forbidden_type not in shell_accounting_qml_registration_source,
            f"registration helper does not expose {forbidden_type}",
        )
    require(
        "qmlRegisterUncreatableType" in shell_accounting_qml_registration_source,
        "registration helper uses uncreatable registration",
    )
    require(
        "ETFDecisionTerminal.ShellAccounting" in shell_accounting_qml_registration_header,
        "registration helper fixes ShellAccounting module name",
    )
    require(
        authorized_shell_accounting_import_count == 1,
        "production QML has exactly one authorized ShellAccounting import after TASK-131",
    )
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "production QML has exactly one authorized nullable accountingPresenter property after TASK-131",
    )

    require(
        shell_accounting_qml_startup_registration_gate_path.exists(),
        "docs/69 QML startup registration gate exists",
    )
    require(
        shell_accounting_qml_startup_registration_test_plan_path.exists(),
        "docs/70 QML startup registration test plan exists",
    )
    require(
        shell_accounting_qml_startup_registration_wiring_path.exists(),
        "docs/71 QML startup registration wiring exists",
    )
    require(
        "docs/69_shell_accounting_qml_startup_registration_gate.md" in readme,
        "README links docs/69",
    )
    require(
        "docs/70_shell_accounting_qml_startup_registration_test_plan.md" in readme,
        "README links docs/70",
    )
    require(
        "docs/71_shell_accounting_qml_startup_registration_wiring.md" in readme,
        "README links docs/71",
    )
    require(
        "69_shell_accounting_qml_startup_registration_gate.md" in docs_index,
        "docs/README links docs/69",
    )
    require(
        "70_shell_accounting_qml_startup_registration_test_plan.md" in docs_index,
        "docs/README links docs/70",
    )
    require(
        "71_shell_accounting_qml_startup_registration_wiring.md" in docs_index,
        "docs/README links docs/71",
    )
    require("TASK-128" in shell_accounting_qml_startup_registration_gate, "docs/69 mentions TASK-128")
    require(
        "Test Matrix" in shell_accounting_qml_startup_registration_test_plan,
        "docs/70 includes Test matrix",
    )
    require("TASK-128" in codex_prompt_template, "docs/12 mentions TASK-128")
    require(
        "shell_accounting_qml_startup_registration_gate"
        in shell_accounting_qml_startup_registration_gate_cmake,
        "tests include startup registration gate",
    )
    require(
        "shell_accounting_qml_startup_registration_location_policy"
        in shell_accounting_qml_startup_registration_gate_cmake,
        "tests include startup registration location policy",
    )
    require(
        "shell_accounting_qml_startup_registration_no_production_wiring"
        in shell_accounting_qml_startup_registration_gate_cmake,
        "tests include startup registration no production wiring",
    )
    require(
        "shell_accounting_qml_startup_registration_no_forbidden_exposure"
        in shell_accounting_qml_startup_registration_gate_cmake,
        "tests include startup registration no forbidden exposure",
    )
    require(
        "shell_accounting_qml_startup_registration_rollback_policy"
        in shell_accounting_qml_startup_registration_gate_cmake,
        "tests include startup registration rollback policy",
    )
    require(
        registration_sources.count("registerShellAccountingQmlTypes") == 1,
        "production startup has exactly one registerShellAccountingQmlTypes call after TASK-129",
    )
    require(
        authorized_shell_accounting_import_count == 1,
        "production QML has exactly one authorized ShellAccounting import after TASK-131",
    )

    require("TASK-129" in shell_accounting_qml_startup_registration_gate, "docs/69 mentions TASK-129")
    require("TASK-129" in shell_accounting_qml_startup_registration_test_plan, "docs/70 mentions TASK-129")
    require("TASK-129" in shell_accounting_qml_startup_registration_wiring, "docs/71 mentions TASK-129")
    require(
        "registerShellAccountingQmlTypes" in shell_accounting_qml_startup_registration_wiring,
        "docs/71 records startup registration helper call",
    )
    require("rollback" in shell_accounting_qml_startup_registration_wiring, "docs/71 includes rollback")
    require("TASK-129" in codex_prompt_template, "docs/12 mentions TASK-129")
    for ctest_name in [
        "shell_accounting_qml_startup_registration_wiring",
        "shell_accounting_qml_startup_registration_wiring_single_call",
        "shell_accounting_qml_startup_registration_wiring_location_policy",
        "shell_accounting_qml_startup_registration_wiring_preserves_readonly_allowlist",
        "shell_accounting_qml_startup_registration_wiring_no_qml_import",
        "shell_accounting_qml_startup_registration_wiring_no_context_exposure",
        "shell_accounting_qml_startup_registration_wiring_no_forbidden_runtime_access",
        "shell_accounting_qml_startup_registration_wiring_rollback_ready",
    ]:
        require(ctest_name in shell_accounting_qml_startup_registration_wiring_cmake, f"tests include {ctest_name}")
    require(
        registration_sources.count("registerShellAccountingQmlTypes") == 1,
        "production startup calls registerShellAccountingQmlTypes exactly once after TASK-129",
    )
    require(
        authorized_shell_accounting_import_count == 1,
        "production QML has exactly one authorized ShellAccounting import after TASK-131",
    )
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "production QML has exactly one authorized nullable accountingPresenter property after TASK-131",
    )
    require(
        registration_sources.count('setContextProperty("accountingPresenter"') == 1,
        "production startup has exactly one authorized accountingPresenter context property after TASK-133",
    )
    require(
        registration_sources.count("ShellAccountingPresenter shellAccountingPresenter") == 1,
        "production startup has exactly one authorized ShellAccountingPresenter creation after TASK-133",
    )

    require(
        shell_accounting_production_qml_binding_gate_path.exists(),
        "docs/72 production QML binding gate exists",
    )
    require(
        shell_accounting_production_qml_binding_test_plan_path.exists(),
        "docs/73 production QML binding test plan exists",
    )
    require(
        "docs/72_shell_accounting_production_qml_binding_gate.md" in readme,
        "README links docs/72",
    )
    require(
        "docs/73_shell_accounting_production_qml_binding_test_plan.md" in readme,
        "README links docs/73",
    )
    require(
        "72_shell_accounting_production_qml_binding_gate.md" in docs_index,
        "docs/README links docs/72",
    )
    require(
        "73_shell_accounting_production_qml_binding_test_plan.md" in docs_index,
        "docs/README links docs/73",
    )
    require("TASK-130" in shell_accounting_production_qml_binding_gate, "docs/72 mentions TASK-130")
    require("TASK-130" in shell_accounting_production_qml_binding_test_plan, "docs/73 mentions TASK-130")
    require("TASK-130" in codex_prompt_template, "docs/12 mentions TASK-130")
    require(
        "production QML binding 不等于 real accounting action" in shell_accounting_production_qml_binding_gate,
        "docs/72 distinguishes production QML binding from real accounting action",
    )
    require("rollback" in shell_accounting_production_qml_binding_gate, "docs/72 includes rollback")
    require("Test matrix" in shell_accounting_production_qml_binding_test_plan, "docs/73 includes Test matrix")
    for ctest_name in [
        "shell_accounting_production_qml_binding_gate",
        "shell_accounting_production_qml_binding_no_import_yet",
        "shell_accounting_production_qml_binding_no_page_yet",
        "shell_accounting_production_qml_binding_no_context_exposure",
        "shell_accounting_production_qml_binding_no_forbidden_runtime_access",
        "shell_accounting_production_qml_binding_unavailable_policy",
        "shell_accounting_production_qml_binding_rollback_policy",
    ]:
        require(ctest_name in shell_accounting_production_qml_binding_gate_cmake, f"tests include {ctest_name}")
    require(
        authorized_shell_accounting_import_count == 1,
        "production QML has exactly one authorized ShellAccounting import after TASK-131",
    )
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "production QML has exactly one authorized nullable accountingPresenter property after TASK-131",
    )
    require(
        registration_sources.count('setContextProperty("accountingPresenter"') == 1,
        "production startup has exactly one authorized accountingPresenter context property after TASK-133",
    )
    require(
        registration_sources.count("ShellAccountingPresenter shellAccountingPresenter") == 1,
        "production startup has exactly one authorized ShellAccountingPresenter creation after TASK-133",
    )

    require(
        shell_accounting_production_qml_binding_implementation_path.exists(),
        "docs/74 production QML binding implementation exists",
    )
    require(
        "docs/74_shell_accounting_production_qml_binding_implementation.md" in readme,
        "README links docs/74",
    )
    require(
        "74_shell_accounting_production_qml_binding_implementation.md" in docs_index,
        "docs/README links docs/74",
    )
    require("TASK-131" in shell_accounting_production_qml_binding_gate, "docs/72 mentions TASK-131")
    require("TASK-131" in shell_accounting_production_qml_binding_test_plan, "docs/73 mentions TASK-131")
    require("TASK-131" in shell_accounting_production_qml_binding_implementation, "docs/74 mentions TASK-131")
    require("TASK-131" in codex_prompt_template, "docs/12 mentions TASK-131")
    require(
        "ShellAccounting production QML binding shell" in readme,
        "README mentions ShellAccounting production QML binding shell",
    )
    require(shell_accounting_readonly_page_path.exists(), "authorized ShellAccounting read-only QML page exists")
    require(
        "import ETFDecisionTerminal.ShellAccounting 1.0" in shell_accounting_readonly_page,
        "authorized page imports ShellAccounting module",
    )
    require(
        authorized_shell_accounting_import_count == 1,
        "production QML has exactly one authorized ShellAccounting import after TASK-131",
    )
    require(
        "property ShellAccountingPresenter accountingPresenter: null" in shell_accounting_readonly_page,
        "authorized page declares nullable accountingPresenter property",
    )
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "production QML has exactly one authorized nullable accountingPresenter property after TASK-131",
    )
    require("shellAccountingReadOnlyPage" in shell_accounting_readonly_page, "authorized page has stable objectName")
    require("shellAccountingUnavailablePanel" in shell_accounting_readonly_page, "authorized page has unavailable panel")
    require("disabled" in shell_accounting_readonly_page, "authorized page displays disabled state")
    require("unavailable-safe" in shell_accounting_readonly_page, "authorized page displays unavailable-safe state")
    for ctest_name in [
        "shell_accounting_production_qml_binding_implementation",
        "shell_accounting_production_qml_binding_authorized_import",
        "shell_accounting_production_qml_binding_authorized_page",
        "shell_accounting_production_qml_binding_unavailable_state",
        "shell_accounting_production_qml_binding_no_context_exposure_after_binding",
        "shell_accounting_production_qml_binding_no_forbidden_runtime_access_after_binding",
        "shell_accounting_production_qml_binding_no_trade_ui_after_binding",
        "shell_accounting_production_qml_binding_rollback_ready_after_binding",
    ]:
        require(ctest_name in shell_accounting_production_qml_binding_implementation_cmake, f"tests include {ctest_name}")
    require(
        registration_sources.count('setContextProperty("accountingPresenter"') == 1,
        "production startup has exactly one authorized accountingPresenter context property after TASK-133",
    )
    require(
        registration_sources.count("ShellAccountingPresenter shellAccountingPresenter") == 1,
        "production startup has exactly one authorized ShellAccountingPresenter creation after TASK-133",
    )
    for forbidden_qml_runtime_token in [
        "DataServiceClient",
        "SQLite",
        "AccountingEngine",
        "createTradeDraft",
        "brokerOrder",
        "strategyExecute",
        "writeEnabled: true",
        "data.audit.append",
    ]:
        require(
            forbidden_qml_runtime_token not in shell_accounting_readonly_page,
            f"ShellAccounting read-only page avoids {forbidden_qml_runtime_token} after TASK-131",
        )
    require("rollback" in shell_accounting_production_qml_binding_implementation, "docs/74 includes rollback")

    require(shell_accounting_presenter_lifecycle_gate_path.exists(), "docs/75 presenter lifecycle gate exists")
    require(
        shell_accounting_presenter_lifecycle_test_plan_path.exists(),
        "docs/76 presenter lifecycle test plan exists",
    )
    require(
        "docs/75_shell_accounting_presenter_lifecycle_gate.md" in readme,
        "README links docs/75",
    )
    require(
        "docs/76_shell_accounting_presenter_lifecycle_test_plan.md" in readme,
        "README links docs/76",
    )
    require(
        "75_shell_accounting_presenter_lifecycle_gate.md" in docs_index,
        "docs/README links docs/75",
    )
    require(
        "76_shell_accounting_presenter_lifecycle_test_plan.md" in docs_index,
        "docs/README links docs/76",
    )
    require("TASK-132" in shell_accounting_presenter_lifecycle_gate, "docs/75 mentions TASK-132")
    require("TASK-132" in shell_accounting_presenter_lifecycle_test_plan, "docs/76 mentions TASK-132")
    require("TASK-132" in codex_prompt_template, "docs/12 mentions TASK-132")
    require(
        "Presenter lifecycle is not real accounting action" in shell_accounting_presenter_lifecycle_gate,
        "docs/75 states presenter lifecycle is not real accounting action",
    )
    require("rollback" in shell_accounting_presenter_lifecycle_gate, "docs/75 includes rollback")
    require("Test Matrix" in shell_accounting_presenter_lifecycle_test_plan, "docs/76 includes Test Matrix")
    for ctest_name in [
        "shell_accounting_presenter_lifecycle_gate",
        "shell_accounting_presenter_lifecycle_no_creation_yet",
        "shell_accounting_presenter_lifecycle_no_context_exposure_yet",
        "shell_accounting_presenter_lifecycle_null_presenter_policy",
        "shell_accounting_presenter_lifecycle_no_forbidden_runtime_access",
        "shell_accounting_presenter_lifecycle_no_trade_ui",
        "shell_accounting_presenter_lifecycle_rollback_policy",
    ]:
        require(ctest_name in shell_accounting_presenter_lifecycle_gate_cmake, f"tests include {ctest_name}")
    require(
        registration_sources.count("ShellAccountingPresenter shellAccountingPresenter") == 1,
        "production startup has exactly one authorized ShellAccountingPresenter creation after TASK-133",
    )
    require(
        registration_sources.count('setContextProperty("accountingPresenter"') == 1,
        "production startup has exactly one authorized accountingPresenter context property after TASK-133",
    )
    require(
        authorized_shell_accounting_presenter_property_count == 1,
        "production QML only keeps the nullable accountingPresenter property after TASK-132",
    )
    require(
        shell_accounting_presenter_lifecycle_implementation_path.exists(),
        "docs/77 presenter lifecycle implementation exists",
    )
    require(
        "docs/77_shell_accounting_presenter_lifecycle_implementation.md" in readme,
        "README links docs/77",
    )
    require(
        "77_shell_accounting_presenter_lifecycle_implementation.md" in docs_index,
        "docs/README links docs/77",
    )
    require("TASK-133" in shell_accounting_presenter_lifecycle_gate, "docs/75 mentions TASK-133")
    require("TASK-133" in shell_accounting_presenter_lifecycle_test_plan, "docs/76 mentions TASK-133")
    require("TASK-133" in shell_accounting_presenter_lifecycle_implementation, "docs/77 mentions TASK-133")
    require("accountingPresenter" in shell_accounting_presenter_lifecycle_implementation, "docs/77 records accountingPresenter")
    require("rollback" in shell_accounting_presenter_lifecycle_implementation, "docs/77 includes rollback")
    require("TASK-133" in codex_prompt_template, "docs/12 mentions TASK-133")
    for ctest_name in [
        "shell_accounting_presenter_lifecycle_implementation",
        "shell_accounting_presenter_lifecycle_authorized_creation",
        "shell_accounting_presenter_lifecycle_authorized_context_exposure",
        "shell_accounting_presenter_lifecycle_ownership_order",
        "shell_accounting_presenter_lifecycle_unavailable_state_after_exposure",
        "shell_accounting_presenter_lifecycle_no_forbidden_runtime_access_after_exposure",
        "shell_accounting_presenter_lifecycle_no_trade_ui_after_exposure",
        "shell_accounting_presenter_lifecycle_rollback_ready_after_exposure",
    ]:
        require(
            ctest_name in shell_accounting_presenter_lifecycle_implementation_cmake,
            f"tests include {ctest_name}",
        )
    require(
        registration_sources.count("ShellAccountingPresenter shellAccountingPresenter") == 1,
        "production startup creates ShellAccountingPresenter exactly once after TASK-133",
    )
    require(
        registration_sources.count('setContextProperty("accountingPresenter"') == 1,
        "production startup exposes accountingPresenter exactly once after TASK-133",
    )
    for forbidden_context_token in [
        'setContextProperty("DataServiceClient"',
        'setContextProperty("AccountingEngine"',
        'setContextProperty("DataAccess"',
        'setContextProperty("SQLite"',
    ]:
        require(
            forbidden_context_token not in registration_sources,
            f"production startup does not expose {forbidden_context_token} after TASK-133",
        )
    for forbidden_trade_ui_token in [
        "买入",
        "卖出",
        "下单",
        "TradeDraft",
        "brokerOrder",
        "strategyExecute",
        "confirmTrade",
        "manualEntry",
        "cashAdjustment",
    ]:
        require(
            forbidden_trade_ui_token not in shell_accounting_readonly_page,
            f"ShellAccounting read-only page avoids {forbidden_trade_ui_token} after TASK-133",
        )

    require(
        shell_accounting_real_data_adapter_gate_path.exists(),
        "docs/78 real data adapter gate exists",
    )
    require(
        shell_accounting_real_data_adapter_test_plan_path.exists(),
        "docs/79 real data adapter test plan exists",
    )
    require(
        shell_accounting_real_data_adapter_implementation_path.exists(),
        "docs/80 real data adapter implementation exists",
    )
    require(
        "docs/78_shell_accounting_real_data_adapter_gate.md" in readme,
        "README links docs/78",
    )
    require(
        "docs/79_shell_accounting_real_data_adapter_test_plan.md" in readme,
        "README links docs/79",
    )
    require(
        "docs/80_shell_accounting_real_data_adapter_implementation.md" in readme,
        "README links docs/80",
    )
    require(
        "78_shell_accounting_real_data_adapter_gate.md" in docs_index,
        "docs/README links docs/78",
    )
    require(
        "79_shell_accounting_real_data_adapter_test_plan.md" in docs_index,
        "docs/README links docs/79",
    )
    require(
        "80_shell_accounting_real_data_adapter_implementation.md" in docs_index,
        "docs/README links docs/80",
    )
    require("TASK-134" in shell_accounting_real_data_adapter_gate, "docs/78 mentions TASK-134")
    require("TASK-134" in shell_accounting_real_data_adapter_test_plan, "docs/79 mentions TASK-134")
    require("TASK-134" in codex_prompt_template, "docs/12 mentions TASK-134")
    require("TASK-135" in shell_accounting_real_data_adapter_gate, "docs/78 mentions TASK-135")
    require("TASK-135" in shell_accounting_real_data_adapter_test_plan, "docs/79 mentions TASK-135")
    require("TASK-135" in shell_accounting_real_data_adapter_implementation, "docs/80 mentions TASK-135")
    require("TASK-135" in codex_prompt_template, "docs/12 mentions TASK-135")
    require(
        "ShellAccountingDataServiceAdapter" in shell_accounting_real_data_adapter_implementation,
        "docs/80 names adapter class",
    )
    require(
        "ShellAccountingDataServiceClientPortAdapter" in shell_accounting_real_data_adapter_implementation,
        "docs/80 names concrete port adapter class",
    )
    require(
        "DataServiceClient" in shell_accounting_real_data_adapter_implementation,
        "docs/80 names DataServiceClient boundary",
    )
    require("rollback" in shell_accounting_real_data_adapter_implementation, "docs/80 includes rollback")
    require(
        "real adapter is not write path" in shell_accounting_real_data_adapter_gate,
        "docs/78 states real adapter is not write path",
    )
    require("rollback" in shell_accounting_real_data_adapter_gate, "docs/78 includes rollback")
    require("Test Matrix" in shell_accounting_real_data_adapter_test_plan, "docs/79 includes Test Matrix")
    for ctest_name in [
        "shell_accounting_real_data_adapter_gate",
        "shell_accounting_real_data_adapter_no_real_action_yet",
        "shell_accounting_real_data_adapter_readonly_action_policy",
        "shell_accounting_real_data_adapter_no_write_path",
        "shell_accounting_real_data_adapter_no_sqlite_direct_access",
        "shell_accounting_real_data_adapter_no_accounting_engine_direct_access",
        "shell_accounting_real_data_adapter_error_mapping_policy",
        "shell_accounting_real_data_adapter_privacy_policy",
        "shell_accounting_real_data_adapter_rollback_policy",
    ]:
        require(
            ctest_name in shell_accounting_real_data_adapter_gate_cmake,
            f"tests include {ctest_name}",
        )
    for ctest_name in [
        "shell_accounting_real_data_adapter_implementation",
        "shell_accounting_real_data_adapter_authorized_readonly_actions",
        "shell_accounting_real_data_adapter_success_mapping",
        "shell_accounting_real_data_adapter_unavailable_mapping",
        "shell_accounting_real_data_adapter_transport_error_mapping",
        "shell_accounting_real_data_adapter_protocol_error_mapping",
        "shell_accounting_real_data_adapter_timeout_mapping",
        "shell_accounting_real_data_adapter_privacy_no_raw_payload",
        "shell_accounting_real_data_adapter_no_write_path_after_integration",
        "shell_accounting_real_data_adapter_no_sqlite_direct_after_integration",
        "shell_accounting_real_data_adapter_no_accounting_engine_direct_after_integration",
        "shell_accounting_real_data_adapter_rollback_ready_after_integration",
    ]:
        require(
            ctest_name in shell_accounting_real_data_adapter_implementation_cmake,
            f"TASK-135 tests include {ctest_name}",
        )
    require(
        "ShellAccountingDataServiceAdapter" in registration_sources
        and "ShellAccountingDataServiceClientPortAdapter" in registration_sources
        and "DataServiceClient" in registration_sources
        and registration_sources.count("setServiceAdapter(") == 1,
        "production startup wires authorized read-only ShellAccounting adapter once",
    )
    require(
        "DataServiceClient" not in shell_accounting_readonly_page,
        "ShellAccounting QML still does not call DataServiceClient",
    )
    require(
        "DataServiceClient/DataServiceClient.h" not in shell_accounting_presenter_header
        and "DataServiceClient/DataServiceClient.h" not in shell_accounting_presenter_source
        and "DataServiceClient/DataServiceClient.h" not in shell_accounting_controller_header
        and "DataServiceClient/DataServiceClient.h" not in shell_accounting_controller_source,
        "presenter/controller still avoid direct DataServiceClient include",
    )
    shell_accounting_production_path = "\n".join(
        [
            registration_sources,
            shell_accounting_readonly_page,
            shell_accounting_presenter_header,
            shell_accounting_presenter_source,
            shell_accounting_controller_header,
            shell_accounting_controller_source,
        ]
    )
    for forbidden_service_token in [
        "SQLiteConnection",
        "sqlite3",
        "SQL SELECT",
        "AccountingTradeFactReader",
        "AccountingEngine",
        "AccountingReplayEngine",
        "replayFromFacts",
        "trade_log write",
        "trade_execution_group write",
        "trade_draft write",
        "cash_snapshot write",
        "position_snapshot write",
        "portfolio_summary write",
        "audit_log write",
        "data.audit.append",
        "confirmTrade",
        "manualEntry",
        "cashAdjustment",
        "createTradeDraft",
        "strategyExecute",
        "writeEnabled: true",
    ]:
        require(
            forbidden_service_token not in shell_accounting_production_path,
            f"production ShellAccounting path avoids {forbidden_service_token} after TASK-134",
        )

    require(
        shell_accounting_dataservice_facts_query_gate_path.exists(),
        "docs/81 DataService facts query gate exists",
    )
    require(
        shell_accounting_dataservice_facts_query_test_plan_path.exists(),
        "docs/82 DataService facts query test plan exists",
    )
    require(
        shell_accounting_dataservice_facts_query_implementation_path.exists(),
        "docs/83 DataService facts query implementation exists",
    )
    require(
        "docs/81_shell_accounting_dataservice_readonly_facts_query_gate.md" in readme,
        "README links docs/81",
    )
    require(
        "docs/82_shell_accounting_dataservice_readonly_facts_query_test_plan.md" in readme,
        "README links docs/82",
    )
    require(
        "docs/83_shell_accounting_dataservice_readonly_facts_query_implementation.md" in readme,
        "README links docs/83",
    )
    require(
        "81_shell_accounting_dataservice_readonly_facts_query_gate.md" in docs_index,
        "docs/README links docs/81",
    )
    require(
        "82_shell_accounting_dataservice_readonly_facts_query_test_plan.md" in docs_index,
        "docs/README links docs/82",
    )
    require(
        "83_shell_accounting_dataservice_readonly_facts_query_implementation.md" in docs_index,
        "docs/README links docs/83",
    )
    require("TASK-136" in shell_accounting_dataservice_facts_query_gate, "docs/81 mentions TASK-136")
    require("TASK-136" in shell_accounting_dataservice_facts_query_test_plan, "docs/82 mentions TASK-136")
    require("TASK-136" in codex_prompt_template, "docs/12 mentions TASK-136")
    require("TASK-137" in shell_accounting_dataservice_facts_query_gate, "docs/81 mentions TASK-137")
    require("TASK-137" in shell_accounting_dataservice_facts_query_test_plan, "docs/82 mentions TASK-137")
    require("TASK-137" in shell_accounting_dataservice_facts_query_implementation, "docs/83 mentions TASK-137")
    require("TASK-137" in codex_prompt_template, "docs/12 mentions TASK-137")
    require(
        "facts query is not write path" in shell_accounting_dataservice_facts_query_gate,
        "docs/81 states facts query is not write path",
    )
    require(
        "facts query is not AccountingEngine replay" in shell_accounting_dataservice_facts_query_gate,
        "docs/81 states no AccountingEngine replay",
    )
    require("rollback" in shell_accounting_dataservice_facts_query_gate, "docs/81 includes rollback")
    require(
        "Test Matrix" in shell_accounting_dataservice_facts_query_test_plan,
        "docs/82 includes Test Matrix",
    )
    require(
        "rollback" in shell_accounting_dataservice_facts_query_implementation,
        "docs/83 includes rollback",
    )
    require(
        "no AccountingEngine replay" in shell_accounting_dataservice_facts_query_implementation,
        "docs/83 states no replay",
    )
    for ctest_name in [
        "shell_accounting_dataservice_facts_query_gate",
        "shell_accounting_dataservice_facts_query_no_query_yet",
        "shell_accounting_dataservice_facts_query_allowed_sources_policy",
        "shell_accounting_dataservice_facts_query_no_write_sql",
        "shell_accounting_dataservice_facts_query_no_snapshot_rebuild",
        "shell_accounting_dataservice_facts_query_no_accounting_engine_replay",
        "shell_accounting_dataservice_facts_query_error_mapping_policy",
        "shell_accounting_dataservice_facts_query_privacy_policy",
        "shell_accounting_dataservice_facts_query_rollback_policy",
    ]:
        require(
            ctest_name in shell_accounting_dataservice_facts_query_gate_cmake,
            f"TASK-136 tests include {ctest_name}",
        )
    data_service_actions_source = (root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp").read_text(
        encoding="utf-8"
    )
    for action in [
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
        "base_position.summary",
        "sniper_pool.summary",
    ]:
        require(action in data_service_actions_source, f"DataService action source contains {action}")
    for ctest_name in [
        "shell_accounting_dataservice_facts_query_implementation",
        "shell_accounting_dataservice_facts_query_position_list_success",
        "shell_accounting_dataservice_facts_query_cash_summary_success",
        "shell_accounting_dataservice_facts_query_portfolio_pnl_summary_success",
        "shell_accounting_dataservice_facts_query_base_position_summary_success",
        "shell_accounting_dataservice_facts_query_sniper_pool_summary_success",
        "shell_accounting_dataservice_facts_query_empty_data_mapping",
        "shell_accounting_dataservice_facts_query_missing_data_mapping",
        "shell_accounting_dataservice_facts_query_query_error_mapping",
        "shell_accounting_dataservice_facts_query_no_write_sql_after_implementation",
        "shell_accounting_dataservice_facts_query_no_snapshot_rebuild_after_implementation",
        "shell_accounting_dataservice_facts_query_no_accounting_engine_replay_after_implementation",
        "shell_accounting_dataservice_facts_query_privacy_after_implementation",
        "shell_accounting_dataservice_facts_query_rollback_ready_after_implementation",
    ]:
        require(
            ctest_name in shell_accounting_dataservice_facts_query_implementation_cmake,
            f"TASK-137 tests include {ctest_name}",
        )
    for implementation_token in [
        "ShellAccountingReadOnlyFactsQuery",
        '\\"implemented\\":true',
        '\\"sqliteAccessed\\":true',
        '\\"accountingEngineCalled\\":',
        '\\"snapshotRebuilt\\":false',
        '\\"tradeDraftGenerated\\":false',
    ]:
        require(
            implementation_token in data_service_actions_source,
            f"DataService read-only facts implementation keeps {implementation_token}",
        )
    readonly_query_source = (root / "libs" / "DataAccess" / "src" / "ShellAccountingReadOnlyFactsQuery.cpp").read_text(
        encoding="utf-8"
    )
    require("SELECT " in readonly_query_source, "read-only facts query source contains SELECT")
    for forbidden_query_token in ["INSERT ", "UPDATE ", "DELETE ", "REPLACE ", "AccountingReplayEngine", "replayFromFacts"]:
        require(
            forbidden_query_token not in readonly_query_source,
            f"read-only facts query source avoids {forbidden_query_token}",
        )
    shell_accounting_production_path_after_137 = "\n".join(
        path.read_text(encoding="utf-8")
        for path in [
            root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp",
            root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp",
            root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml",
        ]
    )
    for forbidden_shell_token in ["SQLiteConnection", "DataAccess", "AccountingReplayEngine", "TradeDraft"]:
        require(
            forbidden_shell_token not in shell_accounting_production_path_after_137,
            f"ShellAccounting production path avoids {forbidden_shell_token} after TASK-137",
        )

    require(
        shell_accounting_replay_snapshot_rebuild_gate_path.exists(),
        "docs/84 replay snapshot rebuild gate exists",
    )
    require(
        shell_accounting_replay_snapshot_rebuild_test_plan_path.exists(),
        "docs/85 replay snapshot rebuild test plan exists",
    )
    require(
        "docs/84_shell_accounting_replay_snapshot_rebuild_gate.md" in readme,
        "README links docs/84",
    )
    require(
        "docs/85_shell_accounting_replay_snapshot_rebuild_test_plan.md" in readme,
        "README links docs/85",
    )
    require(
        "84_shell_accounting_replay_snapshot_rebuild_gate.md" in docs_index,
        "docs/README links docs/84",
    )
    require(
        "85_shell_accounting_replay_snapshot_rebuild_test_plan.md" in docs_index,
        "docs/README links docs/85",
    )
    require("TASK-138" in shell_accounting_replay_snapshot_rebuild_gate, "docs/84 mentions TASK-138")
    require(
        "Test Matrix" in shell_accounting_replay_snapshot_rebuild_test_plan,
        "docs/85 includes Test Matrix",
    )
    require("TASK-138" in codex_prompt_template, "docs/12 mentions TASK-138")
    for ctest_name in [
        "shell_accounting_replay_snapshot_rebuild_gate",
        "shell_accounting_replay_snapshot_rebuild_no_replay_yet",
        "shell_accounting_replay_snapshot_rebuild_no_snapshot_write_yet",
        "shell_accounting_replay_snapshot_rebuild_dataservice_only_policy",
        "shell_accounting_replay_snapshot_rebuild_no_ui_direct_engine_access",
        "shell_accounting_replay_snapshot_rebuild_no_write_path",
        "shell_accounting_replay_snapshot_rebuild_no_trade_or_strategy",
        "shell_accounting_replay_snapshot_rebuild_error_mapping_policy",
        "shell_accounting_replay_snapshot_rebuild_rollback_policy",
    ]:
        require(
            ctest_name in shell_accounting_replay_snapshot_rebuild_gate_cmake,
            f"TASK-138 tests include {ctest_name}",
        )
    require(
        "TradeDraft" not in shell_accounting_readonly_page
        and "买入" not in shell_accounting_readonly_page
        and "卖出" not in shell_accounting_readonly_page
        and "下单" not in shell_accounting_readonly_page,
        "production QML has no new trading UI after TASK-138",
    )
    shell_and_qml_after_138 = "\n".join(
        [
            registration_sources,
            shell_accounting_readonly_page,
            shell_accounting_presenter_header,
            shell_accounting_presenter_source,
            shell_accounting_controller_header,
            shell_accounting_controller_source,
        ]
    )
    for forbidden_replay_token in [
        "AccountingReplayEngine",
        "replayFromFacts",
        "SQLiteConnection",
        "sqlite3",
        "ShellAccountingReadOnlyFactsQuery",
    ]:
        require(
            forbidden_replay_token not in shell_and_qml_after_138,
            f"Shell/QML avoids direct replay or storage token {forbidden_replay_token} after TASK-138",
        )
    require(
        "TASK-138" in shell_accounting_dataservice_facts_query_gate
        and "TASK-138" in shell_accounting_dataservice_facts_query_test_plan
        and "TASK-138" in shell_accounting_dataservice_facts_query_implementation,
        "docs/81 docs/82 docs/83 mention TASK-138",
    )
    require(
        shell_accounting_readonly_replay_implementation_path.exists(),
        "docs/86 read-only replay implementation exists",
    )
    require(
        "docs/86_shell_accounting_readonly_replay_implementation.md" in readme,
        "README links docs/86",
    )
    require(
        "86_shell_accounting_readonly_replay_implementation.md" in docs_index,
        "docs/README links docs/86",
    )
    require("TASK-139" in shell_accounting_readonly_replay_implementation, "docs/86 mentions TASK-139")
    require("TASK-139" in codex_prompt_template, "docs/12 mentions TASK-139")
    for ctest_name in [
        "shell_accounting_readonly_replay_implementation",
        "shell_accounting_readonly_replay_dataservice_only_boundary",
        "shell_accounting_readonly_replay_position_list_success",
        "shell_accounting_readonly_replay_cash_summary_success",
        "shell_accounting_readonly_replay_portfolio_pnl_summary_success",
        "shell_accounting_readonly_replay_base_position_summary_success",
        "shell_accounting_readonly_replay_sniper_pool_summary_success",
        "shell_accounting_readonly_replay_missing_facts_mapping",
        "shell_accounting_readonly_replay_missing_market_price_mapping",
        "shell_accounting_readonly_replay_missing_fx_mapping",
        "shell_accounting_readonly_replay_inconsistent_facts_mapping",
        "shell_accounting_readonly_replay_no_snapshot_rebuild",
        "shell_accounting_readonly_replay_no_snapshot_write",
        "shell_accounting_readonly_replay_no_trade_or_strategy",
        "shell_accounting_readonly_replay_privacy",
        "shell_accounting_readonly_replay_rollback_ready",
    ]:
        require(
            ctest_name in shell_accounting_readonly_replay_implementation_cmake,
            f"TASK-139 tests include {ctest_name}",
        )
    require(
        "AccountingReplayEngine engine" in dataservice_actions_source
        and "calculationMode" in dataservice_actions_source
        and "readonlyReplay" in dataservice_actions_source,
        "DataService contains explicit read-only replay implementation",
    )
    require(
        "snapshotRebuilt\":true" not in dataservice_actions_source,
        "TASK-139 does not enable snapshot rebuild",
    )
    require(
        "TASK-139" in shell_accounting_replay_snapshot_rebuild_gate
        and "TASK-139" in shell_accounting_replay_snapshot_rebuild_test_plan,
        "docs/84 docs/85 mention TASK-139",
    )

    require(
        "v0.4.0-accounting-engine-replay-skeleton" in release_notes_v04,
        "v0.4 release notes include suggested tag",
    )
    require("AccountingEngine production-side DTO-only replay skeleton" in release_notes_v04, "v0.4 release notes summarize skeleton")
    require("DataService action" in release_notes_v04, "v0.4 release notes list DataService action as not included")
    require("SQLite integration" in release_notes_v04, "v0.4 release notes list SQLite integration as not included")
    require(
        "scenario coverage milestone does not imply productionReady=true" in codex_prompt_template,
        "prompt template states scenario coverage milestone does not imply productionReady true",
    )

    require("add_subdirectory(libs/AccountingEngine)" in root_cmake, "root CMake adds AccountingEngine")
    require("add_subdirectory(AccountingEngine)" in tests_cmake, "tests CMake adds AccountingEngine tests")
    require("add_library(ETFAccountingEngine" in accounting_engine_cmake, "AccountingEngine CMake defines library")
    require("ETFDecisionTerminal::AccountingEngine" in accounting_engine_cmake, "AccountingEngine CMake defines alias")
    require("add_test(NAME accounting_engine_boundary" in accounting_engine_test_cmake, "AccountingEngine CMake registers boundary test")
    require(
        "add_test(NAME accounting_replay_dto_parser_boundary" in accounting_engine_test_cmake,
        "AccountingEngine CMake registers DTO parser boundary test",
    )
    require(
        "add_test(NAME accounting_replay_engine_empty_ledger" in accounting_engine_test_cmake,
        "AccountingEngine CMake registers empty ledger replay test",
    )
    require(
        "add_test(NAME accounting_replay_engine_single_buy" in accounting_engine_test_cmake,
        "AccountingEngine CMake registers single buy replay test",
    )
    require(
        "add_test(NAME accounting_replay_engine_buy_sell_partial" in accounting_engine_test_cmake,
        "AccountingEngine CMake registers buy-sell partial replay test",
    )
    require(
        "add_test(NAME accounting_replay_engine_sell_exceeds_position" in accounting_engine_test_cmake,
        "AccountingEngine CMake registers sell exceeds position test",
    )
    require(
        "add_test(NAME accounting_replay_engine_missing_fee" in accounting_engine_test_cmake,
        "AccountingEngine CMake registers missing fee test",
    )
    require(
        "add_test(NAME accounting_replay_engine_negative_cash" in accounting_engine_test_cmake,
        "AccountingEngine CMake registers negative cash test",
    )
    require(
        "add_test(NAME accounting_replay_engine_multi_instrument_buy" in accounting_engine_test_cmake,
        "AccountingEngine CMake registers multi-instrument buy test",
    )
    require(
        "add_test(NAME accounting_replay_engine_multi_account_buy" in accounting_engine_test_cmake,
        "AccountingEngine CMake registers multi-account buy test",
    )
    require(
        "add_test(NAME accounting_replay_engine_multi_currency_unsupported" in accounting_engine_test_cmake,
        "AccountingEngine CMake registers multi-currency unsupported test",
    )
    require(
        "add_test(NAME accounting_replay_engine_missing_market_price" in accounting_engine_test_cmake,
        "AccountingEngine CMake registers missing market price test",
    )
    require("AccountingEngineBoundaryTests" in accounting_engine_test_cmake, "AccountingEngine boundary test target exists")
    require("accountingEngineBoundary" in accounting_engine_boundary_test, "AccountingEngine boundary test source exists")
    require("AccountingReplayDtoParserBoundaryTests" in accounting_engine_test_cmake, "AccountingEngine DTO parser test target exists")
    require("AccountingReplayEngineEmptyLedgerTests" in accounting_engine_test_cmake, "AccountingEngine empty ledger test target exists")
    require("AccountingReplayEngineSingleBuyTests" in accounting_engine_test_cmake, "AccountingEngine single buy test target exists")
    require(
        "AccountingReplayEngineBuySellPartialTests" in accounting_engine_test_cmake,
        "AccountingEngine buy-sell partial test target exists",
    )
    require(
        "AccountingReplayEngineSellExceedsPositionTests" in accounting_engine_test_cmake,
        "AccountingEngine sell exceeds position test target exists",
    )
    require(
        "AccountingReplayEngineMissingFeeTests" in accounting_engine_test_cmake,
        "AccountingEngine missing fee test target exists",
    )
    require(
        "AccountingReplayEngineNegativeCashTests" in accounting_engine_test_cmake,
        "AccountingEngine negative cash test target exists",
    )
    require(
        "AccountingReplayEngineMultiInstrumentBuyTests" in accounting_engine_test_cmake,
        "AccountingEngine multi-instrument buy test target exists",
    )
    require(
        "AccountingReplayEngineMultiAccountBuyTests" in accounting_engine_test_cmake,
        "AccountingEngine multi-account buy test target exists",
    )
    require(
        "AccountingReplayEngineMultiCurrencyUnsupportedTests" in accounting_engine_test_cmake,
        "AccountingEngine multi-currency unsupported test target exists",
    )
    require(
        "AccountingReplayEngineMissingMarketPriceTests" in accounting_engine_test_cmake,
        "AccountingEngine missing market price test target exists",
    )
    require("AccountingReplayEngine" in accounting_replay_engine_header, "AccountingReplayEngine exists")
    require("replayReadOnly" in accounting_replay_engine_header, "AccountingReplayEngine exposes replayReadOnly")
    require("AccountingReplayResult" in accounting_replay_result_header_prod, "production AccountingReplayResult exists")
    require("AccountingReplayStatus" in accounting_replay_result_header_prod, "AccountingReplayStatus exists")
    require("PositionListResponseDto" in accounting_replay_result_header_prod, "PositionListResponseDto exists")
    require("CashSummaryDto" in accounting_replay_result_header_prod, "CashSummaryDto exists")
    require("PortfolioPnlDto" in accounting_replay_result_header_prod, "PortfolioPnlDto exists")
    require("AccountingReplayEngine" in accounting_replay_engine_empty_ledger_test, "empty ledger test uses AccountingReplayEngine")
    require("0.00 CNY" in accounting_replay_engine_empty_ledger_test, "empty ledger test asserts stable zero CNY")
    require("1001.00 CNY" in accounting_replay_engine_single_buy_test, "single buy test asserts cost amount")
    require("98999.00 CNY" in accounting_replay_engine_single_buy_test, "single buy test asserts cash balance")
    require("UNAVAILABLE" in accounting_replay_engine_single_buy_test, "single buy test keeps PnL unavailable")
    require("600.60 CNY" in accounting_replay_engine_buy_sell_partial_test, "partial sell test asserts remaining cost")
    require("99478.00 CNY" in accounting_replay_engine_buy_sell_partial_test, "partial sell test asserts cash balance")
    require("78.60 CNY" in accounting_replay_engine_buy_sell_partial_test, "partial sell test asserts realized PnL")
    require("SELL_EXCEEDS_POSITION" in accounting_replay_engine_buy_sell_partial_test, "partial sell test asserts oversell issue")
    require("UNAVAILABLE" in accounting_replay_engine_buy_sell_partial_test, "partial sell test keeps valuation unavailable")
    require(
        "SELL_EXCEEDS_POSITION" in accounting_replay_engine_sell_exceeds_position_test,
        "sell exceeds test asserts issue code",
    )
    require(
        "sellExceedsPositionDetectionSupported" in accounting_replay_engine_sell_exceeds_position_test,
        "sell exceeds test asserts capability flag",
    )
    require(
        "positionList.positions.empty" in accounting_replay_engine_sell_exceeds_position_test,
        "sell exceeds test asserts no success positions",
    )
    require("MISSING_FEE" in accounting_replay_engine_missing_fee_test, "missing fee test asserts issue code")
    require(
        "missingFeeDetectionSupported" in accounting_replay_engine_missing_fee_test,
        "missing fee test asserts capability flag",
    )
    require("0.00" in accounting_replay_engine_missing_fee_test, "missing fee test verifies explicit zero fee")
    require(
        "positionList.positions.empty" in accounting_replay_engine_missing_fee_test,
        "missing fee test asserts no success positions",
    )
    require("NEGATIVE_CASH" in accounting_replay_engine_negative_cash_test, "negative cash test asserts issue code")
    require(
        "negativeCashDetectionSupported" in accounting_replay_engine_negative_cash_test,
        "negative cash test asserts capability flag",
    )
    require("0.00 CNY" in accounting_replay_engine_negative_cash_test, "negative cash test asserts exact zero cash")
    require(
        "MISSING_FEE" in accounting_replay_engine_negative_cash_test,
        "negative cash test asserts missing fee precedence",
    )
    require(
        "positionList.positions.empty" in accounting_replay_engine_negative_cash_test,
        "negative cash test asserts no success positions",
    )
    require(
        "multiInstrumentBuyReplaySupported" in accounting_replay_engine_multi_instrument_buy_test,
        "multi-instrument buy test asserts capability flag",
    )
    require("159509" in accounting_replay_engine_multi_instrument_buy_test, "multi-instrument buy test asserts 159509")
    require("518880" in accounting_replay_engine_multi_instrument_buy_test, "multi-instrument buy test asserts 518880")
    require(
        "96998.00 CNY" in accounting_replay_engine_multi_instrument_buy_test,
        "multi-instrument buy test asserts cash balance",
    )
    require(
        "marketValueText == \"UNAVAILABLE\"" in accounting_replay_engine_multi_instrument_buy_test,
        "multi-instrument buy test asserts no fabricated market value",
    )
    require(
        "unrealizedPnlText == \"UNAVAILABLE\"" in accounting_replay_engine_multi_instrument_buy_test,
        "multi-instrument buy test asserts no fabricated unrealized pnl",
    )
    require(
        "multiAccountBuyReplaySupported" in accounting_replay_engine_multi_account_buy_test,
        "multi-account buy test asserts capability flag",
    )
    require("ACC-DEMO-001" in accounting_replay_engine_multi_account_buy_test, "multi-account buy test asserts account one")
    require("ACC-DEMO-002" in accounting_replay_engine_multi_account_buy_test, "multi-account buy test asserts account two")
    require(
        "48999.00 CNY" in accounting_replay_engine_multi_account_buy_test,
        "multi-account buy test asserts account one cash balance",
    )
    require(
        "47999.00 CNY" in accounting_replay_engine_multi_account_buy_test,
        "multi-account buy test asserts account two cash balance",
    )
    require("accountCashSummaries" in accounting_replay_engine_multi_account_buy_test, "multi-account buy test asserts per-account cash summaries")
    require(
        "marketValueText == \"UNAVAILABLE\"" in accounting_replay_engine_multi_account_buy_test,
        "multi-account buy test asserts no fabricated market value",
    )
    require(
        "unrealizedPnlText == \"UNAVAILABLE\"" in accounting_replay_engine_multi_account_buy_test,
        "multi-account buy test asserts no fabricated unrealized pnl",
    )
    require(
        "multiCurrencyUnsupportedDetectionSupported" in accounting_replay_engine_multi_currency_unsupported_test,
        "multi-currency unsupported test asserts capability flag",
    )
    require(
        "MULTI_CURRENCY_UNSUPPORTED" in accounting_replay_engine_multi_currency_unsupported_test,
        "multi-currency unsupported test asserts multi-currency issue code",
    )
    require(
        "FX_RATE_MISSING" in accounting_replay_engine_multi_currency_unsupported_test,
        "multi-currency unsupported test asserts FX rate issue code",
    )
    require("USD" in accounting_replay_engine_multi_currency_unsupported_test, "multi-currency unsupported test asserts USD input")
    require("supportsFxRate" in accounting_replay_engine_multi_currency_unsupported_test, "multi-currency unsupported test asserts supportsFxRate false")
    require(
        "supportsMultiCurrency" in accounting_replay_engine_multi_currency_unsupported_test,
        "multi-currency unsupported test asserts supportsMultiCurrency false",
    )
    require(
        "totalAssetsText.empty()" in accounting_replay_engine_multi_currency_unsupported_test,
        "multi-currency unsupported test asserts no fabricated total assets",
    )
    require(
        "marketValueText" in accounting_replay_engine_multi_currency_unsupported_test,
        "multi-currency unsupported test asserts no fabricated market value",
    )
    require(
        "unrealizedPnlText.empty()" in accounting_replay_engine_multi_currency_unsupported_test,
        "multi-currency unsupported test asserts no fabricated unrealized pnl",
    )
    require(
        "missingMarketPriceDetectionSupported" in accounting_replay_engine_missing_market_price_test,
        "missing market price test asserts capability flag",
    )
    require(
        "MARKET_PRICE_MISSING" in accounting_replay_engine_missing_market_price_test,
        "missing market price test asserts issue code",
    )
    require(
        "AccountingReplayStatus::Warning" in accounting_replay_engine_missing_market_price_test,
        "missing market price test asserts warning status",
    )
    require(
        "1001.00 CNY" in accounting_replay_engine_missing_market_price_test,
        "missing market price test asserts position cost",
    )
    require(
        "98999.00 CNY" in accounting_replay_engine_missing_market_price_test,
        "missing market price test asserts cash balance",
    )
    require(
        "marketValueText == \"UNAVAILABLE\"" in accounting_replay_engine_missing_market_price_test,
        "missing market price test asserts no fabricated market value",
    )
    require(
        "unrealizedPnlText == \"UNAVAILABLE\"" in accounting_replay_engine_missing_market_price_test,
        "missing market price test asserts no fabricated unrealized pnl",
    )
    require(
        "totalAssetsText == \"UNAVAILABLE\"" in accounting_replay_engine_missing_market_price_test,
        "missing market price test asserts no fabricated total assets",
    )
    require(
        "supportsMarketPrice" in accounting_replay_engine_missing_market_price_test,
        "missing market price test asserts supportsMarketPrice false",
    )
    require("http://" not in accounting_engine_sources, "AccountingEngine sources do not contain http URL")
    require("https://" not in accounting_engine_sources, "AccountingEngine sources do not contain https URL")
    require("validateTradeFact" in accounting_replay_dto_parser_test, "DTO parser test validates trade facts")
    require("ReplayRequestDto" in accounting_replay_dtos_header, "ReplayRequestDto exists")
    require("TradeFactDto" in accounting_replay_dtos_header, "TradeFactDto exists")
    require("CashFactDto" in accounting_replay_dtos_header, "CashFactDto exists")
    require("MarketPriceFactDto" in accounting_replay_dtos_header, "MarketPriceFactDto exists")
    require("FxRateFactDto" in accounting_replay_dtos_header, "FxRateFactDto exists")
    require("validateReplayRequest" in accounting_replay_validation_header, "validateReplayRequest exists")
    require("validateTradeFact" in accounting_replay_validation_header, "validateTradeFact exists")
    require("validateCashFact" in accounting_replay_validation_header, "validateCashFact exists")
    require("validateMarketPriceFact" in accounting_replay_validation_header, "validateMarketPriceFact exists")
    require("validateFxRateFact" in accounting_replay_validation_header, "validateFxRateFact exists")
    require("parseTradeFactBoundary" in accounting_replay_parser_header, "parseTradeFactBoundary exists")

    for forbidden in ["DataAccess/", "DataServiceApi/", "DataServiceClient/", "ServiceHost/", "Watchdog/"]:
        require(forbidden not in accounting_engine_sources, f"AccountingEngine sources do not include {forbidden}")
    require("AccountingReplayMinimalEngine" not in accounting_engine_sources, "AccountingEngine sources do not copy minimal replay engine")
    require("tests/AccountingFixtures" not in accounting_engine_sources, "AccountingEngine sources do not reference test fixture sources")
    require("SQLiteConnection" not in accounting_engine_sources, "AccountingEngine sources do not reference SQLiteConnection directly")
    require("sqlite" not in accounting_engine_sources.lower(), "AccountingEngine sources do not reference sqlite")

    forbidden_cmake_links = [
        "DataAccess",
        "DataServiceApi",
        "DataServiceClient",
        "ServiceHost",
        "Watchdog",
        "Qt::Quick",
        "Qt6::Quick",
        "Qml",
    ]
    for forbidden in forbidden_cmake_links:
        require(forbidden not in accounting_engine_cmake, f"AccountingEngine CMake does not link {forbidden}")

    require("accounting.health" in accounting_rules, "accounting rules documents accounting.health")
    require("replayImplemented=false" in accounting_rules, "accounting rules state replay is not implemented")
    require("writeEnabled=false" in accounting_rules, "accounting rules state writes are disabled")
    require("accounting.replay.preview" in accounting_rules, "accounting rules documents replay preview guard")
    require("REPLAY_NOT_AVAILABLE" in accounting_rules, "accounting rules documents replay unavailable status")

    require("accounting.health" in protocol_doc, "protocol doc documents accounting.health")
    require("replayImplemented" in protocol_doc, "protocol doc includes replayImplemented")
    require("writeEnabled" in protocol_doc, "protocol doc includes writeEnabled")
    require("REPLAY_NOT_IMPLEMENTED" in protocol_doc, "protocol doc includes replay warning")
    require("position.list" in protocol_doc, "protocol doc lists future position action")
    require("accounting.replay.preview" in protocol_doc, "protocol doc documents replay preview guard")
    require("implemented=false" in protocol_doc, "protocol doc states preview guard is not implemented")
    require("replayExecuted=false" in protocol_doc, "protocol doc states replay is not executed")
    require("REPLAY_NOT_AVAILABLE" in protocol_doc, "protocol doc includes replay unavailable status")
    require("FX001_EMPTY_LEDGER" in protocol_doc, "protocol doc includes first required fixture")
    require("FX013_MULTI_CURRENCY_UNSUPPORTED" in protocol_doc, "protocol doc includes last required fixture")

    require("trade_log 是事实账本" in accounting_rules, "accounting rules state trade_log is fact ledger")
    require("position_accounting_boundary" in accounting_rules, "accounting rules link position boundary")

    require("trade_log 是唯一事实账本" in position_boundary, "boundary doc states trade_log is the only fact ledger")
    require("position_snapshot 是派生数据" in position_boundary, "boundary doc states position_snapshot is derived")
    require("cash_snapshot 是派生数据" in position_boundary, "boundary doc states cash_snapshot is derived")
    require("portfolio_summary 是派生数据" in position_boundary, "boundary doc states portfolio_summary is derived")
    require("20% 底仓" in position_boundary, "boundary doc states 20 percent base position")
    require("80% 狙击资金池" in position_boundary, "boundary doc states 80 percent sniper pool")
    require("不写 trade_log" in position_boundary, "boundary doc prohibits trade_log writes")
    require("不写快照表" in position_boundary, "boundary doc prohibits snapshot writes")
    require("QML 不计算成本" in position_boundary, "boundary doc prohibits QML cost calculation")
    require("accounting.health" in position_boundary, "boundary doc drafts accounting health action")
    require(
        "TASK-048 Implemented Minimal Accounting Health" in position_boundary,
        "boundary doc records implemented accounting health",
    )
    require("TASK-049 Implemented Replay Preview Guard" in position_boundary, "boundary doc records replay preview guard")
    require("FX001-FX013" in position_boundary, "boundary doc references required fixture range")
    require("ShellPositionSummaryViewModel" in position_boundary, "boundary doc drafts position ViewModel")

    require("PositionSummaryDto" in position_contract, "position contract includes PositionSummaryDto")
    require("CashSummaryDto" in position_contract, "position contract includes CashSummaryDto")
    require("PortfolioPnlDto" in position_contract, "position contract includes PortfolioPnlDto")
    require("BasePositionDto" in position_contract, "position contract includes BasePositionDto")
    require("SniperPoolDto" in position_contract, "position contract includes SniperPoolDto")
    require("当前未实现" in position_contract, "position contract states currently not implemented")
    require("DTOs must not write to the database" in position_contract, "position contract prohibits DTO writes")
    require("22_position_accounting_data_contract.md" in position_boundary, "boundary doc links stable data contract")
    require("23_position_accounting_test_fixture_design.md" in position_boundary, "boundary doc links fixture design")
    require("22_position_accounting_data_contract.md" in position_contract, "draft contract links stable data contract")

    require("DataQualityStatus" in position_stable_contract, "stable contract includes DataQualityStatus")
    require("AccountingErrorCode" in position_stable_contract, "stable contract includes AccountingErrorCode")
    require("CalculationMetadataDto" in position_stable_contract, "stable contract includes CalculationMetadataDto")
    require("AccountingIssueDto" in position_stable_contract, "stable contract includes AccountingIssueDto")
    require("PositionSummaryDto" in position_stable_contract, "stable contract includes PositionSummaryDto")
    require("PositionListResponse" in position_stable_contract, "stable contract includes PositionListResponse")
    require("CashSummaryDto" in position_stable_contract, "stable contract includes CashSummaryDto")
    require("PortfolioPnlDto" in position_stable_contract, "stable contract includes PortfolioPnlDto")
    require("BasePositionDto" in position_stable_contract, "stable contract includes BasePositionDto")
    require("SniperPoolDto" in position_stable_contract, "stable contract includes SniperPoolDto")
    require("accounting.health" in position_stable_contract, "stable contract includes accounting.health")
    require("position.list" in position_stable_contract, "stable contract includes position.list")
    require(
        "TASK-048 Minimal Implemented Action Status" in position_stable_contract,
        "stable contract documents implemented accounting health",
    )
    require("REPLAY_NOT_IMPLEMENTED" in position_stable_contract, "stable contract includes replay warning")
    require("TASK-049 Replay Preview Guard Status" in position_stable_contract, "stable contract documents replay preview guard")
    require("REPLAY_NOT_AVAILABLE" in position_stable_contract, "stable contract includes replay unavailable status")
    require("FX001-FX013" in position_stable_contract, "stable contract references required fixtures")
    require("不写 trade_log" in position_stable_contract, "stable contract prohibits trade_log writes")
    require("QML 不计算账务" in position_stable_contract, "stable contract prohibits QML accounting calculation")
    require("ShellPositionListModel" in position_stable_contract, "stable contract includes ShellPositionListModel")
    require(
        "24_accounting_replay_fixture_samples.md" in position_stable_contract,
        "stable contract links accounting fixture samples",
    )
    require(
        "tests/fixtures/accounting_replay" in position_stable_contract,
        "stable contract links static accounting fixture files",
    )
    require(
        "25_position_shell_viewmodel_design.md" in position_stable_contract,
        "stable contract links position ViewModel design",
    )

    require("FX001_EMPTY_LEDGER" in position_fixture_design, "fixture design includes empty ledger")
    require(
        "FX013_MULTI_CURRENCY_UNSUPPORTED" in position_fixture_design,
        "fixture design includes unsupported multi-currency fixture",
    )
    require("Expected issues" in position_fixture_design, "fixture design includes expected issues")
    require(
        "no hidden external market dependency" in position_fixture_design,
        "fixture design prohibits hidden market dependency",
    )
    require("FX010_SNIPER_TIER_COMPLETED" in position_fixture_design, "fixture design includes sniper tier fixture")
    require(
        "24_accounting_replay_fixture_samples.md" in position_fixture_design,
        "fixture design links detailed samples",
    )
    require(
        "tests/fixtures/accounting_replay" in position_fixture_design,
        "fixture design links static fixture files",
    )

    for fixture_id in [
        "FX001_EMPTY_LEDGER",
        "FX002_SINGLE_BUY",
        "FX003_BUY_SELL_PARTIAL",
        "FX004_SELL_EXCEEDS_POSITION",
        "FX005_MISSING_FEE",
        "FX006_NEGATIVE_CASH",
        "FX007_MULTI_INSTRUMENT",
        "FX008_MULTI_ACCOUNT",
        "FX009_BASE_POSITION_LOCKED",
        "FX010_SNIPER_TIER_COMPLETED",
        "FX011_STALE_SNAPSHOT",
        "FX012_MISSING_MARKET_PRICE",
        "FX013_MULTI_CURRENCY_UNSUPPORTED",
    ]:
        require(fixture_id in accounting_fixture_samples, f"fixture samples include {fixture_id}")

    require("Expected PositionSummaryDto" in accounting_fixture_samples, "fixture samples include expected position output")
    require("Expected CashSummaryDto" in accounting_fixture_samples, "fixture samples include expected cash output")
    require("Expected PortfolioPnlDto" in accounting_fixture_samples, "fixture samples include expected pnl output")
    require("Expected issues" in accounting_fixture_samples, "fixture samples include expected issues")
    require(accounting_fixture_samples.count("### Input facts") >= 13, "each fixture includes input facts")
    require(
        accounting_fixture_samples.count("### Expected PositionSummaryDto") >= 13,
        "each fixture includes expected position output",
    )
    require(
        accounting_fixture_samples.count("### Expected CashSummaryDto") >= 13,
        "each fixture includes expected cash output",
    )
    require(
        accounting_fixture_samples.count("### Expected PortfolioPnlDto") >= 13,
        "each fixture includes expected pnl output",
    )
    require(accounting_fixture_samples.count("### Expected issues") >= 13, "each fixture includes expected issues")
    require(
        "no hidden external market dependency" in accounting_fixture_samples,
        "fixture samples prohibit hidden market dependency",
    )
    require("trade_log 是事实账本" in accounting_fixture_samples, "fixture samples state trade_log fact ledger")
    require("不写数据库" in accounting_fixture_samples, "fixture samples state no database writes")
    require(
        "25_position_shell_viewmodel_design.md" in accounting_fixture_samples,
        "fixture samples link position ViewModel design",
    )
    require("accounting.replay.preview" in accounting_fixture_samples, "fixture samples mention replay preview guard")
    require("implemented=false" in accounting_fixture_samples, "fixture samples state guard implemented false")
    require("replayExecuted=false" in accounting_fixture_samples, "fixture samples state replay is not executed")
    require(
        "tests/fixtures/accounting_replay" in accounting_fixture_samples,
        "fixture samples doc links static fixture files",
    )
    require("AccountingFixtureLoader" in accounting_fixture_samples, "fixture samples doc documents loader")
    require("does not calculate cash balances" in accounting_fixture_samples, "fixture samples doc states loader does not calculate")
    require("does not access SQLite" in accounting_fixture_samples, "fixture samples doc states loader does not access SQLite")
    require("does not call DataService" in accounting_fixture_samples, "fixture samples doc states loader does not call service")
    require("AccountingReplayTestHarness" in accounting_fixture_samples, "fixture samples doc documents replay harness")
    require("AccountingReplayStubEngine" in accounting_fixture_samples, "fixture samples doc documents stub engine")
    require("AccountingReplayResult" in accounting_fixture_samples, "fixture samples doc documents replay result")
    require("AccountingReplayResultMapper" in accounting_fixture_samples, "fixture samples doc documents result mapper")
    require(
        "AccountingExpectedOutputInspector" in accounting_fixture_samples,
        "fixture samples doc documents expected output inspector",
    )
    require(
        "AccountingReplayAssertionSkeleton" in accounting_fixture_samples,
        "fixture samples doc documents assertion skeleton",
    )
    require("SKIPPED_BY_DESIGN" in accounting_fixture_samples, "fixture samples doc documents skipped assertions")
    require("AccountingReplayMinimalEngine" in accounting_fixture_samples, "fixture samples doc documents minimal engine")
    require("FX001_EMPTY_LEDGER" in accounting_fixture_samples, "fixture samples doc documents FX001 minimal replay")
    require("FX002_SINGLE_BUY" in accounting_fixture_samples, "fixture samples doc documents FX002 remains guarded")
    require("costAmountText=1001.00 CNY" in accounting_fixture_samples, "fixture samples doc documents FX002 fee-included cost")
    require("cashBalanceText=98999.00 CNY" in accounting_fixture_samples, "fixture samples doc documents FX002 cash balance")
    require("FX003_BUY_SELL_PARTIAL" in accounting_fixture_samples, "fixture samples doc documents FX003 minimal replay")
    require("costAmountText=600.60 CNY" in accounting_fixture_samples, "fixture samples doc documents FX003 remaining cost")
    require("cashBalanceText=99478.00 CNY" in accounting_fixture_samples, "fixture samples doc documents FX003 cash balance")
    require("realizedPnlText=78.60 CNY" in accounting_fixture_samples, "fixture samples doc documents FX003 realized PnL")
    require(
        "realizedPnl = netSellInflow - allocatedSoldCost" in accounting_fixture_samples,
        "fixture samples doc documents FX003 realized PnL formula",
    )
    require("FX004_SELL_EXCEEDS_POSITION" in accounting_fixture_samples, "fixture samples doc documents FX004 minimal replay")
    require("SELL_EXCEEDS_POSITION" in accounting_fixture_samples, "fixture samples doc documents FX004 issue code")
    require("status=ERROR" in accounting_fixture_samples, "fixture samples doc documents FX004 error status")
    require("blocking" in accounting_fixture_samples, "fixture samples doc documents FX004 blocking issue")
    require("does not generate negative holdings" in accounting_fixture_samples, "fixture samples doc documents FX004 no negative holdings")
    require("FX005_MISSING_FEE" in accounting_fixture_samples, "fixture samples doc documents FX005 minimal replay")
    require("MISSING_FEE" in accounting_fixture_samples, "fixture samples doc documents FX005 issue code")
    require("status=WARNING" in accounting_fixture_samples, "fixture samples doc documents FX005 warning status")
    require("non-blocking" in accounting_fixture_samples, "fixture samples doc documents FX005 non-blocking issue")
    require("fee=0" in accounting_fixture_samples, "fixture samples doc documents FX005 no zero-fee fallback")
    require("FX006_NEGATIVE_CASH" in accounting_fixture_samples, "fixture samples doc documents FX006 minimal replay")
    require("NEGATIVE_CASH" in accounting_fixture_samples, "fixture samples doc documents FX006 issue code")
    require("status=ERROR" in accounting_fixture_samples, "fixture samples doc documents FX006 error status")
    require("blocking `NEGATIVE_CASH`" in accounting_fixture_samples, "fixture samples doc documents FX006 blocking issue")
    require(
        "buy cash requirement exceeds available initial cash" in accounting_fixture_samples,
        "fixture samples doc documents FX006 negative-cash trigger",
    )
    require("FX007_MULTI_INSTRUMENT" in accounting_fixture_samples, "fixture samples doc documents FX007 minimal replay")
    require("MARKET_PRICE_MISSING" in accounting_fixture_samples, "fixture samples doc documents FX007 issue code")
    require("cashBalanceText=96998.00 CNY" in accounting_fixture_samples, "fixture samples doc documents FX007 cash balance")
    require("does not mix their `instrumentCode`" in accounting_fixture_samples, "fixture samples doc documents FX007 instrument separation")
    require("does not fabricate valuation or unrealized PnL" in accounting_fixture_samples, "fixture samples doc documents FX007 no fake valuation")
    require("FX008_MULTI_ACCOUNT" in accounting_fixture_samples, "fixture samples doc documents FX008 minimal replay")
    require("accountCashSummaries" in accounting_fixture_samples, "fixture samples doc documents FX008 account cash summaries")
    require("cashBalanceText=48999.00 CNY" in accounting_fixture_samples, "fixture samples doc documents FX008 account one cash")
    require("cashBalanceText=47999.00 CNY" in accounting_fixture_samples, "fixture samples doc documents FX008 account two cash")
    require("does not merge or mix their `accountId`" in accounting_fixture_samples, "fixture samples doc documents FX008 account separation")
    require("FX009_BASE_POSITION_LOCKED" in accounting_fixture_samples, "fixture samples doc documents FX009 minimal replay")
    require("basePositionRaw" in accounting_fixture_samples, "fixture samples doc documents FX009 base position raw")
    require("targetBaseRatioText=20%" in accounting_fixture_samples, "fixture samples doc documents FX009 target ratio")
    require("lockedBaseAmountText=20000.00 CNY" in accounting_fixture_samples, "fixture samples doc documents FX009 locked amount")
    require("sellableAboveBaseAmountText=0.00 CNY" in accounting_fixture_samples, "fixture samples doc documents FX009 sellable amount")
    require("not a sell suggestion" in accounting_fixture_samples, "fixture samples doc documents FX009 no sell suggestion")
    require("does not produce `sniperPoolRaw`" in accounting_fixture_samples, "fixture samples doc documents FX009 no sniper pool")
    require("FX012_MISSING_MARKET_PRICE" in accounting_fixture_samples, "fixture samples doc documents FX012 minimal replay")
    require("quantity / cost can display" in accounting_fixture_samples, "fixture samples doc documents FX012 quantity cost display")
    require("does not fabricate `marketValueText`" in accounting_fixture_samples, "fixture samples doc documents FX012 no fake market value")
    require("does not fabricate `unrealizedPnlText`" in accounting_fixture_samples, "fixture samples doc documents FX012 no fake unrealized pnl")
    require("does not query real market data" in accounting_fixture_samples, "fixture samples doc documents FX012 no market service")
    require("FX013_MULTI_CURRENCY_UNSUPPORTED" in accounting_fixture_samples, "fixture samples doc documents FX013 minimal replay")
    require("MULTI_CURRENCY_UNSUPPORTED" in accounting_fixture_samples, "fixture samples doc documents FX013 multi-currency issue")
    require("FX_RATE_MISSING" in accounting_fixture_samples, "fixture samples doc documents FX013 FX issue")
    require("does not perform FX conversion" in accounting_fixture_samples, "fixture samples doc documents FX013 no FX conversion")
    require("does not fabricate `totalAssetsText`" in accounting_fixture_samples, "fixture samples doc documents FX013 no fake total assets")
    require("test-only minimal engine coverage complete" in accounting_fixture_samples, "fixture samples doc states test-only coverage complete")
    require("This is not production replay" in accounting_fixture_samples, "fixture samples doc distinguishes production replay")
    require("Production implementation requires the" in accounting_fixture_samples, "fixture samples doc requires readiness review")
    require("status=NOT_IMPLEMENTED" in accounting_fixture_samples, "fixture samples doc documents NOT_IMPLEMENTED status")
    require("REPLAY_NOT_IMPLEMENTED" in accounting_fixture_samples, "fixture samples doc documents replay issue")
    require("positionListResponseRaw" in accounting_fixture_samples, "fixture samples doc documents position raw output")
    require("cashSummaryRaw" in accounting_fixture_samples, "fixture samples doc documents cash raw output")
    require("portfolioPnlRaw" in accounting_fixture_samples, "fixture samples doc documents pnl raw output")
    require("all FX001-FX013 fixtures are covered" in accounting_fixture_samples, "fixture samples doc documents full fixture coverage")

    require("ShellPositionListModel" in position_viewmodel_design, "ViewModel design includes ShellPositionListModel")
    require("ShellCashSummaryObject" in position_viewmodel_design, "ViewModel design includes ShellCashSummaryObject")
    require("ShellPortfolioPnlObject" in position_viewmodel_design, "ViewModel design includes ShellPortfolioPnlObject")
    require("ShellBasePositionObject" in position_viewmodel_design, "ViewModel design includes ShellBasePositionObject")
    require("ShellSniperPoolObject" in position_viewmodel_design, "ViewModel design includes ShellSniperPoolObject")
    require("ShellAccountingIssueListModel" in position_viewmodel_design, "ViewModel design includes issue model")
    require("不在 QML 中计算持仓" in position_viewmodel_design, "ViewModel design prohibits QML position calculation")
    require("不在 QML 中计算成本" in position_viewmodel_design, "ViewModel design prohibits QML cost calculation")
    require("不在 QML 中计算收益" in position_viewmodel_design, "ViewModel design prohibits QML return calculation")

    require("PositionSummaryDto" in position_mapping, "mapping doc includes PositionSummaryDto")
    require("CashSummaryDto" in position_mapping, "mapping doc includes CashSummaryDto")
    require("PortfolioPnlDto" in position_mapping, "mapping doc includes PortfolioPnlDto")
    require("BasePositionDto" in position_mapping, "mapping doc includes BasePositionDto")
    require("SniperPoolDto" in position_mapping, "mapping doc includes SniperPoolDto")
    require("AccountingIssueDto" in position_mapping, "mapping doc includes AccountingIssueDto")
    require("DTO field" in position_mapping, "mapping doc includes DTO field column")
    require("ViewModel role" in position_mapping, "mapping doc includes ViewModel role")
    require("QML display label" in position_mapping, "mapping doc includes QML display label")

    expected_fixture_ids = [
        "FX001_EMPTY_LEDGER",
        "FX002_SINGLE_BUY",
        "FX003_BUY_SELL_PARTIAL",
        "FX004_SELL_EXCEEDS_POSITION",
        "FX005_MISSING_FEE",
        "FX006_NEGATIVE_CASH",
        "FX007_MULTI_INSTRUMENT",
        "FX008_MULTI_ACCOUNT",
        "FX009_BASE_POSITION_LOCKED",
        "FX010_SNIPER_TIER_COMPLETED",
        "FX011_STALE_SNAPSHOT",
        "FX012_MISSING_MARKET_PRICE",
        "FX013_MULTI_CURRENCY_UNSUPPORTED",
    ]
    for fixture_id in expected_fixture_ids:
        fixture_path = accounting_fixture_dir / f"{fixture_id}.json"
        require(fixture_path.exists(), f"accounting replay fixture file exists: {fixture_id}")
        fixture_text = fixture_path.read_text(encoding="utf-8")
        require('"inputFacts"' in fixture_text, f"{fixture_id} has inputFacts")
        require('"expectedOutputs"' in fixture_text, f"{fixture_id} has expectedOutputs")
        require('"expectedIssues"' in fixture_text, f"{fixture_id} has expectedIssues")
        require('"blocking"' in fixture_text, f"{fixture_id} has blocking")
        require("data.audit.append" not in fixture_text, f"{fixture_id} does not reference data.audit.append")
        require("--serve-dev-audit" not in fixture_text, f"{fixture_id} does not reference serve-dev-audit")
        require("http://" not in fixture_text and "https://" not in fixture_text, f"{fixture_id} has no external URL")

    require("accounting_replay_v0_3" in accounting_fixture_index, "fixture index names fixture set")
    require("externalDependencies" in accounting_fixture_index, "fixture index declares external dependencies")
    require("not database rows" in accounting_fixture_index, "fixture index states fixtures are not database rows")
    require("do not trigger writes" in accounting_fixture_index, "fixture index states fixtures do not trigger writes")
    require(
        "SELL_EXCEEDS_POSITION" in (accounting_fixture_dir / "FX004_SELL_EXCEEDS_POSITION.json").read_text(encoding="utf-8"),
        "FX004 includes sell exceeds issue",
    )
    require(
        "MISSING_FEE" in (accounting_fixture_dir / "FX005_MISSING_FEE.json").read_text(encoding="utf-8"),
        "FX005 includes missing fee issue",
    )
    require(
        "NEGATIVE_CASH" in (accounting_fixture_dir / "FX006_NEGATIVE_CASH.json").read_text(encoding="utf-8"),
        "FX006 includes negative cash issue",
    )
    require(
        "SNAPSHOT_STALE" in (accounting_fixture_dir / "FX011_STALE_SNAPSHOT.json").read_text(encoding="utf-8"),
        "FX011 includes stale snapshot issue",
    )
    require(
        "MARKET_PRICE_MISSING" in (accounting_fixture_dir / "FX012_MISSING_MARKET_PRICE.json").read_text(encoding="utf-8"),
        "FX012 includes missing market price issue",
    )
    fx013_text = (accounting_fixture_dir / "FX013_MULTI_CURRENCY_UNSUPPORTED.json").read_text(encoding="utf-8")
    require(
        "MULTI_CURRENCY_UNSUPPORTED" in fx013_text or "FX_RATE_MISSING" in fx013_text,
        "FX013 includes multi-currency or FX issue",
    )
    require("sqlite" in accounting_fixture_validator.lower(), "fixture validator mentions SQLite boundary")
    require("forbidden" in accounting_fixture_validator, "fixture validator checks forbidden tokens")
    require("EXPECTED_ERROR_CODES" in accounting_fixture_validator, "fixture validator checks error fixture codes")
    require("AccountingFixtureLoader" in accounting_fixture_loader_header, "loader header declares AccountingFixtureLoader")
    require("loadIndex" in accounting_fixture_loader_header, "loader header declares loadIndex")
    require("loadAll" in accounting_fixture_loader_header, "loader header declares loadAll")
    require("fixtureById" in accounting_fixture_loader_header, "loader header declares fixtureById")
    require("fixtureIds" in accounting_fixture_loader_header, "loader header declares fixtureIds")
    require("validateFixtureStructure" in accounting_fixture_loader_header, "loader header declares fixture structure validation")
    require("QJsonDocument" in accounting_fixture_loader_source, "loader source uses Qt JSON parser")
    require("SELL_EXCEEDS_POSITION" in accounting_fixture_loader_source, "loader validates sell exceeds issue")
    require("MISSING_FEE" in accounting_fixture_loader_source, "loader validates missing fee issue")
    require("NEGATIVE_CASH" in accounting_fixture_loader_source, "loader validates negative cash issue")
    require("SNAPSHOT_STALE" in accounting_fixture_loader_source, "loader validates stale snapshot issue")
    require("MARKET_PRICE_MISSING" in accounting_fixture_loader_source, "loader validates missing market price issue")
    require("MULTI_CURRENCY_UNSUPPORTED" in accounting_fixture_loader_source, "loader validates multi-currency issue")
    require("DataService" not in accounting_fixture_loader_source, "loader source does not call DataService")
    require("sqlite" not in accounting_fixture_loader_source.lower(), "loader source does not access SQLite")
    require("loadAll" in accounting_fixture_loader_test, "loader test covers loadAll")
    require("FX001_EMPTY_LEDGER" in accounting_fixture_loader_test, "loader test covers FX001")
    require("FX013_MULTI_CURRENCY_UNSUPPORTED" in accounting_fixture_loader_test, "loader test covers FX013")
    accounting_fixtures_cmake = (root / "tests" / "AccountingFixtures" / "CMakeLists.txt").read_text(encoding="utf-8")
    require("accounting_replay_fixture_loader" in accounting_fixtures_cmake, "CTest includes fixture loader test")
    require("accounting_replay_harness_skeleton" in accounting_fixtures_cmake, "CTest includes replay harness test")
    require("AccountingReplayResult" in accounting_replay_stub_header, "stub header returns replay result")
    require("AccountingReplayResult" in accounting_replay_result_header, "result header declares AccountingReplayResult")
    require("AccountingReplayIssue" in accounting_replay_result_header, "result header declares issue")
    require("AccountingReplayMetadata" in accounting_replay_result_header, "result header declares metadata")
    require("positionListResponseRaw" in accounting_replay_result_header, "result header has position raw output")
    require("cashSummaryRaw" in accounting_replay_result_header, "result header has cash raw output")
    require("portfolioPnlRaw" in accounting_replay_result_header, "result header has pnl raw output")
    require("basePositionRaw" in accounting_replay_result_header, "result header has base-position raw output")
    require("sniperPoolRaw" in accounting_replay_result_header, "result header has sniper-pool raw output")
    require("hasEmptyReplayOutputs" in accounting_replay_result_source, "result source checks empty outputs")
    require("AccountingReplayResultMapper" in accounting_replay_mapper_header, "mapper header declares mapper")
    require("makeNotImplementedResult" in accounting_replay_mapper_header, "mapper exposes not implemented result")
    require("makeInvalidFixtureResult" in accounting_replay_mapper_header, "mapper exposes invalid fixture result")
    require("REPLAY_NOT_IMPLEMENTED" in accounting_replay_mapper_source, "mapper creates replay not implemented issue")
    require("INVALID_FIXTURE" in accounting_replay_mapper_source, "mapper creates invalid fixture issue")
    require("NOT_IMPLEMENTED" in accounting_replay_mapper_source, "mapper returns NOT_IMPLEMENTED")
    require("INVALID_FIXTURE" in accounting_replay_mapper_source, "mapper handles invalid fixture")
    require("AccountingReplayResultMapper::makeNotImplementedResult" in accounting_replay_stub_source, "stub delegates to mapper")
    require("AccountingReplayTestHarness" in accounting_replay_harness_header, "harness header declares harness")
    require("loadFixtures" in accounting_replay_harness_header, "harness exposes loadFixtures")
    require("runAll" in accounting_replay_harness_header, "harness exposes runAll")
    require("missingFixtureIds" in accounting_replay_harness_header, "harness exposes missingFixtureIds")
    require("AccountingFixtureLoader" in accounting_replay_harness_header, "harness owns fixture loader")
    require("loader_.loadAll" in accounting_replay_harness_source, "harness uses fixture loader")
    require("previewFixture" in accounting_replay_harness_source, "harness calls stub preview")
    require("FX001_EMPTY_LEDGER" in accounting_replay_harness_test, "harness test covers FX001")
    require("FX013_MULTI_CURRENCY_UNSUPPORTED" in accounting_replay_harness_test, "harness test covers FX013")
    require("NOT_IMPLEMENTED" in accounting_replay_harness_test, "harness test expects NOT_IMPLEMENTED")
    require("implemented is false" in accounting_replay_harness_test, "harness test checks implemented false")
    require("replayExecuted is false" in accounting_replay_harness_test, "harness test checks replayExecuted false")
    require("REPLAY_NOT_IMPLEMENTED" in accounting_replay_harness_test, "harness test checks replay issue")
    require("hasEmptyReplayOutputs" in accounting_replay_harness_test, "harness test checks empty raw outputs")
    require("accounting_replay_result_skeleton" in accounting_fixtures_cmake, "CTest includes result skeleton test")
    require("makeNotImplementedResult" in accounting_replay_result_test, "result test covers not implemented mapper")
    require("makeInvalidFixtureResult" in accounting_replay_result_test, "result test covers invalid fixture mapper")
    require("REPLAY_NOT_IMPLEMENTED" in accounting_replay_result_test, "result test checks replay issue")
    require("hasEmptyReplayOutputs" in accounting_replay_result_test, "result test checks empty raw outputs")
    require("AccountingExpectedOutputInspector" in accounting_expected_inspector_header, "inspector header declares inspector")
    require("ExpectedOutputShape" in accounting_expected_inspector_header, "inspector header declares expected output shape")
    require("hasRequiredExpectedOutputs" in accounting_expected_inspector_header, "inspector exposes required output check")
    require("expectedIssueCodes" in accounting_expected_inspector_header, "inspector exposes expected issue codes")
    require("hasErrorCode" in accounting_expected_inspector_header, "inspector exposes error code check")
    require("positionSummaries" in accounting_expected_inspector_source, "inspector checks positionSummaries")
    require("cashSummary" in accounting_expected_inspector_source, "inspector checks cashSummary")
    require("portfolioPnl" in accounting_expected_inspector_source, "inspector checks portfolioPnl")
    require("AccountingReplayAssertionSkeleton" in accounting_replay_assertion_header, "assertion header declares skeleton")
    require("PASS_NOT_IMPLEMENTED_GUARD" in accounting_replay_assertion_header, "assertion header declares guard pass status")
    require("PASS_FX002_SINGLE_BUY" in accounting_replay_assertion_header, "assertion header declares FX002 pass status")
    require("PASS_FX003_BUY_SELL_PARTIAL" in accounting_replay_assertion_header, "assertion header declares FX003 pass status")
    require("PASS_FX004_SELL_EXCEEDS_POSITION" in accounting_replay_assertion_header, "assertion header declares FX004 pass status")
    require("PASS_FX005_MISSING_FEE" in accounting_replay_assertion_header, "assertion header declares FX005 pass status")
    require("PASS_FX006_NEGATIVE_CASH" in accounting_replay_assertion_header, "assertion header declares FX006 pass status")
    require("PASS_FX007_MULTI_INSTRUMENT" in accounting_replay_assertion_header, "assertion header declares FX007 pass status")
    require("PASS_FX008_MULTI_ACCOUNT" in accounting_replay_assertion_header, "assertion header declares FX008 pass status")
    require("PASS_FX009_BASE_POSITION_LOCKED" in accounting_replay_assertion_header, "assertion header declares FX009 pass status")
    require("PASS_FX010_SNIPER_TIER_COMPLETED" in accounting_replay_assertion_header, "assertion header declares FX010 pass status")
    require("PASS_FX011_STALE_SNAPSHOT" in accounting_replay_assertion_header, "assertion header declares FX011 pass status")
    require("PASS_FX012_MISSING_MARKET_PRICE" in accounting_replay_assertion_header, "assertion header declares FX012 pass status")
    require("PASS_FX013_MULTI_CURRENCY_UNSUPPORTED" in accounting_replay_assertion_header, "assertion header declares FX013 pass status")
    require("FAIL_UNEXPECTED_IMPLEMENTED" in accounting_replay_assertion_header, "assertion header declares implemented failure")
    require("FAIL_RESULT_OUTPUT_NOT_EMPTY" in accounting_replay_assertion_header, "assertion header declares non-empty output failure")
    require("SKIPPED_BY_DESIGN" in accounting_replay_assertion_header, "assertion header declares skipped status")
    require("assertCurrentStubResult" in accounting_replay_assertion_header, "assertion exposes current stub result check")
    require("assertFx002SingleBuyResult" in accounting_replay_assertion_header, "assertion exposes FX002 single-buy check")
    require("assertFx003BuySellPartialResult" in accounting_replay_assertion_header, "assertion exposes FX003 buy-sell partial check")
    require("assertFx004SellExceedsPositionResult" in accounting_replay_assertion_header, "assertion exposes FX004 sell-exceeds-position check")
    require("assertFx005MissingFeeResult" in accounting_replay_assertion_header, "assertion exposes FX005 missing-fee check")
    require("assertFx006NegativeCashResult" in accounting_replay_assertion_header, "assertion exposes FX006 negative-cash check")
    require("assertFx007MultiInstrumentResult" in accounting_replay_assertion_header, "assertion exposes FX007 multi-instrument check")
    require("assertFx008MultiAccountResult" in accounting_replay_assertion_header, "assertion exposes FX008 multi-account check")
    require("assertFx009BasePositionLockedResult" in accounting_replay_assertion_header, "assertion exposes FX009 base-position check")
    require("assertFx010SniperTierCompletedResult" in accounting_replay_assertion_header, "assertion exposes FX010 sniper-tier check")
    require("assertFx011StaleSnapshotResult" in accounting_replay_assertion_header, "assertion exposes FX011 stale-snapshot check")
    require("assertFx012MissingMarketPriceResult" in accounting_replay_assertion_header, "assertion exposes FX012 missing-market-price check")
    require("assertFx013MultiCurrencyUnsupportedResult" in accounting_replay_assertion_header, "assertion exposes FX013 multi-currency check")
    require("assertPositionList" in accounting_replay_assertion_header, "assertion exposes position placeholder")
    require("assertCashSummary" in accounting_replay_assertion_header, "assertion exposes cash placeholder")
    require("assertPortfolioPnl" in accounting_replay_assertion_header, "assertion exposes pnl placeholder")
    require("REPLAY_NOT_IMPLEMENTED" in accounting_replay_assertion_source, "assertion requires replay not implemented issue")
    require("FX002_SINGLE_BUY" in accounting_replay_assertion_source, "assertion source checks FX002")
    require("FX003_BUY_SELL_PARTIAL" in accounting_replay_assertion_source, "assertion source checks FX003")
    require("FX004_SELL_EXCEEDS_POSITION" in accounting_replay_assertion_source, "assertion source checks FX004")
    require("FX005_MISSING_FEE" in accounting_replay_assertion_source, "assertion source checks FX005")
    require("FX006_NEGATIVE_CASH" in accounting_replay_assertion_source, "assertion source checks FX006")
    require("FX007_MULTI_INSTRUMENT" in accounting_replay_assertion_source, "assertion source checks FX007")
    require("FX008_MULTI_ACCOUNT" in accounting_replay_assertion_source, "assertion source checks FX008")
    require("FX009_BASE_POSITION_LOCKED" in accounting_replay_assertion_source, "assertion source checks FX009")
    require("FX010_SNIPER_TIER_COMPLETED" in accounting_replay_assertion_source, "assertion source checks FX010")
    require("FX011_STALE_SNAPSHOT" in accounting_replay_assertion_source, "assertion source checks FX011")
    require("FX012_MISSING_MARKET_PRICE" in accounting_replay_assertion_source, "assertion source checks FX012")
    require("FX013_MULTI_CURRENCY_UNSUPPORTED" in accounting_replay_assertion_source, "assertion source checks FX013")
    require("hasEmptyReplayOutputs" in accounting_replay_assertion_source, "assertion checks empty raw outputs")
    require("AccountingExpectedOutputInspector" in accounting_replay_assertion_test, "assertion test uses inspector")
    require("AccountingReplayAssertionSkeleton" in accounting_replay_assertion_test, "assertion test uses skeleton")
    require("FX001_EMPTY_LEDGER" in accounting_replay_assertion_test, "assertion test covers FX001")
    require("FX013_MULTI_CURRENCY_UNSUPPORTED" in accounting_replay_assertion_test, "assertion test covers FX013")
    require("SELL_EXCEEDS_POSITION" in accounting_replay_assertion_test, "assertion test checks sell exceeds issue")
    require("MISSING_FEE" in accounting_replay_assertion_test, "assertion test checks missing fee issue")
    require("NEGATIVE_CASH" in accounting_replay_assertion_test, "assertion test checks negative cash issue")
    require("SNAPSHOT_STALE" in accounting_replay_assertion_test, "assertion test checks stale snapshot issue")
    require("MARKET_PRICE_MISSING" in accounting_replay_assertion_test, "assertion test checks missing market price issue")
    require("PASS_NOT_IMPLEMENTED_GUARD" in accounting_replay_assertion_test, "assertion test checks not implemented guard")
    require("SKIPPED_BY_DESIGN" in accounting_replay_assertion_test, "assertion test checks skipped placeholders")
    require("implemented=true" in accounting_replay_assertion_test, "assertion test fails implemented true result")
    require("raw output" in accounting_replay_assertion_test, "assertion test fails non-empty raw output")
    require("accounting_replay_expected_output_assertions" in accounting_fixtures_cmake, "CTest includes expected output assertion test")
    require("AccountingReplayMinimalEngine" in accounting_replay_minimal_header, "minimal engine header declares minimal engine")
    require("supportsFixture" in accounting_replay_minimal_header, "minimal engine exposes supportsFixture")
    require("replayFixture" in accounting_replay_minimal_header, "minimal engine exposes replayFixture")
    require("FX001_EMPTY_LEDGER" in accounting_replay_minimal_source, "minimal engine supports FX001")
    require("FX002_SINGLE_BUY" in accounting_replay_minimal_source, "minimal engine supports FX002")
    require("FX003_BUY_SELL_PARTIAL" in accounting_replay_minimal_source, "minimal engine supports FX003")
    require("FX004_SELL_EXCEEDS_POSITION" in accounting_replay_minimal_source, "minimal engine supports FX004")
    require("FX005_MISSING_FEE" in accounting_replay_minimal_source, "minimal engine supports FX005")
    require("FX006_NEGATIVE_CASH" in accounting_replay_minimal_source, "minimal engine supports FX006")
    require("FX007_MULTI_INSTRUMENT" in accounting_replay_minimal_source, "minimal engine supports FX007")
    require("FX008_MULTI_ACCOUNT" in accounting_replay_minimal_source, "minimal engine supports FX008")
    require("FX009_BASE_POSITION_LOCKED" in accounting_replay_minimal_source, "minimal engine supports FX009")
    require("FX010_SNIPER_TIER_COMPLETED" in accounting_replay_minimal_source, "minimal engine supports FX010")
    require("FX011_STALE_SNAPSHOT" in accounting_replay_minimal_source, "minimal engine supports FX011")
    require("FX012_MISSING_MARKET_PRICE" in accounting_replay_minimal_source, "minimal engine supports FX012")
    require("FX013_MULTI_CURRENCY_UNSUPPORTED" in accounting_replay_minimal_source, "minimal engine supports FX013")
    require("SELL_EXCEEDS_POSITION" in accounting_replay_minimal_source, "minimal engine emits FX004 issue")
    require("MISSING_FEE" in accounting_replay_minimal_source, "minimal engine emits FX005 issue")
    require("NEGATIVE_CASH" in accounting_replay_minimal_source, "minimal engine emits FX006 issue")
    require("MARKET_PRICE_MISSING" in accounting_replay_minimal_source, "minimal engine emits FX007 issue")
    require("accountCashSummaries" in accounting_replay_minimal_source, "minimal engine emits FX008 account cash summaries")
    require("makeBasePositionRaw" in accounting_replay_minimal_source, "minimal engine emits FX009 base position raw")
    require("makeSniperPoolRaw" in accounting_replay_minimal_source, "minimal engine emits FX010 sniper pool raw")
    require("SNAPSHOT_STALE" in accounting_replay_minimal_source, "minimal engine emits FX011 stale snapshot issue")
    require("MARKET_PRICE_MISSING" in accounting_replay_minimal_source, "minimal engine emits FX012 missing market price issue")
    require("MULTI_CURRENCY_UNSUPPORTED" in accounting_replay_minimal_source, "minimal engine emits FX013 multi-currency issue")
    require("FX_RATE_MISSING" in accounting_replay_minimal_source, "minimal engine emits FX013 missing FX rate issue")
    require("snapshotFactsRawJson" in accounting_fixture_loader_header, "loader header exposes raw snapshot facts")
    require("kReplayStatusError" in accounting_replay_minimal_source, "minimal engine returns ERROR status")
    require("kReplayStatusWarning" in accounting_replay_minimal_source, "minimal engine returns WARNING status")
    require("kReplayStatusOk" in accounting_replay_minimal_source, "minimal engine returns OK status")
    require("positions" in accounting_replay_minimal_source, "minimal engine creates positions array")
    require("cashSummaryRaw" in accounting_replay_minimal_test, "minimal test checks cash summary raw")
    require("portfolioPnlRaw" in accounting_replay_minimal_test, "minimal test checks portfolio pnl raw")
    require("FX002_SINGLE_BUY" in accounting_replay_minimal_test, "minimal test checks FX002")
    require("1001.00 CNY" in accounting_replay_minimal_test, "minimal test checks FX002 cost amount")
    require("98999.00 CNY" in accounting_replay_minimal_test, "minimal test checks FX002 cash balance")
    require("FX003_BUY_SELL_PARTIAL" in accounting_replay_minimal_test, "minimal test checks FX003")
    require("600.60 CNY" in accounting_replay_minimal_test, "minimal test checks FX003 cost amount")
    require("99478.00 CNY" in accounting_replay_minimal_test, "minimal test checks FX003 cash balance")
    require("78.60 CNY" in accounting_replay_minimal_test, "minimal test checks FX003 realized PnL")
    require("FX004_SELL_EXCEEDS_POSITION" in accounting_replay_minimal_test, "minimal test checks FX004")
    require("SELL_EXCEEDS_POSITION" in accounting_replay_minimal_test, "minimal test checks FX004 issue")
    require("FX005_MISSING_FEE" in accounting_replay_minimal_test, "minimal test checks FX005")
    require("MISSING_FEE" in accounting_replay_minimal_test, "minimal test checks FX005 issue")
    require("FX006_NEGATIVE_CASH" in accounting_replay_minimal_test, "minimal test checks FX006")
    require("NEGATIVE_CASH" in accounting_replay_minimal_test, "minimal test checks FX006 issue")
    require("FX007_MULTI_INSTRUMENT" in accounting_replay_minimal_test, "minimal test checks FX007")
    require("MARKET_PRICE_MISSING" in accounting_replay_minimal_test, "minimal test checks FX007 issue")
    require("FX008_MULTI_ACCOUNT" in accounting_replay_minimal_test, "minimal test checks FX008")
    require("ACC-DEMO-001" in accounting_replay_minimal_test, "minimal test checks FX008 account one")
    require("ACC-DEMO-002" in accounting_replay_minimal_test, "minimal test checks FX008 account two")
    require("48999.00 CNY" in accounting_replay_minimal_test, "minimal test checks FX008 account one cash")
    require("47999.00 CNY" in accounting_replay_minimal_test, "minimal test checks FX008 account two cash")
    require("FX009_BASE_POSITION_LOCKED" in accounting_replay_minimal_test, "minimal test checks FX009")
    require("targetBaseRatioText" in accounting_replay_minimal_test, "minimal test checks FX009 target ratio")
    require("lockedBaseAmountText" in accounting_replay_minimal_test, "minimal test checks FX009 locked amount")
    require("sellableAboveBaseAmountText" in accounting_replay_minimal_test, "minimal test checks FX009 sellable amount")
    require("FX010_SNIPER_TIER_COMPLETED" in accounting_replay_minimal_test, "minimal test checks FX010")
    require("80000.00 CNY" in accounting_replay_minimal_test, "minimal test checks FX010 pool amount")
    require("79000.00 CNY" in accounting_replay_minimal_test, "minimal test checks FX010 remaining amount")
    require("T1" in accounting_replay_minimal_test, "minimal test checks FX010 T1 tier")
    require("calculationVersion" in accounting_replay_minimal_test, "minimal test checks FX010 calculation version")
    require("FX011_STALE_SNAPSHOT" in accounting_replay_minimal_test, "minimal test checks FX011")
    require("SNAPSHOT_STALE" in accounting_replay_minimal_test, "minimal test checks FX011 issue")
    require("kReplayStatusStale" in accounting_replay_minimal_test, "minimal test checks FX011 stale status")
    require("FX012_MISSING_MARKET_PRICE" in accounting_replay_minimal_test, "minimal test checks FX012")
    require("MARKET_PRICE_MISSING" in accounting_replay_minimal_test, "minimal test checks FX012 issue")
    require("kReplayStatusWarning" in accounting_replay_minimal_test, "minimal test checks FX012 warning status")
    require("unavailable" in accounting_replay_minimal_test, "minimal test checks FX012 unavailable valuation")
    require("FX013_MULTI_CURRENCY_UNSUPPORTED" in accounting_replay_minimal_test, "minimal test checks FX013")
    require("MULTI_CURRENCY_UNSUPPORTED" in accounting_replay_minimal_test, "minimal test checks FX013 multi-currency issue")
    require("FX_RATE_MISSING" in accounting_replay_minimal_test, "minimal test checks FX013 FX issue")
    require("runAllWithMinimalEngine" in accounting_replay_harness_header, "harness exposes minimal engine mode")
    require("runAllWithMinimalEngine" in accounting_replay_harness_source, "harness implements minimal engine mode")
    require("accounting_replay_minimal_fx001" in accounting_fixtures_cmake, "CTest includes minimal FX001 test")
    require("accounting_replay_minimal_fx002" in accounting_fixtures_cmake, "CTest includes minimal FX002 test")
    require("accounting_replay_minimal_fx003" in accounting_fixtures_cmake, "CTest includes minimal FX003 test")
    require("accounting_replay_minimal_fx004" in accounting_fixtures_cmake, "CTest includes minimal FX004 test")
    require("accounting_replay_minimal_fx005" in accounting_fixtures_cmake, "CTest includes minimal FX005 test")
    require("accounting_replay_minimal_fx006" in accounting_fixtures_cmake, "CTest includes minimal FX006 test")
    require("accounting_replay_minimal_fx007" in accounting_fixtures_cmake, "CTest includes minimal FX007 test")
    require("accounting_replay_minimal_fx008" in accounting_fixtures_cmake, "CTest includes minimal FX008 test")
    require("accounting_replay_minimal_fx009" in accounting_fixtures_cmake, "CTest includes minimal FX009 test")
    require("accounting_replay_minimal_fx010" in accounting_fixtures_cmake, "CTest includes minimal FX010 test")
    require("accounting_replay_minimal_fx011" in accounting_fixtures_cmake, "CTest includes minimal FX011 test")
    require("accounting_replay_minimal_fx012" in accounting_fixtures_cmake, "CTest includes minimal FX012 test")
    require("accounting_replay_minimal_fx013" in accounting_fixtures_cmake, "CTest includes minimal FX013 test")
    require("SQLite" not in accounting_replay_harness_source, "harness source does not access SQLite")
    require("DataService" not in accounting_replay_harness_source, "harness source does not call DataService")
    require("SQLite" not in accounting_replay_stub_source, "stub source does not access SQLite")
    require("DataService" not in accounting_replay_stub_source, "stub source does not call DataService")
    require("SQLite" not in accounting_replay_assertion_source, "assertion source does not access SQLite")
    require("DataService" not in accounting_replay_assertion_source, "assertion source does not call DataService")
    require("SQLite" not in accounting_replay_minimal_source, "minimal engine source does not access SQLite")
    require("DataService" not in accounting_replay_minimal_source, "minimal engine source does not call DataService")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
