#include "PositionListMappingContractTestUtils.h"

#include "AccountingEngine/AccountingIssue.h"
#include "AccountingEngine/AccountingReplayValidation.h"

#include <iostream>
#include <string>

namespace {

int failures = 0;

void require(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}

etfdt::data_access::TradeFactRow buyRow()
{
    return etfdt::data_access::TradeFactRow{
        "TL-001",
        "2026-01-02T10:00:00Z",
        "ACC-DEMO-001",
        "PF-DEMO-001",
        "159509",
        "BUY",
        "1000",
        "1.000",
        "1000.00",
        "1.00",
        "-1001.00",
        "CNY",
        "trade_log",
        "demo buy",
        1,
    };
}

}  // namespace

int main()
{
    using namespace etfdt::tests::position_list_mapping;

    const auto row = buyRow();
    const auto dto = mapTradeFactRowToTradeFactDtoForTest(row);

    require(dto.factId == row.factId, "factId maps to factId");
    require(dto.tradeTime == row.tradeTime, "tradeTime maps to tradeTime");
    require(dto.accountId == row.accountId, "accountId maps to accountId");
    require(dto.portfolioId == row.portfolioId, "portfolioId maps to portfolioId");
    require(dto.instrumentCode == row.instrumentCode, "instrumentCode maps to instrumentCode");
    require(dto.action == row.action, "action maps to action");
    require(dto.quantityText == row.quantityText, "quantityText maps to quantityText");
    require(dto.priceText == row.priceText, "priceText maps to priceText");
    require(dto.amountText == row.amountText, "amountText maps to amountText");
    require(dto.feeText == row.feeText, "feeText maps to feeText");
    require(dto.cashFlowText == row.cashFlowText, "cashFlowText maps to cashFlowText");
    require(dto.currency == row.currency, "currency maps to currency");
    require(dto.source == row.source, "source maps to source");
    require(dto.note == row.note, "note maps to note");
    require(etfdt::accounting::validateTradeFact(dto).valid, "mapped BUY trade fact validates");

    auto sell = buyRow();
    sell.factId = "TL-002";
    sell.action = "SELL";
    sell.cashFlowText = "999.00";
    const auto sellDto = mapTradeFactRowToTradeFactDtoForTest(sell);
    require(sellDto.action == etfdt::accounting::TradeAction::Sell, "SELL action maps correctly");
    require(etfdt::accounting::validateTradeFact(sellDto).valid, "mapped SELL trade fact validates");

    auto missingFee = buyRow();
    missingFee.feeText.clear();
    const auto missingFeeDto = mapTradeFactRowToTradeFactDtoForTest(missingFee);
    require(missingFeeDto.feeText.empty(), "empty feeText is preserved and not defaulted to zero");
    require(std::string(kKnownTradeLogFeeSchemaGapForTest).find("fee_cents") != std::string::npos,
            "known trade_log fee schema gap is documented in the mapping contract");

    auto invalidAction = buyRow();
    invalidAction.action = "DIVIDEND";
    const auto invalidActionDto = mapTradeFactRowToTradeFactDtoForTest(invalidAction);
    const auto invalidActionValidation = etfdt::accounting::validateTradeFact(invalidActionDto);
    require(!invalidActionValidation.valid, "invalid action is caught by validateTradeFact");
    require(invalidActionValidation.hasIssueCode(etfdt::accounting::AccountingIssueCode::UnsupportedAction),
            "invalid action reports UNSUPPORTED_ACTION");

    auto invalidAmount = buyRow();
    invalidAmount.amountText = "CNY 1000";
    const auto invalidAmountDto = mapTradeFactRowToTradeFactDtoForTest(invalidAmount);
    const auto invalidAmountValidation = etfdt::accounting::validateTradeFact(invalidAmountDto);
    require(!invalidAmountValidation.valid, "invalid money text is caught by validateTradeFact");
    require(invalidAmountValidation.hasIssueCode(etfdt::accounting::AccountingIssueCode::InvalidMoneyText),
            "invalid money text reports INVALID_MONEY_TEXT");

    auto invalidQuantity = buyRow();
    invalidQuantity.quantityText = "lots";
    const auto invalidQuantityDto = mapTradeFactRowToTradeFactDtoForTest(invalidQuantity);
    const auto invalidQuantityValidation = etfdt::accounting::validateTradeFact(invalidQuantityDto);
    require(!invalidQuantityValidation.valid, "invalid quantity text is caught by validateTradeFact");
    require(invalidQuantityValidation.hasIssueCode(etfdt::accounting::AccountingIssueCode::InvalidQuantityText),
            "invalid quantity text reports INVALID_QUANTITY_TEXT");

    return failures == 0 ? 0 : 1;
}
