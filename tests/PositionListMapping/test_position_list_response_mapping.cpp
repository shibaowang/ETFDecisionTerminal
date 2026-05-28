#include "PositionListMappingContractTestUtils.h"

#include "AccountingEngine/AccountingIssue.h"

#include <iostream>

namespace {

int failures = 0;

void require(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}

etfdt::accounting::ReplayRequestDto request()
{
    etfdt::accounting::ReplayRequestDto dto;
    dto.accountId = "ACC-DEMO-001";
    dto.portfolioId = "PF-DEMO-001";
    dto.sourceFromTime = "2026-01-01T00:00:00Z";
    dto.sourceToTime = "2026-01-31T23:59:59Z";
    dto.requestedOutputs = {etfdt::accounting::RequestedReplayOutput::Positions,
                            etfdt::accounting::RequestedReplayOutput::Issues};
    dto.calculationVersion = "accounting-replay-v0";
    return dto;
}

}  // namespace

int main()
{
    using namespace etfdt::tests::position_list_mapping;

    const auto baseRequest = request();

    const auto emptyResult = etfdt::accounting::makeEmptyLedgerReplayResult("ACC-DEMO-001", "PF-DEMO-001");
    const auto emptyPayload = mapReplayResultToPositionListResponseForTest(emptyResult, baseRequest);
    require(emptyPayload.action == "position.list", "empty ledger maps action");
    require(emptyPayload.implemented, "empty ledger maps implemented=true");
    require(emptyPayload.readOnly, "empty ledger maps readOnly=true");
    require(!emptyPayload.writeEnabled, "empty ledger maps writeEnabled=false");
    require(emptyPayload.positions.empty(), "empty ledger maps empty positions");
    require(emptyPayload.issues.empty(), "empty ledger maps empty issues");

    const auto singleBuyResult = etfdt::accounting::makeSingleBuyReplayResult(
        "ACC-DEMO-001",
        "PF-DEMO-001",
        "159509",
        "1000",
        100100,
        9899900);
    const auto singleBuyPayload = mapReplayResultToPositionListResponseForTest(singleBuyResult, baseRequest);
    require(singleBuyPayload.positions.size() == 1U, "single BUY maps one position");
    const auto& position = singleBuyPayload.positions.front();
    require(position.accountId == "ACC-DEMO-001", "single BUY maps position accountId");
    require(position.portfolioId == "PF-DEMO-001", "single BUY maps position portfolioId");
    require(position.instrumentCode == "159509", "single BUY maps instrumentCode");
    require(position.quantityText == "1000", "single BUY maps quantityText");
    require(position.costAmountText == "1001.00 CNY", "single BUY maps costAmountText");
    require(position.costPriceText == "1.001 CNY", "single BUY maps costPriceText");
    require(position.marketValueText == "UNAVAILABLE", "single BUY keeps marketValueText unavailable");
    require(position.unrealizedPnlText == "UNAVAILABLE", "single BUY keeps unrealizedPnlText unavailable");
    const auto singleSerialized = serializePositionListPayloadForTest(singleBuyPayload);
    require(singleSerialized.find("tradeDraft") == std::string::npos, "single BUY mapping does not generate TradeDraft field");
    require(singleSerialized.find("tradeSuggestion") == std::string::npos,
            "single BUY mapping does not generate trade suggestion field");

    const auto missingMarketResult = etfdt::accounting::makeMissingMarketPriceReplayResult(
        "ACC-DEMO-001",
        "PF-DEMO-001",
        "159509",
        "1000",
        100100,
        9899900);
    const auto missingMarketPayload = mapReplayResultToPositionListResponseForTest(missingMarketResult, baseRequest);
    require(missingMarketPayload.status == etfdt::accounting::AccountingReplayStatus::Warning,
            "missing market price maps warning status");
    require(missingMarketPayload.dataQualityStatus == "WARNING", "missing market price maps degraded data quality");
    require(containsIssueCode(missingMarketPayload.issues, etfdt::accounting::AccountingIssueCode::MarketPriceMissing),
            "missing market price maps MARKET_PRICE_MISSING issue");
    require(missingMarketPayload.positions.size() == 1U, "missing market price keeps quantity/cost position");
    require(missingMarketPayload.positions.front().marketValueText == "UNAVAILABLE",
            "missing market price does not fabricate marketValueText");
    require(missingMarketPayload.positions.front().unrealizedPnlText == "UNAVAILABLE",
            "missing market price does not fabricate unrealizedPnlText");

    const auto multiCurrencyResult = etfdt::accounting::makeMultiCurrencyUnsupportedReplayResult(true);
    const auto multiCurrencyPayload = mapReplayResultToPositionListResponseForTest(multiCurrencyResult, baseRequest);
    require(containsIssueCode(multiCurrencyPayload.issues,
                              etfdt::accounting::AccountingIssueCode::MultiCurrencyUnsupported),
            "multi-currency maps MULTI_CURRENCY_UNSUPPORTED issue");
    require(containsIssueCode(multiCurrencyPayload.issues, etfdt::accounting::AccountingIssueCode::FxRateMissing),
            "multi-currency maps FX_RATE_MISSING issue");
    bool hasBlocking = false;
    for (const auto& issue : multiCurrencyPayload.issues) {
        if (issue.code == etfdt::accounting::AccountingIssueCode::MultiCurrencyUnsupported && issue.blocking) {
            hasBlocking = true;
        }
    }
    require(hasBlocking, "multi-currency keeps blocking issue");
    require(multiCurrencyPayload.positions.empty(), "multi-currency does not fabricate positions");
    const auto multiSerialized = serializePositionListPayloadForTest(multiCurrencyPayload);
    require(multiSerialized.find("totalAssets") == std::string::npos, "multi-currency does not fabricate totalAssets");
    require(multiSerialized.find("marketValue") == std::string::npos, "multi-currency does not fabricate marketValue");
    require(multiSerialized.find("unrealizedPnl") == std::string::npos, "multi-currency does not fabricate unrealizedPnl");
    require(multiCurrencyPayload.readOnly, "response mapping keeps readOnly=true");
    require(!multiCurrencyPayload.writeEnabled, "response mapping keeps writeEnabled=false");

    return failures == 0 ? 0 : 1;
}
