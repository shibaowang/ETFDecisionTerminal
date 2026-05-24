#pragma once

#include "ServiceRuntime/ActionContext.h"

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace etfdt::service_runtime {

using ActionHandler =
    std::function<etfdt::protocol::ProtocolResponse(const ActionContext&)>;

class ActionDispatcher final {
public:
    explicit ActionDispatcher(
        etfdt::protocol::ServiceName serviceName =
            etfdt::protocol::ServiceName::ETFDataService);

    [[nodiscard]] etfdt::protocol::ServiceName serviceName() const noexcept;

    [[nodiscard]] bool registerAction(std::string actionName, ActionHandler handler);
    [[nodiscard]] bool unregisterAction(const std::string& actionName);
    [[nodiscard]] bool hasAction(const std::string& actionName) const;
    [[nodiscard]] std::vector<std::string> registeredActions() const;

    [[nodiscard]] etfdt::protocol::ProtocolResponse dispatch(
        const etfdt::protocol::MessageEnvelope& request) const;

private:
    [[nodiscard]] etfdt::protocol::ProtocolResponse makeErrorResponse(
        const etfdt::protocol::MessageEnvelope& request,
        etfdt::protocol::ErrorCode errorCode,
        const std::string& errorMessage) const;

    [[nodiscard]] etfdt::protocol::ProtocolResponse normalizeHandlerResponse(
        const etfdt::protocol::MessageEnvelope& request,
        etfdt::protocol::ProtocolResponse response) const;

    etfdt::protocol::ServiceName serviceName_;
    std::map<std::string, ActionHandler> handlers_;
};

}  // namespace etfdt::service_runtime
