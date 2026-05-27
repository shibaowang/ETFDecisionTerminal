from pathlib import Path


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    script_path = root / "tools" / "dev" / "run_readonly_demo.ps1"
    stop_script_path = root / "tools" / "dev" / "stop_readonly_demo.ps1"
    doc_path = root / "docs" / "13_manual_acceptance_readonly_shell.md"
    milestone_path = root / "docs" / "14_milestone_v0_1_readonly_shell_demo.md"
    quick_start_path = root / "docs" / "15_quick_start_readonly_shell_demo.md"
    business_acceptance_path = root / "docs" / "16_manual_acceptance_readonly_business_pages.md"
    ui_readability_path = root / "docs" / "17_ui_readability_checklist.md"
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
    docs_index_path = root / "docs" / "README.md"
    protocol_path = root / "docs" / "04_protocol.md"
    accounting_rules_path = root / "docs" / "06_accounting_rules.md"
    readme_path = root / "README.md"
    gitignore_path = root / ".gitignore"

    require(script_path.exists(), "run_readonly_demo.ps1 exists")
    require(stop_script_path.exists(), "stop_readonly_demo.ps1 exists")
    require(doc_path.exists(), "manual acceptance doc exists")
    require(milestone_path.exists(), "v0.1 milestone doc exists")
    require(quick_start_path.exists(), "quick start doc exists")
    require(business_acceptance_path.exists(), "read-only business pages acceptance doc exists")
    require(ui_readability_path.exists(), "UI readability checklist exists")
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
    require(docs_index_path.exists(), "docs index exists")
    require(protocol_path.exists(), "protocol doc exists")
    require(accounting_rules_path.exists(), "accounting rules doc exists")

    script = script_path.read_text(encoding="utf-8")
    stop_script = stop_script_path.read_text(encoding="utf-8")
    doc = doc_path.read_text(encoding="utf-8")
    milestone = milestone_path.read_text(encoding="utf-8")
    quick_start = quick_start_path.read_text(encoding="utf-8")
    business_acceptance = business_acceptance_path.read_text(encoding="utf-8")
    ui_readability = ui_readability_path.read_text(encoding="utf-8")
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
    release_notes = release_notes_path.read_text(encoding="utf-8")
    release_notes_v02 = release_notes_v02_path.read_text(encoding="utf-8")
    release_notes_v03 = release_notes_v03_path.read_text(encoding="utf-8")
    docs_index = docs_index_path.read_text(encoding="utf-8")
    protocol_doc = protocol_path.read_text(encoding="utf-8")
    accounting_rules = accounting_rules_path.read_text(encoding="utf-8")
    readme = readme_path.read_text(encoding="utf-8")
    gitignore = gitignore_path.read_text(encoding="utf-8")
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
    require("../libs/AccountingEngine" in docs_index, "docs index links AccountingEngine skeleton module")
    require("AccountingEngine public headers" in docs_index, "docs index links AccountingEngine DTO parser boundary")
    require("release_notes/v0_3_accounting_replay_testonly_coverage.md" in docs_index, "docs index links v0.3 accounting replay release notes")
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
    require("replayImplemented=false" in accounting_engine_candidate, "AccountingEngine candidate doc records replay false")
    require("snapshotWriteEnabled=false" in accounting_engine_candidate, "AccountingEngine candidate doc records snapshot write false")
    require("tradeLogWriteEnabled=false" in accounting_engine_candidate, "AccountingEngine candidate doc records trade log write false")
    require("No DataAccess dependency" in accounting_engine_candidate, "AccountingEngine candidate doc keeps DataAccess forbidden")

    require("AccountingEngine skeleton" in readme, "README documents AccountingEngine skeleton")
    require("AccountingEngine replay DTO parser boundary" in readme, "README documents DTO parser boundary")
    require("AccountingEngine empty ledger replay skeleton" in readme, "README documents empty ledger replay skeleton")
    require("AccountingEngine single buy replay skeleton" in readme, "README documents single buy replay skeleton")
    require("AccountingEngine partial sell replay skeleton" in readme, "README documents partial sell replay skeleton")
    require("ReplayRequestDto" in readme, "README documents ReplayRequestDto")
    require("TradeFactDto" in readme, "README documents TradeFactDto")
    require("accounting_replay_dto_parser_boundary" in readme, "README documents DTO parser boundary test")
    require("accounting_replay_engine_empty_ledger" in readme, "README documents empty ledger replay test")
    require("accounting_replay_engine_single_buy" in readme, "README documents single buy replay test")
    require("accounting_replay_engine_buy_sell_partial" in readme, "README documents partial sell replay test")
    require("replayImplemented=false" in readme, "README records AccountingEngine replay false")
    require("productionReady=false" in readme, "README records AccountingEngine productionReady false")
    require("writeEnabled=false" in readme, "README records AccountingEngine write false")
    require("accounting_engine_boundary" in readme, "README documents AccountingEngine boundary test")

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
    require("AccountingEngineBoundaryTests" in accounting_engine_test_cmake, "AccountingEngine boundary test target exists")
    require("accountingEngineBoundary" in accounting_engine_boundary_test, "AccountingEngine boundary test source exists")
    require("AccountingReplayDtoParserBoundaryTests" in accounting_engine_test_cmake, "AccountingEngine DTO parser test target exists")
    require("AccountingReplayEngineEmptyLedgerTests" in accounting_engine_test_cmake, "AccountingEngine empty ledger test target exists")
    require("AccountingReplayEngineSingleBuyTests" in accounting_engine_test_cmake, "AccountingEngine single buy test target exists")
    require(
        "AccountingReplayEngineBuySellPartialTests" in accounting_engine_test_cmake,
        "AccountingEngine buy-sell partial test target exists",
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
