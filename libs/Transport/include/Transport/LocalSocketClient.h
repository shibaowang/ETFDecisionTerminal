#pragma once

#include "Transport/FrameCodec.h"

#include <QLocalSocket>

#include <functional>
#include <string>

namespace etfdt::transport {

class LocalSocketClient final {
public:
    using MessageCallback = std::function<void(const std::string&)>;

    LocalSocketClient();
    ~LocalSocketClient();

    LocalSocketClient(const LocalSocketClient&) = delete;
    LocalSocketClient& operator=(const LocalSocketClient&) = delete;
    LocalSocketClient(LocalSocketClient&&) = delete;
    LocalSocketClient& operator=(LocalSocketClient&&) = delete;

    [[nodiscard]] TransportResult<bool> connectToServer(
        const std::string& serverName,
        int timeoutMs);
    void disconnect();
    [[nodiscard]] bool isConnected() const noexcept;

    [[nodiscard]] TransportResult<bool> send(const std::string& json);
    void setMessageCallback(MessageCallback callback);

private:
    void readFromSocket();
    [[nodiscard]] TransportResult<bool> writeFrame(const std::string& json);

    QLocalSocket socket_;
    FrameCodec codec_;
    MessageCallback onMessage_;
};

}  // namespace etfdt::transport
