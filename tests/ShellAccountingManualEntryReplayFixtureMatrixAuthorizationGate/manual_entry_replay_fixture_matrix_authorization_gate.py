#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path


class Gate:
    def __init__(self) -> None:
        self.checks = 0

    def require(self, condition: bool, message: str) -> None:
        self.checks += 1
        if not condition:
            raise AssertionError(message)

    def contains(self, text: str, token: str, context: str) -> None:
        normalized_text = " ".join(text.split())
        normalized_token = " ".join(token.split())
        self.require(
            token in text or normalized_token in normalized_text,
            f"{context} missing `{token}`",
        )


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def changed_paths(root: Path) -> set[str]:
    completed = subprocess.run(
        ["git", "diff", "--name-only", "main"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def diff_text(root: Path, *paths: str) -> str:
    completed = subprocess.run(
        ["git", "diff", "main", "--", *paths],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return completed.stdout


def added_lines(diff: str) -> str:
    return "\n".join(line[1:] for line in diff.splitlines() if line.startswith("+") and not line.startswith("+++"))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    doc224_path = root / "docs" / "224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md"
    doc225_path = root / "docs" / "225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md"
    test_dir = root / "tests" / "ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate"
    test_cmake_path = test_dir / "CMakeLists.txt"
    test_py_path = test_dir / "manual_entry_replay_fixture_matrix_authorization_gate.py"

    gate.require(doc224_path.exists(), "docs/224 exists")
    gate.require(doc225_path.exists(), "docs/225 exists")
    gate.require(test_cmake_path.exists(), "TASK-214 CMake exists")
    gate.require(test_py_path.exists(), "TASK-214 gate script exists")

    doc224 = read(doc224_path)
    doc225 = read(doc225_path)
    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        gate.contains(text, "TASK-214", context)
        gate.contains(text, "224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md", context)
        gate.contains(text, "225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md", context)

    gate.contains(tests_cmake, "ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate", "tests/CMakeLists")
    gate.contains(read(test_cmake_path), "shell_accounting_manual_entry_replay_fixture_matrix_authorization", "TASK-214 CTest")

    for token in [
        "# ShellAccounting Manual Entry Replay Fixture Matrix Authorization Gate",
        "TASK-214 is gate-only / fixture-matrix-authorization-only.",
        "TASK-214 does not implement replay.",
        "TASK-214 does not create runtime fixture data / JSON fixture files.",
        "TASK-214 does not modify production code.",
        "TASK-214 does not modify DataServiceActions.",
        "TASK-214 does not modify DataServiceActionRegistrar.",
        "TASK-214 does not modify repositories.",
        "TASK-214 does not modify migrations.",
        "TASK-214 does not modify QML / startup / Presenter / Controller / ShellServices adapter / port.",
        "TASK-214 does not call AccountingEngine replay.",
        "TASK-214 does not add runtime SQL / SQLite read/write.",
        "TASK-214 does not write audit / ledger.",
        "TASK-214 does not write snapshot.",
        "TASK-214 does not implement backup/export/restore.",
        "TASK-214 does not connect broker / network / credentials / endpoint.",
        "TASK-214 does not place real orders.",
        "TASK-214 does not enable automatic trading.",
        "Broker sandbox new capability remains paused.",
        "## Fixture Matrix Purpose",
        "future input / output acceptance contract",
        "TASK-213 manual entry replay policy",
        "manual BUY, SELL, Deposit, and Withdrawal",
        "## Fixture Naming Policy",
        "`MRF001` to `MRF0xx`",
        "id",
        "title",
        "purpose",
        "source facts",
        "expected position output",
        "expected cash output",
        "expected PnL output",
        "expected issues",
        "blocking / non-blocking status",
        "privacy expectation",
        "must not depend on test execution order",
        "must not depend on QML display order",
        "## Fixture Source Fact Policy",
        "persisted manual facts",
        "`trade_execution_group` plus `trade_log`",
        "`trade_log` plus `cash_adjustment`",
        "must not use QML raw payload",
        "broker payload",
        "real order id",
        "credentials",
        "endpoint",
        "raw SQL",
        "backup/export payload",
        "account, portfolio, instrument, and currency scope",
        "## Baseline Success Fixture Matrix",
        "`MRF001` empty manual facts -> empty / unavailable-safe output",
        "`MRF002` single BUY -> position quantity increase + cash outflow candidate",
        "`MRF003` BUY + Deposit -> position + cash baseline",
        "`MRF004` BUY + SELL partial reduction -> reduced position candidate",
        "`MRF005` Deposit + Withdrawal -> cash movement baseline",
        "`MRF006` BUY + SELL + Deposit + Withdrawal combined daily-use baseline",
        "design authorization only",
        "## Ordering And Idempotency Fixture Matrix",
        "same timestamp deterministic ordering",
        "same execution group deterministic leg order",
        "duplicate requestId / idempotencyKey no double consumption",
        "out-of-order source rows deterministic replay",
        "created_at vs occurred_at ordering decision candidate",
        "stable uid / id tie-breaker candidate",
        "SQLite unsorted return order must not affect replay",
        "## Position Safety Fixture Matrix",
        "SELL exceeds position -> fail-closed or blocking issue",
        "negative position not silently accepted",
        "zero quantity unsupported",
        "invalid quantity unsupported",
        "missing instrument unsupported",
        "same instrument across multiple accounts remains isolated",
        "same instrument across multiple portfolios remains isolated",
        "holding cost candidate remains unavailable",
        "## Cash Safety Fixture Matrix",
        "Withdrawal exceeds cash -> fail-closed or blocking issue",
        "negative cash not silently accepted",
        "BUY cash outflow with insufficient cash",
        "fee / tax cash impact requires policy",
        "missing currency unsupported",
        "multi-currency unsupported or unavailable-safe",
        "Deposit / Withdrawal cash adjustment linkage must be deterministic",
        "## PnL And Market Dependency Fixture Matrix",
        "realized PnL unavailable until cost basis / lot policy exists",
        "unrealized PnL unavailable until market price policy exists",
        "market value unavailable without market price",
        "no fabricated PnL",
        "no fabricated market value",
        "stale market price behavior requires future authorization",
        "`portfolio.pnl.summary` safe partial output remains unavailable-safe",
        "## Multi-Scope Fixture Matrix",
        "multi-account isolation",
        "multi-portfolio isolation",
        "multi-instrument isolation",
        "aggregate output requires explicit scope policy",
        "cross-account contamination must be blocking",
        "cross-portfolio contamination must be blocking",
        "same symbol / different instrument identity must be deterministic",
        "## Unsupported And Failure Fixture Matrix",
        "malformed persisted fact",
        "unsupported fact type",
        "missing account",
        "missing portfolio",
        "missing instrument",
        "missing currency",
        "duplicate fact ambiguity",
        "correction / void facts unsupported",
        "broker facts unsupported",
        "backup/export/restored facts unsupported until separately authorized",
        "## Expected Output Policy",
        "position quantity candidate",
        "holding cost candidate",
        "cash balance candidate",
        "safe partial PnL candidate",
        "unavailable fields",
        "issue list",
        "blocking issue flag",
        "calculation metadata",
        "source fact trace ids",
        "privacy-safe display text",
        "TASK-214 does not change TASK-207 readback semantics",
        "TASK-214 does not change TASK-209 / TASK-211 daily-use acceptance semantics",
        "TASK-214 does not write snapshot",
        "TASK-214 does not refresh read model",
        "TASK-214 does not call replay",
        "## Privacy And Sanitization Policy",
        "raw SQL",
        "raw payload",
        "credentials",
        "endpoint",
        "broker payload",
        "internal exception stack",
        "full `trade_log` payload",
        "full `cash_adjustment` payload",
        "sanitized issue code",
        "sanitized memo",
        "safe metadata",
        "## Test Implementation Boundary",
        "documentation token checks",
        "path allowlist checks",
        "no-production-drift static checks",
        "must not execute replay",
        "write SQLite",
        "call AccountingEngine replay",
        "network / broker",
        "create fixture JSON / seed DB files",
        "Future TASK-215",
        "## Out-of-Scope Boundaries",
        "runtime fixture data",
        "DataServiceActions or DataServiceActionRegistrar changes",
        "repository changes",
        "migration changes",
        "audit / ledger writes",
        "backup/export/restore implementation",
        "real broker order id generation",
        "real order placement",
        "automatic trading",
        "## Formal Authorization Conclusion And Next Task",
        "TASK-214 authorizes replay fixture matrix design only.",
        "TASK-214 does not authorize replay implementation.",
        "TASK-214 does not authorize creation of replay fixture files.",
        "TASK-214 does not authorize AccountingEngine replay calls.",
        "TASK-214 does not authorize runtime SQL / SQLite read/write.",
        "TASK-214 does not authorize audit / ledger writes.",
        "TASK-214 does not authorize backup/export/restore.",
        "TASK-214 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
        "Recommended next task: TASK-215 manual entry replay fixture files authorization gate.",
    ]:
        gate.contains(doc224, token, "docs/224")

    for token in [
        "# ShellAccounting Manual Entry Replay Fixture Matrix Authorization Test Plan",
        "## Document Purpose",
        "## Test Matrix",
        "## Required Probes",
        "## Go / No-Go Checklist",
        "docs/224 exists",
        "docs/225 exists",
        "Documentation registration",
        "Gate-only scope",
        "Fixture matrix purpose",
        "Fixture naming policy",
        "Fixture source fact policy",
        "Baseline success fixture matrix",
        "Ordering and idempotency fixture matrix",
        "Position safety fixture matrix",
        "Cash safety fixture matrix",
        "PnL and market dependency fixture matrix",
        "Multi-scope fixture matrix",
        "Unsupported and failure fixture matrix",
        "Expected output policy",
        "Privacy and sanitization policy",
        "Test implementation boundary",
        "Formal conclusion",
        "Production drift",
        "TASK-213 regression",
        "TASK-212 regression",
        "TASK-211 regression",
        "TASK-209 regression",
        "TASK-207 regression",
        "TASK-204 regression",
        "TASK-198 regression",
        "TASK-196 regression",
        "TASK-192 regression",
        "Broker / real broker gates",
        "transport_local_socket_echo 50-repeat regression",
        "Static source scan",
        "Documentation token scan",
        "Changed-path allowlist scan",
        "No production code changed scan",
        "No production QML / startup / Presenter / Controller drift scan",
        "No ShellServices adapter / port drift scan",
        "No DataServiceActions drift scan",
        "No DataServiceActionRegistrar drift scan",
        "No DataAccess repository drift scan",
        "No migrations drift scan",
        "No runtime SQL / SQLite read/write scan",
        "No AccountingEngine replay implementation scan",
        "No AccountingEngine replay call scan",
        "No replay fixture file scan",
        "No seed DB or runtime fixture data scan",
        "No audit / ledger write scan",
        "No snapshot write scan",
        "No backup/export/restore implementation scan",
        "No broker / network / credentials / endpoint scan",
        "No real order / automatic trading scan",
        "Retained TASK-213 / TASK-212 / TASK-211 / TASK-209 / TASK-207 / TASK-204 / TASK-198 / TASK-196 / TASK-192 regression probes",
        "Retained broker and real broker gate probes",
        "Retained transport 50-repeat regression probe",
        "TASK-214 CTest is registered",
        "TASK-214 authorizes fixture matrix design only",
        "TASK-214 does not authorize replay implementation",
        "TASK-214 does not authorize creation of replay fixture files",
        "Replay implementation appears",
        "AccountingEngine replay call appears",
        "Replay fixture JSON / seed DB / runtime fixture files appear",
        "Runtime SQL / SQLite read/write appears",
        "DataServiceActions runtime behavior changes",
        "Repository / migration changes appear",
        "Audit / ledger write appears",
        "Backup/export/restore implementation appears",
        "Real order placement or broker order id appears",
        "Automatic trading appears",
        "Fabricated PnL or fabricated market value appears",
    ]:
        gate.contains(doc225, token, "docs/225")

    authorized_task217_fixture_scaffold_paths = {
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json",
        "tests/fixtures/manual_entry_replay/MRF002_single_buy.json",
        "tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json",
        "tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json",
    }

    def is_authorized_task217_fixture_scaffold_path(path: str) -> bool:
        return path in authorized_task217_fixture_scaffold_paths

    allowed_changes = {
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md",
        "docs/225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md",
        "docs/230_shell_accounting_manual_entry_replay_fixture_files_scaffold.md",
        "docs/231_shell_accounting_manual_entry_replay_fixture_files_scaffold_test_plan.md",
        "tests/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json",
        "tests/fixtures/manual_entry_replay/MRF002_single_buy.json",
        "tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json",
        "tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
        "docs/226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md",
        "docs/227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md",
        "docs/234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md",
        "docs/235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/manual_entry_replay_fixture_static_validator.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py",
    }
    changes = changed_paths(root)
    unexpected = sorted(path for path in changes if path not in allowed_changes)
    gate.require(not unexpected, "TASK-214 changed unauthorized paths: " + ", ".join(unexpected))

    forbidden_prefixes = [
        "apps/",
        "libs/ShellServices/",
        "libs/ShellCore/",
        "libs/DataServiceApi/",
        "libs/DataServiceClient/",
        "libs/DataAccess/",
        "libs/AccountingEngine/",
        "libs/StrategyEngine/",
        "libs/MarketEngine/",
        "libs/Transport/",
        "libs/ServiceRuntime/",
        "libs/ServiceHost/",
        "migrations/",
    ]
    for prefix in forbidden_prefixes:
        gate.require(not any(path.startswith(prefix) for path in changes), f"TASK-214 must not change {prefix}")
    gate.require(not any(path.endswith(".sql") for path in changes), "TASK-214 must not add migration or schema files")
    gate.require(
        not any(
            path.endswith(".json") and not is_authorized_task217_fixture_scaffold_path(path)
            for path in changes
        ),
        "TASK-214 must not add replay fixture JSON files",
    )
    gate.require(
        not any(
            "fixture" in path.lower()
            and path.endswith((".json", ".db", ".sqlite"))
            and not is_authorized_task217_fixture_scaffold_path(path)
            for path in changes
        ),
        "TASK-214 must not add fixture data files",
    )

    production_diff = added_lines(diff_text(root, "apps", "libs", "migrations"))
    for token in [
        "INSERT ",
        "UPDATE ",
        "DELETE ",
        "REPLACE ",
        "audit_log",
        "ledger",
        "snapshot",
        "backup",
        "export",
        "restore",
        "AccountingReplay",
        "runReplay",
        "Broker",
        "broker",
        "network",
        "credentials",
        "endpoint",
        "realOrder",
        "brokerOrderId",
        "automaticTrading",
    ]:
        gate.require(token not in production_diff, f"production diff must not add `{token}`")

    retained_paths = [
        "docs/222_shell_accounting_manual_entry_replay_policy_authorization_gate.md",
        "docs/223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/CMakeLists.txt",
        "docs/220_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_gate.md",
        "docs/221_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualCashMovementRepositoryDualWriteImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualTransactionRepositoryWriteImplementation/CMakeLists.txt",
        "tests/ShellAccountingBrokerAdapterDisabledWiring/CMakeLists.txt",
        "tests/ShellAccountingBrokerOrderImplementation/CMakeLists.txt",
        "tests/ShellAccountingRealBrokerOrderAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingRealBrokerOrderImplementationGate/CMakeLists.txt",
        "tests/Transport/CMakeLists.txt",
    ]
    for path in retained_paths:
        gate.require((root / path).exists(), f"retained path exists: {path}")
    gate.contains(read(root / "tests" / "Transport" / "CMakeLists.txt"), "transport_local_socket_echo", "transport CTest")

    gate.require(gate.checks >= 90, "TASK-214 gate must execute at least 90 checks")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
