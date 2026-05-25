from pathlib import Path


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    script_path = root / "tools" / "dev" / "run_readonly_demo.ps1"
    doc_path = root / "docs" / "13_manual_acceptance_readonly_shell.md"
    readme_path = root / "README.md"

    require(script_path.exists(), "run_readonly_demo.ps1 exists")
    require(doc_path.exists(), "manual acceptance doc exists")

    script = script_path.read_text(encoding="utf-8")
    doc = doc_path.read_text(encoding="utf-8")
    readme = readme_path.read_text(encoding="utf-8")

    require("run_readonly_demo.ps1" in readme, "README references run_readonly_demo.ps1")
    require("13_manual_acceptance_readonly_shell.md" in readme, "README references manual acceptance doc")
    require("只读数据预览" in doc, "manual doc mentions readonly page")
    require("ETFDataServiceReadonly" in doc, "manual doc mentions default socket")
    require("不得调用 `data.audit.append`" in doc, "manual doc prohibits audit append")

    require("data.audit.append" not in script, "script must not call audit append")
    require("--serve-dev-audit" not in script, "script must not start audit dev service")
    require("--serve-readonly" in script, "script starts read-only service")
    require("--init-db" in script, "script initializes demo database")
    require("--check-db" in script, "script checks demo database")
    require("ETFDecisionShell" in script, "script can launch ETFDecisionShell")
    require("runtime\\dev\\readonly_demo\\ETFDecision.db" in script, "script uses default demo database")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
