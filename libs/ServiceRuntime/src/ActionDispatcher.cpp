#include "ServiceRuntime/ActionDispatcher.h"

#include <exception>
#include <utility>

namespace etfdt::service_runtime {
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

}  // namespace

ActionDispatcher::ActionDispatcher(etfdt::protocol::ServiceName serviceName)
    : serviceName_(serviceName)
{
}

etfdt::protocol::ServiceName ActionDispatcher::serviceName() const noexcept
{
    return serviceName_;
}

bool ActionDispatcher::registerAction(std::string actionName, ActionHandler handler)
{
    if (isBlank(actionName) || !handler) {
        return false;
    }
    handlers_[std::move(actionName)] = std::move(handler);
    return true;
}

bool ActionDispatcher::unregisterAction(const std::string& actionName)
{
    return handlers_.erase(actionName) != 0U;
}

bool ActionDispatcher::hasAction(const std::string& actionName) const
{
    return handlers_.find(actionName) != handlers_.end();
}

std::vector<std::string> ActionDispatcher::registeredActions() const
{
    std::vector<std::string> actions;
    actions.reserve(handlers_.size());
    for (const auto& [actionName, handler] : handlers_) {
        (void)handler;
        actions.push_back(actionName);
    }
    return actions;
}

etfdt::protocol::ProtocolResponse ActionDispatcher::dispatch(
    const etfdt::protocol::MessageEnvelope& request) const
{
    const auto validation = etfdt::protocol::validateMessageEnvelope(request);
    if (!validation.valid) {
        const auto code = validation.error.has_value()
            ? validation.error->code
            : etfdt::protocol::ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR;
        const auto message = validation.error.has_value()
            ? validation.error->message
            : std::string("MessageEnvelope validation failed");
        return makeErrorResponse(request, code, message);
    }

    const auto handlerIt = handlers_.find(request.action);
    if (handlerIt == handlers_.end()) {
        return makeErrorResponse(
            request,
            etfdt::protocol::ErrorCode::E1004_INVALID_ACTION,
            "Unsupported action: " + request.action);
    }

    try {
        auto response = handlerIt->second(makeActionContext(request, serviceName_));
        return normalizeHandlerResponse(request, std::move(response));
    }
    catch (const std::exception& ex) {
        return makeErrorResponse(
            request,
            etfdt::protocol::ErrorCode::E9000_SERVICE_ERROR,
            std::string("Action handler failed: ") + ex.what());
    }
    catch (...) {
        return makeErrorResponse(
            request,
            etfdt::protocol::ErrorCode::E9000_SERVICE_ERROR,
            "Action handler failed with unknown exception");
    }
}

etfdt::protocol::ProtocolResponse ActionDispatcher::makeErrorResponse(
    const etfdt::protocol::MessageEnvelope& request,
    etfdt::protocol::ErrorCode errorCode,
    const std::string& errorMessage) const
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = request.msgId;
    response.traceId = request.traceId;
    response.success = false;
    response.errorCode = errorCode;
    response.errorMessage = errorMessage.empty() ? "ServiceRuntime error" : errorMessage;
    response.payloadJson = "{}";
    return response;
}

etfdt::protocol::ProtocolResponse ActionDispatcher::normalizeHandlerResponse(
    const etfdt::protocol::MessageEnvelope& request,
    etfdt::protocol::ProtocolResponse response) const
{
    response.msgId = request.msgId;
    response.traceId = request.traceId;

    const auto validation = etfdt::protocol::validateProtocolResponse(response);
    if (validation.valid) {
        return response;
    }

    const std::string message = validation.error.has_value()
        ? validation.error->message
        : std::string("Action handler returned invalid ProtocolResponse");
    return makeErrorResponse(
        request,
        etfdt::protocol::ErrorCode::E9000_SERVICE_ERROR,
        "Action handler returned invalid response: " + message);
}

}  // namespace etfdt::service_runtime
