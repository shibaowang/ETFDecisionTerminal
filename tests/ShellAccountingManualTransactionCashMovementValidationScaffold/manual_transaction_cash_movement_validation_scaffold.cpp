#include "DataServiceApi/ShellAccountingManualTransactionCashMovementValidation.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

namespace fs = std::filesystem;
using namespace etfdt::data_service_api;

struct Harness {
    fs::path root;
};

std::string readFile(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Unable to read " + path.string());
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

bool contains(const std::string& text, const std::string& token)
{
    return text.find(token) != std::string::npos;
}

void requireContains(const std::string& text, const std::string& token, const std::string& context)
{
    require(contains(text, token), context + " missing `" + token + "`");
}

std::vector<fs::path> filesUnder(const fs::path& root)
{
    std::vector<fs::path> files;
    if (!fs::exists(root)) {
        return files;
    }
    for (const auto& entry : fs::recursive_directory_iterator(root)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }
    return files;
}

void requireNoTokens(const std::vector<fs::path>& files, const std::vector<std::string>& tokens, const std::string& context)
{
    for (const auto& file : files) {
        const auto text = readFile(file);
        for (const auto& token : tokens) {
            if (contains(text, token)) {
                throw std::runtime_error(context + " found `" + token + "` in " + file.string());
            }
        }
    }
}

fs::path headerPath(const Harness& h)
{
    return h.root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" /
        "ShellAccountingManualTransactionCashMovementValidation.h";
}

fs::path sourcePath(const Harness& h)
{
    return h.root / "libs" / "DataServiceApi" / "src" /
        "ShellAccountingManualTransactionCashMovementValidation.cpp";
}

std::vector<fs::path> validationFiles(const Harness& h)
{
    return {headerPath(h), sourcePath(h)};
}

ShellAccountingManualTransactionEntry validTransaction(ShellAccountingManualTradeSide side)
{
    ShellAccountingManualTransactionEntry entry;
    entry.accountId = "ACC-001";
    entry.portfolioId = "PF-001";
    entry.securityCode = "510300.SH";
    entry.tradeSide = side;
    entry.quantityUnits = 10000;
    entry.priceAmountMinor = 395;
    entry.grossAmountMinor = 39500;
    entry.feeAmountMinor = 8;
    entry.taxAmountMinor = 0;
    entry.occurredAt = "2026-06-01T09:45:00+08:00";
    entry.sourceMemo = "local manual entry";
    return entry;
}

ShellAccountingManualCashMovementEntry validCash(ShellAccountingManualCashMovementType type)
{
    ShellAccountingManualCashMovementEntry entry;
    entry.accountId = "ACC-001";
    entry.portfolioId = "PF-001";
    entry.movementType = type;
    entry.amountMinor = 1000000;
    entry.occurredAt = "2026-06-01T09:30:00+08:00";
    entry.sourceMemo = "local cash entry";
    return entry;
}

bool hasIssue(const ShellAccountingManualEntryValidationResult& result, const std::string& code)
{
    return std::any_of(result.issues.begin(), result.issues.end(), [&](const auto& issue) {
        return issue.code == code;
    });
}

void testDocs(const Harness& h)
{
    require(fs::exists(h.root / "docs" / "154_shell_accounting_manual_transaction_cash_movement_dto_validation_scaffold.md"),
        "docs/154 must exist");
    require(fs::exists(h.root / "docs" / "155_shell_accounting_manual_transaction_cash_movement_dto_validation_test_plan.md"),
        "docs/155 must exist");
    const auto doc154 = readFile(h.root / "docs" / "154_shell_accounting_manual_transaction_cash_movement_dto_validation_scaffold.md");
    const auto doc155 = readFile(h.root / "docs" / "155_shell_accounting_manual_transaction_cash_movement_dto_validation_test_plan.md");
    requireContains(doc154, "TASK-178", "docs/154");
    requireContains(doc154, "DTO / validation scaffold", "docs/154");
    requireContains(doc154, "does not write SQLite", "docs/154");
    requireContains(doc154, "does not add a DataService action", "docs/154");
    requireContains(doc155, "Test Matrix", "docs/155");
    requireContains(doc155, "Forbidden Boundary", "docs/155");
}

void testDocsIndex(const Harness& h)
{
    const auto readme = readFile(h.root / "README.md");
    const auto docsReadme = readFile(h.root / "docs" / "README.md");
    requireContains(readme, "docs/154_shell_accounting_manual_transaction_cash_movement_dto_validation_scaffold.md", "README");
    requireContains(readme, "docs/155_shell_accounting_manual_transaction_cash_movement_dto_validation_test_plan.md", "README");
    requireContains(docsReadme, "154_shell_accounting_manual_transaction_cash_movement_dto_validation_scaffold.md", "docs/README");
    requireContains(docsReadme, "155_shell_accounting_manual_transaction_cash_movement_dto_validation_test_plan.md", "docs/README");
}

void testPromptTemplate(const Harness& h)
{
    const auto prompt = readFile(h.root / "docs" / "12_codex_prompt_template.md");
    requireContains(prompt, "TASK-178", "docs/12");
    requireContains(prompt, "docs/154", "docs/12");
    requireContains(prompt, "docs/155", "docs/12");
    requireContains(prompt, "in-memory DTO / validation scaffold", "docs/12");
}

void testHeaderExists(const Harness& h)
{
    require(fs::exists(headerPath(h)), "validation header must exist");
    const auto header = readFile(headerPath(h));
    requireContains(header, "ShellAccountingManualTransactionEntry", "validation header");
    requireContains(header, "ShellAccountingManualCashMovementEntry", "validation header");
    requireContains(header, "validateManualTransactionEntry", "validation header");
    requireContains(header, "validateManualCashMovement", "validation header");
}

void testSourceExists(const Harness& h)
{
    require(fs::exists(sourcePath(h)), "validation source must exist");
}

void testCmakeRegistered(const Harness& h)
{
    const auto cmake = readFile(h.root / "libs" / "DataServiceApi" / "CMakeLists.txt");
    requireContains(cmake, "ShellAccountingManualTransactionCashMovementValidation.cpp", "DataServiceApi CMake");
}

void testBuyValid()
{
    const auto result = validateManualTransactionEntry(validTransaction(ShellAccountingManualTradeSide::Buy));
    require(result.accepted, "valid buy should be accepted");
    require(result.valid(), "valid buy should report valid");
}

void testSellValid()
{
    const auto result = validateManualTransactionEntry(validTransaction(ShellAccountingManualTradeSide::Sell));
    require(result.accepted, "valid sell should be accepted");
}

void testDepositValid()
{
    const auto result = validateManualCashMovement(validCash(ShellAccountingManualCashMovementType::Deposit));
    require(result.accepted, "valid deposit should be accepted");
}

void testWithdrawalValid()
{
    const auto result = validateManualCashMovement(validCash(ShellAccountingManualCashMovementType::Withdrawal));
    require(result.accepted, "valid withdrawal should be accepted");
}

void testInvalidTradeSide()
{
    auto entry = validTransaction(ShellAccountingManualTradeSide::Buy);
    entry.tradeSide = ShellAccountingManualTradeSide::Unknown;
    const auto result = validateManualTransactionEntry(entry);
    require(!result.accepted, "unknown trade side should be rejected");
    require(hasIssue(result, "TRADE_SIDE_INVALID"), "trade side issue expected");
}

void testInvalidCashType()
{
    auto entry = validCash(ShellAccountingManualCashMovementType::Deposit);
    entry.movementType = ShellAccountingManualCashMovementType::Unknown;
    const auto result = validateManualCashMovement(entry);
    require(!result.accepted, "unknown cash movement type should be rejected");
    require(hasIssue(result, "CASH_MOVEMENT_TYPE_INVALID"), "cash movement type issue expected");
}

void testQuantityPositive()
{
    auto entry = validTransaction(ShellAccountingManualTradeSide::Buy);
    entry.quantityUnits = 0;
    require(hasIssue(validateManualTransactionEntry(entry), "QUANTITY_POSITIVE_REQUIRED"), "quantity issue expected");
}

void testPricePositive()
{
    auto entry = validTransaction(ShellAccountingManualTradeSide::Buy);
    entry.priceAmountMinor = 0;
    require(hasIssue(validateManualTransactionEntry(entry), "PRICE_POSITIVE_REQUIRED"), "price issue expected");
}

void testGrossPositive()
{
    auto entry = validTransaction(ShellAccountingManualTradeSide::Buy);
    entry.grossAmountMinor = 0;
    require(hasIssue(validateManualTransactionEntry(entry), "GROSS_AMOUNT_POSITIVE_REQUIRED"), "gross amount issue expected");
}

void testFeeTaxNonNegative()
{
    auto feeEntry = validTransaction(ShellAccountingManualTradeSide::Buy);
    feeEntry.feeAmountMinor = -1;
    require(hasIssue(validateManualTransactionEntry(feeEntry), "FEE_NON_NEGATIVE_REQUIRED"), "fee issue expected");

    auto taxEntry = validTransaction(ShellAccountingManualTradeSide::Buy);
    taxEntry.taxAmountMinor = -1;
    require(hasIssue(validateManualTransactionEntry(taxEntry), "TAX_NON_NEGATIVE_REQUIRED"), "tax issue expected");
}

void testCashAmountPositive()
{
    auto entry = validCash(ShellAccountingManualCashMovementType::Deposit);
    entry.amountMinor = 0;
    require(hasIssue(validateManualCashMovement(entry), "CASH_AMOUNT_POSITIVE_REQUIRED"), "cash amount issue expected");
}

void testRequiredIds()
{
    auto tx = validTransaction(ShellAccountingManualTradeSide::Buy);
    tx.accountId.clear();
    tx.portfolioId.clear();
    tx.securityCode.clear();
    const auto txResult = validateManualTransactionEntry(tx);
    require(hasIssue(txResult, "ACCOUNT_ID_REQUIRED"), "account issue expected");
    require(hasIssue(txResult, "PORTFOLIO_ID_REQUIRED"), "portfolio issue expected");
    require(hasIssue(txResult, "SECURITY_CODE_REQUIRED"), "security issue expected");

    auto cash = validCash(ShellAccountingManualCashMovementType::Deposit);
    cash.accountId.clear();
    cash.portfolioId.clear();
    const auto cashResult = validateManualCashMovement(cash);
    require(hasIssue(cashResult, "ACCOUNT_ID_REQUIRED"), "cash account issue expected");
    require(hasIssue(cashResult, "PORTFOLIO_ID_REQUIRED"), "cash portfolio issue expected");
}

void testOccurredAtRequired()
{
    auto tx = validTransaction(ShellAccountingManualTradeSide::Buy);
    tx.occurredAt.clear();
    require(hasIssue(validateManualTransactionEntry(tx), "OCCURRED_AT_REQUIRED"), "transaction occurredAt issue expected");

    auto cash = validCash(ShellAccountingManualCashMovementType::Deposit);
    cash.occurredAt.clear();
    require(hasIssue(validateManualCashMovement(cash), "OCCURRED_AT_REQUIRED"), "cash occurredAt issue expected");
}

void testSensitiveMemoPolicy()
{
    auto tx = validTransaction(ShellAccountingManualTradeSide::Buy);
    tx.sourceMemo = "password=example";
    require(hasIssue(validateManualTransactionEntry(tx), "SENSITIVE_MEMO_TOKEN"), "sensitive transaction memo issue expected");

    auto cash = validCash(ShellAccountingManualCashMovementType::Deposit);
    cash.sourceMemo = "token abc";
    require(hasIssue(validateManualCashMovement(cash), "SENSITIVE_MEMO_TOKEN"), "sensitive cash memo issue expected");
    require(containsSensitiveManualEntryMemoToken("api_key=example"), "memo helper should detect api_key");
}

void testValidationNoSqlite(const Harness& h)
{
    requireNoTokens(validationFiles(h), {"SQLite", "sqlite3", "SQLiteConnection"}, "validation SQLite access");
}

void testValidationNoTradeLogWrite(const Harness& h)
{
    requireNoTokens(validationFiles(h), {"trade_log", "insertTradeLog", "appendTradeLog"}, "validation trade_log write");
}

void testValidationNoCashFactWrite(const Harness& h)
{
    requireNoTokens(validationFiles(h), {"cash_facts", "insertCashFact", "cash_snapshot"}, "validation cash fact write");
}

void testValidationNoAuditLedgerWrite(const Harness& h)
{
    requireNoTokens(validationFiles(h), {"audit_log", "ledger", "trade_execution_group"}, "validation audit/ledger write");
}

void testValidationNoTradeDraft(const Harness& h)
{
    requireNoTokens(validationFiles(h), {"TradeDraft", "trade_draft", "createTradeDraft"}, "validation TradeDraft creation");
}

void testValidationNoBroker(const Harness& h)
{
    requireNoTokens(validationFiles(h), {"Broker", "broker", "orderPlacement", "placeOrder"}, "validation broker call");
}

void testValidationNoNetworkEndpoint(const Harness& h)
{
    requireNoTokens(validationFiles(h), {"QNetwork", "http://", "https://", "socket", "endpointUrl"}, "validation network endpoint");
}

void testValidationNoCredentialsRuntimeSource(const Harness& h)
{
    requireNoTokens(validationFiles(h), {"getenv", "secretStore", "readCredential", "credentialProvider"}, "validation credential runtime source");
}

void testDataServiceActionsUnmodified(const Harness& h)
{
    const auto source = readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
    require(!contains(source, "accounting.manual_transaction"), "DataServiceActions must not use raw manual transaction action string");
    require(!contains(source, "accounting.cash_movement"), "DataServiceActions must not use deprecated cash movement action string");
    requireContains(source, "validateManualTransactionEntry", "DataServiceActions TASK-182 manual transaction validation wiring");
    requireContains(source, "validateManualCashMovement", "DataServiceActions TASK-182 manual cash validation wiring");
    require(!contains(source, "insertManualTransaction"), "DataServiceActions must not implement manual transaction write");
    require(!contains(source, "insertCashMovement"), "DataServiceActions must not implement manual cash write");
}

void testSchemaUnmodified(const Harness& h)
{
    const auto migration = readFile(h.root / "migrations" / "001_initial_schema.sql");
    require(!contains(migration, "manual_transaction"), "schema must not add manual_transaction");
    require(!contains(migration, "cash_movement"), "schema must not add cash_movement");
}

void testProductionQmlUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "apps" / "ETFDecisionShell" / "qml"), {
        "manualTransaction",
        "manualCashMovement",
        "recordDeposit",
        "recordWithdraw",
    }, "production QML");
}

void testStartupUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "apps" / "ETFDecisionShell" / "src"), {
        "ManualTransaction",
        "ManualCashMovement",
        "registerManual",
    }, "production startup");
}

void testNoDataAccessWriteRepository(const Harness& h)
{
    std::vector<fs::path> files = filesUnder(h.root / "libs" / "DataAccess" / "include");
    auto src = filesUnder(h.root / "libs" / "DataAccess" / "src");
    files.insert(files.end(), src.begin(), src.end());
    // TASK-196 authorizes ShellAccountingManualCashMovementRepository as a
    // DataAccess-only direct repository. This legacy validation gate still
    // blocks older runtime/scaffold write escape hatches.
    requireNoTokens(files, {
        "insertManualTransaction",
        "insertCashMovement",
    }, "DataAccess manual write repository");
}

void testAccountingEngineReplayUnmodified(const Harness& h)
{
    std::vector<fs::path> files = filesUnder(h.root / "libs" / "AccountingEngine" / "include");
    auto src = filesUnder(h.root / "libs" / "AccountingEngine" / "src");
    files.insert(files.end(), src.begin(), src.end());
    requireNoTokens(files, {
        "ManualTransaction",
        "ManualCashMovement",
        "validateManualTransactionEntry",
        "validateManualCashMovement",
    }, "AccountingEngine manual replay");
}

void testStrategyMarketUnmodified(const Harness& h)
{
    std::vector<fs::path> files = filesUnder(h.root / "libs" / "StrategyEngine");
    auto market = filesUnder(h.root / "libs" / "MarketEngine");
    files.insert(files.end(), market.begin(), market.end());
    requireNoTokens(files, {
        "ManualTransaction",
        "ManualCashMovement",
        "manualTradeSuggestion",
        "goldenTowerSuggestion",
        "sniperSuggestion",
    }, "StrategyEngine/MarketEngine");
}

void testExistingBrokerGatesRetained(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "CMakeLists.txt");
    requireContains(cmake, "ShellAccountingBrokerAdapterDisabledWiring", "tests/CMake broker gates");
    requireContains(cmake, "ShellAccountingBrokerOrderImplementation", "tests/CMake broker gates");
    requireContains(cmake, "ShellAccountingRealBrokerOrderAuthorizationGate", "tests/CMake broker gates");
    requireContains(cmake, "ShellAccountingRealBrokerOrderImplementationGate", "tests/CMake broker gates");
}

void testNoGateWeakeningOrSkip(const Harness& h)
{
    const std::vector<fs::path> gateFiles = {
        h.root / "tests" / "ShellAccountingBrokerAdapterDisabledWiring" / "CMakeLists.txt",
        h.root / "tests" / "ShellAccountingBrokerOrderImplementation" / "CMakeLists.txt",
        h.root / "tests" / "ShellAccountingRealBrokerOrderAuthorizationGate" / "CMakeLists.txt",
        h.root / "tests" / "ShellAccountingRealBrokerOrderImplementationGate" / "CMakeLists.txt",
    };
    for (const auto& file : gateFiles) {
        const auto text = readFile(file);
        require(!contains(text, "DISABLED"), "broker gate must not be disabled in " + file.string());
        require(!contains(text, "SKIP_REGULAR_EXPRESSION"), "broker gate must not be skipped in " + file.string());
        require(!contains(text, "WILL_FAIL"), "broker gate must not be weakened in " + file.string());
    }
}

void runCase(const Harness& h, const std::string& name)
{
    if (name == "docs") return testDocs(h);
    if (name == "docs_index") return testDocsIndex(h);
    if (name == "prompt_template") return testPromptTemplate(h);
    if (name == "header_exists") return testHeaderExists(h);
    if (name == "source_exists") return testSourceExists(h);
    if (name == "cmake_registered") return testCmakeRegistered(h);
    if (name == "buy_valid") return testBuyValid();
    if (name == "sell_valid") return testSellValid();
    if (name == "deposit_valid") return testDepositValid();
    if (name == "withdrawal_valid") return testWithdrawalValid();
    if (name == "invalid_trade_side") return testInvalidTradeSide();
    if (name == "invalid_cash_type") return testInvalidCashType();
    if (name == "quantity_positive") return testQuantityPositive();
    if (name == "price_positive") return testPricePositive();
    if (name == "gross_positive") return testGrossPositive();
    if (name == "fee_tax_non_negative") return testFeeTaxNonNegative();
    if (name == "cash_amount_positive") return testCashAmountPositive();
    if (name == "required_ids") return testRequiredIds();
    if (name == "occurred_at_required") return testOccurredAtRequired();
    if (name == "sensitive_memo_policy") return testSensitiveMemoPolicy();
    if (name == "no_sqlite_access") return testValidationNoSqlite(h);
    if (name == "no_trade_log_write") return testValidationNoTradeLogWrite(h);
    if (name == "no_cash_fact_write") return testValidationNoCashFactWrite(h);
    if (name == "no_audit_ledger_write") return testValidationNoAuditLedgerWrite(h);
    if (name == "no_tradedraft") return testValidationNoTradeDraft(h);
    if (name == "no_broker") return testValidationNoBroker(h);
    if (name == "no_network_endpoint") return testValidationNoNetworkEndpoint(h);
    if (name == "no_credentials_runtime_source") return testValidationNoCredentialsRuntimeSource(h);
    if (name == "dataserviceactions_unmodified") return testDataServiceActionsUnmodified(h);
    if (name == "schema_unmodified") return testSchemaUnmodified(h);
    if (name == "production_qml_unmodified") return testProductionQmlUnmodified(h);
    if (name == "startup_unmodified") return testStartupUnmodified(h);
    if (name == "no_dataaccess_write_repository") return testNoDataAccessWriteRepository(h);
    if (name == "accountingengine_replay_unmodified") return testAccountingEngineReplayUnmodified(h);
    if (name == "strategy_market_unmodified") return testStrategyMarketUnmodified(h);
    if (name == "existing_broker_gates_retained") return testExistingBrokerGatesRetained(h);
    if (name == "no_gate_weakening_or_skip") return testNoGateWeakeningOrSkip(h);
    throw std::runtime_error("Unknown case: " + name);
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        Harness h;
        std::string caseName;
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--source-root" && i + 1 < argc) {
                h.root = argv[++i];
            } else if (arg == "--case" && i + 1 < argc) {
                caseName = argv[++i];
            }
        }
        require(!h.root.empty(), "--source-root is required");
        require(!caseName.empty(), "--case is required");
        runCase(h, caseName);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
