#include "DataAccess/ShellAccountingTradeDraftRepository.h"

#include "DataAccess/AuditLogRepository.h"
#include "DataAccess/TransactionRunner.h"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>

namespace etfdt::data_access {
namespace {

bool isBlank(const std::string& value)
{
    for (const char ch : value) {
        if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') {
            return false;
        }
    }
    return true;
}

std::string jsonString(std::string value)
{
    std::string escaped;
    escaped.reserve(value.size());
    for (const char ch : value) {
        switch (ch) {
        case '"':
            escaped += "\\\"";
            break;
        case '\\':
            escaped += "\\\\";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            escaped += "\\r";
            break;
        case '\t':
            escaped += "\\t";
            break;
        default:
            escaped.push_back(ch);
            break;
        }
    }
    return "\"" + escaped + "\"";
}

std::string sanitizeUidPart(std::string value)
{
    for (char& ch : value) {
        const bool allowed = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')
            || (ch >= '0' && ch <= '9') || ch == '-' || ch == '_';
        if (!allowed) {
            ch = '-';
        }
    }
    return value;
}

std::string sourceId(const ShellAccountingTradeDraftCreateRequest& request)
{
    if (!isBlank(request.idempotencyKey)) {
        return request.idempotencyKey;
    }
    if (!isBlank(request.sourceSnapshotId)) {
        return request.sourceSnapshotId;
    }
    return request.sourceReplayId;
}

std::string draftSignature(const ShellAccountingTradeDraftCreateRequest& request)
{
    if (!isBlank(request.recommendationDigest)) {
        return "EPIC-278|" + request.idempotencyKey + "|" + request.recommendationDigest + "|"
            + request.side + "|" + request.instrumentCode + "|" + request.quantityText + "|"
            + request.amountText;
    }
    std::ostringstream stream;
    stream << "TASK-148|" << request.accountId << '|' << request.portfolioId << '|'
           << request.strategyCode << '|' << request.instrumentCode << '|' << request.side << '|'
           << request.quantity1e6 << '|' << sourceId(request);
    return stream.str();
}

std::string draftUid(const ShellAccountingTradeDraftCreateRequest& request)
{
    std::ostringstream stream;
    stream << (isBlank(request.recommendationDigest) ? "task-148-draft-" : "epic-278-draft-")
           << request.accountId << '-' << request.portfolioId << '-'
           << sanitizeUidPart(request.strategyCode) << '-' << sanitizeUidPart(request.instrumentCode)
           << '-' << request.side << '-' << request.quantity1e6 << '-'
           << sanitizeUidPart(sourceId(request));
    return stream.str();
}

std::string auditReason(const ShellAccountingTradeDraftCreateRequest& request)
{
    return isBlank(request.recommendationDigest)
        ? "ShellAccounting TradeDraft creation: " + draftSignature(request)
        : "ShellAccounting TradeDraft create from recommendation: " + draftSignature(request);
}

std::string statusForInsert(const ShellAccountingTradeDraftCreateRequest& request)
{
    if (request.status == "EXPIRED") {
        return "EXPIRED";
    }
    if (request.status == "CANCELED" || request.status == "REJECTED" || request.status == "NOT_ELIGIBLE") {
        return "CANCELED";
    }
    return "ACTIVE";
}

std::string tradeSourceForInsert(const ShellAccountingTradeDraftCreateRequest& request)
{
    if (request.tradeSource == "ETF_DIRECT" || request.tradeSource == "EXCHANGE_DIRECT") {
        return "EXCHANGE_DIRECT";
    }
    if (request.tradeSource == "OTC_REPLACEMENT_GENERIC" || request.tradeSource == "OTC_REPLACEMENT"
        || request.tradeSource == "OTC_SUBSTITUTE") {
        return "OTC_SUBSTITUTE";
    }
    if (request.tradeSource == "CASH") {
        return "CASH";
    }
    if (request.tradeSource == "MANUAL") {
        return "MANUAL";
    }
    return "SYSTEM";
}

std::string intentForInsert(const ShellAccountingTradeDraftCreateRequest& request)
{
    if (request.sourceRecommendationReason == "TAKE_PROFIT") {
        return "TAKE_PROFIT";
    }
    if (request.sourceRecommendationReason == "EXTREME_PREMIUM") {
        return "PREMIUM_SELL";
    }
    if (request.side == "SELL") {
        return "NORMAL_SELL";
    }
    if (request.sourceRecommendationReason == "BUY_GRID_TIER") {
        return "GRID_BUY";
    }
    return "NORMAL_BUY";
}

std::int64_t parseMoneyCents(const std::string& text)
{
    if (isBlank(text)) {
        return 0;
    }
    return static_cast<std::int64_t>(std::llround(std::strtod(text.c_str(), nullptr) * 100.0));
}

std::int64_t parseScaled1e6(const std::string& text)
{
    if (isBlank(text)) {
        return 0;
    }
    return static_cast<std::int64_t>(std::llround(std::strtod(text.c_str(), nullptr) * 1000000.0));
}

std::string formatCents(std::int64_t cents)
{
    const bool negative = cents < 0;
    const auto absolute = negative ? -cents : cents;

    std::ostringstream output;
    if (negative) {
        output << '-';
    }
    output << (absolute / 100) << '.' << std::setw(2) << std::setfill('0') << (absolute % 100);
    return output.str();
}

std::string formatFixed1e6(std::int64_t value)
{
    const bool negative = value < 0;
    const auto absolute = negative ? -value : value;
    const auto whole = absolute / 1000000;
    auto fraction = absolute % 1000000;

    std::ostringstream output;
    if (negative) {
        output << '-';
    }
    output << whole;
    if (fraction == 0) {
        return output.str();
    }

    output << '.' << std::setw(6) << std::setfill('0') << fraction;
    auto text = output.str();
    while (!text.empty() && text.back() == '0') {
        text.pop_back();
    }
    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }
    return text;
}

std::string netCashImpactText(const std::string& side, std::int64_t amountCents)
{
    return formatCents(side == "BUY" ? -amountCents : amountCents);
}

std::string sanitizedAuditPayload(
    const ShellAccountingTradeDraftCreateRequest& request,
    std::int64_t draftId,
    const std::string& uid)
{
    std::ostringstream stream;
    stream << "{"
           << "\"sourceAction\":\"accounting.tradedraft.create\","
           << "\"authorizationToken\":" << jsonString(request.authorizationToken) << ','
           << "\"draftId\":" << draftId << ','
           << "\"draftUid\":" << jsonString(uid) << ','
           << "\"draftSignature\":" << jsonString(draftSignature(request)) << ','
           << "\"accountId\":" << request.accountId << ','
           << "\"portfolioId\":" << request.portfolioId << ','
           << "\"instrumentCode\":" << jsonString(request.instrumentCode) << ','
           << "\"side\":" << jsonString(request.side) << ','
           << "\"quantity1e6\":" << request.quantity1e6 << ','
           << "\"idempotencyKey\":" << jsonString(request.idempotencyKey) << ','
           << "\"recommendationDigest\":" << jsonString(request.recommendationDigest) << ','
           << "\"sourceRecommendationAction\":" << jsonString(request.sourceRecommendationAction) << ','
           << "\"sourceRecommendationReason\":" << jsonString(request.sourceRecommendationReason) << ','
           << "\"amountText\":" << jsonString(request.amountText) << ','
           << "\"netCashImpactText\":" << jsonString(request.netCashImpactText) << ','
           << "\"userConfirmed\":" << (request.userConfirmed ? "true" : "false") << ','
           << "\"reason\":" << jsonString(request.reason) << ','
           << "\"sourceSnapshotId\":" << jsonString(request.sourceSnapshotId) << ','
           << "\"sourceReplayId\":" << jsonString(request.sourceReplayId) << ','
           << "\"riskFlags\":{\"checked\":true,\"blocked\":false},"
           << "\"privacy\":{"
           << "\"rawSqlExposed\":false,"
           << "\"rawTradeLogPayloadExposed\":false,"
           << "\"fullSnapshotPayloadExposed\":false,"
           << "\"internalStackExposed\":false,"
           << "\"sensitiveRawPayloadExposed\":false"
           << "}"
           << "}";
    return stream.str();
}

DatabaseResult<bool> requireReferencedRow(
    SQLiteConnection& connection,
    const char* table,
    std::int64_t id,
    const char* label)
{
    auto rows = connection.queryRows(
        std::string("SELECT id FROM ") + table + " WHERE id = ? LIMIT 1;",
        {std::to_string(id)});
    if (!rows) {
        return DatabaseResult<bool>::failure(rows.error().errorCode, rows.error().message);
    }
    if (rows.value().empty()) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            std::string("ShellAccounting TradeDraft requires existing ") + label);
    }
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> propagateFailure(const DatabaseResult<bool>& result)
{
    return DatabaseResult<bool>::failure(result.error().errorCode, result.error().message);
}

}  // namespace

ShellAccountingTradeDraftRepository::ShellAccountingTradeDraftRepository(
    SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

DatabaseResult<ShellAccountingTradeDraftCreateResult>
ShellAccountingTradeDraftRepository::createTradeDraft(
    const ShellAccountingTradeDraftCreateRequest& request)
{
    auto validation = validateRequest(request);
    if (!validation) {
        return DatabaseResult<ShellAccountingTradeDraftCreateResult>::failure(
            validation.error().errorCode,
            validation.error().message);
    }

    auto existing = findExistingDraft(request);
    if (!existing) {
        return DatabaseResult<ShellAccountingTradeDraftCreateResult>::failure(
            existing.error().errorCode,
            existing.error().message);
    }
    if (existing.value().has_value()) {
        auto existingSignature = loadDraftSignature(*existing.value());
        if (!existingSignature) {
            return DatabaseResult<ShellAccountingTradeDraftCreateResult>::failure(
                existingSignature.error().errorCode,
                existingSignature.error().message);
        }
        ShellAccountingTradeDraftCreateResult result;
        result.draftId = *existing.value();
        result.draftSignature = draftSignature(request);
        result.draftUid = draftUid(request);
        result.side = request.side;
        result.instrumentCode = request.instrumentCode;
        result.quantityText = request.quantityText;
        result.amountText = request.amountText;
        result.netCashImpactText = request.netCashImpactText;
        if (existingSignature.value() == draftSignature(request)) {
            result.duplicateDraft = true;
            result.status = "DUPLICATE";
        } else {
            result.idempotencyConflict = true;
            result.status = "IDEMPOTENCY_CONFLICT";
            result.issueCodes.push_back("TRADEDRAFT_IDEMPOTENCY_CONFLICT");
        }
        return DatabaseResult<ShellAccountingTradeDraftCreateResult>::success(result);
    }

    ShellAccountingTradeDraftCreateResult result;
    TransactionRunner runner(connection_);
    auto transactionResult = runner.runInTransaction([&]() {
        auto insertedDraft = insertDraft(request);
        if (!insertedDraft) {
            return DatabaseResult<bool>::failure(
                insertedDraft.error().errorCode,
                insertedDraft.error().message);
        }

        result.draftId = insertedDraft.value();
        result.draftUid = draftUid(request);
        result.draftSignature = draftSignature(request);
        result.draftWritten = true;
        result.side = request.side;
        result.instrumentCode = request.instrumentCode;
        result.quantityText = request.quantityText;
        result.amountText = request.amountText;
        result.netCashImpactText = request.netCashImpactText;

        auto insertedLeg = insertDraftLeg(request, result.draftId);
        if (!insertedLeg) {
            return DatabaseResult<bool>::failure(
                insertedLeg.error().errorCode,
                insertedLeg.error().message);
        }
        result.legWritten = true;

        auto insertedAudit = insertAuditEvent(request, result.draftId, result.draftUid);
        if (!insertedAudit) {
            return DatabaseResult<bool>::failure(
                insertedAudit.error().errorCode,
                insertedAudit.error().message);
        }

        result.auditLogId = insertedAudit.value();
        result.auditWritten = true;
        return DatabaseResult<bool>::success(true);
    });

    if (!transactionResult) {
        return DatabaseResult<ShellAccountingTradeDraftCreateResult>::failure(
            transactionResult.error().errorCode,
            transactionResult.error().message);
    }

    result.transactionCommitted = true;
    return DatabaseResult<ShellAccountingTradeDraftCreateResult>::success(result);
}

DatabaseResult<ShellAccountingTradeDraftSummary> ShellAccountingTradeDraftRepository::readTradeDraftSummary(
    const std::string& idempotencyKey,
    std::int64_t draftId)
{
    if (draftId <= 0 && isBlank(idempotencyKey)) {
        return DatabaseResult<ShellAccountingTradeDraftSummary>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft summary requires draftId or idempotencyKey");
    }

    auto rows = draftId > 0
        ? connection_.queryRows(
              "SELECT d.id,d.uid,d.draft_signature,d.status,d.action_type,d.account_id,d.portfolio_id,"
              "d.strategy_code,d.total_quantity_1e6,d.total_amount_cents,COALESCE(d.draft_group_no,''),"
              "COALESCE(l.actual_code,''),COALESCE(l.suggested_quantity_1e6,d.total_quantity_1e6),"
              "COALESCE(l.suggested_amount_cents,d.total_amount_cents) "
              "FROM trade_draft d LEFT JOIN trade_draft_leg l ON l.draft_id = d.id "
              "WHERE d.id = ? ORDER BY d.id DESC,l.id ASC LIMIT 1;",
              {std::to_string(draftId)})
        : connection_.queryRows(
              "SELECT d.id,d.uid,d.draft_signature,d.status,d.action_type,d.account_id,d.portfolio_id,"
              "d.strategy_code,d.total_quantity_1e6,d.total_amount_cents,COALESCE(d.draft_group_no,''),"
              "COALESCE(l.actual_code,''),COALESCE(l.suggested_quantity_1e6,d.total_quantity_1e6),"
              "COALESCE(l.suggested_amount_cents,d.total_amount_cents) "
              "FROM trade_draft d LEFT JOIN trade_draft_leg l ON l.draft_id = d.id "
              "WHERE d.draft_group_no = ? ORDER BY d.id DESC,l.id ASC LIMIT 1;",
              {idempotencyKey});
    if (!rows) {
        return DatabaseResult<ShellAccountingTradeDraftSummary>::failure(
            rows.error().errorCode,
            rows.error().message);
    }

    ShellAccountingTradeDraftSummary summary;
    summary.readOnly = true;
    if (rows.value().empty()) {
        return DatabaseResult<ShellAccountingTradeDraftSummary>::success(summary);
    }

    const auto& row = rows.value().front();
    summary.found = true;
    summary.draftId = row[0].int64Value;
    summary.draftUid = row[1].text;
    summary.draftSignature = row[2].text;
    summary.status = row[3].text;
    summary.side = row[4].text;
    summary.accountId = std::to_string(row[5].int64Value);
    summary.portfolioId = std::to_string(row[6].int64Value);
    summary.strategyCode = row[7].text;
    summary.quantityText = formatFixed1e6(row[12].int64Value);
    summary.amountText = formatCents(row[13].int64Value);
    summary.netCashImpactText = netCashImpactText(summary.side, row[13].int64Value);
    summary.idempotencyKey = row[10].text;
    summary.instrumentCode = row[11].text;
    return DatabaseResult<ShellAccountingTradeDraftSummary>::success(summary);
}

DatabaseResult<bool> ShellAccountingTradeDraftRepository::validateRequest(
    const ShellAccountingTradeDraftCreateRequest& request)
{
    const bool epic278RecommendationWrite =
        request.authorizationToken == "EPIC-278_TRADEDRAFT_RECOMMENDATION_WRITE";
    if (request.authorizationToken != "TASK-148_TRADEDRAFT_WRITE" && !epic278RecommendationWrite) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E8001_AUTH_REQUIRED,
            "ShellAccounting TradeDraft creation requires authorized TradeDraft write token");
    }
    if (request.accountId <= 0 || request.portfolioId <= 0) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft requires accountId and portfolioId");
    }
    if (request.strategyId <= 0 || isBlank(request.strategyCode)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft requires strategyId and strategyCode");
    }
    if (request.instrumentId <= 0 || isBlank(request.instrumentCode)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft requires instrumentId and instrumentCode");
    }
    if (request.side != "BUY" && request.side != "SELL") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft side must be BUY or SELL");
    }
    if (request.quantity1e6 <= 0) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft quantity must be positive");
    }
    if (isBlank(request.reason)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft requires reason");
    }
    if (epic278RecommendationWrite && (isBlank(request.idempotencyKey) || isBlank(request.recommendationDigest))) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft recommendation creation requires idempotencyKey and recommendationDigest");
    }
    if (!epic278RecommendationWrite && isBlank(request.sourceSnapshotId) && isBlank(request.sourceReplayId)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft requires sourceSnapshotId or sourceReplayId");
    }

    auto account = requireReferencedRow(connection_, "account", request.accountId, "account");
    if (!account) {
        return propagateFailure(account);
    }
    auto portfolio = requireReferencedRow(connection_, "portfolio", request.portfolioId, "portfolio");
    if (!portfolio) {
        return propagateFailure(portfolio);
    }
    auto strategy = requireReferencedRow(connection_, "strategy", request.strategyId, "strategy");
    if (!strategy) {
        return propagateFailure(strategy);
    }
    auto instrument = requireReferencedRow(connection_, "instrument", request.instrumentId, "instrument");
    if (!instrument) {
        return propagateFailure(instrument);
    }

    return DatabaseResult<bool>::success(true);
}

DatabaseResult<std::optional<std::int64_t>>
ShellAccountingTradeDraftRepository::findExistingDraft(
    const ShellAccountingTradeDraftCreateRequest& request)
{
    if (!isBlank(request.idempotencyKey)) {
        return findExistingDraftByIdempotencyKey(request);
    }
    auto rows = connection_.queryRows(
        "SELECT id FROM trade_draft "
        "WHERE account_id = ? AND portfolio_id = ? AND strategy_code = ? AND draft_signature = ? "
        "ORDER BY id DESC LIMIT 1;",
        {std::to_string(request.accountId),
         std::to_string(request.portfolioId),
         request.strategyCode,
         draftSignature(request)});
    if (!rows) {
        return DatabaseResult<std::optional<std::int64_t>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    if (rows.value().empty() || rows.value().front().empty()) {
        return DatabaseResult<std::optional<std::int64_t>>::success(std::nullopt);
    }
    return DatabaseResult<std::optional<std::int64_t>>::success(rows.value().front().front().int64Value);
}

DatabaseResult<std::optional<std::int64_t>>
ShellAccountingTradeDraftRepository::findExistingDraftByIdempotencyKey(
    const ShellAccountingTradeDraftCreateRequest& request)
{
    auto rows = connection_.queryRows(
        "SELECT id FROM trade_draft "
        "WHERE account_id = ? AND portfolio_id = ? AND draft_group_no = ? "
        "ORDER BY id DESC LIMIT 1;",
        {std::to_string(request.accountId),
         std::to_string(request.portfolioId),
         request.idempotencyKey});
    if (!rows) {
        return DatabaseResult<std::optional<std::int64_t>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    if (rows.value().empty() || rows.value().front().empty()) {
        return DatabaseResult<std::optional<std::int64_t>>::success(std::nullopt);
    }
    return DatabaseResult<std::optional<std::int64_t>>::success(rows.value().front().front().int64Value);
}

DatabaseResult<std::string> ShellAccountingTradeDraftRepository::loadDraftSignature(std::int64_t draftId)
{
    auto rows = connection_.queryRows(
        "SELECT draft_signature FROM trade_draft WHERE id = ? LIMIT 1;",
        {std::to_string(draftId)});
    if (!rows) {
        return DatabaseResult<std::string>::failure(rows.error().errorCode, rows.error().message);
    }
    if (rows.value().empty() || rows.value().front().empty()) {
        return DatabaseResult<std::string>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft summary requires existing draft");
    }
    return DatabaseResult<std::string>::success(rows.value().front().front().text);
}

DatabaseResult<std::int64_t> ShellAccountingTradeDraftRepository::insertDraft(
    const ShellAccountingTradeDraftCreateRequest& request)
{
    const auto uid = draftUid(request);
    auto inserted = connection_.executeStatement(
        "INSERT INTO trade_draft("
        "uid,account_id,portfolio_id,strategy_id,strategy_code,action_type,trade_source,"
        "trade_intent_type,trigger_reason,status,total_amount_cents,total_quantity_1e6,draft_signature,"
        "draft_group_no,market_status,created_at_utc,last_seen_at_utc,expires_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
        {SqlStatementParameter::textValue(uid),
         SqlStatementParameter::int64ValueOf(request.accountId),
         SqlStatementParameter::int64ValueOf(request.portfolioId),
         SqlStatementParameter::int64ValueOf(request.strategyId),
         SqlStatementParameter::textValue(request.strategyCode),
         SqlStatementParameter::textValue(request.side),
         SqlStatementParameter::textValue(tradeSourceForInsert(request)),
         SqlStatementParameter::textValue(intentForInsert(request)),
         SqlStatementParameter::textValue(request.reason),
         SqlStatementParameter::textValue(statusForInsert(request)),
         SqlStatementParameter::int64ValueOf(parseMoneyCents(request.amountText)),
         SqlStatementParameter::int64ValueOf(request.quantity1e6),
         SqlStatementParameter::textValue(draftSignature(request)),
         SqlStatementParameter::textValue(request.idempotencyKey),
         SqlStatementParameter::textValue("UNKNOWN"),
         SqlStatementParameter::textValue(request.createdAtUtc),
         SqlStatementParameter::textValue(request.createdAtUtc),
         SqlStatementParameter::textValue(request.expiresAtUtc)});
    if (!inserted) {
        return DatabaseResult<std::int64_t>::failure(
            inserted.error().errorCode,
            inserted.error().message);
    }
    return DatabaseResult<std::int64_t>::success(connection_.lastInsertRowId());
}

DatabaseResult<std::int64_t> ShellAccountingTradeDraftRepository::insertDraftLeg(
    const ShellAccountingTradeDraftCreateRequest& request,
    std::int64_t draftId)
{
    const auto uid = draftUid(request) + "-leg-1";
    auto inserted = connection_.executeStatement(
        "INSERT INTO trade_draft_leg("
        "uid,draft_id,strategy_code,actual_instrument_id,actual_code,fund_class,action_type,"
        "trade_source,suggested_price_1e6,suggested_quantity_1e6,suggested_amount_cents,"
        "suggested_fee_cents,tier_no,tier_name,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
        {SqlStatementParameter::textValue(uid),
         SqlStatementParameter::int64ValueOf(draftId),
         SqlStatementParameter::textValue(request.strategyCode),
         SqlStatementParameter::int64ValueOf(request.instrumentId),
         SqlStatementParameter::textValue(request.instrumentCode),
         SqlStatementParameter::textValue(request.instrumentType),
         SqlStatementParameter::textValue(request.side),
         SqlStatementParameter::textValue(tradeSourceForInsert(request)),
         SqlStatementParameter::int64ValueOf(parseScaled1e6(request.priceText)),
         SqlStatementParameter::int64ValueOf(request.quantity1e6),
         SqlStatementParameter::int64ValueOf(parseMoneyCents(request.amountText)),
         SqlStatementParameter::int64ValueOf(parseMoneyCents(request.feeEstimateText)),
         SqlStatementParameter::int64ValueOf(0),
         SqlStatementParameter::textValue(request.sourceRecommendationReason),
         SqlStatementParameter::textValue(request.netCashImpactText),
         SqlStatementParameter::textValue(request.createdAtUtc)});
    if (!inserted) {
        return DatabaseResult<std::int64_t>::failure(
            inserted.error().errorCode,
            inserted.error().message);
    }
    return DatabaseResult<std::int64_t>::success(connection_.lastInsertRowId());
}

DatabaseResult<std::int64_t> ShellAccountingTradeDraftRepository::insertAuditEvent(
    const ShellAccountingTradeDraftCreateRequest& request,
    std::int64_t draftId,
    const std::string& uid)
{
    AuditLogEntry entry;
    entry.accountId = request.accountId;
    entry.portfolioId = request.portfolioId;
    entry.entityType = "SHELL_ACCOUNTING_TRADEDRAFT";
    entry.entityId = draftId;
    entry.action = "ACCOUNTING_TRADEDRAFT_CREATE";
    entry.oldValueJson = "{}";
    entry.newValueJson = sanitizedAuditPayload(request, draftId, uid);
    entry.reason = auditReason(request);
    entry.operatorName = "dataservice";
    entry.createdAtUtc = request.createdAtUtc;

    AuditLogRepository auditLogs(connection_);
    return auditLogs.insertAuditLog(entry);
}

}  // namespace etfdt::data_access
