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
    release_notes_path = root / "docs" / "release_notes" / "v0_1_readonly_shell_demo.md"
    docs_index_path = root / "docs" / "README.md"
    readme_path = root / "README.md"
    gitignore_path = root / ".gitignore"

    require(script_path.exists(), "run_readonly_demo.ps1 exists")
    require(stop_script_path.exists(), "stop_readonly_demo.ps1 exists")
    require(doc_path.exists(), "manual acceptance doc exists")
    require(milestone_path.exists(), "v0.1 milestone doc exists")
    require(quick_start_path.exists(), "quick start doc exists")
    require(release_notes_path.exists(), "release notes doc exists")
    require(docs_index_path.exists(), "docs index exists")

    script = script_path.read_text(encoding="utf-8")
    stop_script = stop_script_path.read_text(encoding="utf-8")
    doc = doc_path.read_text(encoding="utf-8")
    milestone = milestone_path.read_text(encoding="utf-8")
    quick_start = quick_start_path.read_text(encoding="utf-8")
    release_notes = release_notes_path.read_text(encoding="utf-8")
    docs_index = docs_index_path.read_text(encoding="utf-8")
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

    require("Release Notes - v0.1 ReadOnly Shell Demo" in release_notes, "release notes title")
    require("Suggested Tag After Merge" in release_notes, "release notes include suggested tag section")
    require("v0.1.0-readonly-shell-demo" in release_notes, "release notes include suggested tag")
    require("Automatic trading" in release_notes, "release notes list excluded scope")

    require("14_milestone_v0_1_readonly_shell_demo.md" in docs_index, "docs index links milestone")
    require("15_quick_start_readonly_shell_demo.md" in docs_index, "docs index links quick start")
    require("release_notes/v0_1_readonly_shell_demo.md" in docs_index, "docs index links release notes")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
