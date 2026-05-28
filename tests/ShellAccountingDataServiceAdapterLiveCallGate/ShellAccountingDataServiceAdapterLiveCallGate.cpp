#include "ShellAccountingDataServiceAdapterLiveCallGate.h"

#include <algorithm>

namespace etfdt::tests::shell_accounting_dataservice_adapter_live_call_gate {

std::vector<LiveCallAllowedWrapper> liveCallReadOnlyAllowlist()
{
    return {
        {"position.list", "DataServiceClient::positionList", true},
        {"cash.summary", "DataServiceClient::cashSummary", true},
        {"portfolio.pnl.summary", "DataServiceClient::portfolioPnlSummary", true},
        {"base_position.summary", "DataServiceClient::basePositionSummary", true},
        {"sniper_pool.summary", "DataServiceClient::sniperPoolSummary", true},
    };
}

std::vector<std::string> liveCallForbiddenDenylist()
{
    return {
        "data.audit.append",
        "trade.write",
        "trade_execution_group.write",
        "trade_draft.create",
        "trade_draft.update",
        "trade_draft.send",
        "cash_snapshot.write",
        "position_snapshot.write",
        "portfolio_summary.write",
        "snapshot.write",
        "strategy.execute",
        "broker.order.submit",
        "writeEnabled=true",
        "QML direct call",
        "SQLite direct access",
        "AccountingEngine direct call",
    };
}

LiveCallRequestMappingGate liveCallRequestMappingGate()
{
    return {
        {
            "accountId",
            "portfolioId",
            "sourceFromTime",
            "sourceToTime",
            "includeIssues",
            "includeDebugMetadata",
            "calculationVersion",
            "requestedOutputs",
            "timeoutMs",
        },
        {
            "writeEnabled=true",
            "TradeDraft",
            "tradeSuggestion",
            "strategyExecution",
            "brokerOrder",
        },
    };
}

LiveCallResponseMappingGate liveCallResponseMappingGate()
{
    return {
        {
            "ProtocolResponse.success -> protocolSuccess",
            "payload.implemented -> implemented",
            "payload.readOnly -> readOnly",
            "payload.writeEnabled -> writeEnabled",
            "payload.status -> payloadStatus",
            "payload.dataQualityStatus -> dataQualityStatus",
            "payload.issues -> issues",
            "payload.warnings -> warnings",
            "payload.errors -> errors",
            "raw payload -> rawPayload",
        },
        true,
        true,
    };
}

LiveCallErrorGate liveCallErrorGate()
{
    return {
        {
            "malformed JSON",
            "ProtocolResponse.success=false",
            "timeout",
            "transport failure",
            "server unavailable",
            "implemented=false",
            "*_NOT_AVAILABLE",
            "MARKET_PRICE_MISSING",
            "MULTI_CURRENCY_UNSUPPORTED",
            "FX_RATE_MISSING",
            "REPLAY_NOT_AVAILABLE",
            "unknown issue",
        },
        true,
        true,
        true,
    };
}

LiveCallReadinessGate liveCallReadinessGate()
{
    return {};
}

bool isLiveCallAllowedAction(const std::string& actionName)
{
    const auto allowlist = liveCallReadOnlyAllowlist();
    return std::any_of(
        allowlist.begin(),
        allowlist.end(),
        [&](const LiveCallAllowedWrapper& wrapper) {
            return wrapper.actionName == actionName;
        });
}

}  // namespace etfdt::tests::shell_accounting_dataservice_adapter_live_call_gate
