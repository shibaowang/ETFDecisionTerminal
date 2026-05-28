#include "ShellAccountingDataServiceClientPortAdapterTestUtils.h"

#include "Protocol/Protocol.h"

#include <QByteArray>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

#include <chrono>
#include <algorithm>
#include <functional>
#include <sstream>
#include <utility>

namespace etfdt::shell_services::tests {
namespace {

std::string nextSocketName()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    return "ETFDecisionTerminalShellAccountingPortAdapterTest_"
        + std::to_string(QCoreApplication::applicationPid()) + "_" + std::to_string(now);
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

std::string statusForAction(const std::string& action)
{
    if (action == "position.list") {
        return "POSITION_LIST_NOT_AVAILABLE";
    }
    if (action == "cash.summary") {
        return "CASH_SUMMARY_NOT_AVAILABLE";
    }
    if (action == "portfolio.pnl.summary") {
        return "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE";
    }
    if (action == "base_position.summary") {
        return "BASE_POSITION_SUMMARY_NOT_AVAILABLE";
    }
    if (action == "sniper_pool.summary") {
        return "SNIPER_POOL_SUMMARY_NOT_AVAILABLE";
    }
    return "UNSUPPORTED_ACCOUNTING_ACTION";
}

std::string guardPayload(const std::string& action)
{
    const auto status = statusForAction(action);
    std::ostringstream payload;
    payload << "{"
            << "\"module\":\"accounting\","
            << "\"action\":\"" << action << "\","
            << "\"implemented\":false,"
            << "\"readOnly\":true,"
            << "\"writeEnabled\":false,"
            << "\"status\":\"" << status << "\","
            << "\"dataQualityStatus\":\"UNAVAILABLE\","
            << "\"issues\":[{"
            << "\"level\":\"ERROR\","
            << "\"code\":\"" << status << "\","
            << "\"message\":\"guard payload\","
            << "\"blocking\":true"
            << "}],"
            << "\"warnings\":[\"GUARD_WRAPPER_NOT_IMPLEMENTED\"],"
            << "\"errors\":[{"
            << "\"level\":\"ERROR\","
            << "\"code\":\"" << status << "\","
            << "\"message\":\"guard payload\","
            << "\"blocking\":true"
            << "}],"
            << "\"unknownField\":\"preserved\""
            << "}";
    return payload.str();
}

std::string stringField(const QJsonObject& object, const QString& key)
{
    const auto value = object.value(key);
    return value.isString() ? value.toString().toStdString() : std::string{};
}

std::string compactJson(const QJsonValue& value)
{
    if (value.isObject()) {
        return QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact).toStdString();
    }
    if (value.isArray()) {
        return QJsonDocument(value.toArray()).toJson(QJsonDocument::Compact).toStdString();
    }
    return {};
}

}  // namespace

FakeAccountingWrapperServer::FakeAccountingWrapperServer(ResponseMode mode)
    : mode_(mode)
    , socketName_(nextSocketName())
{
    QObject::connect(&server_, &QLocalServer::newConnection, &server_, [this]() {
        acceptPendingConnections();
    });
}

FakeAccountingWrapperServer::~FakeAccountingWrapperServer()
{
    if (clientSocket_) {
        QObject::disconnect(clientSocket_.get(), nullptr, nullptr, nullptr);
        clientSocket_->disconnectFromServer();
        clientSocket_->close();
        clientSocket_.reset();
    }
    if (server_.isListening()) {
        server_.close();
    }
    if (!socketName_.empty()) {
        QLocalServer::removeServer(QString::fromStdString(socketName_));
    }
}

bool FakeAccountingWrapperServer::listen()
{
    QLocalServer::removeServer(QString::fromStdString(socketName_));
    return server_.listen(QString::fromStdString(socketName_));
}

bool FakeAccountingWrapperServer::connectClient(
    etfdt::data_service_client::DataServiceClient& client)
{
    const auto connected = client.connect(socketName_, 1000);
    return connected.hasValue()
        && waitUntil([this]() {
               return clientSocket_ && clientSocket_->state() == QLocalSocket::ConnectedState;
           },
           1000);
}

const std::string& FakeAccountingWrapperServer::socketName() const noexcept
{
    return socketName_;
}

const std::vector<std::string>& FakeAccountingWrapperServer::actions() const noexcept
{
    return actions_;
}

const std::vector<std::string>& FakeAccountingWrapperServer::payloads() const noexcept
{
    return payloads_;
}

bool FakeAccountingWrapperServer::waitForActionCount(std::size_t expectedCount, int timeoutMs)
{
    return waitUntil([&]() { return actions_.size() >= expectedCount; }, timeoutMs);
}

void FakeAccountingWrapperServer::handleMessage(
    const std::string& json)
{
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json));
    if (!document.isObject()) {
        return;
    }
    const auto object = document.object();
    const auto action = stringField(object, "action");
    actions_.push_back(action);
    payloads_.push_back(compactJson(object.value("payload")));

    if (mode_ == ResponseMode::NoResponse) {
        return;
    }
    if (mode_ == ResponseMode::MalformedResponse) {
        sendResponse(R"({"not":"a protocol response"})");
        return;
    }

    etfdt::protocol::ProtocolResponse response;
    response.msgId = stringField(object, "msgId");
    response.traceId = stringField(object, "traceId");
    response.success = mode_ != ResponseMode::ProtocolFailure;
    response.errorMessage = response.success ? "" : "fake protocol failure";
    response.payloadJson = response.success ? guardPayload(action) : R"({"error":"fake"})";
    if (!response.success) {
        response.errorCode = etfdt::protocol::ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR;
    }
    const auto responseJson = etfdt::protocol::toJsonString(response);
    sendResponse(responseJson);
}

void FakeAccountingWrapperServer::acceptPendingConnections()
{
    while (server_.hasPendingConnections()) {
        auto* socket = server_.nextPendingConnection();
        if (socket == nullptr) {
            continue;
        }

        clientSocket_.reset(socket);
        clientSocket_->setParent(nullptr);
        codec_.reset();
        QObject::connect(clientSocket_.get(), &QLocalSocket::readyRead, clientSocket_.get(), [this]() {
            readFromClient();
        });
    }
}

void FakeAccountingWrapperServer::readFromClient()
{
    if (!clientSocket_) {
        return;
    }

    const QByteArray data = clientSocket_->readAll();
    auto appended = codec_.appendData(
        reinterpret_cast<const std::uint8_t*>(data.constData()),
        static_cast<std::size_t>(data.size()));
    if (!appended) {
        clientSocket_->disconnectFromServer();
        return;
    }

    while (true) {
        auto decoded = codec_.tryDecodeNextFrame();
        if (!decoded) {
            clientSocket_->disconnectFromServer();
            return;
        }
        if (!decoded.value().has_value()) {
            return;
        }
        handleMessage(*decoded.value());
    }
}

void FakeAccountingWrapperServer::sendResponse(const std::string& json)
{
    if (!clientSocket_ || clientSocket_->state() != QLocalSocket::ConnectedState) {
        return;
    }

    auto frame = etfdt::transport::FrameCodec::encodeFrame(json);
    if (!frame) {
        return;
    }
    const auto& bytes = frame.value();
    const QByteArray data(
        reinterpret_cast<const char*>(bytes.data()),
        static_cast<int>(bytes.size()));
    static_cast<void>(clientSocket_->write(data));
    static_cast<void>(clientSocket_->flush());
}

ShellAccountingDataServiceClientRequest makePortAdapterRequest(
    std::string actionName,
    std::string payloadJson)
{
    ShellAccountingDataServiceClientRequest request;
    request.actionName = std::move(actionName);
    request.payloadJson = std::move(payloadJson);
    request.timeoutMs = 200;
    return request;
}

std::shared_ptr<etfdt::data_service_client::DataServiceClient> makeConnectedClient(
    FakeAccountingWrapperServer& server)
{
    auto client = std::make_shared<etfdt::data_service_client::DataServiceClient>();
    if (!server.connectClient(*client)) {
        return {};
    }
    return client;
}

bool containsIssueCode(
    const ShellAccountingDataServiceClientResponse& response,
    const std::string& code)
{
    const auto matches = [&](const ShellAccountingIssue& issue) { return issue.code == code; };
    return std::any_of(response.issues.begin(), response.issues.end(), matches)
        || std::any_of(response.warnings.begin(), response.warnings.end(), matches)
        || std::any_of(response.errors.begin(), response.errors.end(), matches);
}

}  // namespace etfdt::shell_services::tests
