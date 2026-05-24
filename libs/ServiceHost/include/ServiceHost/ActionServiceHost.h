#pragma once

#include "Protocol/Protocol.h"
#include "ServiceRuntime/ServiceRuntime.h"
#include "Transport/Transport.h"

#include <cstddef>
#include <string>

namespace etfdt::service_host {

class ActionServiceHost final {
public:
    explicit ActionServiceHost(etfdt::service_runtime::ActionDispatcher& dispatcher);
    ~ActionServiceHost();

    ActionServiceHost(const ActionServiceHost&) = delete;
    ActionServiceHost& operator=(const ActionServiceHost&) = delete;
    ActionServiceHost(ActionServiceHost&&) = delete;
    ActionServiceHost& operator=(ActionServiceHost&&) = delete;

    [[nodiscard]] etfdt::transport::TransportResult<bool> listen(
        const std::string& socketName);
    void close();
    [[nodiscard]] bool isListening() const noexcept;
    [[nodiscard]] std::size_t clientCount() const noexcept;

private:
    struct EnvelopeParseResult;

    void handleTransportMessage(
        etfdt::transport::LocalSocketServer::ClientId clientId,
        const std::string& json);

    [[nodiscard]] EnvelopeParseResult parseEnvelope(const std::string& json) const;
    [[nodiscard]] etfdt::protocol::ProtocolResponse makeParseErrorResponse(
        const EnvelopeParseResult& result) const;
    void sendResponse(
        etfdt::transport::LocalSocketServer::ClientId clientId,
        const etfdt::protocol::ProtocolResponse& response);

    etfdt::service_runtime::ActionDispatcher& dispatcher_;
    etfdt::transport::LocalSocketServer server_;
};

}  // namespace etfdt::service_host
