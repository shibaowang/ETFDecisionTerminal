#include "ServiceRuntime/ServiceRuntime.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

void expectEqual(std::string_view actual, std::string_view expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

void expectErrorCode(
    const etfdt::protocol::ProtocolResponse& response,
    etfdt::protocol::ErrorCode expected,
    std::string_view message)
{
    if (!response.errorCode.has_value() || response.errorCode.value() != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

void expectContains(
    const std::string& haystack,
    std::string_view needle,
    std::string_view message)
{
    if (haystack.find(needle) == std::string::npos) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " missing " << needle << '\n';
    }
}

etfdt::protocol::MessageEnvelope validEnvelope(std::string action)
{
    etfdt::protocol::MessageEnvelope envelope;
    envelope.msgId = "runtime-msg-001";
    envelope.traceId = "runtime-trace-001";
    envelope.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    envelope.to = etfdt::protocol::ServiceName::ETFDataService;
    envelope.action = std::move(action);
    envelope.timestampUtc = "2026-05-24T12:30:00Z";
    envelope.payloadJson = "{}";
    return envelope;
}

etfdt::protocol::ProtocolResponse okResponse(
    const etfdt::service_runtime::ActionContext& context,
    std::string payloadJson = R"({"ok":true})")
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = true;
    response.payloadJson = std::move(payloadJson);
    return response;
}

etfdt::protocol::ProtocolResponse defaultOkResponse(
    const etfdt::service_runtime::ActionContext& context)
{
    return okResponse(context);
}

void testRegisterAndDispatch()
{
    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);

    expectTrue(
        !dispatcher.registerAction("", defaultOkResponse),
        "registerAction rejects empty action");
    expectTrue(
        dispatcher.registerAction("demo.ok", defaultOkResponse),
        "registerAction accepts valid action");
    expectTrue(dispatcher.hasAction("demo.ok"), "hasAction finds registered action");

    auto actions = dispatcher.registeredActions();
    expectTrue(
        std::find(actions.begin(), actions.end(), "demo.ok") != actions.end(),
        "registeredActions includes action");

    const auto request = validEnvelope("demo.ok");
    auto response = dispatcher.dispatch(request);
    expectTrue(response.success, "registered action dispatch succeeds");
    expectEqual(response.msgId, request.msgId, "response keeps msgId");
    expectEqual(response.traceId, request.traceId, "response keeps traceId");
    expectEqual(response.payloadJson, R"({"ok":true})", "handler payload preserved");
    expectTrue(
        etfdt::protocol::validateProtocolResponse(response).valid,
        "success response validates");

    expectTrue(dispatcher.unregisterAction("demo.ok"), "unregisterAction removes action");
    expectTrue(!dispatcher.hasAction("demo.ok"), "hasAction false after unregister");
}

void testUnknownAction()
{
    etfdt::service_runtime::ActionDispatcher dispatcher;
    const auto request = validEnvelope("not.registered");

    auto response = dispatcher.dispatch(request);
    expectTrue(!response.success, "unknown action fails");
    expectErrorCode(
        response,
        etfdt::protocol::ErrorCode::E1004_INVALID_ACTION,
        "unknown action returns E1004");
    expectEqual(response.msgId, request.msgId, "unknown action response keeps msgId");
    expectEqual(response.traceId, request.traceId, "unknown action response keeps traceId");
    expectTrue(!response.errorMessage.empty(), "unknown action has error message");
}

void testEnvelopeValidationFailure()
{
    etfdt::service_runtime::ActionDispatcher dispatcher;
    (void)dispatcher.registerAction("demo.ok", defaultOkResponse);

    auto missingMsgId = validEnvelope("demo.ok");
    missingMsgId.msgId.clear();
    auto response = dispatcher.dispatch(missingMsgId);
    expectTrue(!response.success, "invalid envelope fails");
    expectErrorCode(
        response,
        etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
        "invalid envelope returns protocol validation code");
}

void testHandlerException()
{
    etfdt::service_runtime::ActionDispatcher dispatcher;
    (void)dispatcher.registerAction(
        "demo.throw",
        [](const etfdt::service_runtime::ActionContext&) -> etfdt::protocol::ProtocolResponse {
            throw std::runtime_error("boom");
        });

    auto response = dispatcher.dispatch(validEnvelope("demo.throw"));
    expectTrue(!response.success, "throwing handler fails");
    expectErrorCode(
        response,
        etfdt::protocol::ErrorCode::E9000_SERVICE_ERROR,
        "throwing handler returns service error");
    expectContains(response.errorMessage, "boom", "exception message is included");
}

void testInvalidHandlerResponse()
{
    etfdt::service_runtime::ActionDispatcher dispatcher;
    (void)dispatcher.registerAction(
        "demo.bad",
        [](const etfdt::service_runtime::ActionContext& context) {
            return okResponse(context, "not-json");
        });

    auto response = dispatcher.dispatch(validEnvelope("demo.bad"));
    expectTrue(!response.success, "invalid handler response fails");
    expectErrorCode(
        response,
        etfdt::protocol::ErrorCode::E9000_SERVICE_ERROR,
        "invalid handler response returns service error");
}

void testBuiltinActions()
{
    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);

    expectTrue(
        dispatcher.hasAction(etfdt::service_runtime::kActionSystemPing),
        "system.ping registered");
    expectTrue(
        dispatcher.hasAction(etfdt::service_runtime::kActionSystemHealth),
        "system.health registered");

    auto ping = dispatcher.dispatch(validEnvelope(etfdt::service_runtime::kActionSystemPing));
    expectTrue(ping.success, "system.ping succeeds");
    expectContains(ping.payloadJson, "\"pong\": true", "ping payload contains pong");
    expectContains(
        ping.payloadJson,
        "\"service\": \"ETFDataService\"",
        "ping payload contains service");
    expectTrue(
        etfdt::protocol::validateProtocolResponse(ping).valid,
        "system.ping response validates");

    auto health = dispatcher.dispatch(validEnvelope(etfdt::service_runtime::kActionSystemHealth));
    expectTrue(health.success, "system.health succeeds");
    expectContains(health.payloadJson, "\"healthy\": true", "health payload contains healthy");
    expectContains(health.payloadJson, "\"status\": \"ok\"", "health payload contains status");
    expectContains(
        health.payloadJson,
        "\"service\": \"ETFDataService\"",
        "health payload contains service");
    expectTrue(
        etfdt::protocol::validateProtocolResponse(health).valid,
        "system.health response validates");
}

void testActionContext()
{
    const auto request = validEnvelope("demo.context");
    auto context = etfdt::service_runtime::makeActionContext(
        request,
        etfdt::protocol::ServiceName::ETFDataService,
        std::string("peer-001"),
        {{"transport", "test"}});

    expectEqual(context.traceId, request.traceId, "context traceId");
    expectTrue(context.remotePeerId.has_value(), "context remotePeerId exists");
    if (context.remotePeerId.has_value()) {
        expectEqual(*context.remotePeerId, "peer-001", "context remotePeerId value");
    }
    expectTrue(!context.receivedAtUtc.empty(), "context receivedAtUtc populated");
    expectEqual(context.metadata["transport"], "test", "context metadata");
}

}  // namespace

int main()
{
    testRegisterAndDispatch();
    testUnknownAction();
    testEnvelopeValidationFailure();
    testHandlerException();
    testInvalidHandlerResponse();
    testBuiltinActions();
    testActionContext();

    if (gFailures != 0) {
        std::cerr << gFailures << " ServiceRuntime test failure(s)" << '\n';
        return 1;
    }

    std::cout << "ServiceRuntime tests passed." << '\n';
    return 0;
}
