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
    command.sourceMemo = "task 186 scaffold probe";
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
    command.sourceMemo = "task 186 scaffold probe";
    command.requestId = "request-2";
    command.idempotencyKey = "idem-2";
    return command;
}

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-186-manual-entry-repository-implementation-authorization";
    request.traceId = "task-186-trace";
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
        + "\"sourceMemo\":\"task 186 validation only\""
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
        + "\"sourceMemo\":\"task 186 validation only\""
        + "}";
}

etfdt::protocol::ProtocolResponse dispatchManualAction(const std::string& action, const std::string& payloadJson)
{
    etfdt::data_access::SQLiteConnection connection;
    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
    return dispatcher.dispatch(envelope(action, payloadJson));
}

void requireScaffoldDisabled(const etfdt::data_access::ShellAccountingManualEntryPersistenceResult& result)
{
    require(!result.accepted, "scaffold must not accept persistence");
    require(!result.success, "scaffold must return success=false");
    require(!result.writeImplemented, "scaffold must return writeImplemented=false");
    require(!result.databaseWritten, "scaffold must return databaseWritten=false");
    require(!result.tradeLogWritten, "scaffold must return tradeLogWritten=false");
    require(!result.cashFactsWritten, "scaffold must return cashFactsWritten=false");
    require(!result.auditWritten, "scaffold must return auditWritten=false");
    require(!result.ledgerWritten, "scaffold must return ledgerWritten=false");
    require(result.status == "MANUAL_ENTRY_REPOSITORY_SCAFFOLD_DISABLED", "scaffold status must remain disabled");
}

void requireNoPersistentIds(const std::string& text)
{
    for (const auto& token : {"tradeLogId", "cashFactId", "auditLogId", "ledgerId", "persistentId"}) {
        requireNotContains(text, token, "persistent id scan");
    }
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
    requireNoPersistentIds(response.payloadJson);
}

void testGate(const Harness& h)
{
    require(fs::exists(h.root / "docs" / "170_shell_accounting_manual_entry_repository_implementation_authorization_gate.md"),
        "docs/170 must exist");
    require(fs::exists(h.root / "docs" / "171_shell_accounting_manual_entry_repository_implementation_authorization_test_plan.md"),
        "docs/171 must exist");
    require(fs::exists(scaffoldHeaderPath(h)), "TASK-185 scaffold header must exist");
    require(fs::exists(scaffoldSourcePath(h)), "TASK-185 scaffold source must exist");
}

void testDocs(const Harness& h)
{
    const auto doc = readFile(h.root / "docs" / "170_shell_accounting_manual_entry_repository_implementation_authorization_gate.md");
    const auto plan = readFile(h.root / "docs" / "171_shell_accounting_manual_entry_repository_implementation_authorization_test_plan.md");
    requireAll(doc, {
        "TASK-186",
        "authorization gate only",
        "does not implement a repository write path",
        "does not execute SQL",
        "does not write SQLite",
        "TASK-185 scaffold continues to return disabled / write-not-implemented",
        "TASK-182 validation wiring continues to return validation accepted plus",
        "separate schema authorization TASK",
        "repository boundary",
        "transaction, rollback, idempotency, duplicate handling",
        "audit",
        "privacy",
        "failure atomicity",
        "Manual transaction repository implementation is not broker order implementation",
        "Manual cash movement repository implementation is not broker transfer implementation",
        "broker sandbox work paused",
        "does not add broker SDKs",
        "no-network",
        "no-credentials",
        "no-order-placement",
    }, "docs/170");
    requireAll(plan, {
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "No repository implementation is added",
        "No SQLite write path is added",
        "Existing broker, real broker, no-network, no-credentials",
    }, "docs/171");
}

void testDocsIndexPrompt(const Harness& h)
{
    const auto readme = readFile(h.root / "README.md");
    const auto docsIndex = readFile(h.root / "docs" / "README.md");
    const auto prompt = readFile(h.root / "docs" / "12_codex_prompt_template.md");
    requireContains(readme, "docs/170_shell_accounting_manual_entry_repository_implementation_authorization_gate.md", "README");
    requireContains(readme, "docs/171_shell_accounting_manual_entry_repository_implementation_authorization_test_plan.md", "README");
    requireContains(docsIndex, "170_shell_accounting_manual_entry_repository_implementation_authorization_gate.md", "docs index");
    requireContains(docsIndex, "171_shell_accounting_manual_entry_repository_implementation_authorization_test_plan.md", "docs index");
    requireContains(prompt, "TASK-186", "Codex prompt template");
    requireContains(prompt, "docs/170 and docs/171", "Codex prompt template");
}

void testScaffoldHeaderUnmodified(const Harness& h)
{
    requireNotContains(scaffoldHeader(h), "TASK-186", "TASK-185 scaffold header");
}

void testScaffoldSourceUnmodified(const Harness& h)
{
    requireNotContains(scaffoldSource(h), "TASK-186", "TASK-185 scaffold source");
}

etfdt::data_access::ShellAccountingManualEntryPersistenceResult transactionResult()
{
    const etfdt::data_access::ShellAccountingManualEntryRepositoryScaffold scaffold;
    return scaffold.persistManualTransaction(transactionCommand());
}

etfdt::data_access::ShellAccountingManualEntryPersistenceResult cashResult()
{
    const etfdt::data_access::ShellAccountingManualEntryRepositoryScaffold scaffold;
    return scaffold.persistManualCashMovement(cashCommand());
}

void testScaffoldSuccessFalse(const Harness&)
{
    require(!transactionResult().success, "transaction scaffold success must be false");
    require(!cashResult().success, "cash scaffold success must be false");
}

void testScaffoldWriteImplementedFalse(const Harness&)
{
    require(!transactionResult().writeImplemented, "transaction scaffold writeImplemented must be false");
    require(!cashResult().writeImplemented, "cash scaffold writeImplemented must be false");
}

void testScaffoldDatabaseWrittenFalse(const Harness&)
{
    require(!transactionResult().databaseWritten, "transaction scaffold databaseWritten must be false");
    require(!cashResult().databaseWritten, "cash scaffold databaseWritten must be false");
}

void testScaffoldTradeLogWrittenFalse(const Harness&)
{
    require(!transactionResult().tradeLogWritten, "transaction scaffold tradeLogWritten must be false");
    require(!cashResult().tradeLogWritten, "cash scaffold tradeLogWritten must be false");
}

void testScaffoldCashFactsWrittenFalse(const Harness&)
{
    require(!transactionResult().cashFactsWritten, "transaction scaffold cashFactsWritten must be false");
    require(!cashResult().cashFactsWritten, "cash scaffold cashFactsWritten must be false");
}

void testScaffoldAuditWrittenFalse(const Harness&)
{
    require(!transactionResult().auditWritten, "transaction scaffold auditWritten must be false");
    require(!cashResult().auditWritten, "cash scaffold auditWritten must be false");
}

void testScaffoldLedgerWrittenFalse(const Harness&)
{
    require(!transactionResult().ledgerWritten, "transaction scaffold ledgerWritten must be false");
    require(!cashResult().ledgerWritten, "cash scaffold ledgerWritten must be false");
}

void testNoPersistentIds(const Harness& h)
{
    requireNoPersistentIds(scaffoldHeader(h));
    requireNoPersistentIds(scaffoldSource(h));
    requireNoPersistentIds(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        validManualTransactionPayload()).payloadJson);
    requireNoPersistentIds(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        validManualCashMovementPayload()).payloadJson);
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

void testNoSql(const Harness& h)
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
    require(!transactionResult().tradeLogWritten, "transaction scaffold must not write trade log");
}

void testNoCashFactWrite(const Harness& h)
{
    requireNotContains(scaffoldSource(h), "cash_fact", "TASK-185 scaffold");
    requireNotContains(scaffoldSource(h), "cash_ledger", "TASK-185 scaffold");
    require(!cashResult().cashFactsWritten, "cash scaffold must not write cash facts");
}

void testNoAuditLedgerWrite(const Harness& h)
{
    requireNotContains(scaffoldSource(h), "audit_log", "TASK-185 scaffold");
    require(!transactionResult().auditWritten, "transaction scaffold must not write audit");
    require(!cashResult().ledgerWritten, "cash scaffold must not write ledger");
}

void testDataServiceActionsCppUnmodified(const Harness& h)
{
    const auto source = dataServiceActions(h);
    requireNotContains(source, "TASK-186", "DataServiceActions.cpp");
    requireNotContains(source, "ShellAccountingManualEntryRepositoryScaffold", "DataServiceActions.cpp");
}

void testDataServiceActionsHUnmodified(const Harness& h)
{
    const auto header = dataServiceActionsHeader(h);
    requireNotContains(header, "TASK-186", "DataServiceActions.h");
    requireNotContains(header, "ShellAccountingManualEntryRepositoryScaffold", "DataServiceActions.h");
}

void testRegistrarUnmodified(const Harness& h)
{
    requireNotContains(registrar(h), "TASK-186", "DataServiceActionRegistrar.cpp");
}

void testValidationCodeUnmodified(const Harness& h)
{
    requireNotContains(validationHeader(h), "TASK-186", "TASK-178 validation header");
    requireNotContains(validationSource(h), "TASK-186", "TASK-178 validation source");
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

void testSchemaUnmodified(const Harness& h)
{
    const auto schema = readFile(h.root / "migrations" / "001_initial_schema.sql");
    requireNotContains(schema, "TASK-186", "initial schema");
    requireNotContains(schema, "manual_entry_repository_implementation", "initial schema");
}

void testNoNewMigration(const Harness& h)
{
    for (const auto& file : filesUnder(h.root / "migrations")) {
        requireNotContains(file.filename().string(), "186", "migration filename");
        requireNotContains(file.filename().string(), "manual_entry_repository_implementation", "migration filename");
    }
}

void testProductionQmlUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "apps" / "ETFDecisionShell" / "qml"), {
        "TASK-186",
        "manualEntryRepositoryImplementation",
        "repositoryImplementation",
    }, "production QML TASK-186 scan");
}

void testStartupUnmodified(const Harness& h)
{
    requireNotContains(readFile(h.root / "apps" / "ETFDecisionShell" / "src" / "main.cpp"), "TASK-186", "production startup");
}

void testPresenterControllerUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "ShellServices"), {
        "TASK-186",
        "ShellAccountingManualEntryRepositoryScaffold",
        "persistManualTransaction",
        "persistManualCashMovement",
    }, "ShellServices TASK-186 scan");
    requireNoTokens(filesUnder(h.root / "libs" / "ShellCore"), {
        "TASK-186",
        "ShellAccountingManualEntryRepositoryScaffold",
        "persistManualTransaction",
        "persistManualCashMovement",
    }, "ShellCore TASK-186 scan");
}

void testAccountingEngineReplayUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "AccountingEngine"), {
        "TASK-186",
        "manualEntryRepository",
        "persistManualTransaction",
    }, "AccountingEngine TASK-186 scan");
}

void testStrategyMarketUnmodified(const Harness& h)
{
    requireNoTokens(filesUnder(h.root / "libs" / "StrategyEngine"), {
        "TASK-186",
        "manualEntryRepository",
        "manualCashMovement",
    }, "StrategyEngine TASK-186 scan");
    requireNoTokens(filesUnder(h.root / "libs" / "MarketEngine"), {
        "TASK-186",
        "manualEntryRepository",
        "manualCashMovement",
    }, "MarketEngine TASK-186 scan");
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

void testFutureSplitDocumented(const Harness& h)
{
    const auto doc = readFile(h.root / "docs" / "170_shell_accounting_manual_entry_repository_implementation_authorization_gate.md");
    requireAll(doc, {
        "separate schema authorization TASK",
        "Future implementation must use a DataAccess repository boundary",
        "DataService action behavior",
        "Manual transaction repository implementation is not broker order implementation",
    }, "future split documentation");
}

void runCase(const std::string& testCase, const Harness& h)
{
    if (testCase == "gate") return testGate(h);
    if (testCase == "docs") return testDocs(h);
    if (testCase == "docs_index_prompt") return testDocsIndexPrompt(h);
    if (testCase == "scaffold_header_unmodified") return testScaffoldHeaderUnmodified(h);
    if (testCase == "scaffold_source_unmodified") return testScaffoldSourceUnmodified(h);
    if (testCase == "scaffold_success_false") return testScaffoldSuccessFalse(h);
    if (testCase == "scaffold_write_implemented_false") return testScaffoldWriteImplementedFalse(h);
    if (testCase == "scaffold_database_written_false") return testScaffoldDatabaseWrittenFalse(h);
    if (testCase == "scaffold_trade_log_written_false") return testScaffoldTradeLogWrittenFalse(h);
    if (testCase == "scaffold_cash_facts_written_false") return testScaffoldCashFactsWrittenFalse(h);
    if (testCase == "scaffold_audit_written_false") return testScaffoldAuditWrittenFalse(h);
    if (testCase == "scaffold_ledger_written_false") return testScaffoldLedgerWrittenFalse(h);
    if (testCase == "no_persistent_ids") return testNoPersistentIds(h);
    if (testCase == "no_repository_implementation") return testNoRepositoryImplementation(h);
    if (testCase == "no_sql") return testNoSql(h);
    if (testCase == "no_sqlite_write") return testNoSqliteWrite(h);
    if (testCase == "no_trade_log_write") return testNoTradeLogWrite(h);
    if (testCase == "no_cash_fact_write") return testNoCashFactWrite(h);
    if (testCase == "no_audit_ledger_write") return testNoAuditLedgerWrite(h);
    if (testCase == "dataserviceactions_cpp_unmodified") return testDataServiceActionsCppUnmodified(h);
    if (testCase == "dataserviceactions_h_unmodified") return testDataServiceActionsHUnmodified(h);
    if (testCase == "registrar_unmodified") return testRegistrarUnmodified(h);
    if (testCase == "validation_code_unmodified") return testValidationCodeUnmodified(h);
    if (testCase == "task182_validation_only") return testTask182ValidationOnly(h);
    if (testCase == "schema_unmodified") return testSchemaUnmodified(h);
    if (testCase == "no_new_migration") return testNoNewMigration(h);
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
    if (testCase == "task185_still_valid") return testTask185StillValid(h);
    if (testCase == "task184_still_valid") return testTask184StillValid(h);
    if (testCase == "task183_still_valid") return testTask183StillValid(h);
    if (testCase == "task182_still_valid") return testTask182StillValid(h);
    if (testCase == "task178_still_pure") return testTask178StillPure(h);
    if (testCase == "task177_still_valid") return testTask177StillValid(h);
    if (testCase == "broker_gates_retained") return testBrokerGatesRetained(h);
    if (testCase == "future_split_documented") return testFutureSplitDocumented(h);
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
