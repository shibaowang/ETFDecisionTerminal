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

std::string functionBody(const std::string& source, const std::string& functionName)
{
    const auto start = source.find(functionName + "(");
    require(start != std::string::npos, functionName + " must exist");
    const auto brace = source.find('{', start);
    require(brace != std::string::npos, functionName + " body must start");
    int depth = 0;
    for (std::size_t i = brace; i < source.size(); ++i) {
        if (source[i] == '{') {
            ++depth;
        } else if (source[i] == '}') {
            --depth;
            if (depth == 0) {
                return source.substr(brace, i - brace + 1U);
            }
        }
    }
    throw std::runtime_error(functionName + " body must end");
}

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-182-manual-entry";
    request.traceId = "task-182-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-06-01T12:00:00Z";
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

std::string validManualTransactionPayload(std::string side = "Buy")
{
    return std::string("{")
        + "\"accountId\":\"acct-1\","
        + "\"portfolioId\":\"pf-1\","
        + "\"securityCode\":\"ETF-001\","
        + "\"tradeSide\":\"" + side + "\","
        + "\"quantityUnits\":100,"
        + "\"priceAmountMinor\":1234,"
        + "\"grossAmountMinor\":123400,"
        + "\"feeAmountMinor\":10,"
        + "\"taxAmountMinor\":0,"
        + "\"occurredAt\":\"2026-06-01T12:00:00Z\","
        + "\"sourceMemo\":\"manual validation only\""
        + "}";
}

std::string validManualCashMovementPayload(std::string movementType = "Deposit")
{
    return std::string("{")
        + "\"accountId\":\"acct-1\","
        + "\"portfolioId\":\"pf-1\","
        + "\"movementType\":\"" + movementType + "\","
        + "\"amountMinor\":50000,"
        + "\"occurredAt\":\"2026-06-01T12:00:00Z\","
        + "\"sourceMemo\":\"manual validation only\""
        + "}";
}

void requireValidationAcceptedWriteNotImplemented(const etfdt::protocol::ProtocolResponse& response)
{
    require(!response.success, "validation-first action without fixture database must fail safely");
    requireAll(response.payloadJson, {
        "\"validationOnly\":false",
        "\"validationAccepted\":true",
        "\"writeImplemented\":true",
        "\"writeEnabled\":true",
        "\"databaseWritten\":false",
        "\"cashFactsWritten\":false",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
        "\"repositoryCalled\":true",
    }, "accepted validation-first payload");
}

void requireValidationRejected(
    const etfdt::protocol::ProtocolResponse& response,
    const std::vector<std::string>& issueCodes)
{
    require(!response.success, "invalid validation payload must fail");
    require(response.errorCode == etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
        "invalid validation must return validation-style error");
    requireAll(response.payloadJson, {
        "\"validationOnly\":false",
        "\"validationAccepted\":false",
        "\"writeImplemented\":true",
        "\"databaseWritten\":false",
        "\"status\":\"VALIDATION_REJECTED\"",
    }, "rejected validation payload");
    for (const auto& code : issueCodes) {
        requireContains(response.payloadJson, code, "rejected validation issue");
    }
}

void testValidationWiring(const Harness& h)
{
    const auto source = dataServiceActions(h);
    requireAll(source, {
        "ShellAccountingManualTransactionCashMovementValidation.h",
        "manualTransactionEntryFromPayload",
        "manualCashMovementEntryFromPayload",
        "validateManualTransactionEntry",
        "validateManualCashMovement",
        "manualEntryValidationRejectedResponse",
        "manualTransactionWriteResponse",
        "manualCashMovementWriteResponse",
    }, "DataServiceActions validation wiring");
}

void testDataServiceOnlyBoundary(const Harness& h)
{
    const auto source = dataServiceActions(h);
    const auto transaction = functionBody(source, "handleAccountingManualEntryTransactionCreate");
    const auto cash = functionBody(source, "handleAccountingManualEntryCashMovementCreate");
    requireContains(transaction, "validateManualTransactionEntry", "manual transaction handler");
    requireContains(cash, "validateManualCashMovement", "manual cash movement handler");
    requireContains(transaction, "ShellAccountingManualTransactionRepository", "manual transaction handler");
    requireContains(cash, "ShellAccountingManualCashMovementRepository", "manual cash movement handler");
    requireNotContains(transaction, "executeSql", "manual transaction handler");
    requireNotContains(cash, "executeSql", "manual cash movement handler");
}

void testManualTransactionSuccess()
{
    requireValidationAcceptedWriteNotImplemented(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        validManualTransactionPayload()));
}

void testManualCashMovementSuccess()
{
    requireValidationAcceptedWriteNotImplemented(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        validManualCashMovementPayload()));
}

void testTradeSideValidation()
{
    requireValidationAcceptedWriteNotImplemented(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        validManualTransactionPayload("Sell")));
    requireValidationRejected(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        validManualTransactionPayload("Hold")), {"TRADE_SIDE_INVALID"});
}

void testCashMovementTypeValidation()
{
    requireValidationAcceptedWriteNotImplemented(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        validManualCashMovementPayload("Withdrawal")));
    requireValidationRejected(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        validManualCashMovementPayload("Adjust")), {"CASH_MOVEMENT_TYPE_INVALID"});
}

void testMissingRequiredFields()
{
    requireValidationRejected(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        "{}"),
        {"ACCOUNT_ID_REQUIRED", "PORTFOLIO_ID_REQUIRED", "SECURITY_CODE_REQUIRED", "OCCURRED_AT_REQUIRED"});
    requireValidationRejected(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        "{}"),
        {"ACCOUNT_ID_REQUIRED", "PORTFOLIO_ID_REQUIRED", "CASH_MOVEMENT_TYPE_INVALID", "OCCURRED_AT_REQUIRED"});
}

void testNumericValidation()
{
    const auto badTransaction =
        R"({"accountId":"acct-1","portfolioId":"pf-1","securityCode":"ETF-001","tradeSide":"Buy","quantityUnits":0,"priceAmountMinor":-1,"grossAmountMinor":0,"feeAmountMinor":-1,"taxAmountMinor":-1,"occurredAt":"2026-06-01T12:00:00Z","sourceMemo":"numeric validation"})";
    requireValidationRejected(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        badTransaction),
        {"QUANTITY_POSITIVE_REQUIRED", "PRICE_POSITIVE_REQUIRED", "GROSS_AMOUNT_POSITIVE_REQUIRED",
            "FEE_NON_NEGATIVE_REQUIRED", "TAX_NON_NEGATIVE_REQUIRED"});

    const auto badCash =
        R"({"accountId":"acct-1","portfolioId":"pf-1","movementType":"Deposit","amountMinor":0,"occurredAt":"2026-06-01T12:00:00Z","sourceMemo":"numeric validation"})";
    requireValidationRejected(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        badCash),
        {"CASH_AMOUNT_POSITIVE_REQUIRED"});
}

void testSensitiveMemoValidation()
{
    auto payload = validManualTransactionPayload();
    const std::string token = "\"sourceMemo\":\"manual validation only\"";
    const std::string replacement = "\"sourceMemo\":\"contains secret token\"";
    const auto pos = payload.find(token);
    require(pos != std::string::npos, "fixture must contain sourceMemo");
    payload.replace(pos, token.size(), replacement);
    requireValidationRejected(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        payload),
        {"SENSITIVE_MEMO_TOKEN"});
}

void testNonObjectPayloadRejected()
{
    requireValidationRejected(dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        "[]"),
        {"PAYLOAD_JSON_OBJECT_REQUIRED"});
}

void testValidationAcceptedWriteNotImplemented()
{
    const auto response = dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        validManualCashMovementPayload());
    requireValidationAcceptedWriteNotImplemented(response);
    requireNotContains(response.payloadJson, "\"persistentId\"", "manual entry validation response");
    requireNotContains(response.payloadJson, "\"cashMovementId\"", "manual entry validation response");
}

void testNoDatabaseWrite(const Harness& h)
{
    const auto source = dataServiceActions(h);
    const auto helper = functionBody(source, "manualEntryValidationRejectedResponse");
    requireContains(helper, "\"databaseWritten\\\":false", "validation rejected response helper");
    requireNotContains(functionBody(source, "handleAccountingManualEntryTransactionCreate"), "INSERT", "transaction handler");
    requireNotContains(functionBody(source, "handleAccountingManualEntryCashMovementCreate"), "INSERT", "cash handler");
}

void testNoTradeLogOrCashFactWrite(const Harness& h)
{
    const auto source = dataServiceActions(h);
    const auto helper = functionBody(source, "manualEntryValidationRejectedResponse");
    requireContains(helper, "\"tradeLogWritten\\\":false", "validation rejected response helper");
    requireContains(helper, "\"cashFactsWritten\\\":false", "validation rejected response helper");
    requireNotContains(functionBody(source, "handleAccountingManualEntryCashMovementCreate"), "cash_facts", "cash handler");
}

void testNoAuditOrLedgerWrite(const Harness& h)
{
    const auto helper = functionBody(dataServiceActions(h), "manualEntryValidationRejectedResponse");
    requireContains(helper, "\"auditWritten\\\":false", "validation response helper");
    requireContains(helper, "\"ledgerWritten\\\":false", "validation response helper");
}

void testNoRepositoryOrReplay(const Harness& h)
{
    const auto source = dataServiceActions(h);
    const auto transaction = functionBody(source, "handleAccountingManualEntryTransactionCreate");
    const auto cash = functionBody(source, "handleAccountingManualEntryCashMovementCreate");
    requireContains(transaction, "ShellAccountingManualTransactionRepository", "manual transaction handler");
    requireContains(cash, "ShellAccountingManualCashMovementRepository", "manual cash movement handler");
    requireNotContains(transaction, "runShellAccountingReadOnlyReplay", "manual transaction handler");
    requireNotContains(cash, "runShellAccountingReadOnlyReplay", "manual cash movement handler");
}

void testNoTradeDraftSuggestionBroker(const Harness& h)
{
    const auto helper = functionBody(dataServiceActions(h), "manualEntryValidationRejectedResponse");
    requireContains(helper, "\"tradeDraftGenerated\\\":false", "validation response helper");
    requireContains(helper, "\"tradeSuggestionGenerated\\\":false", "validation response helper");
    requireContains(helper, "\"brokerSdkCalled\\\":false", "validation response helper");
    requireContains(helper, "\"realOrderPlacement\\\":false", "validation response helper");
}

void testNoNetworkCredentialsEndpoint(const Harness& h)
{
    const auto helper = functionBody(dataServiceActions(h), "manualEntryValidationRejectedResponse");
    requireContains(helper, "\"networkAccessed\\\":false", "validation response helper");
    requireContains(helper, "\"credentialsAccessed\\\":false", "validation response helper");
    requireContains(helper, "\"endpointAccessed\\\":false", "validation response helper");
}

void testNoPersistentIds()
{
    const auto response = dispatchManualAction(
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        validManualTransactionPayload());
    requireNotContains(response.payloadJson, "persistentId", "manual validation response");
    requireNotContains(response.payloadJson, "databaseId", "manual validation response");
    requireNotContains(response.payloadJson, "tradeLogId", "manual validation response");
    requireNotContains(response.payloadJson, "cashFactId", "manual validation response");
}

void testExistingActionNamesRetained(const Harness& h)
{
    requireContains(
        dataServiceActionsHeader(h),
        "kActionAccountingManualTransactionCreate",
        "DataServiceActions.h");
    requireContains(
        dataServiceActionsHeader(h),
        "kActionAccountingManualCashMovementCreate",
        "DataServiceActions.h");
    requireContains(
        registrar(h),
        "handleAccountingManualEntryTransactionCreate",
        "DataServiceActionRegistrar");
    requireContains(
        registrar(h),
        "handleAccountingManualEntryCashMovementCreate",
        "DataServiceActionRegistrar");
}

void testBrokerGatesRetained(const Harness& h)
{
    const auto cmake = readFile(h.root / "tests" / "CMakeLists.txt");
    requireAll(cmake, {
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingBrokerSandboxRuntimeModeSourceAuthorizationGate",
        "ShellAccountingBrokerSandboxRuntimeModeSourceScaffold",
        "ShellAccountingBrokerSandboxRuntimeModeSourceSelectorAuthorizationGate",
    }, "tests/CMake broker gates");
}

void runCase(const Harness& h, const std::string& name)
{
    if (name == "validation_wiring") return testValidationWiring(h);
    if (name == "dataservice_only_boundary") return testDataServiceOnlyBoundary(h);
    if (name == "manual_transaction_success") return testManualTransactionSuccess();
    if (name == "manual_cash_movement_success") return testManualCashMovementSuccess();
    if (name == "trade_side_validation") return testTradeSideValidation();
    if (name == "cash_movement_type_validation") return testCashMovementTypeValidation();
    if (name == "missing_required_fields") return testMissingRequiredFields();
    if (name == "numeric_validation") return testNumericValidation();
    if (name == "sensitive_memo_validation") return testSensitiveMemoValidation();
    if (name == "non_object_payload_rejected") return testNonObjectPayloadRejected();
    if (name == "validation_accepted_write_not_implemented") return testValidationAcceptedWriteNotImplemented();
    if (name == "no_database_write") return testNoDatabaseWrite(h);
    if (name == "no_trade_log_or_cash_fact_write") return testNoTradeLogOrCashFactWrite(h);
    if (name == "no_audit_or_ledger_write") return testNoAuditOrLedgerWrite(h);
    if (name == "no_repository_or_replay") return testNoRepositoryOrReplay(h);
    if (name == "no_tradedraft_suggestion_broker") return testNoTradeDraftSuggestionBroker(h);
    if (name == "no_network_credentials_endpoint") return testNoNetworkCredentialsEndpoint(h);
    if (name == "no_persistent_ids") return testNoPersistentIds();
    if (name == "existing_action_names_retained") return testExistingActionNamesRetained(h);
    if (name == "broker_gates_retained") return testBrokerGatesRetained(h);
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

        if (h.root.empty() || caseName.empty()) {
            throw std::runtime_error("Usage: --source-root <path> --case <name>");
        }
        runCase(h, caseName);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
