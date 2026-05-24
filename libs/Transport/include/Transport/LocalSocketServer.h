#pragma once

#include "Transport/FrameCodec.h"

#include <QLocalServer>
#include <QLocalSocket>

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace etfdt::transport {

class LocalSocketServer final {
public:
    using ClientId = std::size_t;
    using MessageCallback = std::function<void(ClientId, const std::string&)>;

    LocalSocketServer();
    ~LocalSocketServer();

    LocalSocketServer(const LocalSocketServer&) = delete;
    LocalSocketServer& operator=(const LocalSocketServer&) = delete;
    LocalSocketServer(LocalSocketServer&&) = delete;
    LocalSocketServer& operator=(LocalSocketServer&&) = delete;

    [[nodiscard]] TransportResult<bool> listen(const std::string& serverName);
    void close();
    [[nodiscard]] bool isListening() const noexcept;
    [[nodiscard]] std::size_t clientCount() const noexcept;

    void setMessageCallback(MessageCallback callback);

    [[nodiscard]] TransportResult<bool> sendToClient(ClientId clientId, const std::string& json);
    [[nodiscard]] TransportResult<bool> broadcast(const std::string& json);

private:
    struct ClientState final {
        ClientId id = 0;
        std::unique_ptr<QLocalSocket> socket;
        FrameCodec codec;
    };

    void acceptPendingConnections();
    void readFromClient(QLocalSocket* socket);
    void removeClient(QLocalSocket* socket);
    [[nodiscard]] ClientState* findClient(QLocalSocket* socket) noexcept;
    [[nodiscard]] ClientState* findClient(ClientId clientId) noexcept;
    [[nodiscard]] TransportResult<bool> writeFrame(QLocalSocket& socket, const std::string& json);

    QLocalServer server_;
    std::string serverName_;
    std::vector<std::unique_ptr<ClientState>> clients_;
    MessageCallback onMessage_;
    ClientId nextClientId_ = 1;
    bool closing_ = false;
};

}  // namespace etfdt::transport
