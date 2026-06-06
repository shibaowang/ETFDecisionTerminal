#pragma once

#include "DataServiceClient/DataServiceClientTypes.h"
#include "Protocol/Protocol.h"
#include "Transport/Transport.h"

#include <optional>
#include <string>

namespace etfdt::data_service_client {

class DataServiceClient final {
public:
    explicit DataServiceClient(
        etfdt::protocol::ServiceName from = etfdt::protocol::ServiceName::ETFDecisionShell,
        etfdt::protocol::ServiceName to = etfdt::protocol::ServiceName::ETFDataService);
    ~DataServiceClient();

    DataServiceClient(const DataServiceClient&) = delete;
    DataServiceClient& operator=(const DataServiceClient&) = delete;
    DataServiceClient(DataServiceClient&&) = delete;
    DataServiceClient& operator=(DataServiceClient&&) = delete;

    [[nodiscard]] DataServiceClientResult<bool> connect(
        const std::string& socketName,
        int timeoutMs);
    void disconnect();
    [[nodiscard]] bool isConnected() const noexcept;

    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> sendRaw(
        etfdt::protocol::MessageEnvelope request,
        int timeoutMs);

    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> ping(
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> health(
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> summary(
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> listAccounts(
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> listPortfolios(
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> listInstruments(
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> listStrategies(
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> listOtc(
        const std::string& strategyCode,
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> accountingHealth(
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> accountingReplayPreview(
        const std::string& payloadJson = "{}",
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<ExcelVbaImportReadOnlyPreviewResult>
    accountingExcelVbaImportReadOnlyPreview(
        const std::string& payloadJson = "{}",
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> positionList(
        const std::string& payloadJson = "{}",
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> cashSummary(
        const std::string& payloadJson = "{}",
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> portfolioPnlSummary(
        const std::string& payloadJson = "{}",
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> basePositionSummary(
        const std::string& payloadJson = "{}",
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> sniperPoolSummary(
        const std::string& payloadJson = "{}",
        int timeoutMs = 2000);
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> appendAuditDemo(
        const AuditAppendRequest& request,
        int timeoutMs = 2000);

    [[nodiscard]] etfdt::protocol::MessageEnvelope makeRequest(
        const std::string& action,
        const std::string& payloadJson = "{}") const;

private:
    [[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> sendAction(
        const std::string& action,
        const std::string& payloadJson,
        int timeoutMs);

    etfdt::protocol::ServiceName from_;
    etfdt::protocol::ServiceName to_;
    etfdt::transport::LocalSocketClient socket_;
    std::optional<std::string> lastResponseJson_;
};

}  // namespace etfdt::data_service_client
