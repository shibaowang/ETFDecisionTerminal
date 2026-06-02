#include "DataAccess/ShellAccountingManualEntryRepositoryScaffold.h"
#include "DataAccess/DataAccess.h"
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

void requireNoTokens(
    const std::vector<fs::path>& files,
    const std::vector<std::string>& tokens,
    const std::string& context)
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
    return readFile(
        h.root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" / "DataServiceActions.h");
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

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-185-manual-entry-repository-scaffold";
    request.traceId = "task-185-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-06-02T12:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

etfdt::protocol::ProtocolResponse dispatchManualAction(const std::string& action, const std::string& payloadJson)
{
    etfdt::data_access::SQLiteConnection connection;
    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
    return dispatcher.dispatch(envelope(action, payloadJson));
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
        + "\"sourceMemo\":\"task 185 validation only\""
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
        + "\"sourceMemo\":\"task 185 validation only\""
        + "}";
}

etfdt::data_access::ShellAccountingManualTransactionPersistenceCommand transactionCommand()
{
    etfdt::data_access::ShellAccountingManualTransactionPersistenceCommand command;
    command.accountId = "acct-1";
    command.portfolioId = "pf-1";
    command.securityCode = "ETF-001";
    command.tradeSide = "Buy";
    command.quantityUnits = 100;
    command.priceAmountMinor = 1234;
    command.grossAmountMinor = 123400;
    command.feeAmountMinor = 10;
    command.taxAmountMinor = 0;
    command.occurredAt = "2026-06-02T12:00:00Z";
    command.sourceMemo = "task 185 scaffold";
    command.requestId = "request-1";
    command.idempotencyKey = "idem-1";
    return command;
}

etfdt::data_access::ShellAccountingManualCashMovementPersistenceCommand cashCommand()
{
    etfdt::data_access::ShellAccountingManualCashMovementPersistenceCommand command;
    command.accountId = "acct-1";
    command.portfolioId = "pf-1";
    command.movementType = "Deposit";
    command.amountMinor = 50000;
    command.occurredAt = "2026-06-02T12:00:00Z";
    command.sourceMemo = "task 185 scaffold";
    command.requestId = "request-2";
    command.idempotencyKey = "idem-2";
    return command;
}

etfdt::data_access::ShellAccountingManualEntryPersistenceResult transactionResult()
{
    etfdt::data_access::ShellAccountingManualEntryRepositoryScaffold repository;
    return repository.persistManualTransaction(transactionCommand());
}

etfdt::data_access::ShellAccountingManualEntryPersistenceResult cashResult()
{
    etfdt::data_access::ShellAccountingManualEntryRepositoryScaffold repository;
    return repository.persistManualCashMovement(cashCommand());
}

void requireDisabledResult(const etfdt::data_access::ShellAccountingManualEntryPersistenceResult& result)
{
    require(!result.accepted, "scaffold result must not be accepted as persistence");
    require(!result.success, "scaffold result must fail closed");
    require(!result.writeImplemented, "scaffold result must keep writeImplemented=false");
    require(!result.databaseWritten, "scaffold result must not write database");
    require(!result.tradeLogWritten, "scaffold result must not write trade_log");
    require(!result.cashFactsWritten, "scaffold result must not write cash facts");
    require(!result.auditWritten, "scaffold result must not write audit");
    require(!result.ledgerWritten, "scaffold result must not write ledger");
    require(result.errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        "scaffold result must be unavailable");
    require(result.status == "MANUAL_ENTRY_REPOSITORY_SCAFFOLD_DISABLED",
        "scaffold result must expose disabled status");
    require(!result.issues.empty(), "scaffold result must include disabled issue");
}

void requireTask182ResponseStillValidationOnly(const etfdt::protocol::ProtocolResponse& response)
{
    require(!response.success, "TASK-182 response must fail closed");
    require(response.errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        "TASK-182 response remains unavailable");
    requireAll(response.payloadJson, {
        "\"validationOnly\":true",
        "\"validationAccepted\":true",
        "\"writeImplemented\":false",
        "\"databaseWritten\":false",
        "\"tradeLogWritten\":false",
        "\"cashFactsWritten\":false",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
    }, "TASK-182 validation response");
}

void testDocs(const Harness& h)
{
    require(fs::exists(h.root / "docs" / "168_shell_accounting_manual_entry_repository_scaffold.md"),
        "docs/168 must exist");
    require(fs::exists(h.root / "docs" / "169_shell_accounting_manual_entry_repository_scaffold_test_plan.md"),
        "docs/169 must exist");
    const auto doc = readFile(h.root / "docs" / "168_shell_accounting_manual_entry_repository_scaffold.md");
    const auto plan = readFile(h.root / "docs" / "169_shell_accounting_manual_entry_repository_scaffold_test_plan.md");
    requireAll(doc, {
        "TASK-185",
        "not a repository implementation",
        "not a write implementation",
        "does not write a database",
        "does not execute SQL",
        "ShellAccountingManualEntryRepositoryScaffold",
        "TASK-182 validation wiring still returns validation accepted plus `writeImplemented=false`",
    }, "docs/168");
    requireAll(plan, {
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "No SQL or SQLite write exists",
        "Existing broker gates remain present and passing",
    }, "docs/169");
}

void testDocsIndexPrompt(const Harness& h)
{
    const auto readme = readFile(h.root / "README.md");
    const auto docsIndex = readFile(h.root / "docs" / "README.md");
    const auto prompt = readFile(h.root / "docs" / "12_codex_prompt_template.md");
    const auto doc166 = readFile(
        h.root / "docs" / "166_shell_accounting_manual_entry_repository_scaffold_authorization_gate.md");
    const auto doc167 = readFile(
        h.root / "docs" / "167_shell_accounting_manual_entry_repository_scaffold_authorization_test_plan.md");
    requireContains(readme, "docs/168_shell_accounting_manual_entry_repository_scaffold.md", "README");
    requireContains(readme, "docs/169_shell_accounting_manual_entry_repository_scaffold_test_plan.md", "README");
    requireContains(docsIndex, "168_shell_accounting_manual_entry_repository_scaffold.md", "docs/README");
    requireContains(docsIndex, "169_shell_accounting_manual_entry_repository_scaffold_test_plan.md", "docs/README");
    requireContains(prompt, "TASK-185", "docs/12");
    requireContains(doc166, "TASK-185", "docs/166 TASK-185 evolution");
    requireContains(doc167, "TASK-185", "docs/167 TASK-185 evolution");
}

void testHeaderExists(const Harness& h) { require(fs::exists(scaffoldHeaderPath(h)), "scaffold header must exist"); }
void testSourceExists(const Harness& h) { require(fs::exists(scaffoldSourcePath(h)), "scaffold source must exist"); }

void testCMakeRegistered(const Harness& h)
{
    const auto cmake = readFile(h.root / "libs" / "DataAccess" / "CMakeLists.txt");
    requireContains(cmake, "src/ShellAccountingManualEntryRepositoryScaffold.cpp", "DataAccess CMake");
}

void testTransactionCommandDto(const Harness& h)
{
    requireAll(scaffoldHeader(h), {
        "ShellAccountingManualTransactionPersistenceCommand",
        "accountId",
        "portfolioId",
        "securityCode",
        "tradeSide",
        "quantityUnits",
        "priceAmountMinor",
        "grossAmountMinor",
        "feeAmountMinor",
        "taxAmountMinor",
        "occurredAt",
        "sourceMemo",
        "requestId",
        "idempotencyKey",
    }, "transaction command DTO");
    const auto command = transactionCommand();
    require(command.quantityUnits == 100, "transaction command can carry quantity");
}

void testCashCommandDto(const Harness& h)
{
    requireAll(scaffoldHeader(h), {
        "ShellAccountingManualCashMovementPersistenceCommand",
        "accountId",
        "portfolioId",
        "movementType",
        "amountMinor",
        "occurredAt",
        "sourceMemo",
        "requestId",
        "idempotencyKey",
    }, "cash command DTO");
    const auto command = cashCommand();
    require(command.amountMinor == 50000, "cash command can carry amount");
}

void testResultDto(const Harness& h)
{
    requireAll(scaffoldHeader(h), {
        "ShellAccountingManualEntryPersistenceResult",
        "accepted",
        "success",
        "writeImplemented",
        "databaseWritten",
        "tradeLogWritten",
        "cashFactsWritten",
        "auditWritten",
        "ledgerWritten",
        "errorCode",
        "status",
        "issues",
    }, "result DTO");
}

void testTransactionMethod(const Harness& h)
{
    requireContains(scaffoldHeader(h), "persistManualTransaction", "transaction method declaration");
    requireContains(scaffoldSource(h), "persistManualTransaction", "transaction method definition");
}

void testCashMethod(const Harness& h)
{
    requireContains(scaffoldHeader(h), "persistManualCashMovement", "cash method declaration");
    requireContains(scaffoldSource(h), "persistManualCashMovement", "cash method definition");
}

void testTransactionDisabledResult(const Harness&) { requireDisabledResult(transactionResult()); }
void testCashDisabledResult(const Harness&) { requireDisabledResult(cashResult()); }
void testDatabaseWrittenFalse(const Harness&) { require(!transactionResult().databaseWritten && !cashResult().databaseWritten, "databaseWritten false"); }
void testTradeLogWrittenFalse(const Harness&) { require(!transactionResult().tradeLogWritten && !cashResult().tradeLogWritten, "tradeLogWritten false"); }
void testCashFactsWrittenFalse(const Harness&) { require(!transactionResult().cashFactsWritten && !cashResult().cashFactsWritten, "cashFactsWritten false"); }
void testAuditWrittenFalse(const Harness&) { require(!transactionResult().auditWritten && !cashResult().auditWritten, "auditWritten false"); }
void testLedgerWrittenFalse(const Harness&) { require(!transactionResult().ledgerWritten && !cashResult().ledgerWritten, "ledgerWritten false"); }

void testNoPersistentIds(const Harness& h)
{
    const auto header = scaffoldHeader(h);
    const auto source = scaffoldSource(h);
    for (const auto& token : {"tradeLogId", "cashFactId", "auditLogId", "ledgerId", "persistentId"}) {
        requireNotContains(header, token, "scaffold header persistent id scan");
        requireNotContains(source, token, "scaffold source persistent id scan");
    }
}

void testNoSql(const Harness& h)
{
    const auto source = scaffoldSource(h);
    for (const auto& token : {"executeSql", "executeNonQuery", "queryRows", "sqlite3_exec"}) {
        requireNotContains(source, token, "scaffold SQL API scan");
    }
}

void testNoInsertUpdateDelete(const Harness& h)
{
    const auto source = scaffoldSource(h);
    for (const auto& token : {"INSERT ", "UPDATE ", "DELETE ", "REPLACE ", "CREATE ", "DROP ", "ALTER "}) {
        requireNotContains(source, token, "scaffold SQL token scan");
    }
}

void testNoSqliteMutation(const Harness& h)
{
    const auto source = scaffoldSource(h);
    for (const auto& token : {"SQLiteConnection", "beginTransaction", "commit", "rollback", "TransactionRunner"}) {
        requireNotContains(source, token, "scaffold SQLite mutation scan");
    }
}

void testNoTradeLogWrite(const Harness& h)
{
    requireNotContains(scaffoldSource(h), "trade_log", "scaffold source");
    require(!transactionResult().tradeLogWritten && !cashResult().tradeLogWritten, "trade_log not written");
}

void testNoCashFactWrite(const Harness& h)
{
    const auto source = scaffoldSource(h);
    requireNotContains(source, "cash_fact", "scaffold source");
    requireNotContains(source, "cash_ledger", "scaffold source");
    require(!transactionResult().cashFactsWritten && !cashResult().cashFactsWritten, "cash facts not written");
}

void testNoAuditLedgerWrite(const Harness& h)
{
    const auto source = scaffoldSource(h);
    requireNotContains(source, "audit_log", "scaffold source");
    require(!transactionResult().auditWritten && !cashResult().auditWritten, "audit not written");
    require(!transactionResult().ledgerWritten && !cashResult().ledgerWritten, "ledger not written");
}

void testDataServiceActionsCppUnmodified(const Harness& h)
{
    const auto source = dataServiceActions(h);
    requireNotContains(source, "TASK-185", "DataServiceActions.cpp");
    requireNotContains(source, "ShellAccountingManualEntryRepositoryScaffold", "DataServiceActions.cpp");
}

void testDataServiceActionsHUnmodified(const Harness& h)
{
    const auto header = dataServiceActionsHeader(h);
    requireNotContains(header, "TASK-185", "DataServiceActions.h");
    requireNotContains(header, "ShellAccountingManualEntryRepositoryScaffold", "DataServiceActions.h");
}

void testRegistrarUnmodified(const Harness& h)
{
    const auto source = registrar(h);
    requireNotContains(source, "TASK-185", "DataServiceActionRegistrar.cpp");
    requireNotContains(source, "ShellAccountingManualEntryRepositoryScaffold", "DataServiceActionRegistrar.cpp");
}

void testValidationCodeUnmodified(const Harness& h)
{
    requireNotContains(validationHeader(h), "TASK-185", "TASK-178 validation header");
    requireNotContains(validationSource(h), "TASK-185", "TASK-178 validation source");
}

void testTask182WriteNotImplemented(const Harness&)
{
    const auto tradeResponse = dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        validManualTransactionPayload());
    const auto cashResponse = dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        validManualCashMovementPayload());
    requireTask182ResponseStillValidationOnly(tradeResponse);
    requireTask182ResponseStillValidationOnly(cashResponse);
}

void testSchemaUnmodified(const Harness& h)
{
    const auto schema = readFile(h.root / "migrations" / "001_initial_schema.sql");
    requireNotContains(schema, "TASK-185", "initial schema");
    requireNotContains(schema, "manual_entry_repository", "initial schema");
}

void testNoNewMigration(const Harness& h)
{
    for (const auto& file : filesUnder(h.root / "migrations")) {
        requireNotContains(file.filename().string(), "185", "migration filename");
        requireNotContains(file.filename().string(), "manual_entry_repository", "migration filename");
    }
}

void testProductionQmlUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "apps" / "ETFDecisionShell" / "qml"), {
        "ShellAccountingManualEntryRepositoryScaffold",
        "persistManualTransaction",
        "persistManualCashMovement",
    }, "production QML scan");
}

void testStartupUnmodified(const Harness& h)
{
    const auto startup = readFile(h.root / "apps" / "ETFDecisionShell" / "src" / "main.cpp");
    requireNotContains(startup, "ShellAccountingManualEntryRepositoryScaffold", "production startup");
}

void testPresenterControllerUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "ShellServices"), {
        "ShellAccountingManualEntryRepositoryScaffold",
        "persistManualTransaction",
        "persistManualCashMovement",
    }, "ShellServices scan");
    requireNoTokens(filesUnder(h.root / "libs" / "ShellCore"), {
        "ShellAccountingManualEntryRepositoryScaffold",
        "persistManualTransaction",
        "persistManualCashMovement",
    }, "ShellCore scan");
}

void testAccountingEngineReplayUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "AccountingEngine"), {
        "ShellAccountingManualEntryRepositoryScaffold",
        "persistManualTransaction",
        "persistManualCashMovement",
    }, "AccountingEngine scan");
}

void testStrategyMarketUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "StrategyEngine"), {
        "ShellAccountingManualEntryRepositoryScaffold",
        "persistManualTransaction",
    }, "StrategyEngine scan");
    requireNoTokens(filesUnder(h.root / "libs" / "MarketEngine"), {
        "ShellAccountingManualEntryRepositoryScaffold",
        "persistManualTransaction",
    }, "MarketEngine scan");
}

void testNoTradeDraftSuggestion(const Harness& h)
{
    const auto source = scaffoldSource(h);
    requireNotContains(source, "TradeDraft", "scaffold source");
    requireNotContains(source, "Suggestion", "scaffold source");
    requireNotContains(source, "recommend", "scaffold source");
}

void testNoBrokerSdk(const Harness& h)
{
    const auto source = scaffoldSource(h);
    requireNotContains(source, "Broker", "scaffold source");
    requireNotContains(source, "broker", "scaffold source");
}

void testNoNetworkEndpoint(const Harness& h)
{
    const auto source = scaffoldSource(h);
    for (const auto& token : {"http://", "https://", "endpoint", "network", "socket"}) {
        requireNotContains(source, token, "scaffold network scan");
    }
}

void testNoCredentials(const Harness& h)
{
    const auto source = scaffoldSource(h);
    for (const auto& token : {"credential", "secret", "token", "password", "apiKey"}) {
        requireNotContains(source, token, "scaffold credential scan");
    }
}

void testNoRealOrderId(const Harness& h)
{
    const auto source = scaffoldSource(h);
    requireNotContains(source, "brokerOrderId", "scaffold source");
    requireNotContains(source, "realOrderId", "scaffold source");
}

void testNoRealOrderPlacement(const Harness& h)
{
    const auto source = scaffoldSource(h);
    for (const auto& token : {"placeOrder", "executeOrder", "submitOrder", "orderPlacement"}) {
        requireNotContains(source, token, "scaffold order scan");
    }
}

void testNoAutomaticTrading(const Harness& h)
{
    const auto source = scaffoldSource(h);
    requireNotContains(source, "automaticTrading", "scaffold source");
    requireNotContains(source, "autoTrade", "scaffold source");
}

void testTask184Evolved(const Harness& h)
{
    const auto doc = readFile(
        h.root / "docs" / "166_shell_accounting_manual_entry_repository_scaffold_authorization_gate.md");
    requireContains(doc, "TASK-185 implements the first authorized disabled repository scaffold", "TASK-184 doc");
}

void testTask183StillValid(const Harness& h)
{
    const auto cmake = readFile(
        h.root / "tests" / "ShellAccountingManualEntryPersistenceAuthorizationGate" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_entry_persistence_authorization", "TASK-183 CMake");
}

void testTask182StillValid(const Harness& h)
{
    const auto cmake = readFile(
        h.root / "tests" / "ShellAccountingManualEntryDataServiceActionValidationWiring" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_entry_dataservice_action_validation_wiring", "TASK-182 CMake");
    testTask182WriteNotImplemented(h);
}

void testTask178StillPure(const Harness& h)
{
    const auto source = validationSource(h);
    requireNotContains(source, "SQLite", "TASK-178 validation source");
    requireNotContains(source, "Repository", "TASK-178 validation source");
    requireNotContains(source, "trade_log", "TASK-178 validation source");
}

void testTask177StillValid(const Harness& h)
{
    const auto cmake = readFile(
        h.root / "tests" / "ShellAccountingManualTransactionCashMovementMvpAuthorizationGate" / "CMakeLists.txt");
    requireContains(cmake, "shell_accounting_manual_transaction_cash_movement_mvp_authorization", "TASK-177 CMake");
}

void testBrokerGatesRetained(const Harness& h)
{
    const auto testsCmake = readFile(h.root / "tests" / "CMakeLists.txt");
    requireAll(testsCmake, {
        "ShellAccountingBrokerAdapterDisabledWiring",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementation",
        "ShellAccountingBrokerSandboxRuntimeEnablementStateDisabledWiring",
    }, "broker gate retention");
}

Harness parseArgs(int argc, char** argv, std::string& testCase)
{
    Harness harness;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--source-root" && i + 1 < argc) {
            harness.root = fs::path(argv[++i]);
        } else if (arg == "--case" && i + 1 < argc) {
            testCase = argv[++i];
        }
    }
    require(!harness.root.empty(), "--source-root is required");
    require(!testCase.empty(), "--case is required");
    return harness;
}

} // namespace

int main(int argc, char** argv)
{
    try {
        std::string testCase;
        const auto harness = parseArgs(argc, argv, testCase);

        if (testCase == "docs") testDocs(harness);
        else if (testCase == "docs_index_prompt") testDocsIndexPrompt(harness);
        else if (testCase == "header_exists") testHeaderExists(harness);
        else if (testCase == "source_exists") testSourceExists(harness);
        else if (testCase == "cmake_registered") testCMakeRegistered(harness);
        else if (testCase == "transaction_command_dto") testTransactionCommandDto(harness);
        else if (testCase == "cash_command_dto") testCashCommandDto(harness);
        else if (testCase == "result_dto") testResultDto(harness);
        else if (testCase == "transaction_method") testTransactionMethod(harness);
        else if (testCase == "cash_method") testCashMethod(harness);
        else if (testCase == "transaction_disabled_result") testTransactionDisabledResult(harness);
        else if (testCase == "cash_disabled_result") testCashDisabledResult(harness);
        else if (testCase == "database_written_false") testDatabaseWrittenFalse(harness);
        else if (testCase == "trade_log_written_false") testTradeLogWrittenFalse(harness);
        else if (testCase == "cash_facts_written_false") testCashFactsWrittenFalse(harness);
        else if (testCase == "audit_written_false") testAuditWrittenFalse(harness);
        else if (testCase == "ledger_written_false") testLedgerWrittenFalse(harness);
        else if (testCase == "no_persistent_ids") testNoPersistentIds(harness);
        else if (testCase == "no_sql") testNoSql(harness);
        else if (testCase == "no_insert_update_delete") testNoInsertUpdateDelete(harness);
        else if (testCase == "no_sqlite_mutation") testNoSqliteMutation(harness);
        else if (testCase == "no_trade_log_write") testNoTradeLogWrite(harness);
        else if (testCase == "no_cash_fact_write") testNoCashFactWrite(harness);
        else if (testCase == "no_audit_ledger_write") testNoAuditLedgerWrite(harness);
        else if (testCase == "dataserviceactions_cpp_unmodified") testDataServiceActionsCppUnmodified(harness);
        else if (testCase == "dataserviceactions_h_unmodified") testDataServiceActionsHUnmodified(harness);
        else if (testCase == "registrar_unmodified") testRegistrarUnmodified(harness);
        else if (testCase == "validation_code_unmodified") testValidationCodeUnmodified(harness);
        else if (testCase == "task182_write_not_implemented") testTask182WriteNotImplemented(harness);
        else if (testCase == "schema_unmodified") testSchemaUnmodified(harness);
        else if (testCase == "no_new_migration") testNoNewMigration(harness);
        else if (testCase == "production_qml_unmodified") testProductionQmlUnmodified(harness);
        else if (testCase == "startup_unmodified") testStartupUnmodified(harness);
        else if (testCase == "presenter_controller_unmodified") testPresenterControllerUnmodified(harness);
        else if (testCase == "accountingengine_replay_unmodified") testAccountingEngineReplayUnmodified(harness);
        else if (testCase == "strategy_market_unmodified") testStrategyMarketUnmodified(harness);
        else if (testCase == "no_tradedraft_suggestion") testNoTradeDraftSuggestion(harness);
        else if (testCase == "no_broker_sdk") testNoBrokerSdk(harness);
        else if (testCase == "no_network_endpoint") testNoNetworkEndpoint(harness);
        else if (testCase == "no_credentials") testNoCredentials(harness);
        else if (testCase == "no_real_order_id") testNoRealOrderId(harness);
        else if (testCase == "no_real_order_placement") testNoRealOrderPlacement(harness);
        else if (testCase == "no_automatic_trading") testNoAutomaticTrading(harness);
        else if (testCase == "task184_evolved") testTask184Evolved(harness);
        else if (testCase == "task183_still_valid") testTask183StillValid(harness);
        else if (testCase == "task182_still_valid") testTask182StillValid(harness);
        else if (testCase == "task178_still_pure") testTask178StillPure(harness);
        else if (testCase == "task177_still_valid") testTask177StillValid(harness);
        else if (testCase == "broker_gates_retained") testBrokerGatesRetained(harness);
        else throw std::runtime_error("Unknown test case: " + testCase);

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
