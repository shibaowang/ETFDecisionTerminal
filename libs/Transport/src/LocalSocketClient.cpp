#include "Transport/LocalSocketClient.h"

#include <QByteArray>

#include <utility>

namespace etfdt::transport {

LocalSocketClient::LocalSocketClient()
{
    QObject::connect(&socket_, &QLocalSocket::readyRead, &socket_, [this]() {
        readFromSocket();
    });
}

LocalSocketClient::~LocalSocketClient()
{
    disconnect();
}

TransportResult<bool> LocalSocketClient::connectToServer(
    const std::string& serverName,
    int timeoutMs)
{
    if (serverName.empty()) {
        return TransportResult<bool>::failure("Local socket server name is required");
    }

    disconnect();
    socket_.connectToServer(QString::fromStdString(serverName));
    if (!socket_.waitForConnected(timeoutMs)) {
        return TransportResult<bool>::failure(
            "Failed to connect local socket: " + socket_.errorString().toStdString());
    }

    return TransportResult<bool>::success(true);
}

void LocalSocketClient::disconnect()
{
    if (socket_.state() == QLocalSocket::UnconnectedState) {
        codec_.reset();
        return;
    }

    socket_.disconnectFromServer();
    if (socket_.state() != QLocalSocket::UnconnectedState) {
        socket_.waitForDisconnected(100);
    }
    socket_.abort();
    codec_.reset();
}

bool LocalSocketClient::isConnected() const noexcept
{
    return socket_.state() == QLocalSocket::ConnectedState;
}

TransportResult<bool> LocalSocketClient::send(const std::string& json)
{
    if (!isConnected()) {
        return TransportResult<bool>::failure("Local socket client is not connected");
    }
    return writeFrame(json);
}

void LocalSocketClient::setMessageCallback(MessageCallback callback)
{
    onMessage_ = std::move(callback);
}

void LocalSocketClient::readFromSocket()
{
    const QByteArray data = socket_.readAll();
    auto appended = codec_.appendData(
        reinterpret_cast<const std::uint8_t*>(data.constData()),
        static_cast<std::size_t>(data.size()));
    if (!appended) {
        socket_.disconnectFromServer();
        return;
    }

    while (true) {
        auto decoded = codec_.tryDecodeNextFrame();
        if (!decoded) {
            socket_.disconnectFromServer();
            return;
        }
        if (!decoded.value().has_value()) {
            return;
        }
        if (onMessage_) {
            onMessage_(*decoded.value());
        }
    }
}

TransportResult<bool> LocalSocketClient::writeFrame(const std::string& json)
{
    auto frame = FrameCodec::encodeFrame(json);
    if (!frame) {
        return TransportResult<bool>::failure(frame.error().message);
    }

    const auto& bytes = frame.value();
    const QByteArray data(
        reinterpret_cast<const char*>(bytes.data()),
        static_cast<int>(bytes.size()));
    const qint64 written = socket_.write(data);
    if (written != data.size()) {
        return TransportResult<bool>::failure(
            "Failed to write full local socket frame: " + socket_.errorString().toStdString());
    }
    if (!socket_.waitForBytesWritten(1000)) {
        return TransportResult<bool>::failure(
            "Timed out writing local socket frame: " + socket_.errorString().toStdString());
    }

    return TransportResult<bool>::success(true);
}

}  // namespace etfdt::transport
