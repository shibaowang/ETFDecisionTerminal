#include "DataServiceApi/DataServiceActions.h"

#include "DataServiceApi/JsonBuilders.h"
#include "Protocol/Json.h"

#include <regex>
#include <sstream>
#include <string>
#include <utility>

namespace etfdt::data_service_api {
namespace {

template <typename T>
etfdt::protocol::ProtocolResponse errorResponse(
    const etfdt::service_runtime::ActionContext& context,
    const etfdt::data_access::DatabaseResult<T>& result)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = false;
    response.errorCode = result.error().errorCode;
    response.errorMessage = result.error().message.empty() ? "DataService query failed"
                                                           : result.error().message;
    response.payloadJson = "{}";
    return response;
}

etfdt::protocol::ProtocolResponse protocolErrorResponse(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::protocol::ErrorCode code,
    std::string message)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = false;
    response.errorCode = code;
    response.errorMessage = std::move(message);
    response.payloadJson = "{}";
    return response;
}

etfdt::protocol::ProtocolResponse successResponse(
    const etfdt::service_runtime::ActionContext& context,
    std::string payloadJson)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = true;
    response.payloadJson = std::move(payloadJson);
    return response;
}

std::string normalizedSchemaName(
    const std::vector<etfdt::data_access::SchemaVersionRecord>& versions)
{
    if (versions.empty()) {
        return {};
    }

    const std::string& migrationName = versions.back().migrationName;
    if (migrationName.find("001_initial_schema") != std::string::npos) {
        return "001_initial_schema";
    }
    return migrationName;
}

std::string jsonStringValue(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

std::string decodeJsonString(std::string_view value)
{
    std::string decoded;
    decoded.reserve(value.size());
    bool escaping = false;
    for (const char ch : value) {
        if (!escaping) {
            if (ch == '\\') {
                escaping = true;
            }
            else {
                decoded.push_back(ch);
            }
            continue;
        }

        switch (ch) {
        case '"':
        case '\\':
        case '/':
            decoded.push_back(ch);
            break;
        case 'b':
            decoded.push_back('\b');
            break;
        case 'f':
            decoded.push_back('\f');
            break;
        case 'n':
            decoded.push_back('\n');
            break;
        case 'r':
            decoded.push_back('\r');
            break;
        case 't':
            decoded.push_back('\t');
            break;
        default:
            decoded.push_back(ch);
            break;
        }
        escaping = false;
    }
    return decoded;
}

std::string trimCopy(std::string value)
{
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1U);
}

std::string extractStrategyCode(const std::string& payloadJson)
{
    static const std::regex pattern(
        R"json("strategyCode"\s*:\s*"((?:\\.|[^"\\])*)")json",
        std::regex::ECMAScript);

    std::smatch match;
    if (!std::regex_search(payloadJson, match, pattern) || match.size() < 2U) {
        return {};
    }
    return trimCopy(decodeJsonString(match[1].str()));
}

}  // namespace

etfdt::protocol::ProtocolResponse handleDataHealth(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    auto health = connection.healthCheck();
    if (!health) {
        return errorResponse(context, health);
    }

    etfdt::data_access::SchemaVersionRepository schemaVersions(connection);
    auto versions = schemaVersions.listVersions();
    if (!versions) {
        return errorResponse(context, versions);
    }

    const auto& status = health.value();
    const bool healthy = status.opened && status.walEnabled && status.foreignKeysEnabled
        && status.schemaVersionPresent && status.initialMigrationApplied
        && status.missingCoreTables.empty();

    std::ostringstream payload;
    payload << "{"
            << "\"healthy\": " << (healthy ? "true" : "false") << ','
            << "\"schemaVersion\": " << jsonStringValue(normalizedSchemaName(versions.value()))
            << ','
            << "\"walEnabled\": " << (status.walEnabled ? "true" : "false") << ','
            << "\"foreignKeysEnabled\": " << (status.foreignKeysEnabled ? "true" : "false")
            << '}';
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleDataSummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::SchemaVersionRepository schemaVersions(connection);
    etfdt::data_access::AccountRepository accounts(connection);
    etfdt::data_access::PortfolioRepository portfolios(connection);
    etfdt::data_access::InstrumentRepository instruments(connection);
    etfdt::data_access::StrategyRepository strategies(connection);
    etfdt::data_access::OtcChannelRepository otcChannels(connection);

    auto versions = schemaVersions.listVersions();
    if (!versions) {
        return errorResponse(context, versions);
    }
    auto activeAccount = accounts.getActiveAccount();
    if (!activeAccount) {
        return errorResponse(context, activeAccount);
    }
    auto activePortfolio = portfolios.getActivePortfolio();
    if (!activePortfolio) {
        return errorResponse(context, activePortfolio);
    }
    auto enabledInstruments = instruments.listEnabledInstruments();
    if (!enabledInstruments) {
        return errorResponse(context, enabledInstruments);
    }
    auto enabledStrategies = strategies.listEnabledStrategies();
    if (!enabledStrategies) {
        return errorResponse(context, enabledStrategies);
    }
    auto otcCount = otcChannels.countAll();
    if (!otcCount) {
        return errorResponse(context, otcCount);
    }

    std::ostringstream payload;
    payload << "{"
            << "\"activeAccount\": " << accountToJson(activeAccount.value()) << ','
            << "\"activePortfolio\": " << portfolioToJson(activePortfolio.value()) << ','
            << "\"enabledInstrumentCount\": " << enabledInstruments.value().size() << ','
            << "\"enabledStrategyCount\": " << enabledStrategies.value().size() << ','
            << "\"otcChannelCount\": " << otcCount.value() << ','
            << "\"schemaVersion\": " << jsonStringValue(normalizedSchemaName(versions.value()))
            << '}';
    return successResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleDataAccountsList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::AccountRepository repository(connection);
    auto records = repository.listAccounts();
    if (!records) {
        return errorResponse(context, records);
    }
    return successResponse(context, "{\"accounts\":" + accountsToJsonArray(records.value()) + "}");
}

etfdt::protocol::ProtocolResponse handleDataPortfoliosList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::PortfolioRepository repository(connection);
    auto records = repository.listPortfolios();
    if (!records) {
        return errorResponse(context, records);
    }
    return successResponse(
        context,
        "{\"portfolios\":" + portfoliosToJsonArray(records.value()) + "}");
}

etfdt::protocol::ProtocolResponse handleDataInstrumentsList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::InstrumentRepository repository(connection);
    auto records = repository.listEnabledInstruments();
    if (!records) {
        return errorResponse(context, records);
    }
    return successResponse(
        context,
        "{\"instruments\":" + instrumentsToJsonArray(records.value()) + "}");
}

etfdt::protocol::ProtocolResponse handleDataStrategiesList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::StrategyRepository repository(connection);
    auto records = repository.listEnabledStrategies();
    if (!records) {
        return errorResponse(context, records);
    }
    return successResponse(
        context,
        "{\"strategies\":" + strategiesToJsonArray(records.value()) + "}");
}

etfdt::protocol::ProtocolResponse handleDataOtcList(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    const std::string strategyCode = extractStrategyCode(context.request.payloadJson);
    if (strategyCode.empty()) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "data.otc.list requires payload.strategyCode");
    }

    etfdt::data_access::OtcChannelRepository repository(connection);
    auto records = repository.listAllByStrategyCode(strategyCode);
    if (!records) {
        return errorResponse(context, records);
    }
    return successResponse(
        context,
        "{\"strategyCode\":" + jsonStringValue(strategyCode)
            + ",\"channels\":" + otcChannelsToJsonArray(records.value()) + "}");
}

}  // namespace etfdt::data_service_api
