#include "DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iterator>
#include <string_view>
#include <unordered_map>

namespace etfdt::data_service_api {
namespace {

struct ParsedSheet {
    std::string originalName;
    std::string canonicalName;
    std::unordered_map<std::string, std::size_t> headerIndex;
    std::vector<std::vector<std::string>> rows;
    bool found = false;
};

constexpr std::string_view ZH_INITIAL_CASH =
    "\xE5""\x88""\x9D""\xE5""\xA7""\x8B""\xE7""\x8E""\xB0""\xE9""\x87""\x91";
constexpr std::string_view ZH_CASH = "\xE7""\x8E""\xB0""\xE9""\x87""\x91";
constexpr std::string_view ZH_CASH_FLOW_LEDGER =
    "\xE7""\x8E""\xB0""\xE9""\x87""\x91""\xE6""\xB5""\x81""\xE6""\xB0""\xB4";
constexpr std::string_view ZH_TRADE = "\xE4""\xBA""\xA4""\xE6""\x98""\x93";
constexpr std::string_view ZH_TRADE_LEDGER =
    "\xE4""\xBA""\xA4""\xE6""\x98""\x93""\xE6""\xB5""\x81""\xE6""\xB0""\xB4";
constexpr std::string_view ZH_EXECUTION_RECORD =
    "\xE6""\x88""\x90""\xE4""\xBA""\xA4""\xE8""\xAE""\xB0""\xE5""\xBD""\x95";
constexpr std::string_view ZH_MARKET_DATA = "\xE8""\xA1""\x8C""\xE6""\x83""\x85";
constexpr std::string_view ZH_MARKET_PRICE =
    "\xE5""\xB8""\x82""\xE5""\x9C""\xBA""\xE4""\xBB""\xB7""\xE6""\xA0""\xBC";
constexpr std::string_view ZH_FX_RATE = "\xE6""\xB1""\x87""\xE7""\x8E""\x87";
constexpr std::string_view ZH_FOREIGN_FX_RATE =
    "\xE5""\xA4""\x96""\xE6""\xB1""\x87""\xE6""\xB1""\x87""\xE7""\x8E""\x87";
constexpr std::string_view ZH_ROW_ID =
    "\xE6""\xB5""\x81""\xE6""\xB0""\xB4""\xE5""\x8F""\xB7";
constexpr std::string_view ZH_ROW_NUMBER = "\xE8""\xA1""\x8C""\xE5""\x8F""\xB7";
constexpr std::string_view ZH_TIME_UTC =
    "\xE6""\x97""\xB6""\xE9""\x97""\xB4""\x55""\x54""\x43";
constexpr std::string_view ZH_OCCURRED_TIME =
    "\xE5""\x8F""\x91""\xE7""\x94""\x9F""\xE6""\x97""\xB6""\xE9""\x97""\xB4";
constexpr std::string_view ZH_TRADE_TIME_UTC =
    "\xE6""\x88""\x90""\xE4""\xBA""\xA4""\xE6""\x97""\xB6""\xE9""\x97""\xB4""\x55""\x54""\x43";
constexpr std::string_view ZH_TRADE_TIME =
    "\xE4""\xBA""\xA4""\xE6""\x98""\x93""\xE6""\x97""\xB6""\xE9""\x97""\xB4";
constexpr std::string_view ZH_EXECUTION_TIME =
    "\xE6""\x88""\x90""\xE4""\xBA""\xA4""\xE6""\x97""\xB6""\xE9""\x97""\xB4";
constexpr std::string_view ZH_ACCOUNT_CODE =
    "\xE8""\xB4""\xA6""\xE6""\x88""\xB7""\xE4""\xBB""\xA3""\xE7""\xA0""\x81";
constexpr std::string_view ZH_ACCOUNT = "\xE8""\xB4""\xA6""\xE6""\x88""\xB7";
constexpr std::string_view ZH_PORTFOLIO_CODE =
    "\xE7""\xBB""\x84""\xE5""\x90""\x88""\xE4""\xBB""\xA3""\xE7""\xA0""\x81";
constexpr std::string_view ZH_PORTFOLIO = "\xE7""\xBB""\x84""\xE5""\x90""\x88";
constexpr std::string_view ZH_SECURITY_CODE =
    "\xE8""\xAF""\x81""\xE5""\x88""\xB8""\xE4""\xBB""\xA3""\xE7""\xA0""\x81";
constexpr std::string_view ZH_INSTRUMENT_CODE =
    "\xE6""\xA0""\x87""\xE7""\x9A""\x84""\xE4""\xBB""\xA3""\xE7""\xA0""\x81";
constexpr std::string_view ZH_SIDE = "\xE6""\x96""\xB9""\xE5""\x90""\x91";
constexpr std::string_view ZH_BUY_SELL_SIDE =
    "\xE4""\xB9""\xB0""\xE5""\x8D""\x96""\xE6""\x96""\xB9""\xE5""\x90""\x91";
constexpr std::string_view ZH_ACTION = "\xE5""\x8A""\xA8""\xE4""\xBD""\x9C";
constexpr std::string_view ZH_QUANTITY = "\xE6""\x95""\xB0""\xE9""\x87""\x8F";
constexpr std::string_view ZH_PRICE = "\xE4""\xBB""\xB7""\xE6""\xA0""\xBC";
constexpr std::string_view ZH_TRADE_PRICE =
    "\xE6""\x88""\x90""\xE4""\xBA""\xA4""\xE4""\xBB""\xB7""\xE6""\xA0""\xBC";
constexpr std::string_view ZH_AMOUNT = "\xE9""\x87""\x91""\xE9""\xA2""\x9D";
constexpr std::string_view ZH_TRADE_AMOUNT =
    "\xE6""\x88""\x90""\xE4""\xBA""\xA4""\xE9""\x87""\x91""\xE9""\xA2""\x9D";
constexpr std::string_view ZH_FEE =
    "\xE6""\x89""\x8B""\xE7""\xBB""\xAD""\xE8""\xB4""\xB9";
constexpr std::string_view ZH_EXPENSE = "\xE8""\xB4""\xB9""\xE7""\x94""\xA8";
constexpr std::string_view ZH_CASH_FLOW =
    "\xE7""\x8E""\xB0""\xE9""\x87""\x91""\xE6""\xB5""\x81";
constexpr std::string_view ZH_CURRENCY = "\xE5""\xB8""\x81""\xE7""\xA7""\x8D";
constexpr std::string_view ZH_SOURCE = "\xE6""\x9D""\xA5""\xE6""\xBA""\x90";
constexpr std::string_view ZH_MEMO = "\xE5""\xA4""\x87""\xE6""\xB3""\xA8";
constexpr std::string_view ZH_NOTE = "\xE8""\xAF""\xB4""\xE6""\x98""\x8E";
constexpr std::string_view ZH_BUY = "\xE4""\xB9""\xB0""\xE5""\x85""\xA5";
constexpr std::string_view ZH_SELL = "\xE5""\x8D""\x96""\xE5""\x87""\xBA";
constexpr std::string_view ZH_DEPOSIT = "\xE5""\x85""\xA5""\xE9""\x87""\x91";
constexpr std::string_view ZH_WITHDRAW = "\xE5""\x87""\xBA""\xE9""\x87""\x91";
constexpr std::string_view ZH_ADJUSTMENT = "\xE8""\xB0""\x83""\xE6""\x95""\xB4";

[[nodiscard]] std::string trimCopy(std::string_view value)
{
    std::size_t first = 0;
    while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first])) != 0) {
        ++first;
    }

    std::size_t last = value.size();
    while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1U])) != 0) {
        --last;
    }

    return std::string(value.substr(first, last - first));
}

[[nodiscard]] std::string upperAsciiCopy(std::string_view value)
{
    std::string output = trimCopy(value);
    std::transform(output.begin(), output.end(), output.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return output;
}

[[nodiscard]] std::string lowerAsciiCopy(std::string_view value)
{
    std::string output = trimCopy(value);
    std::transform(output.begin(), output.end(), output.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return output;
}

[[nodiscard]] bool blank(std::string_view value)
{
    return trimCopy(value).empty();
}

[[nodiscard]] bool containsSensitiveToken(std::string_view value)
{
    const auto lowered = lowerAsciiCopy(value);
    const char* tokens[] = {
        "credential",
        "password",
        "secret",
        "token",
        "api_key",
        "apikey",
        "access_key",
        "private_key",
        "bearer ",
        "endpoint",
        "account_number",
    };

    return std::any_of(std::begin(tokens), std::end(tokens), [&](const char* token) {
        return lowered.find(token) != std::string::npos;
    });
}

[[nodiscard]] bool numericText(std::string_view value)
{
    const auto text = trimCopy(value);
    if (text.empty()) {
        return false;
    }

    char* end = nullptr;
    (void)std::strtod(text.c_str(), &end);
    return end != text.c_str() && end != nullptr && *end == '\0';
}

[[nodiscard]] bool stringIn(std::string_view value, const std::vector<std::string_view>& accepted)
{
    return std::any_of(accepted.begin(), accepted.end(), [&](std::string_view entry) {
        return value == entry;
    });
}

[[nodiscard]] std::string canonicalSheetName(std::string_view sheetName)
{
    const auto trimmed = trimCopy(sheetName);
    const auto lowered = lowerAsciiCopy(trimmed);
    if (stringIn(lowered, {"initialcash", "initial_cash", "cash", "cashlog", "cashsummary"}) ||
        stringIn(trimmed, {ZH_INITIAL_CASH, ZH_CASH, ZH_CASH_FLOW_LEDGER})) {
        return "InitialCash";
    }
    if (stringIn(lowered, {"trades", "tradelog", "trade_log", "transactions"}) ||
        stringIn(trimmed, {ZH_TRADE, ZH_TRADE_LEDGER, ZH_EXECUTION_RECORD})) {
        return "Trades";
    }
    if (stringIn(lowered, {"marketprices", "market_prices", "prices"}) ||
        stringIn(trimmed, {ZH_MARKET_DATA, ZH_MARKET_PRICE})) {
        return "MarketPrices";
    }
    if (stringIn(lowered, {"fxrates", "fx_rates", "foreignexchange"}) ||
        stringIn(trimmed, {ZH_FX_RATE, ZH_FOREIGN_FX_RATE})) {
        return "FxRates";
    }
    return trimmed;
}

[[nodiscard]] std::string canonicalHeaderName(std::string_view header)
{
    const auto trimmed = trimCopy(header);
    const auto upper = upperAsciiCopy(trimmed);

    if (stringIn(upper, {"ROW_ID", "ROWID", "ID"}) || stringIn(trimmed, {ZH_ROW_ID, ZH_ROW_NUMBER})) {
        return "ROW_ID";
    }
    if (stringIn(upper, {"TIME_UTC", "TIME", "OCCURRED_AT"}) ||
        stringIn(trimmed, {ZH_TIME_UTC, ZH_OCCURRED_TIME})) {
        return "TIME_UTC";
    }
    if (stringIn(upper, {"TRADE_TIME_UTC", "TRADE_TIME", "TRADED_AT"}) ||
        stringIn(trimmed, {ZH_TRADE_TIME_UTC, ZH_TRADE_TIME, ZH_EXECUTION_TIME})) {
        return "TRADE_TIME_UTC";
    }
    if (stringIn(upper, {"ACCOUNT_CODE", "ACCOUNT_ID", "ACCOUNT"}) ||
        stringIn(trimmed, {ZH_ACCOUNT_CODE, ZH_ACCOUNT})) {
        return "ACCOUNT_CODE";
    }
    if (stringIn(upper, {"PORTFOLIO_CODE", "PORTFOLIO_ID", "PORTFOLIO"}) ||
        stringIn(trimmed, {ZH_PORTFOLIO_CODE, ZH_PORTFOLIO})) {
        return "PORTFOLIO_CODE";
    }
    if (stringIn(upper, {"INSTRUMENT_CODE", "SECURITY_CODE", "SYMBOL"}) ||
        stringIn(trimmed, {ZH_SECURITY_CODE, ZH_INSTRUMENT_CODE})) {
        return "INSTRUMENT_CODE";
    }
    if (stringIn(upper, {"SIDE", "ACTION"}) || stringIn(trimmed, {ZH_SIDE, ZH_BUY_SELL_SIDE, ZH_ACTION})) {
        return upper == "ACTION" || trimmed == ZH_ACTION ? "ACTION" : "SIDE";
    }
    if (stringIn(upper, {"QUANTITY", "QTY"}) || stringIn(trimmed, {ZH_QUANTITY})) {
        return "QUANTITY";
    }
    if (stringIn(upper, {"PRICE", "TRADE_PRICE"}) || stringIn(trimmed, {ZH_PRICE, ZH_TRADE_PRICE})) {
        return "PRICE";
    }
    if (stringIn(upper, {"AMOUNT", "GROSS_AMOUNT"}) || stringIn(trimmed, {ZH_AMOUNT, ZH_TRADE_AMOUNT})) {
        return "AMOUNT";
    }
    if (stringIn(upper, {"FEE", "COMMISSION"}) || stringIn(trimmed, {ZH_FEE, ZH_EXPENSE})) {
        return "FEE";
    }
    if (stringIn(upper, {"CASH_FLOW", "CASHFLOW"}) || stringIn(trimmed, {ZH_CASH_FLOW})) {
        return "CASH_FLOW";
    }
    if (stringIn(upper, {"CURRENCY", "CCY"}) || stringIn(trimmed, {ZH_CURRENCY})) {
        return "CURRENCY";
    }
    if (stringIn(upper, {"SOURCE", "SRC"}) || stringIn(trimmed, {ZH_SOURCE})) {
        return "SOURCE";
    }
    if (stringIn(upper, {"MEMO", "NOTE", "REMARK"}) || stringIn(trimmed, {ZH_MEMO, ZH_NOTE})) {
        return "MEMO";
    }
    if (stringIn(upper, {"PRICE_TIME_UTC", "PRICE_TIME"})) {
        return "PRICE_TIME_UTC";
    }
    if (stringIn(upper, {"BASE_CURRENCY", "BASE_CCY"})) {
        return "BASE_CURRENCY";
    }
    if (stringIn(upper, {"QUOTE_CURRENCY", "QUOTE_CCY"})) {
        return "QUOTE_CURRENCY";
    }
    if (stringIn(upper, {"RATE_TIME_UTC", "RATE_TIME"})) {
        return "RATE_TIME_UTC";
    }
    if (stringIn(upper, {"RATE", "FX_RATE"})) {
        return "RATE";
    }
    return upper;
}

void addDiagnostic(
    ShellAccountingExcelVbaImportReadOnlyParseResult& result,
    std::string code,
    std::string field,
    std::string sheetName,
    std::string rowId = {})
{
    result.diagnostics.push_back(ShellAccountingExcelVbaImportDiagnostic{
        "ERROR",
        std::move(code),
        std::move(field),
        std::move(sheetName),
        std::move(rowId),
    });
}

[[nodiscard]] ParsedSheet parseSheet(const ShellAccountingExcelVbaImportSheet& sheet)
{
    ParsedSheet parsed;
    parsed.originalName = sheet.sheetName;
    parsed.canonicalName = canonicalSheetName(sheet.sheetName);
    parsed.rows = sheet.rows;
    parsed.found = true;
    for (std::size_t index = 0; index < sheet.headers.size(); ++index) {
        parsed.headerIndex.emplace(canonicalHeaderName(sheet.headers[index]), index);
    }
    return parsed;
}

[[nodiscard]] const ParsedSheet* findSheet(const std::vector<ParsedSheet>& sheets, std::string_view canonicalName)
{
    const auto found = std::find_if(sheets.begin(), sheets.end(), [&](const ParsedSheet& sheet) {
        return sheet.canonicalName == canonicalName;
    });
    return found == sheets.end() ? nullptr : &*found;
}

void validateRequiredHeaders(
    ShellAccountingExcelVbaImportReadOnlyParseResult& result,
    const ParsedSheet* sheet,
    std::string_view canonicalName,
    const std::vector<std::string_view>& requiredHeaders)
{
    if (sheet == nullptr) {
        addDiagnostic(result, "MISSING_REQUIRED_SHEET", std::string(canonicalName), std::string(canonicalName));
        return;
    }
    for (const auto header : requiredHeaders) {
        if (sheet->headerIndex.find(std::string(header)) == sheet->headerIndex.end()) {
            addDiagnostic(result, "MISSING_REQUIRED_HEADER", std::string(header), sheet->originalName);
        }
    }
}

[[nodiscard]] std::string cellValue(const ParsedSheet& sheet, const std::vector<std::string>& row, std::string_view field)
{
    const auto found = sheet.headerIndex.find(std::string(field));
    if (found == sheet.headerIndex.end() || found->second >= row.size()) {
        return {};
    }
    return trimCopy(row[found->second]);
}

[[nodiscard]] std::string normalizedTradeAction(std::string_view value)
{
    const auto upper = upperAsciiCopy(value);
    const auto trimmed = trimCopy(value);
    if (upper == "BUY" || trimmed == ZH_BUY) {
        return etfdt::accounting::TradeAction::Buy;
    }
    if (upper == "SELL" || trimmed == ZH_SELL) {
        return etfdt::accounting::TradeAction::Sell;
    }
    return {};
}

[[nodiscard]] std::string normalizedCashAction(std::string_view value)
{
    const auto upper = upperAsciiCopy(value);
    const auto trimmed = trimCopy(value);
    if (upper == "INITIAL_CASH" || upper == "INITIALCASH" || trimmed == ZH_INITIAL_CASH) {
        return etfdt::accounting::CashAction::InitialCash;
    }
    if (upper == "DEPOSIT" || trimmed == ZH_DEPOSIT) {
        return etfdt::accounting::CashAction::Deposit;
    }
    if (upper == "WITHDRAW" || upper == "WITHDRAWAL" || trimmed == ZH_WITHDRAW) {
        return etfdt::accounting::CashAction::Withdraw;
    }
    if (upper == "ADJUSTMENT" || trimmed == ZH_ADJUSTMENT) {
        return etfdt::accounting::CashAction::Adjustment;
    }
    return {};
}

void validateSensitiveText(
    ShellAccountingExcelVbaImportReadOnlyParseResult& result,
    const std::string& value,
    const std::string& field,
    const std::string& sheetName,
    const std::string& rowId)
{
    if (containsSensitiveToken(value)) {
        addDiagnostic(result, "SENSITIVE_FIELD_TOKEN", field, sheetName, rowId);
    }
}

void importCashRows(
    ShellAccountingExcelVbaImportReadOnlyParseResult& result,
    const ParsedSheet& sheet)
{
    for (const auto& row : sheet.rows) {
        const auto rowId = cellValue(sheet, row, "ROW_ID");
        const auto action = normalizedCashAction(cellValue(sheet, row, "ACTION"));
        const auto amount = cellValue(sheet, row, "AMOUNT");
        const auto currency = cellValue(sheet, row, "CURRENCY");
        const auto note = cellValue(sheet, row, "MEMO");
        const auto issueCountBefore = result.diagnostics.size();

        if (action.empty()) {
            addDiagnostic(result, "INVALID_ACTION", "ACTION", sheet.originalName, rowId);
        }
        if (blank(amount)) {
            addDiagnostic(result, "MISSING_CASH_FIELD", "AMOUNT", sheet.originalName, rowId);
        } else if (!numericText(amount)) {
            addDiagnostic(result, "NON_NUMERIC_AMOUNT", "AMOUNT", sheet.originalName, rowId);
        }
        if (blank(currency)) {
            addDiagnostic(result, "MISSING_CASH_FIELD", "CURRENCY", sheet.originalName, rowId);
        }
        validateSensitiveText(result, note, "MEMO", sheet.originalName, rowId);

        if (result.diagnostics.size() != issueCountBefore) {
            continue;
        }

        result.cashFacts.push_back(etfdt::accounting::CashFactDto{
            rowId,
            cellValue(sheet, row, "TIME_UTC"),
            cellValue(sheet, row, "ACCOUNT_CODE"),
            cellValue(sheet, row, "PORTFOLIO_CODE"),
            action,
            amount,
            currency,
            note,
        });
    }
}

void importTradeRows(
    ShellAccountingExcelVbaImportReadOnlyParseResult& result,
    const ParsedSheet& sheet)
{
    for (const auto& row : sheet.rows) {
        const auto rowId = cellValue(sheet, row, "ROW_ID");
        const auto action = normalizedTradeAction(cellValue(sheet, row, "SIDE"));
        const auto source = cellValue(sheet, row, "SOURCE");
        const auto memo = cellValue(sheet, row, "MEMO");
        const auto issueCountBefore = result.diagnostics.size();

        if (action.empty()) {
            addDiagnostic(result, "INVALID_ACTION", "SIDE", sheet.originalName, rowId);
        }
        for (std::string_view field : {"QUANTITY", "PRICE", "AMOUNT", "FEE", "CASH_FLOW"}) {
            const auto value = cellValue(sheet, row, field);
            if (!numericText(value)) {
                addDiagnostic(result, "NON_NUMERIC_AMOUNT", std::string(field), sheet.originalName, rowId);
            }
        }
        validateSensitiveText(result, source, "SOURCE", sheet.originalName, rowId);
        validateSensitiveText(result, memo, "MEMO", sheet.originalName, rowId);

        if (result.diagnostics.size() != issueCountBefore) {
            continue;
        }

        result.tradeFacts.push_back(etfdt::accounting::TradeFactDto{
            rowId,
            cellValue(sheet, row, "TRADE_TIME_UTC"),
            cellValue(sheet, row, "ACCOUNT_CODE"),
            cellValue(sheet, row, "PORTFOLIO_CODE"),
            cellValue(sheet, row, "INSTRUMENT_CODE"),
            action,
            cellValue(sheet, row, "QUANTITY"),
            cellValue(sheet, row, "PRICE"),
            cellValue(sheet, row, "AMOUNT"),
            cellValue(sheet, row, "FEE"),
            cellValue(sheet, row, "CASH_FLOW"),
            cellValue(sheet, row, "CURRENCY"),
            source,
            memo,
        });
    }
}

void importMarketPriceRows(
    ShellAccountingExcelVbaImportReadOnlyParseResult& result,
    const ParsedSheet* sheet)
{
    if (sheet == nullptr) {
        return;
    }
    for (const auto& row : sheet->rows) {
        const auto price = cellValue(*sheet, row, "PRICE");
        if (!numericText(price)) {
            addDiagnostic(result, "NON_NUMERIC_AMOUNT", "PRICE", sheet->originalName, cellValue(*sheet, row, "INSTRUMENT_CODE"));
            continue;
        }
        result.marketPriceFacts.push_back(etfdt::accounting::MarketPriceFactDto{
            cellValue(*sheet, row, "INSTRUMENT_CODE"),
            cellValue(*sheet, row, "PRICE_TIME_UTC"),
            price,
            cellValue(*sheet, row, "CURRENCY"),
            cellValue(*sheet, row, "SOURCE"),
        });
    }
}

void importFxRows(
    ShellAccountingExcelVbaImportReadOnlyParseResult& result,
    const ParsedSheet* sheet)
{
    if (sheet == nullptr) {
        return;
    }
    for (const auto& row : sheet->rows) {
        const auto rate = cellValue(*sheet, row, "RATE");
        if (!numericText(rate)) {
            addDiagnostic(result, "NON_NUMERIC_AMOUNT", "RATE", sheet->originalName, cellValue(*sheet, row, "BASE_CURRENCY"));
            continue;
        }
        result.fxRateFacts.push_back(etfdt::accounting::FxRateFactDto{
            cellValue(*sheet, row, "BASE_CURRENCY"),
            cellValue(*sheet, row, "QUOTE_CURRENCY"),
            cellValue(*sheet, row, "RATE_TIME_UTC"),
            rate,
            cellValue(*sheet, row, "SOURCE"),
        });
    }
}

}  // namespace

ShellAccountingExcelVbaImportReadOnlyParseResult parseShellAccountingExcelVbaImportReadOnly(
    const ShellAccountingExcelVbaImportPayload& payload)
{
    ShellAccountingExcelVbaImportReadOnlyParseResult result;
    result.rawUserDataExposed = payload.rawUserDataExposed;
    if (payload.rawUserDataExposed) {
        addDiagnostic(result, "RAW_USER_DATA_EXPOSED", "rawUserDataExposed", "Workbook");
        return result;
    }

    std::vector<ParsedSheet> sheets;
    sheets.reserve(payload.sheets.size());
    for (const auto& sheet : payload.sheets) {
        sheets.push_back(parseSheet(sheet));
    }

    const auto* cashSheet = findSheet(sheets, "InitialCash");
    const auto* tradeSheet = findSheet(sheets, "Trades");
    const auto* priceSheet = findSheet(sheets, "MarketPrices");
    const auto* fxSheet = findSheet(sheets, "FxRates");

    validateRequiredHeaders(
        result,
        cashSheet,
        "InitialCash",
        {"ROW_ID", "TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "ACTION", "AMOUNT", "CURRENCY", "MEMO"});
    validateRequiredHeaders(
        result,
        tradeSheet,
        "Trades",
        {"ROW_ID", "TRADE_TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "INSTRUMENT_CODE", "SIDE", "QUANTITY", "PRICE", "AMOUNT", "FEE", "CASH_FLOW", "CURRENCY", "SOURCE", "MEMO"});

    if (!result.diagnostics.empty()) {
        return result;
    }

    importCashRows(result, *cashSheet);
    importTradeRows(result, *tradeSheet);
    importMarketPriceRows(result, priceSheet);
    importFxRows(result, fxSheet);

    result.accepted = result.diagnostics.empty();
    if (!result.accepted) {
        result.cashFacts.clear();
        result.tradeFacts.clear();
        result.marketPriceFacts.clear();
        result.fxRateFacts.clear();
    }
    return result;
}

}  // namespace etfdt::data_service_api
