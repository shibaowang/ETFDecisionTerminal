#pragma once

#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_dataservice_adapter_live_call_gate {

struct LiveCallAllowedWrapper final {
    std::string actionName;
    std::string wrapperMethod;
    bool readOnly = true;
};

struct LiveCallRequestMappingGate final {
    std::vector<std::string> requiredFields;
    std::vector<std::string> forbiddenFields;
};

struct LiveCallResponseMappingGate final {
    std::vector<std::string> mappedFields;
    bool implementedFalseIsDomainUnavailable = true;
    bool preserveUnknownRawPayload = true;
};

struct LiveCallErrorGate final {
    std::vector<std::string> handledStatuses;
    bool protocolErrorDistinctFromDomainIssue = true;
    bool warningVisible = true;
    bool blockingIssueVisible = true;
};

struct LiveCallReadinessGate final {
    bool requiresExplicitAuthorization = true;
    bool qmlRequiresSeparateAuthorization = true;
    bool sqliteForbidden = true;
    bool accountingEngineForbidden = true;
    bool rollbackToNotConnectedRequired = true;
    bool transportRepeatRequired = true;
};

[[nodiscard]] std::vector<LiveCallAllowedWrapper> liveCallReadOnlyAllowlist();
[[nodiscard]] std::vector<std::string> liveCallForbiddenDenylist();
[[nodiscard]] LiveCallRequestMappingGate liveCallRequestMappingGate();
[[nodiscard]] LiveCallResponseMappingGate liveCallResponseMappingGate();
[[nodiscard]] LiveCallErrorGate liveCallErrorGate();
[[nodiscard]] LiveCallReadinessGate liveCallReadinessGate();
[[nodiscard]] bool isLiveCallAllowedAction(const std::string& actionName);

}  // namespace etfdt::tests::shell_accounting_dataservice_adapter_live_call_gate
