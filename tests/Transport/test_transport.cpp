#include "Protocol/Protocol.h"
#include "Transport/Transport.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QThread>

#include <algorithm>
#include <cstdint>
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

bool hasOption(const std::vector<std::string>& args, const std::string& option)
{
    return std::find(args.begin(), args.end(), option) != args.end();
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

etfdt::protocol::MessageEnvelope validEnvelope()
{
    etfdt::protocol::MessageEnvelope envelope;
    envelope.msgId = "transport-msg-001";
    envelope.traceId = "transport-trace-001";
    envelope.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    envelope.to = etfdt::protocol::ServiceName::ETFDataService;
    envelope.action = "Ping";
    envelope.timestampUtc = "2026-05-24T12:00:00Z";
    envelope.payloadJson = "{}";
    return envelope;
}

void testFrameCodec()
{
    using etfdt::transport::FrameCodec;

    const std::string json = R"({"type":"ping"})";
    auto frame = FrameCodec::encodeFrame(json);
    expectTrue(frame.hasValue(), "encodeFrame accepts JSON object");

    FrameCodec decoder;
    if (frame.hasValue()) {
        auto appended = decoder.appendData(frame.value());
        expectTrue(appended.hasValue(), "appendData accepts encoded frame");
        auto decoded = decoder.tryDecodeNextFrame();
        expectTrue(decoded.hasValue(), "decode succeeds");
        if (decoded.hasValue()) {
            expectTrue(decoded.value().has_value(), "decode has frame");
            if (decoded.value().has_value()) {
                expectEqual(*decoded.value(), json, "decoded frame matches original JSON");
            }
        }
    }

    FrameCodec halfDecoder;
    if (frame.hasValue()) {
        const auto& bytes = frame.value();
        auto firstHalf = halfDecoder.appendData(bytes.data(), 3);
        expectTrue(firstHalf.hasValue(), "appendData accepts partial prefix");
        auto partial = halfDecoder.tryDecodeNextFrame();
        expectTrue(partial.hasValue(), "partial decode succeeds");
        if (partial.hasValue()) {
            expectTrue(!partial.value().has_value(), "partial frame waits for more data");
        }

        auto secondHalf = halfDecoder.appendData(bytes.data() + 3, bytes.size() - 3);
        expectTrue(secondHalf.hasValue(), "appendData accepts remaining bytes");
        auto complete = halfDecoder.tryDecodeNextFrame();
        expectTrue(complete.hasValue(), "complete frame decodes after second half");
        if (complete.hasValue() && complete.value().has_value()) {
            expectEqual(*complete.value(), json, "partial frame decoded message");
        }
    }

    auto frameA = FrameCodec::encodeFrame(R"({"n":1})");
    auto frameB = FrameCodec::encodeFrame(R"({"n":2})");
    expectTrue(frameA.hasValue(), "encode first sticky frame");
    expectTrue(frameB.hasValue(), "encode second sticky frame");
    if (frameA.hasValue() && frameB.hasValue()) {
        std::vector<std::uint8_t> sticky = frameA.value();
        sticky.insert(sticky.end(), frameB.value().begin(), frameB.value().end());

        FrameCodec stickyDecoder;
        auto appended = stickyDecoder.appendData(sticky);
        expectTrue(appended.hasValue(), "appendData accepts sticky frames");
        auto first = stickyDecoder.tryDecodeNextFrame();
        auto second = stickyDecoder.tryDecodeNextFrame();
        expectTrue(first.hasValue() && first.value().has_value(), "first sticky frame decodes");
        expectTrue(second.hasValue() && second.value().has_value(), "second sticky frame decodes");
        if (first.hasValue() && first.value().has_value()) {
            expectEqual(*first.value(), R"({"n":1})", "first sticky payload");
        }
        if (second.hasValue() && second.value().has_value()) {
            expectEqual(*second.value(), R"({"n":2})", "second sticky payload");
        }
    }

    auto oversize = FrameCodec::encodeFrame(R"({"too":"large"})", 4);
    expectTrue(!oversize.hasValue(), "oversize payload rejects");

    auto notJson = FrameCodec::encodeFrame("not-json");
    expectTrue(!notJson.hasValue(), "non JSON-looking payload rejects");
}

void testLocalSocketEcho()
{
    using etfdt::transport::LocalSocketClient;
    using etfdt::transport::LocalSocketServer;

    const std::string serverName = "ETFDecisionTerminalTransportTest_"
        + std::to_string(QCoreApplication::applicationPid());

    LocalSocketServer server;
    LocalSocketClient client;

    std::string serverReceived;
    std::string clientReceived;

    server.setMessageCallback([&](LocalSocketServer::ClientId clientId, const std::string& json) {
        serverReceived = json;
        auto echoed = server.sendToClient(clientId, json);
        expectTrue(echoed.hasValue(), "server echoes message to client");
    });
    client.setMessageCallback([&](const std::string& json) {
        clientReceived = json;
    });

    auto listening = server.listen(serverName);
    expectTrue(listening.hasValue(), "server listen succeeds");
    expectTrue(server.isListening(), "server is listening");

    auto connected = client.connectToServer(serverName, 1000);
    expectTrue(connected.hasValue(), "client connects to server");
    expectTrue(client.isConnected(), "client is connected");

    expectTrue(
        waitUntil([&]() { return server.clientCount() == 1; }, 1000),
        "server accepts client");

    const std::string json = etfdt::protocol::toJsonString(validEnvelope());
    auto sent = client.send(json);
    expectTrue(sent.hasValue(), "client sends envelope JSON");

    expectTrue(
        waitUntil([&]() { return clientReceived == json; }, 2000),
        "client receives echo");
    expectEqual(serverReceived, json, "server received original JSON");
    expectEqual(clientReceived, json, "client received echoed JSON");

    client.disconnect();
    expectTrue(
        waitUntil([&]() { return !client.isConnected(); }, 1000),
        "client disconnects cleanly");
    server.close();
    expectTrue(!server.isListening(), "server closes cleanly");
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    if (args.empty() || hasOption(args, "--frame-codec")) {
        testFrameCodec();
    }
    if (args.empty() || hasOption(args, "--local-socket")) {
        testLocalSocketEcho();
    }

    if (gFailures != 0) {
        std::cerr << gFailures << " Transport test failure(s)" << '\n';
        return 1;
    }

    std::cout << "Transport tests passed." << '\n';
    return 0;
}
