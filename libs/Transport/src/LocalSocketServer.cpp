#include "Transport/LocalSocketServer.h"

#include <QByteArray>

#include <algorithm>
#include <utility>

namespace etfdt::transport {

LocalSocketServer::LocalSocketServer()
{
    QObject::connect(&server_, &QLocalServer::newConnection, &server_, [this]() {
        acceptPendingConnections();
    });
}

LocalSocketServer::~LocalSocketServer()
{
    close();
}

TransportResult<bool> LocalSocketServer::listen(const std::string& serverName)
{
    if (serverName.empty()) {
        return TransportResult<bool>::failure("Local socket server name is required");
    }

    close();
    serverName_ = serverName;
    const QString qServerName = QString::fromStdString(serverName_);
    QLocalServer::removeServer(qServerName);

    if (!server_.listen(qServerName)) {
        return TransportResult<bool>::failure(
            "Failed to listen on local socket: " + server_.errorString().toStdString());
    }

    return TransportResult<bool>::success(true);
}

void LocalSocketServer::close()
{
    closing_ = true;
    for (auto& client : clients_) {
        if (client->socket) {
            QObject::disconnect(client->socket.get(), nullptr, nullptr, nullptr);
            client->socket->disconnectFromServer();
            client->socket->close();
        }
    }
    clients_.clear();

    if (server_.isListening()) {
        server_.close();
    }
    if (!serverName_.empty()) {
        QLocalServer::removeServer(QString::fromStdString(serverName_));
    }
    closing_ = false;
}

bool LocalSocketServer::isListening() const noexcept
{
    return server_.isListening();
}

std::size_t LocalSocketServer::clientCount() const noexcept
{
    return clients_.size();
}

void LocalSocketServer::setMessageCallback(MessageCallback callback)
{
    onMessage_ = std::move(callback);
}

TransportResult<bool> LocalSocketServer::sendToClient(ClientId clientId, const std::string& json)
{
    auto* client = findClient(clientId);
    if (client == nullptr || !client->socket) {
        return TransportResult<bool>::failure("Local socket client not found");
    }
    if (client->socket->state() != QLocalSocket::ConnectedState) {
        return TransportResult<bool>::failure("Local socket client is not connected");
    }
    return writeFrame(*client->socket, json);
}

TransportResult<bool> LocalSocketServer::broadcast(const std::string& json)
{
    for (const auto& client : clients_) {
        if (!client->socket || client->socket->state() != QLocalSocket::ConnectedState) {
            continue;
        }
        auto sent = writeFrame(*client->socket, json);
        if (!sent) {
            return sent;
        }
    }
    return TransportResult<bool>::success(true);
}

void LocalSocketServer::acceptPendingConnections()
{
    while (server_.hasPendingConnections()) {
        auto socket = std::unique_ptr<QLocalSocket>(server_.nextPendingConnection());
        if (!socket) {
            continue;
        }
        socket->setParent(nullptr);
        QLocalSocket* rawSocket = socket.get();

        auto client = std::make_unique<ClientState>();
        client->id = nextClientId_++;
        client->socket = std::move(socket);

        QObject::connect(rawSocket, &QLocalSocket::readyRead, rawSocket, [this, rawSocket]() {
            readFromClient(rawSocket);
        });
        QObject::connect(
            rawSocket,
            &QLocalSocket::disconnected,
            rawSocket,
            [this, rawSocket]() {
                removeClient(rawSocket);
            });

        clients_.push_back(std::move(client));
    }
}

void LocalSocketServer::readFromClient(QLocalSocket* socket)
{
    auto* client = findClient(socket);
    if (client == nullptr || !client->socket) {
        return;
    }

    const QByteArray data = socket->readAll();
    auto appended = client->codec.appendData(
        reinterpret_cast<const std::uint8_t*>(data.constData()),
        static_cast<std::size_t>(data.size()));
    if (!appended) {
        socket->disconnectFromServer();
        return;
    }

    while (true) {
        auto decoded = client->codec.tryDecodeNextFrame();
        if (!decoded) {
            socket->disconnectFromServer();
            return;
        }
        if (!decoded.value().has_value()) {
            return;
        }
        if (onMessage_) {
            onMessage_(client->id, *decoded.value());
        }
    }
}

void LocalSocketServer::removeClient(QLocalSocket* socket)
{
    if (closing_) {
        return;
    }

    const auto it = std::find_if(clients_.begin(), clients_.end(), [socket](const auto& client) {
        return client->socket.get() == socket;
    });
    if (it == clients_.end()) {
        return;
    }

    auto detachedSocket = std::move((*it)->socket);
    clients_.erase(it);
    if (detachedSocket) {
        QObject::disconnect(detachedSocket.get(), nullptr, nullptr, nullptr);
        detachedSocket.release()->deleteLater();
    }
}

LocalSocketServer::ClientState* LocalSocketServer::findClient(QLocalSocket* socket) noexcept
{
    const auto it = std::find_if(clients_.begin(), clients_.end(), [socket](const auto& client) {
        return client->socket.get() == socket;
    });
    return it == clients_.end() ? nullptr : it->get();
}

LocalSocketServer::ClientState* LocalSocketServer::findClient(ClientId clientId) noexcept
{
    const auto it = std::find_if(clients_.begin(), clients_.end(), [clientId](const auto& client) {
        return client->id == clientId;
    });
    return it == clients_.end() ? nullptr : it->get();
}

TransportResult<bool> LocalSocketServer::writeFrame(QLocalSocket& socket, const std::string& json)
{
    auto frame = FrameCodec::encodeFrame(json);
    if (!frame) {
        return TransportResult<bool>::failure(frame.error().message);
    }

    const auto& bytes = frame.value();
    const QByteArray data(
        reinterpret_cast<const char*>(bytes.data()),
        static_cast<int>(bytes.size()));
    const qint64 written = socket.write(data);
    if (written != data.size()) {
        return TransportResult<bool>::failure(
            "Failed to write full local socket frame: " + socket.errorString().toStdString());
    }
    if (!socket.waitForBytesWritten(1000)) {
        return TransportResult<bool>::failure(
            "Timed out writing local socket frame: " + socket.errorString().toStdString());
    }

    return TransportResult<bool>::success(true);
}

}  // namespace etfdt::transport
