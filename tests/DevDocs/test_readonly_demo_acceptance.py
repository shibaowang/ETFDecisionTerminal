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
    release_notes_path = root / "docs" / "release_notes" / "v0_1_readonly_shell_demo.md"
    release_notes_v02_path = root / "docs" / "release_notes" / "v0_2_readonly_business_pages.md"
    docs_index_path = root / "docs" / "README.md"
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
    require(release_notes_path.exists(), "release notes doc exists")
    require(release_notes_v02_path.exists(), "v0.2 release notes doc exists")
    require(docs_index_path.exists(), "docs index exists")
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
    release_notes = release_notes_path.read_text(encoding="utf-8")
    release_notes_v02 = release_notes_v02_path.read_text(encoding="utf-8")
    docs_index = docs_index_path.read_text(encoding="utf-8")
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
    require("ShellPositionSummaryViewModel" in position_boundary, "boundary doc drafts position ViewModel")

    require("PositionSummaryDto" in position_contract, "position contract includes PositionSummaryDto")
    require("CashSummaryDto" in position_contract, "position contract includes CashSummaryDto")
    require("PortfolioPnlDto" in position_contract, "position contract includes PortfolioPnlDto")
    require("BasePositionDto" in position_contract, "position contract includes BasePositionDto")
    require("SniperPoolDto" in position_contract, "position contract includes SniperPoolDto")
    require("当前未实现" in position_contract, "position contract states currently not implemented")
    require("DTOs must not write to the database" in position_contract, "position contract prohibits DTO writes")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
