#pragma once

#include "Protocol/Protocol.h"

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace etfdt::data_service_client {

struct DataServiceClientError final {
    std::optional<etfdt::protocol::ErrorCode> errorCode;
    std::string message;
};

template <typename T>
class DataServiceClientResult final {
public:
    static DataServiceClientResult success(T value)
    {
        return DataServiceClientResult(std::move(value));
    }

    static DataServiceClientResult failure(
        std::optional<etfdt::protocol::ErrorCode> errorCode,
        std::string message)
    {
        return DataServiceClientResult(DataServiceClientError{errorCode, std::move(message)});
    }

    [[nodiscard]] bool hasValue() const noexcept
    {
        return hasValue_;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return hasValue();
    }

    [[nodiscard]] const T& value() const
    {
        return value_;
    }

    [[nodiscard]] T& value()
    {
        return value_;
    }

    [[nodiscard]] const DataServiceClientError& error() const noexcept
    {
        return error_;
    }

private:
    explicit DataServiceClientResult(T value)
        : hasValue_(true)
        , value_(std::move(value))
    {
    }

    explicit DataServiceClientResult(DataServiceClientError error)
        : hasValue_(false)
        , error_(std::move(error))
    {
    }

    bool hasValue_ = false;
    T value_{};
    DataServiceClientError error_;
};

struct AuditAppendRequest final {
    std::string entityType;
    std::string entityId;
    std::string action;
    std::string reason;
    std::string operatorName = "local-user";
    std::string oldValueJson = "{}";
    std::string newValueJson = "{}";
};

struct ExcelVbaImportReadOnlyPreviewDiagnostic final {
    std::string level;
    std::string code;
    std::string field;
    std::string sheetName;
    std::string rowId;
};

struct ExcelVbaImportReadOnlyPreviewFactSummary final {
    int tradeFactCount = 0;
    int cashFactCount = 0;
    int marketPriceFactCount = 0;
    int fxRateFactCount = 0;
};

struct ExcelVbaImportReadOnlyPreviewResult final {
    std::string action;
    std::string task;
    std::string mode;
    bool dataServiceReadOnlyActionRegistered = false;
    bool parserBoundaryCalled = false;
    bool previewActionExecuted = false;
    bool diagnosticsBuilt = false;
    bool replayFactSummaryBuilt = false;
    bool accountingEngineCalled = false;
    bool productionRuntimeIntegrationImplemented = false;
    bool accepted = false;
    std::vector<ExcelVbaImportReadOnlyPreviewDiagnostic> diagnostics;
    std::vector<std::string> diagnosticCodes;
    ExcelVbaImportReadOnlyPreviewFactSummary replayFactSummary;
    bool parserReadOnly = false;
    bool parserNoSqliteAccess = false;
    bool parserNoWrite = false;
    bool productionWrite = false;
    bool sqliteProductionWrite = false;
    bool auditWritten = false;
    bool ledgerWritten = false;
    bool snapshotWritten = false;
    bool tradeLogWritten = false;
    bool readModelPersistentWrite = false;
    bool tradeDraftGenerated = false;
    bool strategyExecuted = false;
    bool brokerOrderSubmitted = false;
    bool networkAccess = false;
    bool credentialAccess = false;
    bool endpointAccess = false;
    bool automaticTrading = false;
    bool rawUserDataExposed = false;
};

}  // namespace etfdt::data_service_client
