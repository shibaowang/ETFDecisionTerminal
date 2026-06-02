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

std::string doc176(const Harness& h)
{
    return readFile(h.root / "docs" / "176_shell_accounting_manual_entry_schema_implementation_authorization_gate.md");
}

std::string doc177(const Harness& h)
{
    return readFile(h.root / "docs" / "177_shell_accounting_manual_entry_schema_implementation_authorization_test_plan.md");
}

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-189-manual-entry-schema-implementation-authorization";
    request.traceId = "task-189-trace";
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
        + "\"sourceMemo\":\"task 189 validation only\""
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
        + "\"sourceMemo\":\"task 189 validation only\""
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
    require(fs::exists(h.root / "docs" / "176_shell_accounting_manual_entry_schema_implementation_authorization_gate.md"),
        "docs/176 must exist");
    require(fs::exists(h.root / "docs" / "177_shell_accounting_manual_entry_schema_implementation_authorization_test_plan.md"),
        "docs/177 must exist");
    require(fs::exists(schemaPath(h)), "initial schema must exist");
}

void testDocs(const Harness& h)
{
    requireAll(doc176(h), {
        "TASK-189",
        "manual entry schema implementation authorization gate-only",
        "does not modify schema",
        "does not add a migration",
        "does not add a schema file",
        "does not implement repository implementation",
        "does not write a database",
        "does not execute SQL",
        "does not modify DataServiceActions",
        "does not modify the TASK-185 repository scaffold",
        "does not modify TASK-178 validation production code",
        "only defines future schema implementation authorization boundary",
        "migrations/001_initial_schema.sql",
        "source of truth",
        "independent migration file",
        "must not directly edit",
        "migration filename / id policy",
        "manual transaction trace / idempotency fields",
        "manual cash movement trace / idempotency fields",
        "audit trace / payload classification / redaction support",
        "Broker sandbox new capability development remains paused",
        "no-network",
        "no-credentials",
        "no-order-placement",
    }, "docs/176");
    requireAll(doc177(h), {
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "No repository implementation is added",
        "No SQL / SQLite write is added",
        "Existing broker / real broker / no-network / no-credentials",
    }, "docs/177");
}

void testDocsIndexPrompt(const Harness& h)
{
    const auto readme = readFile(h.root / "README.md");
    const auto docsIndex = readFile(h.root / "docs" / "README.md");
    const auto prompt = readFile(h.root / "docs" / "12_codex_prompt_template.md");
    requireContains(readme, "docs/176_shell_accounting_manual_entry_schema_implementation_authorization_gate.md", "README");
    requireContains(readme, "docs/177_shell_accounting_manual_entry_schema_implementation_authorization_test_plan.md", "README");
    requireContains(docsIndex, "176_shell_accounting_manual_entry_schema_implementation_authorization_gate.md", "docs index");
    requireContains(docsIndex, "177_shell_accounting_manual_entry_schema_implementation_authorization_test_plan.md", "docs index");
    requireContains(prompt, "TASK-189", "Codex prompt template");
    requireContains(prompt, "docs/176 and docs/177", "Codex prompt template");
}

void testSourceOfTruth(const Harness& h)
{
    requireContains(doc176(h), "The source of truth remains `migrations/001_initial_schema.sql`", "docs/176");
    requireContains(doc176(h), "must not directly edit `migrations/001_initial_schema.sql`", "docs/176");
}

void testAuthorizationOnly(const Harness& h)
{
    requireAll(doc176(h), {
        "TASK-189 is a manual entry schema implementation authorization gate-only task",
        "TASK-189 only defines future schema implementation authorization boundary",
        "Future schema implementation must be a separately authorized TASK",
    }, "docs/176 authorization-only policy");
}

void testNoSchemaCurrentScope(const Harness& h)
{
    requireAll(doc176(h), {
        "TASK-189 does not modify schema",
        "TASK-189 does not add a migration",
        "TASK-189 does not add a schema file",
        "TASK-189 does not implement repository implementation",
        "TASK-189 does not write a database",
        "TASK-189 does not execute SQL",
    }, "docs/176 no current schema scope");
}

void testFutureMigrationReadiness(const Harness& h)
{
    requireAll(doc176(h), {
        "migration filename / id policy",
        "forward migration policy",
        "rollback / forward-fix policy",
        "compatibility with existing data",
        "compatibility with existing CTest and read-only demo",
        "no direct edit to `001_initial_schema.sql`",
        "no repository implementation in schema task",
        "no DataService write implementation in schema task",
        "no QML / UI in schema task",
        "no broker / network / credentials / endpoint in schema task",
        "no real order / automatic trading in schema task",
    }, "docs/176 future migration readiness");
}

void testFutureMigrationCandidateCategories(const Harness& h)
{
    requireAll(doc176(h), {
        "Future Migration Candidate Categories",
        "manual transaction trace / idempotency fields",
        "manual transaction occurredAt field or approved lossless mapping",
        "manual transaction tax amount support or approved lossless mapping",
        "manual cash movement trace / idempotency fields",
        "manual cash movement type mapping support",
        "cash adjustment / trade_log linkage support",
        "audit trace / payload classification / redaction support",
        "failure atomicity / rollback policy support",
    }, "docs/176 candidate categories");
}

void testManualTransactionCategories(const Harness& h)
{
    requireAll(doc176(h), {
        "manual transaction `request_id`",
        "manual transaction `idempotency_key`",
        "`occurred_at_utc`",
        "`tax_cents`",
        "manual transaction duplicate handling constraints",
    }, "docs/176 manual transaction categories");
}

void testManualCashCategories(const Harness& h)
{
    requireAll(doc176(h), {
        "manual cash movement `request_id`",
        "manual cash movement `idempotency_key`",
        "`occurred_at_utc`",
        "manual cash movement type mapping support",
        "cash adjustment / trade_log linkage support",
    }, "docs/176 manual cash categories");
}

void testAuditPrivacyCategories(const Harness& h)
{
    requireAll(doc176(h), {
        "audit request trace and audit idempotency",
        "payload classification",
        "redaction policy",
        "failure atomicity",
        "sanitized memo / source reference support",
        "must not store raw SQL",
        "raw manual entry payload",
        "credentials",
        "broker payloads",
    }, "docs/176 audit privacy categories");
}

void testRepositoryWaitPolicy(const Harness& h)
{
    requireAll(doc176(h), {
        "Future repository implementation may continue only after the schema gaps are resolved",
        "separately authorized lossless mapping is approved",
        "split from repository implementation and DataService write implementation",
    }, "docs/176 repository wait policy");
}

void testSchemaUnmodified(const Harness& h)
{
    const auto s = schema(h);
    requireNotContains(s, "TASK-189", "initial schema");
    requireNotContains(s, "manual_entry_schema_implementation", "initial schema");
    requireNotContains(s, "idempotency_key", "initial schema");
    requireNotContains(s, "tax_cents", "initial schema");
    requireNotContains(s, "occurred_at_utc", "initial schema");
}

void testNoNewMigration(const Harness& h)
{
    for (const auto& file : filesUnder(h.root / "migrations")) {
        requireNotContains(file.filename().string(), "189", "migration filename");
        requireNotContains(file.filename().string(), "manual_entry_schema_implementation", "migration filename");
        requireNotContains(readFile(file), "TASK-189", "migration content");
    }
}

void testNoSchemaFile(const Harness& h)
{
    require(!fs::exists(h.root / "schema"), "TASK-189 must not add a schema directory");
}

void testDataServiceActionsCppUnmodified(const Harness& h)
{
    requireNotContains(dataServiceActions(h), "TASK-189", "DataServiceActions.cpp");
    requireNotContains(dataServiceActions(h), "ManualEntrySchemaImplementation", "DataServiceActions.cpp");
}

void testDataServiceActionsHUnmodified(const Harness& h)
{
    requireNotContains(dataServiceActionsHeader(h), "TASK-189", "DataServiceActions.h");
    requireNotContains(dataServiceActionsHeader(h), "ManualEntrySchemaImplementation", "DataServiceActions.h");
}

void testRegistrarUnmodified(const Harness& h)
{
    requireNotContains(registrar(h), "TASK-189", "DataServiceActionRegistrar.cpp");
    requireNotContains(registrar(h), "ManualEntrySchemaImplementation", "DataServiceActionRegistrar.cpp");
}

void testValidationCodeUnmodified(const Harness& h)
{
    requireNotContains(validationHeader(h), "TASK-189", "TASK-178 validation header");
    requireNotContains(validationSource(h), "TASK-189", "TASK-178 validation source");
}

void testScaffoldUnmodified(const Harness& h)
{
    requireNotContains(scaffoldHeader(h), "TASK-189", "TASK-185 scaffold header");
    requireNotContains(scaffoldSource(h), "TASK-189", "TASK-185 scaffold source");
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

void testTask182ValidationOnly(const Harness&)
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
    requireNoTokens(filesUnder(h.root / "libs" / "DataAccess"), {
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
        "TASK-189",
        "manualEntrySchemaImplementation",
        "schemaImplementationAuthorization",
    }, "production QML TASK-189 scan");
}

void testStartupUnmodified(const Harness& h)
{
    requireNotContains(readFile(h.root / "apps" / "ETFDecisionShell" / "src" / "main.cpp"), "TASK-189", "production startup");
}

void testPresenterControllerUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "ShellServices"), {
        "TASK-189",
        "ManualEntrySchemaImplementation",
        "schemaImplementationAuthorization",
    }, "ShellServices TASK-189 scan");
    requireNoTokens(filesUnder(h.root / "libs" / "ShellCore"), {
        "TASK-189",
        "ManualEntrySchemaImplementation",
        "schemaImplementationAuthorization",
    }, "ShellCore TASK-189 scan");
}

void testAccountingEngineReplayUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "AccountingEngine"), {
        "TASK-189",
        "manualEntrySchemaImplementation",
        "manualEntryRepository",
    }, "AccountingEngine TASK-189 scan");
}

void testStrategyMarketUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "StrategyEngine"), {
        "TASK-189",
        "manualEntrySchemaImplementation",
        "manualCashMovement",
    }, "StrategyEngine TASK-189 scan");
    requireNoTokens(filesUnder(h.root / "libs" / "MarketEngine"), {
        "TASK-189",
        "manualEntrySchemaImplementation",
        "manualCashMovement",
    }, "MarketEngine TASK-189 scan");
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
    requireNoTokens({scaffoldSourcePath(h)}, {"automaticTrading", "autoTrading", "auto trade", "autotrade"},
        "TASK-185 scaffold automatic trading scan");
}

void testBrokerPolicy(const Harness& h)
{
    requireAll(doc176(h), {
        "Broker sandbox new capability development remains paused",
        "Pausing broker sandbox work does not remove existing broker gates",
        "no-real-order",
        "no-network",
        "no-credentials",
        "no-order-placement",
    }, "docs/176 broker policy");
}

void requireCMakeContains(const Harness& h, const fs::path& rel, const std::vector<std::string>& tokens, const std::string& context)
{
    requireAll(readFile(h.root / rel), tokens, context);
}

void testTask188StillValid(const Harness& h)
{
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingManualEntrySchemaGapAuthorizationGate" / "CMakeLists.txt", {
        "shell_accounting_manual_entry_schema_gap_authorization_gate",
        "shell_accounting_manual_entry_schema_gap_authorization_schema_unmodified",
        "shell_accounting_manual_entry_schema_gap_authorization_broker_gates_retained",
    }, "TASK-188 CMake");
}

void testTask187StillValid(const Harness& h)
{
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingManualEntrySchemaAdequacyReviewGate" / "CMakeLists.txt", {
        "shell_accounting_manual_entry_schema_adequacy_review_gate",
        "shell_accounting_manual_entry_schema_adequacy_review_schema_unmodified",
        "shell_accounting_manual_entry_schema_adequacy_review_broker_gates_retained",
    }, "TASK-187 CMake");
}

void testTask186StillValid(const Harness& h)
{
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingManualEntryRepositoryImplementationAuthorizationGate" / "CMakeLists.txt", {
        "shell_accounting_manual_entry_repository_implementation_authorization_gate",
        "shell_accounting_manual_entry_repository_implementation_authorization_no_repository_implementation",
        "shell_accounting_manual_entry_repository_implementation_authorization_broker_gates_retained",
    }, "TASK-186 CMake");
}

void testTask185StillValid(const Harness& h)
{
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingManualEntryRepositoryScaffold" / "CMakeLists.txt", {
        "shell_accounting_manual_entry_repository_scaffold_transaction_method",
        "shell_accounting_manual_entry_repository_scaffold_cash_method",
        "shell_accounting_manual_entry_repository_scaffold_task182_write_not_implemented",
        "shell_accounting_manual_entry_repository_scaffold_no_sql",
    }, "TASK-185 CMake");
}

void testTask184StillValid(const Harness& h)
{
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingManualEntryRepositoryScaffoldAuthorizationGate" / "CMakeLists.txt", {
        "shell_accounting_manual_entry_repository_scaffold_authorization_gate",
        "shell_accounting_manual_entry_repository_scaffold_authorization_no_sql",
        "shell_accounting_manual_entry_repository_scaffold_authorization_broker_gates_retained",
    }, "TASK-184 CMake");
}

void testTask183StillValid(const Harness& h)
{
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingManualEntryPersistenceAuthorizationGate" / "CMakeLists.txt", {
        "shell_accounting_manual_entry_persistence_authorization_gate",
        "shell_accounting_manual_entry_persistence_authorization_no_dataaccess_write",
        "shell_accounting_manual_entry_persistence_authorization_broker_gates_retained",
    }, "TASK-183 CMake");
}

void testTask182StillValid(const Harness& h)
{
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingManualEntryDataServiceActionValidationWiring" / "CMakeLists.txt", {
        "shell_accounting_manual_entry_dataservice_action_validation_wiring_manual_transaction_success",
        "shell_accounting_manual_entry_dataservice_action_validation_wiring_manual_cash_movement_success",
        "shell_accounting_manual_entry_dataservice_action_validation_wiring_no_database_write",
    }, "TASK-182 CMake");
}

void testTask178StillPure(const Harness& h)
{
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingManualTransactionCashMovementValidationScaffold" / "CMakeLists.txt", {
        "shell_accounting_manual_transaction_cash_movement_validation_scaffold_docs",
        "shell_accounting_manual_transaction_cash_movement_validation_scaffold_no_sqlite_access",
        "shell_accounting_manual_transaction_cash_movement_validation_scaffold_no_broker",
    }, "TASK-178 CMake");
}

void testTask177StillValid(const Harness& h)
{
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingManualTransactionCashMovementMvpAuthorizationGate" / "CMakeLists.txt", {
        "shell_accounting_manual_transaction_cash_movement_mvp_authorization_docs",
        "shell_accounting_manual_transaction_cash_movement_mvp_authorization_priority_shift",
        "shell_accounting_manual_transaction_cash_movement_mvp_authorization_no_dataaccess_manual_write_repository",
        "shell_accounting_manual_transaction_cash_movement_mvp_authorization_no_broker_sdk",
        "shell_accounting_manual_transaction_cash_movement_mvp_authorization_no_real_order_placement",
    }, "TASK-177 CMake");
}

void testBrokerGatesRetained(const Harness& h)
{
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingBrokerAdapterDisabledWiring" / "CMakeLists.txt", {
        "shell_accounting_broker_adapter_disabled_no_network_call",
        "shell_accounting_broker_adapter_disabled_no_credentials",
        "shell_accounting_broker_adapter_disabled_no_order_placement",
    }, "broker adapter disabled CMake");
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingBrokerOrderImplementation" / "CMakeLists.txt", {
        "shell_accounting_broker_order_no_broker_sdk_call",
        "shell_accounting_broker_order_no_real_order_placement",
        "shell_accounting_broker_order_no_auto_trading",
    }, "broker order CMake");
    requireCMakeContains(h, fs::path("tests") / "ShellAccountingRealBrokerOrderImplementationGate" / "CMakeLists.txt", {
        "shell_accounting_real_broker_order_implementation_no_network_call",
        "shell_accounting_real_broker_order_implementation_no_credentials",
        "shell_accounting_real_broker_order_implementation_no_real_order_yet",
    }, "real broker CMake");
}

using TestFn = void (*)(const Harness&);

struct TestCase {
    const char* name;
    TestFn fn;
};

const std::vector<TestCase> tests = {
    {"gate", testGate},
    {"docs", testDocs},
    {"docs_index_prompt", testDocsIndexPrompt},
    {"source_of_truth", testSourceOfTruth},
    {"authorization_only", testAuthorizationOnly},
    {"no_schema_current_scope", testNoSchemaCurrentScope},
    {"future_migration_readiness", testFutureMigrationReadiness},
    {"future_migration_candidate_categories", testFutureMigrationCandidateCategories},
    {"manual_transaction_categories", testManualTransactionCategories},
    {"manual_cash_categories", testManualCashCategories},
    {"audit_privacy_categories", testAuditPrivacyCategories},
    {"repository_wait_policy", testRepositoryWaitPolicy},
    {"schema_unmodified", testSchemaUnmodified},
    {"no_new_migration", testNoNewMigration},
    {"no_schema_file", testNoSchemaFile},
    {"dataserviceactions_cpp_unmodified", testDataServiceActionsCppUnmodified},
    {"dataserviceactions_h_unmodified", testDataServiceActionsHUnmodified},
    {"registrar_unmodified", testRegistrarUnmodified},
    {"validation_code_unmodified", testValidationCodeUnmodified},
    {"scaffold_unmodified", testScaffoldUnmodified},
    {"scaffold_disabled", testScaffoldDisabled},
    {"task182_validation_only", testTask182ValidationOnly},
    {"no_repository_implementation", testNoRepositoryImplementation},
    {"no_sql_added", testNoSqlAdded},
    {"no_sqlite_write", testNoSqliteWrite},
    {"no_trade_log_write", testNoTradeLogWrite},
    {"no_cash_fact_write", testNoCashFactWrite},
    {"no_audit_ledger_write", testNoAuditLedgerWrite},
    {"production_qml_unmodified", testProductionQmlUnmodified},
    {"startup_unmodified", testStartupUnmodified},
    {"presenter_controller_unmodified", testPresenterControllerUnmodified},
    {"accountingengine_replay_unmodified", testAccountingEngineReplayUnmodified},
    {"strategy_market_unmodified", testStrategyMarketUnmodified},
    {"no_tradedraft_suggestion", testNoTradeDraftSuggestion},
    {"no_broker_sdk", testNoBrokerSdk},
    {"no_network_endpoint", testNoNetworkEndpoint},
    {"no_credentials", testNoCredentials},
    {"no_real_order_id", testNoRealOrderId},
    {"no_real_order_placement", testNoRealOrderPlacement},
    {"no_automatic_trading", testNoAutomaticTrading},
    {"broker_policy", testBrokerPolicy},
    {"task188_still_valid", testTask188StillValid},
    {"task187_still_valid", testTask187StillValid},
    {"task186_still_valid", testTask186StillValid},
    {"task185_still_valid", testTask185StillValid},
    {"task184_still_valid", testTask184StillValid},
    {"task183_still_valid", testTask183StillValid},
    {"task182_still_valid", testTask182StillValid},
    {"task178_still_pure", testTask178StillPure},
    {"task177_still_valid", testTask177StillValid},
    {"broker_gates_retained", testBrokerGatesRetained},
};

} // namespace

int main(int argc, char** argv)
{
    fs::path sourceRoot;
    std::string caseName = "gate";
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--source-root" && i + 1 < argc) {
            sourceRoot = argv[++i];
        } else if (arg == "--case" && i + 1 < argc) {
            caseName = argv[++i];
        }
    }
    if (sourceRoot.empty()) {
        std::cerr << "--source-root is required\n";
        return 2;
    }

    try {
        const Harness h{sourceRoot};
        for (const auto& test : tests) {
            if (caseName == test.name) {
                test.fn(h);
                return 0;
            }
        }
        std::cerr << "Unknown case: " << caseName << "\n";
        return 2;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }
}
