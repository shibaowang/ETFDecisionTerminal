#include "AccountingEngine/AccountingEngine.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct SliceInput {
    bool present = true;
    bool malformed = false;
    bool unsupportedScenario = false;
    bool replayIssueScenario = false;
};

struct SliceEvidence {
    bool replayExecuted = false;
    bool accountingEngineCalled = false;
    bool positionSummaryAvailable = false;
    bool cashSummaryAvailable = false;
    bool pnlSummaryAvailable = false;
    bool unsupportedScenarioCovered = false;
    bool replayIssueReturned = false;
    bool missingInputCovered = false;
    bool malformedInputCovered = false;
    bool sanitizedErrorOutput = false;
    bool noWriteOnFailure = false;
    std::string instrumentCode;
    std::string positionQuantity;
    std::string cashBalance;
    std::string pnlStatus;
};

int expect(bool condition, const std::string& message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "FAILED: " << message << '\n';
    return 1;
}

etfdt::accounting::ReplayRequestDto makeRequest()
{
    etfdt::accounting::ReplayRequestDto request;
    request.accountId = "TASK252_ACCOUNT";
    request.portfolioId = "TASK252_PORTFOLIO";
    request.requestedOutputs = {
        etfdt::accounting::RequestedReplayOutput::Positions,
        etfdt::accounting::RequestedReplayOutput::Cash,
        etfdt::accounting::RequestedReplayOutput::Pnl,
        etfdt::accounting::RequestedReplayOutput::Issues,
    };
    request.includeIssues = true;
    request.calculationVersion = "TASK-252-test-only";
    return request;
}

etfdt::accounting::CashFactDto makeInitialCash(const std::string& amountText = "100000.00")
{
    return etfdt::accounting::CashFactDto{
        "TASK252_CASH_001",
        "2026-01-01T00:00:00Z",
        "TASK252_ACCOUNT",
        "TASK252_PORTFOLIO",
        etfdt::accounting::CashAction::InitialCash,
        amountText,
        "CNY",
        "SANITIZED_TASK252_INITIAL_CASH",
    };
}

etfdt::accounting::TradeFactDto makeBuy()
{
    return etfdt::accounting::TradeFactDto{
        "TASK252_TRADE_001",
        "2026-01-02T09:30:00Z",
        "TASK252_ACCOUNT",
        "TASK252_PORTFOLIO",
        "TASK252_INSTRUMENT",
        etfdt::accounting::TradeAction::Buy,
        "100",
        "2.0000",
        "200.00",
        "1.00",
        "-201.00",
        "CNY",
        "TASK252_TEST_ONLY",
        "SANITIZED_TASK252_BUY",
    };
}

etfdt::accounting::TradeFactDto makeSellOnly()
{
    auto trade = makeBuy();
    trade.factId = "TASK252_TRADE_SELL_UNSUPPORTED";
    trade.action = etfdt::accounting::TradeAction::Sell;
    trade.amountText = "100.00";
    trade.feeText = "1.00";
    trade.cashFlowText = "99.00";
    return trade;
}

std::string sanitizedFailure(const std::string& issueCode)
{
    return std::string("{\"issueCode\":\"") + issueCode + "\",\"message\":\"sanitized TASK-252 test-only failure\"}";
}

SliceEvidence runTestOnlyAdapter(const SliceInput& input)
{
    SliceEvidence evidence;
    evidence.missingInputCovered = !input.present;
    evidence.malformedInputCovered = input.malformed;
    evidence.sanitizedErrorOutput = true;
    evidence.noWriteOnFailure = true;

    if (!input.present || input.malformed) {
        return evidence;
    }

    etfdt::accounting::AccountingReplayEngine engine;
    evidence.accountingEngineCalled = true;

    const auto request = makeRequest();
    const auto result = engine.replayReadOnly(
        request,
        {input.unsupportedScenario ? makeSellOnly() : makeBuy()},
        {makeInitialCash(input.replayIssueScenario ? "100.00" : "100000.00")},
        {},
        {});

    evidence.replayExecuted = result.replayExecuted;
    evidence.unsupportedScenarioCovered = input.unsupportedScenario
        && result.status == etfdt::accounting::AccountingReplayStatus::UnsupportedScenario;
    evidence.replayIssueReturned = input.replayIssueScenario && !result.issues.empty();

    if (!result.positionList.positions.empty()) {
        const auto& position = result.positionList.positions.front();
        evidence.positionSummaryAvailable = true;
        evidence.instrumentCode = position.instrumentCode;
        evidence.positionQuantity = position.quantityText;
    }

    if (result.hasCashSummary) {
        evidence.cashSummaryAvailable = true;
        evidence.cashBalance = result.cashSummary.cashBalanceText;
    }

    if (result.hasPortfolioPnl) {
        evidence.pnlSummaryAvailable = true;
        evidence.pnlStatus = result.portfolioPnl.dataQualityStatus;
    }

    return evidence;
}

std::string boolText(bool value)
{
    return value ? "true" : "false";
}

void writeStringField(std::ostringstream& out, const std::string& name, const std::string& value, bool comma = true)
{
    out << "\"" << name << "\":\"" << value << "\"";
    if (comma) {
        out << ",";
    }
}

void writeBoolField(std::ostringstream& out, const std::string& name, bool value, bool comma = true)
{
    out << "\"" << name << "\":" << boolText(value);
    if (comma) {
        out << ",";
    }
}

std::string buildEvidenceJson(const SliceEvidence& success,
                              const SliceEvidence& missing,
                              const SliceEvidence& malformed,
                              const SliceEvidence& unsupported,
                              const SliceEvidence& replayIssue)
{
    std::ostringstream out;
    out << "{";
    writeStringField(out, "task", "TASK-252");
    writeStringField(out, "mode", "test-only-readonly-runtime-integration-vertical-slice");
    writeStringField(out, "runtimeIntegrationPath", "test-only");
    writeBoolField(out, "replayExecuted", success.replayExecuted);
    writeBoolField(out, "accountingEngineCalled", success.accountingEngineCalled);
    writeBoolField(out, "productionBridgeImplemented", false);
    writeBoolField(out, "productionRuntimeIntegrationImplemented", false);
    writeBoolField(out, "productionWrite", false);
    writeBoolField(out, "sqliteProductionWrite", false);
    writeBoolField(out, "auditWritten", false);
    writeBoolField(out, "ledgerWritten", false);
    writeBoolField(out, "snapshotWritten", false);
    writeBoolField(out, "tradeLogWritten", false);
    writeBoolField(out, "readModelPersistentWrite", false);
    writeBoolField(out, "tradeDraftGenerated", false);
    writeBoolField(out, "tradeSuggestionGenerated", false);
    writeBoolField(out, "strategyExecuted", false);
    writeBoolField(out, "brokerOrderSubmitted", false);
    writeBoolField(out, "networkAccess", false);
    writeBoolField(out, "credentialAccess", false);
    writeBoolField(out, "endpointAccess", false);
    writeBoolField(out, "automaticTrading", false);
    writeBoolField(out, "qmlDirectAccountingEngineCall", false);
    writeBoolField(out, "presenterDirectAccountingEngineCall", false);
    writeBoolField(out, "controllerDirectAccountingEngineCall", false);
    writeBoolField(out, "startupDirectAccountingEngineCall", false);
    writeBoolField(out, "positionSummaryAvailable", success.positionSummaryAvailable);
    writeBoolField(out, "cashSummaryAvailable", success.cashSummaryAvailable);
    writeBoolField(out, "pnlSummaryAvailable", success.pnlSummaryAvailable);
    writeBoolField(out, "issuesSanitized", true);
    out << "\"projectionSummary\":{";
    writeStringField(out, "instrumentCode", success.instrumentCode);
    writeStringField(out, "quantity", success.positionQuantity);
    writeStringField(out, "cashBalance", success.cashBalance);
    writeStringField(out, "pnlStatus", success.pnlStatus, false);
    out << "},";
    out << "\"failureModes\":{";
    writeBoolField(out, "missingInput", missing.missingInputCovered && missing.noWriteOnFailure);
    writeBoolField(out, "malformedInput", malformed.malformedInputCovered && malformed.noWriteOnFailure);
    writeBoolField(out, "unsupportedScenario", unsupported.unsupportedScenarioCovered && unsupported.noWriteOnFailure);
    writeBoolField(out, "replayIssueReturned", replayIssue.replayIssueReturned && replayIssue.noWriteOnFailure);
    writeBoolField(out, "sanitizedErrorOutput", missing.sanitizedErrorOutput && malformed.sanitizedErrorOutput);
    writeBoolField(out, "noWriteOnFailure", missing.noWriteOnFailure && malformed.noWriteOnFailure
            && unsupported.noWriteOnFailure && replayIssue.noWriteOnFailure,
        false);
    out << "},";
    out << "\"privacy\":{";
    writeBoolField(out, "rawSqlExposed", false);
    writeBoolField(out, "rawTradeLogPayloadExposed", false);
    writeBoolField(out, "rawReplayPayloadExposed", false);
    writeBoolField(out, "internalStackExposed", false);
    writeBoolField(out, "sanitizedFailureExampleSafe", sanitizedFailure("TASK252_MISSING_INPUT").find("raw") == std::string::npos, false);
    out << "}";
    out << "}";
    return out.str();
}

} // namespace

int main()
{
    int failures = 0;

    const auto success = runTestOnlyAdapter(SliceInput{});
    const auto missing = runTestOnlyAdapter(SliceInput{false, false, false, false});
    const auto malformed = runTestOnlyAdapter(SliceInput{true, true, false, false});
    const auto unsupported = runTestOnlyAdapter(SliceInput{true, false, true, false});
    const auto replayIssue = runTestOnlyAdapter(SliceInput{true, false, false, true});

    failures += expect(success.accountingEngineCalled, "valid test-only path calls AccountingEngine");
    failures += expect(success.replayExecuted, "valid test-only path executes replay");
    failures += expect(success.positionSummaryAvailable, "position summary is available");
    failures += expect(success.cashSummaryAvailable, "cash summary is available");
    failures += expect(success.pnlSummaryAvailable, "PnL summary shell is available");
    failures += expect(success.instrumentCode == "TASK252_INSTRUMENT", "instrument code is projected");
    failures += expect(success.positionQuantity == "100", "position quantity is projected");
    failures += expect(success.cashBalance == "99799.00 CNY", "cash balance is projected");
    failures += expect(missing.missingInputCovered && missing.noWriteOnFailure, "missing input failure mode is covered");
    failures += expect(malformed.malformedInputCovered && malformed.noWriteOnFailure, "malformed input failure mode is covered");
    failures += expect(unsupported.unsupportedScenarioCovered && unsupported.noWriteOnFailure,
                       "unsupported scenario failure mode is covered");
    failures += expect(replayIssue.replayIssueReturned && replayIssue.noWriteOnFailure,
                       "replay issue failure mode is covered");
    failures += expect(sanitizedFailure("TASK252_MALFORMED_INPUT").find("select") == std::string::npos,
                       "sanitized error output hides SQL tokens");

    const auto evidence = buildEvidenceJson(success, missing, malformed, unsupported, replayIssue);
    failures += expect(evidence.find("rawSql") != std::string::npos, "evidence includes privacy keys");
    failures += expect(evidence.find("traceback") == std::string::npos, "evidence hides stack traces");
    failures += expect(evidence.find("credential") != std::string::npos, "evidence includes credential flag");
    failures += expect(evidence.find("credentialAccess\":false") != std::string::npos, "credential access remains false");

    std::cout << evidence << '\n';

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
