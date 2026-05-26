import argparse
import json
import re
from pathlib import Path


# Static JSON validation only. This script does not access SQLite, does not
# call DataService, does not use network resources, and does not calculate
# accounting replay outputs.
REQUIRED_FIXTURES = [
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

REQUIRED_TOP_LEVEL = {
    "fixtureId",
    "name",
    "purpose",
    "schemaVersion",
    "externalDependencies",
    "defaultScope",
    "inputFacts",
    "expectedOutputs",
    "expectedIssues",
    "blocking",
    "notes",
}

REQUIRED_INPUT_FACTS = {"tradeFacts", "cashFacts", "marketPriceFacts"}
REQUIRED_EXPECTED_OUTPUTS = {"positionSummaries", "cashSummary", "portfolioPnl"}

EXPECTED_ERROR_CODES = {
    "FX004_SELL_EXCEEDS_POSITION": {"SELL_EXCEEDS_POSITION"},
    "FX005_MISSING_FEE": {"MISSING_FEE"},
    "FX006_NEGATIVE_CASH": {"NEGATIVE_CASH"},
    "FX011_STALE_SNAPSHOT": {"SNAPSHOT_STALE"},
    "FX012_MISSING_MARKET_PRICE": {"MARKET_PRICE_MISSING"},
    "FX013_MULTI_CURRENCY_UNSUPPORTED": {"MULTI_CURRENCY_UNSUPPORTED", "FX_RATE_MISSING"},
}

FORBIDDEN_TEXT = [
    "data.audit.append",
    "--serve-dev-audit",
    "writeActions",
    "serviceAction",
    "brokerOrder",
    "placeOrder",
    "executeTrade",
    "real broker",
    "real account",
]


def fail(message: str) -> None:
    raise AssertionError(message)


def load_json(path: Path) -> object:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        fail(f"{path}: invalid JSON: {exc}")


def collect_issue_codes(fixture: dict) -> set[str]:
    codes: set[str] = set()
    for issue in fixture.get("expectedIssues", []):
        if isinstance(issue, dict) and isinstance(issue.get("code"), str):
            codes.add(issue["code"])
    return codes


def assert_no_forbidden_text(path: Path, text: str) -> None:
    lowered = text.lower()
    for token in FORBIDDEN_TEXT:
        if token.lower() in lowered:
            fail(f"{path}: forbidden token found: {token}")
    if re.search(r"https?://", text, flags=re.IGNORECASE):
        fail(f"{path}: external URL is not allowed")
    if re.search(r"\b\d{11}\b", text):
        fail(f"{path}: possible personal phone number is not allowed")
    if re.search(r"[\w.+-]+@[\w-]+\.[\w.-]+", text):
        fail(f"{path}: possible personal email is not allowed")


def validate_fixture(path: Path, fixture_id: str) -> None:
    text = path.read_text(encoding="utf-8")
    assert_no_forbidden_text(path, text)
    fixture = load_json(path)
    if not isinstance(fixture, dict):
        fail(f"{path}: fixture must be a JSON object")

    missing = REQUIRED_TOP_LEVEL - fixture.keys()
    if missing:
        fail(f"{path}: missing top-level fields: {sorted(missing)}")
    if fixture.get("fixtureId") != fixture_id:
        fail(f"{path}: fixtureId mismatch")
    if fixture.get("externalDependencies") != []:
        fail(f"{path}: externalDependencies must be []")
    if not isinstance(fixture.get("blocking"), bool):
        fail(f"{path}: blocking must be boolean")
    if not isinstance(fixture.get("notes"), list):
        fail(f"{path}: notes must be an array")
    if not isinstance(fixture.get("expectedIssues"), list):
        fail(f"{path}: expectedIssues must be an array")

    input_facts = fixture.get("inputFacts")
    if not isinstance(input_facts, dict):
        fail(f"{path}: inputFacts must be an object")
    missing_facts = REQUIRED_INPUT_FACTS - input_facts.keys()
    if missing_facts:
        fail(f"{path}: missing inputFacts fields: {sorted(missing_facts)}")
    for key in REQUIRED_INPUT_FACTS:
        if not isinstance(input_facts.get(key), list):
            fail(f"{path}: inputFacts.{key} must be an array")

    outputs = fixture.get("expectedOutputs")
    if not isinstance(outputs, dict):
        fail(f"{path}: expectedOutputs must be an object")
    missing_outputs = REQUIRED_EXPECTED_OUTPUTS - outputs.keys()
    if missing_outputs:
        fail(f"{path}: missing expectedOutputs fields: {sorted(missing_outputs)}")
    if not isinstance(outputs.get("positionSummaries"), list):
        fail(f"{path}: expectedOutputs.positionSummaries must be an array")
    if not isinstance(outputs.get("cashSummary"), dict):
        fail(f"{path}: expectedOutputs.cashSummary must be an object")
    if not isinstance(outputs.get("portfolioPnl"), dict):
        fail(f"{path}: expectedOutputs.portfolioPnl must be an object")

    expected_codes = EXPECTED_ERROR_CODES.get(fixture_id)
    if expected_codes:
        actual_codes = collect_issue_codes(fixture)
        if fixture_id == "FX013_MULTI_CURRENCY_UNSUPPORTED":
            if actual_codes.isdisjoint(expected_codes):
                fail(f"{path}: expected at least one of {sorted(expected_codes)}")
        elif not expected_codes.issubset(actual_codes):
            fail(f"{path}: expected issue codes {sorted(expected_codes)}")


def validate_index(fixtures_dir: Path) -> None:
    index_path = fixtures_dir / "fixtures_index.json"
    if not index_path.exists():
        fail("fixtures_index.json is missing")
    index_text = index_path.read_text(encoding="utf-8")
    assert_no_forbidden_text(index_path, index_text)
    index = load_json(index_path)
    if not isinstance(index, dict):
        fail("fixtures_index.json must be an object")
    if index.get("externalDependencies") != []:
        fail("fixtures_index.json externalDependencies must be []")
    if "not database rows" not in index.get("description", ""):
        fail("fixtures_index.json description must state fixtures are not database rows")
    fixtures = index.get("fixtures")
    if not isinstance(fixtures, list):
        fail("fixtures_index.json fixtures must be an array")

    seen: set[str] = set()
    files_by_id: dict[str, str] = {}
    for item in fixtures:
        if not isinstance(item, dict):
            fail("fixtures_index.json fixture entry must be an object")
        fixture_id = item.get("fixtureId")
        file_name = item.get("file")
        if not isinstance(fixture_id, str) or not isinstance(file_name, str):
            fail("fixtures_index.json fixture entry requires fixtureId and file")
        if fixture_id in seen:
            fail(f"duplicate fixtureId: {fixture_id}")
        seen.add(fixture_id)
        files_by_id[fixture_id] = file_name

    missing_ids = set(REQUIRED_FIXTURES) - seen
    extra_ids = seen - set(REQUIRED_FIXTURES)
    if missing_ids or extra_ids:
        fail(f"fixture id mismatch; missing={sorted(missing_ids)} extra={sorted(extra_ids)}")

    for fixture_id in REQUIRED_FIXTURES:
        fixture_path = fixtures_dir / files_by_id[fixture_id]
        if not fixture_path.exists():
            fail(f"{fixture_id}: file does not exist: {fixture_path}")
        validate_fixture(fixture_path, fixture_id)


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate static accounting replay fixtures.")
    parser.add_argument("--fixtures-dir", required=True)
    args = parser.parse_args()
    validate_index(Path(args.fixtures_dir))
    print("Accounting replay fixture static validation passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
