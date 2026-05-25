#pragma once

#include "DataServiceClient/DataServiceClient.h"
#include "ShellServices/ShellDataTypes.h"

#include <string>

namespace etfdt::shell_services {

class ShellReadOnlyDataFacade final {
public:
    ShellReadOnlyDataFacade();
    ~ShellReadOnlyDataFacade();

    ShellReadOnlyDataFacade(const ShellReadOnlyDataFacade&) = delete;
    ShellReadOnlyDataFacade& operator=(const ShellReadOnlyDataFacade&) = delete;
    ShellReadOnlyDataFacade(ShellReadOnlyDataFacade&&) = delete;
    ShellReadOnlyDataFacade& operator=(ShellReadOnlyDataFacade&&) = delete;

    [[nodiscard]] ShellDataResult<bool> connect(const std::string& socketName, int timeoutMs);
    void disconnect();
    [[nodiscard]] bool isConnected() const noexcept;
    [[nodiscard]] const ShellDataConnectionState& connectionState() const noexcept;
    [[nodiscard]] const std::string& lastError() const noexcept;

    [[nodiscard]] ShellDataResult<ShellDataResponse> ping(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<ShellDataResponse> health(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<ShellDataResponse> summary(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<ShellDataResponse> listAccounts(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<ShellDataResponse> listPortfolios(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<ShellDataResponse> listInstruments(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<ShellDataResponse> listStrategies(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<ShellDataResponse> listOtc(
        const std::string& strategyCode,
        int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<ShellReadOnlyDataSnapshot> loadSnapshot(int timeoutMs = 2000);

private:
    [[nodiscard]] ShellDataResult<ShellDataResponse> convertResponse(
        const etfdt::data_service_client::DataServiceClientResult<
            etfdt::protocol::ProtocolResponse>& result);
    [[nodiscard]] ShellDataResult<ShellDataResponse> requireConnected(
        const char* operation);
    void setError(std::optional<etfdt::protocol::ErrorCode> errorCode, std::string message);
    void clearError();

    etfdt::data_service_client::DataServiceClient client_;
    ShellDataConnectionState state_;
};

}  // namespace etfdt::shell_services
