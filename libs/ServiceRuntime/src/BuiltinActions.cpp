#include "ServiceRuntime/BuiltinActions.h"

#include "Protocol/Json.h"

#include <sstream>

namespace etfdt::service_runtime {
namespace {

etfdt::protocol::ProtocolResponse makeSuccessResponse(
    const ActionContext& context,
    std::string payloadJson)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = true;
    response.payloadJson = std::move(payloadJson);
    return response;
}

std::string jsonStringField(std::string_view key, std::string_view value, bool trailingComma)
{
    std::ostringstream stream;
    stream << '"' << key << "\": \"" << etfdt::protocol::escapeJsonString(value) << '"';
    if (trailingComma) {
        stream << ',';
    }
    return stream.str();
}

}  // namespace

etfdt::protocol::ProtocolResponse handleSystemPing(const ActionContext& context)
{
    std::ostringstream payload;
    payload << "{\n"
            << "\"pong\": true,\n"
            << jsonStringField("service", etfdt::protocol::toString(context.serviceName), false)
            << "\n}";
    return makeSuccessResponse(context, payload.str());
}

etfdt::protocol::ProtocolResponse handleSystemHealth(const ActionContext& context)
{
    std::ostringstream payload;
    payload << "{\n"
            << "\"healthy\": true,\n"
            << jsonStringField("service", etfdt::protocol::toString(context.serviceName), true)
            << '\n'
            << jsonStringField("status", "ok", true) << '\n'
            << jsonStringField("receivedAtUtc", context.receivedAtUtc, false)
            << "\n}";
    return makeSuccessResponse(context, payload.str());
}

void registerBuiltinActions(ActionDispatcher& dispatcher)
{
    (void)dispatcher.registerAction(kActionSystemPing, handleSystemPing);
    (void)dispatcher.registerAction(kActionSystemHealth, handleSystemHealth);
}

}  // namespace etfdt::service_runtime
