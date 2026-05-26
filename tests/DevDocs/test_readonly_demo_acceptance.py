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
    accounting_replay_harness_header_path = root / "tests" / "AccountingFixtures" / "AccountingReplayTestHarness.h"
    accounting_replay_harness_source_path = root / "tests" / "AccountingFixtures" / "AccountingReplayTestHarness.cpp"
    accounting_replay_harness_test_path = root / "tests" / "AccountingFixtures" / "test_accounting_replay_harness.cpp"
    release_notes_path = root / "docs" / "release_notes" / "v0_1_readonly_shell_demo.md"
    release_notes_v02_path = root / "docs" / "release_notes" / "v0_2_readonly_business_pages.md"
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
    require(accounting_fixture_dir.exists(), "accounting replay fixture directory exists")
    require(accounting_fixture_index_path.exists(), "accounting replay fixture index exists")
    require(accounting_fixture_validator_path.exists(), "accounting replay fixture validator exists")
    require(accounting_fixture_loader_header_path.exists(), "accounting fixture loader header exists")
    require(accounting_fixture_loader_source_path.exists(), "accounting fixture loader source exists")
    require(accounting_fixture_loader_test_path.exists(), "accounting fixture loader test exists")
    require(accounting_replay_stub_header_path.exists(), "accounting replay stub header exists")
    require(accounting_replay_stub_source_path.exists(), "accounting replay stub source exists")
    require(accounting_replay_harness_header_path.exists(), "accounting replay harness header exists")
    require(accounting_replay_harness_source_path.exists(), "accounting replay harness source exists")
    require(accounting_replay_harness_test_path.exists(), "accounting replay harness test exists")
    require(release_notes_path.exists(), "release notes doc exists")
    require(release_notes_v02_path.exists(), "v0.2 release notes doc exists")
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
    accounting_fixture_index = accounting_fixture_index_path.read_text(encoding="utf-8")
    accounting_fixture_validator = accounting_fixture_validator_path.read_text(encoding="utf-8")
    accounting_fixture_loader_header = accounting_fixture_loader_header_path.read_text(encoding="utf-8")
    accounting_fixture_loader_source = accounting_fixture_loader_source_path.read_text(encoding="utf-8")
    accounting_fixture_loader_test = accounting_fixture_loader_test_path.read_text(encoding="utf-8")
    accounting_replay_stub_header = accounting_replay_stub_header_path.read_text(encoding="utf-8")
    accounting_replay_stub_source = accounting_replay_stub_source_path.read_text(encoding="utf-8")
    accounting_replay_harness_header = accounting_replay_harness_header_path.read_text(encoding="utf-8")
    accounting_replay_harness_source = accounting_replay_harness_source_path.read_text(encoding="utf-8")
    accounting_replay_harness_test = accounting_replay_harness_test_path.read_text(encoding="utf-8")
    release_notes = release_notes_path.read_text(encoding="utf-8")
    release_notes_v02 = release_notes_v02_path.read_text(encoding="utf-8")
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
    require("20_position_accounting_boundary.md" in docs_index, "docs index links position boundary")
    require("21_position_readonly_data_contract_draft.md" in docs_index, "docs index links position data contract")
    require("22_position_accounting_data_contract.md" in docs_index, "docs index links stable position contract")
    require("23_position_accounting_test_fixture_design.md" in docs_index, "docs index links position fixture design")
    require("24_accounting_replay_fixture_samples.md" in docs_index, "docs index links accounting fixture samples")
    require("tests/fixtures/accounting_replay" in docs_index, "docs index links accounting replay fixture files")
    require("tests/AccountingFixtures" in docs_index, "docs index links accounting fixture loader")
    require("25_position_shell_viewmodel_design.md" in docs_index, "docs index links position ViewModel design")
    require("26_position_dto_viewmodel_mapping.md" in docs_index, "docs index links position DTO mapping")

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
    require("status=NOT_IMPLEMENTED" in accounting_fixture_samples, "fixture samples doc documents NOT_IMPLEMENTED status")
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
    require("AccountingReplayStubResult" in accounting_replay_stub_header, "stub header declares result")
    require("implemented" in accounting_replay_stub_header, "stub result exposes implemented")
    require("replayExecuted" in accounting_replay_stub_header, "stub result exposes replayExecuted")
    require("requiredFutureTask" in accounting_replay_stub_header, "stub result exposes requiredFutureTask")
    require("NOT_IMPLEMENTED" in accounting_replay_stub_source, "stub returns NOT_IMPLEMENTED")
    require("INVALID_FIXTURE" in accounting_replay_stub_source, "stub handles invalid fixture")
    require("expectedIssues.size" in accounting_replay_stub_source, "stub reads issue count metadata")
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
    require("SQLite" not in accounting_replay_harness_source, "harness source does not access SQLite")
    require("DataService" not in accounting_replay_harness_source, "harness source does not call DataService")
    require("SQLite" not in accounting_replay_stub_source, "stub source does not access SQLite")
    require("DataService" not in accounting_replay_stub_source, "stub source does not call DataService")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
