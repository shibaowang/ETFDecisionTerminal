#include "Protocol/Protocol.h"
#include "ServiceHost/ServiceHost.h"
#include "Transport/Transport.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

#include <algorithm>
#include <functional>
#include <iostream>
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

bool waitUntil(const std::function<bool()>& predicate, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        if (predicate()) {
            return true;
        }
        QThread::msleep(5);
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    return predicate();
}

etfdt::protocol::MessageEnvelope envelope(std::string action)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "service-host-msg-" + action;
    request.traceId = "service-host-trace-" + action;
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-05-24T13:00:00Z";
    request.payloadJson = "{}";
    return request;
}

QJsonObject parseJsonObject(const std::string& json)
{
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json));
    if (!document.isObject()) {
        ++gFailures;
        std::cerr << "FAILED: response is not a JSON object: " << json << '\n';
        return {};
    }
    return document.object();
}

bool sendAndWait(
    etfdt::transport::LocalSocketClient& client,
    const std::string& json,
    std::vector<std::string>& responses)
{
    const auto before = responses.size();
    auto sent = client.send(json);
    expectTrue(sent.hasValue(), "client sends request frame");
    if (!sent) {
        return false;
    }
    return waitUntil([&]() { return responses.size() > before; }, 2000);
}

void testDemoServiceHost()
{
    const std::string socketName = "ETFDecisionTerminalServiceHostTest_"
        + std::to_string(QCoreApplication::applicationPid());

    etfdt::service_host::DemoServiceHost host;
    etfdt::transport::LocalSocketClient client;
    std::vector<std::string> responses;

    client.setMessageCallback([&](const std::string& json) {
        responses.push_back(json);
    });

    auto listenResult = host.listen(socketName);
    expectTrue(listenResult.hasValue(), "DemoServiceHost listen succeeds");
    expectTrue(host.isListening(), "DemoServiceHost is listening");

    auto connected = client.connectToServer(socketName, 1000);
    expectTrue(connected.hasValue(), "LocalSocketClient connects to DemoServiceHost");
    expectTrue(client.isConnected(), "LocalSocketClient is connected");
    expectTrue(
        waitUntil([&]() { return host.clientCount() == 1; }, 1000),
        "DemoServiceHost accepts client");

    const auto pingRequest = envelope(etfdt::service_runtime::kActionSystemPing);
    const bool pingReceived =
        sendAndWait(client, etfdt::protocol::toJsonString(pingRequest), responses);
    expectTrue(pingReceived, "system.ping response received");
    if (!pingReceived) {
        client.disconnect();
        host.close();
        return;
    }
    auto ping = parseJsonObject(responses.back());
    expectTrue(ping.value("success").toBool(false), "system.ping success=true");
    expectEqual(
        ping.value("traceId").toString().toStdString(),
        pingRequest.traceId,
        "system.ping traceId matches");
    const auto pingPayload = ping.value("payload").toObject();
    expectTrue(pingPayload.value("pong").toBool(false), "system.ping payload pong=true");

    const auto healthRequest = envelope(etfdt::service_runtime::kActionSystemHealth);
    const bool healthReceived =
        sendAndWait(client, etfdt::protocol::toJsonString(healthRequest), responses);
    expectTrue(healthReceived, "system.health response received");
    if (!healthReceived) {
        client.disconnect();
        host.close();
        return;
    }
    auto health = parseJsonObject(responses.back());
    expectTrue(health.value("success").toBool(false), "system.health success=true");
    expectEqual(
        health.value("traceId").toString().toStdString(),
        healthRequest.traceId,
        "system.health traceId matches");
    const auto healthPayload = health.value("payload").toObject();
    expectTrue(healthPayload.value("healthy").toBool(false), "system.health healthy=true");

    const auto unknownRequest = envelope("demo.unknown");
    const bool unknownReceived =
        sendAndWait(client, etfdt::protocol::toJsonString(unknownRequest), responses);
    expectTrue(unknownReceived, "unknown action response received");
    if (!unknownReceived) {
        client.disconnect();
        host.close();
        return;
    }
    auto unknown = parseJsonObject(responses.back());
    expectTrue(!unknown.value("success").toBool(true), "unknown action success=false");
    expectEqual(
        unknown.value("errorCode").toString().toStdString(),
        "E1004_INVALID_ACTION",
        "unknown action returns E1004");

    const bool invalidReceived = sendAndWait(client, "{ invalid }", responses);
    expectTrue(invalidReceived, "invalid JSON response received");
    if (!invalidReceived) {
        client.disconnect();
        host.close();
        return;
    }
    auto invalid = parseJsonObject(responses.back());
    expectTrue(!invalid.value("success").toBool(true), "invalid JSON success=false");
    const std::string invalidError = invalid.value("errorCode").toString().toStdString();
    expectTrue(
        invalidError == "E1001_INVALID_JSON" || invalidError == "E1002_MISSING_REQUIRED_FIELD",
        "invalid JSON returns protocol error");

    client.disconnect();
    expectTrue(waitUntil([&]() { return !client.isConnected(); }, 1000), "client disconnects");
    host.close();
    expectTrue(!host.isListening(), "DemoServiceHost closes");
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    testDemoServiceHost();

    if (gFailures != 0) {
        std::cerr << gFailures << " ServiceHost test failure(s)" << '\n';
        return 1;
    }

    std::cout << "ServiceHost tests passed." << '\n';
    return 0;
}
