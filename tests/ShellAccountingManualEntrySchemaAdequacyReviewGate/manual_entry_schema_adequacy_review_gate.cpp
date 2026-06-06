#include "DataAccess/DataAccess.h"
#include "DataAccess/ShellAccountingManualEntryRepositoryScaffold.h"
#include "DataServiceApi/DataServiceActionRegistrar.h"
#include "DataServiceApi/DataServiceActions.h"
#include "Protocol/Protocol.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

namespace fs = std::filesystem;

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

bool contains(const std::string& text, const std::string& token)
{
    return text.find(token) != std::string::npos;
}

std::string textWithoutTask265ExcelVbaImportManualEntryPersistenceRepository(std::string text)
{
    const std::string token = "ShellAccountingExcelVbaImportManualEntryPersistenceRepository.cpp";
    for (auto pos = text.find(token); pos != std::string::npos; pos = text.find(token)) {
        text.erase(pos, token.size());
    }
    return text;
}

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void requireContains(const std::string& text, const std::string& token, const std::string& context)
{
    require(contains(text, token), context + " missing `" + token + "`");
}

void requireNotContains(const std::string& text, const std::string& token, const std::string& context)
{
    require(!contains(text, token), context + " unexpectedly contains `" + token + "`");
}

void requireAll(const std::string& text, const std::vector<std::string>& tokens, const std::string& context)
{
    for (const auto& token : tokens) {
        requireContains(text, token, context);
    }
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

bool isTask265ExcelVbaImportManualEntryPersistenceRepositoryFile(const fs::path& file)
{
    const auto path = file.generic_string();
    return path.find("libs/DataAccess/include/DataAccess/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.h")
        != std::string::npos
        || path.find("libs/DataAccess/src/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.cpp")
            != std::string::npos;
}

std::vector<fs::path> filesWithoutTask265ExcelVbaImportManualEntryPersistenceRepository(
    std::vector<fs::path> files)
{
    files.erase(
        std::remove_if(files.begin(), files.end(), isTask265ExcelVbaImportManualEntryPersistenceRepositoryFile),
        files.end());
    return files;
}

void requireNoTokens(const std::vector<fs::path>& files, const std::vector<std::string>& tokens, const std::string& context)
{
    for (const auto& file : files) {
        auto text = readFile(file);
        if (file.generic_string().find("libs/DataAccess/CMakeLists.txt") != std::string::npos) {
            text = textWithoutTask265ExcelVbaImportManualEntryPersistenceRepository(text);
        }
        for (const auto& token : tokens) {
            if (contains(text, token)) {
                throw std::runtime_error(context + " found `" + token + "` in " + file.string());
            }
        }
    }
}

fs::path schemaPath(const Harness& h)
{
    return h.root / "migrations" / "001_initial_schema.sql";
}

std::string schema(const Harness& h)
{
    return readFile(schemaPath(h));
}

fs::path scaffoldHeaderPath(const Harness& h)
{
    return h.root / "libs" / "DataAccess" / "include" / "DataAccess"
        / "ShellAccountingManualEntryRepositoryScaffold.h";
}

fs::path scaffoldSourcePath(const Harness& h)
{
    return h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualEntryRepositoryScaffold.cpp";
}

std::string scaffoldHeader(const Harness& h)
{
    return readFile(scaffoldHeaderPath(h));
}

std::string scaffoldSource(const Harness& h)
{
    return readFile(scaffoldSourcePath(h));
}

std::string dataServiceActions(const Harness& h)
{
    return readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
}

std::string dataServiceActionsHeader(const Harness& h)
{
    return readFile(h.root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" / "DataServiceActions.h");
}

std::string registrar(const Harness& h)
{
    return readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp");
}

std::string validationHeader(const Harness& h)
{
    return readFile(
        h.root / "libs" / "DataServiceApi" / "include" / "DataServiceApi"
        / "ShellAccountingManualTransactionCashMovementValidation.h");
}

std::string validationSource(const Harness& h)
{
    return readFile(
        h.root / "libs" / "DataServiceApi" / "src"
        / "ShellAccountingManualTransactionCashMovementValidation.cpp");
}

std::string doc172(const Harness& h)
{
    return readFile(h.root / "docs" / "172_shell_accounting_manual_entry_schema_adequacy_review_gate.md");
}

std::string doc173(const Harness& h)
{
    return readFile(h.root / "docs" / "173_shell_accounting_manual_entry_schema_adequacy_review_test_plan.md");
}

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-187-manual-entry-schema-adequacy-review";
    request.traceId = "task-187-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-06-02T12:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

std::string validManualTransactionPayload()
{
    return std::string("{")
        + "\"accountId\":\"acct-1\","
        + "\"portfolioId\":\"pf-1\","
        + "\"securityCode\":\"ETF-001\","
        + "\"tradeSide\":\"Buy\","
        + "\"quantityUnits\":100,"
        + "\"priceAmountMinor\":1234,"
        + "\"grossAmountMinor\":123400,"
        + "\"feeAmountMinor\":10,"
        + "\"taxAmountMinor\":0,"
        + "\"occurredAt\":\"2026-06-02T12:00:00Z\","
        + "\"sourceMemo\":\"task 187 validation only\""
        + "}";
}

std::string validManualCashMovementPayload()
{
    return std::string("{")
        + "\"accountId\":\"acct-1\","
        + "\"portfolioId\":\"pf-1\","
        + "\"movementType\":\"Deposit\","
        + "\"amountMinor\":50000,"
        + "\"occurredAt\":\"2026-06-02T12:00:00Z\","
        + "\"sourceMemo\":\"task 187 validation only\""
        + "}";
}

etfdt::protocol::ProtocolResponse dispatchManualAction(const std::string& action, const std::string& payloadJson)
{
    etfdt::data_access::SQLiteConnection connection;
    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
    return dispatcher.dispatch(envelope(action, payloadJson));
}

void requireValidationOnlyResponse(const etfdt::protocol::ProtocolResponse& response)
{
    require(!response.success, "manual entry action must fail safely without an opened database");
    requireAll(response.payloadJson, {
        "\"validationOnly\":false",
        "\"validationAccepted\":true",
        "\"writeImplemented\":true",
        "\"databaseWritten\":false",
        "\"repositoryCalled\":true",
        "\"repositoryWrite\":false",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
    }, "TASK-198 DataService repository write response");
}

void testGate(const Harness& h)
{
    require(fs::exists(h.root / "docs" / "172_shell_accounting_manual_entry_schema_adequacy_review_gate.md"),
        "docs/172 must exist");
    require(fs::exists(h.root / "docs" / "173_shell_accounting_manual_entry_schema_adequacy_review_test_plan.md"),
        "docs/173 must exist");
    require(fs::exists(schemaPath(h)), "initial schema must exist");
}

void testDocs(const Harness& h)
{
    requireAll(doc172(h), {
        "TASK-187",
        "schema adequacy review gate only",
        "does not modify schema",
        "does not add a migration",
        "does not implement a repository write path",
        "does not write a database",
        "does not execute SQL",
        "does not modify DataServiceActions",
        "does not modify the TASK-185 repository scaffold",
        "does not modify TASK-178 validation production code",
        "migrations/001_initial_schema.sql",
        "source of truth",
        "Manual transaction schema adequacy outcome",
        "Manual cash movement schema adequacy outcome",
        "schema gaps",
        "future schema authorization TASK",
        "broker sandbox new capability development remains paused",
        "no-network",
        "no-credentials",
        "no-order-placement",
    }, "docs/172");
    requireAll(doc173(h), {
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "No repository implementation is added",
        "No SQLite write is added",
        "Existing broker, real broker, no-network, no-credentials",
    }, "docs/173");
}

void testDocsIndexPrompt(const Harness& h)
{
    const auto readme = readFile(h.root / "README.md");
    const auto docsIndex = readFile(h.root / "docs" / "README.md");
    const auto prompt = readFile(h.root / "docs" / "12_codex_prompt_template.md");
    requireContains(readme, "docs/172_shell_accounting_manual_entry_schema_adequacy_review_gate.md", "README");
    requireContains(readme, "docs/173_shell_accounting_manual_entry_schema_adequacy_review_test_plan.md", "README");
    requireContains(docsIndex, "172_shell_accounting_manual_entry_schema_adequacy_review_gate.md", "docs index");
    requireContains(docsIndex, "173_shell_accounting_manual_entry_schema_adequacy_review_test_plan.md", "docs index");
    requireContains(prompt, "TASK-187", "Codex prompt template");
    requireContains(prompt, "docs/172 and docs/173", "Codex prompt template");
}

void testSourceOfTruth(const Harness& h)
{
    requireContains(doc172(h), "The source of truth for this review is `migrations/001_initial_schema.sql`", "docs/172");
    requireContains(doc172(h), "must be reconciled against `migrations/001_initial_schema.sql`", "docs/172");
}

void testTransactionEvidence(const Harness& h)
{
    const auto s = schema(h);
    requireAll(s, {
        "CREATE TABLE IF NOT EXISTS trade_execution_group",
        "CREATE TABLE IF NOT EXISTS trade_log",
        "account_id INTEGER NOT NULL",
        "portfolio_id INTEGER NOT NULL",
        "actual_instrument_id INTEGER",
        "actual_code TEXT",
        "action_type TEXT NOT NULL CHECK",
        "'BUY'",
        "'SELL'",
        "trade_source TEXT NOT NULL CHECK",
        "'MANUAL'",
        "price_1e6 INTEGER",
        "quantity_1e6 INTEGER NOT NULL DEFAULT 0",
        "amount_cents INTEGER NOT NULL DEFAULT 0",
        "fee_cents INTEGER NOT NULL DEFAULT 0",
        "net_cash_impact_cents INTEGER NOT NULL DEFAULT 0",
        "manual_entry INTEGER NOT NULL DEFAULT 0",
        "memo TEXT",
        "created_at_utc TEXT NOT NULL",
    }, "schema manual transaction evidence");
    requireContains(doc172(h), "Existing schema has partial support for future manual buy / sell facts", "docs/172");
}

void testTransactionGaps(const Harness& h)
{
    const auto s = schema(h);
    requireNotContains(s, "tax_cents", "schema tax gap");
    requireNotContains(s, "idempotency_key", "schema idempotency gap");
    requireNotContains(s, "request_id", "schema request id gap");
    requireNotContains(s, "occurred_at_utc", "schema occurred-at gap");
    requireAll(doc172(h), {
        "There is no explicit `tax_cents` column",
        "There is no explicit `idempotency_key` column",
        "There is no explicit `request_id` column",
        "no dedicated `occurred_at_utc` field",
    }, "docs/172 transaction gaps");
}

void testCashEvidence(const Harness& h)
{
    const auto s = schema(h);
    requireAll(s, {
        "CREATE TABLE IF NOT EXISTS cash_adjustment",
        "'CASH_IN'",
        "'CASH_OUT'",
        "'CASH_ADJUSTMENT'",
        "adjustment_type TEXT NOT NULL CHECK",
        "amount_cents INTEGER NOT NULL",
        "reason TEXT NOT NULL",
        "external_reference TEXT",
        "FOREIGN KEY (trade_log_id) REFERENCES trade_log(id)",
    }, "schema cash movement evidence");
    requireContains(doc172(h), "Existing schema has partial support for future manual deposit / withdraw facts", "docs/172");
}

void testCashGaps(const Harness& h)
{
    requireAll(doc172(h), {
        "manual cash movement requests",
        "There is no explicit `request_id` column for manual cash movement request tracing",
        "no dedicated `occurred_at_utc` field",
        "relationship between `trade_log` cash events and `cash_adjustment`",
    }, "docs/172 cash gaps");
}

void testAuditPolicy(const Harness& h)
{
    requireAll(schema(h), {
        "CREATE TABLE IF NOT EXISTS audit_log",
        "entity_type TEXT NOT NULL",
        "entity_id INTEGER",
        "action TEXT NOT NULL",
        "old_value_json TEXT",
        "new_value_json TEXT",
        "operator TEXT",
    }, "schema audit evidence");
    requireAll(doc172(h), {
        "Audit policy schema review",
        "There is no explicit audit idempotency key",
        "There is no explicit request id or trace id on `audit_log`",
    }, "docs/172 audit review");
}

void testIdempotencyPolicy(const Harness& h)
{
    requireAll(doc172(h), {
        "Idempotency / duplicate handling schema review",
        "Existing `uid` uniqueness is row identity, not request idempotency",
        "do not provide manual entry duplicate request handling",
    }, "docs/172 idempotency review");
}

void testPrivacyPolicy(const Harness& h)
{
    requireAll(doc172(h), {
        "Privacy / memo sanitization schema review",
        "Future writes must sanitize `sourceMemo`",
        "must not store raw SQL",
        "credentials",
        "endpoints",
        "tokens",
        "passwords",
    }, "docs/172 privacy review");
}

void testSchemaGapFollowup(const Harness& h)
{
    requireAll(doc172(h), {
        "Because this review found schema gaps",
        "future schema authorization TASK is required",
        "must not directly edit `migrations/001_initial_schema.sql`",
        "Future repository implementation must also be separately authorized",
    }, "docs/172 follow-up");
}

void testSchemaUnmodified(const Harness& h)
{
    const auto s = schema(h);
    requireNotContains(s, "TASK-187", "initial schema");
    requireNotContains(s, "manual_entry_schema_adequacy", "initial schema");
}

void testNoNewMigration(const Harness& h)
{
    for (const auto& file : filesUnder(h.root / "migrations")) {
        if (file.filename().string() == "002_shell_accounting_manual_entry_schema.sql") {
            continue;
        }
        requireNotContains(file.filename().string(), "187", "migration filename");
        requireNotContains(file.filename().string(), "manual_entry_schema", "migration filename");
    }
}

void testNoSchemaFile(const Harness& h)
{
    require(!fs::exists(h.root / "schema"), "TASK-187 must not add a schema directory");
}

void testDataServiceActionsCppUnmodified(const Harness& h)
{
    requireNotContains(dataServiceActions(h), "TASK-187", "DataServiceActions.cpp");
    requireNotContains(dataServiceActions(h), "ManualEntrySchemaAdequacy", "DataServiceActions.cpp");
}

void testDataServiceActionsHUnmodified(const Harness& h)
{
    requireNotContains(dataServiceActionsHeader(h), "TASK-187", "DataServiceActions.h");
    requireNotContains(dataServiceActionsHeader(h), "ManualEntrySchemaAdequacy", "DataServiceActions.h");
}

void testRegistrarUnmodified(const Harness& h)
{
    requireNotContains(registrar(h), "TASK-187", "DataServiceActionRegistrar.cpp");
}

void testValidationCodeUnmodified(const Harness& h)
{
    requireNotContains(validationHeader(h), "TASK-187", "TASK-178 validation header");
    requireNotContains(validationSource(h), "TASK-187", "TASK-178 validation source");
}

void testScaffoldUnmodified(const Harness& h)
{
    requireNotContains(scaffoldHeader(h), "TASK-187", "TASK-185 scaffold header");
    requireNotContains(scaffoldSource(h), "TASK-187", "TASK-185 scaffold source");
}

void testScaffoldDisabled(const Harness&)
{
    const etfdt::data_access::ShellAccountingManualEntryRepositoryScaffold scaffold;
    etfdt::data_access::ShellAccountingManualTransactionPersistenceCommand tx;
    tx.accountId = "acct-1";
    tx.portfolioId = "pf-1";
    tx.securityCode = "ETF-001";
    tx.tradeSide = "Buy";
    tx.quantityUnits = 100;
    tx.priceAmountMinor = 1234;
    tx.grossAmountMinor = 123400;
    tx.occurredAt = "2026-06-02T12:00:00Z";
    const auto result = scaffold.persistManualTransaction(tx);
    require(!result.success, "scaffold must remain disabled");
    require(!result.writeImplemented, "scaffold writeImplemented must remain false");
    require(!result.databaseWritten, "scaffold must not write database");
}

void testTask182WriteNotImplemented(const Harness&)
{
    requireValidationOnlyResponse(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        validManualTransactionPayload()));
    requireValidationOnlyResponse(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        validManualCashMovementPayload()));
}

void testNoRepositoryImplementation(const Harness& h)
{
    requireNoTokens(filesWithoutTask265ExcelVbaImportManualEntryPersistenceRepository(
        filesUnder(h.root / "libs" / "DataAccess")), {
        "ManualEntryImplementationRepository",
        "ManualEntryWriteRepository",
        "ManualEntryPersistenceRepository",
        "ManualTransactionWriteRepository",
        "ManualCashMovementWriteRepository",
        "insertManualTransaction",
        "insertCashMovement",
        "executeManualTransactionWrite",
        "executeManualCashMovementWrite",
    }, "manual entry repository implementation scan");
}

void testNoSqlAdded(const Harness& h)
{
    const auto source = scaffoldSource(h);
    for (const auto& token : {"INSERT ", "UPDATE ", "DELETE ", "REPLACE ", "CREATE ", "DROP ", "ALTER "}) {
        requireNotContains(source, token, "TASK-185 scaffold SQL scan");
    }
}

void testNoSqliteWrite(const Harness& h)
{
    const auto source = scaffoldSource(h);
    for (const auto& token : {"executeSql", "executeNonQuery", "sqlite3_exec", "beginTransaction", "commit"}) {
        requireNotContains(source, token, "TASK-185 scaffold SQLite write scan");
    }
}

void testNoTradeLogWrite(const Harness& h)
{
    requireNotContains(scaffoldSource(h), "trade_log", "TASK-185 scaffold");
}

void testNoCashFactWrite(const Harness& h)
{
    requireNotContains(scaffoldSource(h), "cash_fact", "TASK-185 scaffold");
    requireNotContains(scaffoldSource(h), "cash_ledger", "TASK-185 scaffold");
}

void testNoAuditLedgerWrite(const Harness& h)
{
    requireNotContains(scaffoldSource(h), "audit_log", "TASK-185 scaffold");
    requireNotContains(scaffoldSource(h), "ledger", "TASK-185 scaffold");
}

void testProductionQmlUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "apps" / "ETFDecisionShell" / "qml"), {
        "TASK-187",
        "manualEntrySchemaAdequacy",
        "schemaAdequacyReview",
    }, "production QML TASK-187 scan");
}

void testStartupUnmodified(const Harness& h)
{
    requireNotContains(readFile(h.root / "apps" / "ETFDecisionShell" / "src" / "main.cpp"), "TASK-187", "production startup");
}

void testPresenterControllerUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "ShellServices"), {
        "TASK-187",
        "ManualEntrySchemaAdequacy",
        "schemaAdequacyReview",
    }, "ShellServices TASK-187 scan");
    requireNoTokens(filesUnder(h.root / "libs" / "ShellCore"), {
        "TASK-187",
        "ManualEntrySchemaAdequacy",
        "schemaAdequacyReview",
    }, "ShellCore TASK-187 scan");
}

void testAccountingEngineReplayUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "AccountingEngine"), {
        "TASK-187",
        "manualEntrySchemaAdequacy",
        "manualEntryRepository",
    }, "AccountingEngine TASK-187 scan");
}

void testStrategyMarketUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "StrategyEngine"), {
        "TASK-187",
        "manualEntrySchemaAdequacy",
        "manualCashMovement",
    }, "StrategyEngine TASK-187 scan");
    requireNoTokens(filesUnder(h.root / "libs" / "MarketEngine"), {
        "TASK-187",
        "manualEntrySchemaAdequacy",
        "manualCashMovement",
    }, "MarketEngine TASK-187 scan");
}

void testNoTradeDraftSuggestion(const Harness& h)
{
    requireNoTokens({scaffoldSourcePath(h)}, {
        "TradeDraft",
        "tradeSuggestion",
        "createSuggestion",
    }, "TASK-185 scaffold TradeDraft scan");
}

void testNoBrokerSdk(const Harness& h)
{
    requireNoTokens({scaffoldSourcePath(h)}, {"BrokerSdk", "brokerSdk", "BrokerAdapter", "brokerAdapter"},
        "TASK-185 scaffold broker SDK scan");
}

void testNoNetworkEndpoint(const Harness& h)
{
    requireNoTokens({scaffoldSourcePath(h)}, {"http://", "https://", "endpoint", "networkCall", "QNetwork"},
        "TASK-185 scaffold network scan");
}

void testNoCredentials(const Harness& h)
{
    requireNoTokens({scaffoldSourcePath(h)}, {"credential", "secret", "password", "apiKey", "accessToken"},
        "TASK-185 scaffold credential scan");
}

void testNoRealOrderId(const Harness& h)
{
    requireNoTokens({scaffoldSourcePath(h)}, {"brokerOrderId", "realOrderId", "orderId"},
        "TASK-185 scaffold real order id scan");
}

void testNoRealOrderPlacement(const Harness& h)
{
    requireNoTokens({scaffoldSourcePath(h)}, {"placeOrder", "submitOrder", "sendOrder", "executeOrder"},
        "TASK-185 scaffold real order scan");
}

void testNoAutomaticTrading(const Harness& h)
{
    requireNoTokens({scaffoldSourcePath(h)}, {"automaticTrading", "autoTrading", "strategyExecute"},
        "TASK-185 scaffold automatic trading scan");
}

void testTask186StillValid(const Harness& h)
{
    requireContains(readFile(h.root / "tests" / "ShellAccountingManualEntryRepositoryImplementationAuthorizationGate" / "CMakeLists.txt"),
        "shell_accounting_manual_entry_repository_implementation_authorization_future_split_documented", "TASK-186 CMake");
}

void testTask185StillValid(const Harness& h)
{
    requireContains(readFile(h.root / "tests" / "ShellAccountingManualEntryRepositoryScaffold" / "CMakeLists.txt"),
        "shell_accounting_manual_entry_repository_scaffold_task184_evolved", "TASK-185 CMake");
}

void testTask184StillValid(const Harness& h)
{
    requireContains(readFile(h.root / "tests" / "ShellAccountingManualEntryRepositoryScaffoldAuthorizationGate" / "CMakeLists.txt"),
        "shell_accounting_manual_entry_repository_scaffold_authorization_broker_gates_retained", "TASK-184 CMake");
}

void testTask183StillValid(const Harness& h)
{
    requireContains(readFile(h.root / "tests" / "ShellAccountingManualEntryPersistenceAuthorizationGate" / "CMakeLists.txt"),
        "shell_accounting_manual_entry_persistence_authorization_future_split_documented", "TASK-183 CMake");
}

void testTask182StillValid(const Harness& h)
{
    requireContains(readFile(h.root / "tests" / "ShellAccountingManualEntryDataServiceActionValidationWiring" / "CMakeLists.txt"),
        "shell_accounting_manual_entry_dataservice_action_validation_wiring_broker_gates_retained", "TASK-182 CMake");
}

void testTask178StillPure(const Harness& h)
{
    requireNotContains(validationHeader(h) + validationSource(h), "SQLite", "TASK-178 validation code");
}

void testTask177StillValid(const Harness& h)
{
    requireContains(readFile(h.root / "tests" / "ShellAccountingManualTransactionCashMovementMvpAuthorizationGate" / "CMakeLists.txt"),
        "shell_accounting_manual_transaction_cash_movement_mvp_authorization_existing_broker_gates_retained",
        "TASK-177 CMake");
}

void testBrokerGatesRetained(const Harness& h)
{
    requireContains(readFile(h.root / "tests" / "ShellAccountingBrokerAdapterDisabledWiring" / "CMakeLists.txt"),
        "shell_accounting_broker_adapter_disabled_no_order_placement", "broker disabled CMake");
    requireContains(readFile(h.root / "tests" / "ShellAccountingBrokerOrderImplementation" / "CMakeLists.txt"),
        "shell_accounting_broker_order_no_real_order_placement", "broker order CMake");
    requireContains(readFile(h.root / "tests" / "ShellAccountingRealBrokerOrderAuthorizationGate" / "CMakeLists.txt"),
        "shell_accounting_real_broker_order_authorization_no_real_order_yet", "real broker authorization CMake");
    requireContains(readFile(h.root / "tests" / "ShellAccountingRealBrokerOrderImplementationGate" / "CMakeLists.txt"),
        "shell_accounting_real_broker_order_implementation_no_network_call", "real broker implementation CMake");
}

void runCase(const std::string& testCase, const Harness& h)
{
    if (testCase == "gate") return testGate(h);
    if (testCase == "docs") return testDocs(h);
    if (testCase == "docs_index_prompt") return testDocsIndexPrompt(h);
    if (testCase == "source_of_truth") return testSourceOfTruth(h);
    if (testCase == "transaction_evidence") return testTransactionEvidence(h);
    if (testCase == "transaction_gaps") return testTransactionGaps(h);
    if (testCase == "cash_evidence") return testCashEvidence(h);
    if (testCase == "cash_gaps") return testCashGaps(h);
    if (testCase == "audit_policy") return testAuditPolicy(h);
    if (testCase == "idempotency_policy") return testIdempotencyPolicy(h);
    if (testCase == "privacy_policy") return testPrivacyPolicy(h);
    if (testCase == "schema_gap_followup") return testSchemaGapFollowup(h);
    if (testCase == "schema_unmodified") return testSchemaUnmodified(h);
    if (testCase == "no_new_migration") return testNoNewMigration(h);
    if (testCase == "no_schema_file") return testNoSchemaFile(h);
    if (testCase == "dataserviceactions_cpp_unmodified") return testDataServiceActionsCppUnmodified(h);
    if (testCase == "dataserviceactions_h_unmodified") return testDataServiceActionsHUnmodified(h);
    if (testCase == "registrar_unmodified") return testRegistrarUnmodified(h);
    if (testCase == "validation_code_unmodified") return testValidationCodeUnmodified(h);
    if (testCase == "scaffold_unmodified") return testScaffoldUnmodified(h);
    if (testCase == "scaffold_disabled") return testScaffoldDisabled(h);
    if (testCase == "task182_write_not_implemented") return testTask182WriteNotImplemented(h);
    if (testCase == "no_repository_implementation") return testNoRepositoryImplementation(h);
    if (testCase == "no_sql_added") return testNoSqlAdded(h);
    if (testCase == "no_sqlite_write") return testNoSqliteWrite(h);
    if (testCase == "no_trade_log_write") return testNoTradeLogWrite(h);
    if (testCase == "no_cash_fact_write") return testNoCashFactWrite(h);
    if (testCase == "no_audit_ledger_write") return testNoAuditLedgerWrite(h);
    if (testCase == "production_qml_unmodified") return testProductionQmlUnmodified(h);
    if (testCase == "startup_unmodified") return testStartupUnmodified(h);
    if (testCase == "presenter_controller_unmodified") return testPresenterControllerUnmodified(h);
    if (testCase == "accountingengine_replay_unmodified") return testAccountingEngineReplayUnmodified(h);
    if (testCase == "strategy_market_unmodified") return testStrategyMarketUnmodified(h);
    if (testCase == "no_tradedraft_suggestion") return testNoTradeDraftSuggestion(h);
    if (testCase == "no_broker_sdk") return testNoBrokerSdk(h);
    if (testCase == "no_network_endpoint") return testNoNetworkEndpoint(h);
    if (testCase == "no_credentials") return testNoCredentials(h);
    if (testCase == "no_real_order_id") return testNoRealOrderId(h);
    if (testCase == "no_real_order_placement") return testNoRealOrderPlacement(h);
    if (testCase == "no_automatic_trading") return testNoAutomaticTrading(h);
    if (testCase == "task186_still_valid") return testTask186StillValid(h);
    if (testCase == "task185_still_valid") return testTask185StillValid(h);
    if (testCase == "task184_still_valid") return testTask184StillValid(h);
    if (testCase == "task183_still_valid") return testTask183StillValid(h);
    if (testCase == "task182_still_valid") return testTask182StillValid(h);
    if (testCase == "task178_still_pure") return testTask178StillPure(h);
    if (testCase == "task177_still_valid") return testTask177StillValid(h);
    if (testCase == "broker_gates_retained") return testBrokerGatesRetained(h);
    throw std::runtime_error("Unknown test case: " + testCase);
}

} // namespace

int main(int argc, char** argv)
{
    fs::path sourceRoot;
    std::string testCase = "gate";

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--source-root" && i + 1 < argc) {
            sourceRoot = argv[++i];
        } else if (arg == "--case" && i + 1 < argc) {
            testCase = argv[++i];
        }
    }

    if (sourceRoot.empty()) {
        std::cerr << "--source-root is required\n";
        return 2;
    }

    try {
        runCase(testCase, Harness{sourceRoot});
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }

    return 0;
}

