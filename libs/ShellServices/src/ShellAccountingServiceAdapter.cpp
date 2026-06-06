#include "ShellServices/ShellAccountingServiceAdapter.h"

namespace etfdt::shell_services {

namespace {

ShellAccountingServiceResult makeUnavailableWriteResult(
    const ShellAccountingServiceRequest& request,
    const char* fallbackAction,
    const char* status)
{
    ShellAccountingServiceResult result;
    result.actionName = request.actionName.empty() ? fallbackAction : request.actionName;
    result.protocolSuccess = true;
    result.implemented = false;
    result.readOnly = false;
    result.writeEnabled = false;
    result.payloadStatus = status;
    result.dataQualityStatus = "UNAVAILABLE";
    result.domainError = true;
    result.generatedTradeDraft = false;
    result.generatedTradeSuggestion = false;
    result.strategyExecuted = false;
    result.brokerOrderSubmitted = false;
    result.issues.push_back(
        {status, "ERROR", "ShellAccounting write UI adapter path is not configured.", true,
         "ShellAccountingServiceAdapter"});
    return result;
}

ShellAccountingServiceResult makeUnavailablePreviewResult(
    const ShellAccountingServiceRequest& request,
    const char* fallbackAction,
    const char* status)
{
    ShellAccountingServiceResult result;
    result.actionName = request.actionName.empty() ? fallbackAction : request.actionName;
    result.protocolSuccess = true;
    result.implemented = false;
    result.readOnly = true;
    result.writeEnabled = false;
    result.payloadStatus = status;
    result.dataQualityStatus = "UNAVAILABLE";
    result.domainError = true;
    result.importPreviewRejected = true;
    result.issues.push_back(
        {status, "ERROR", "ShellAccounting preview adapter path is not configured.", true,
         "ShellAccountingServiceAdapter"});
    return result;
}

}  // namespace

ShellAccountingServiceResult ShellAccountingServiceAdapter::createDraft(
    const ShellAccountingServiceRequest& request)
{
    return makeUnavailableWriteResult(
        request,
        "accounting.tradedraft.create",
        "SHELL_ACCOUNTING_CREATE_DRAFT_ADAPTER_NOT_CONFIGURED");
}

ShellAccountingServiceResult ShellAccountingServiceAdapter::confirmDraft(
    const ShellAccountingServiceRequest& request)
{
    return makeUnavailableWriteResult(
        request,
        "accounting.tradedraft.confirm",
        "SHELL_ACCOUNTING_CONFIRM_DRAFT_ADAPTER_NOT_CONFIGURED");
}

ShellAccountingServiceResult ShellAccountingServiceAdapter::submitManualTransaction(
    const ShellAccountingServiceRequest& request)
{
    return makeUnavailableWriteResult(
        request,
        "accounting.manual_transaction.create",
        "SHELL_ACCOUNTING_MANUAL_TRANSACTION_ADAPTER_NOT_CONFIGURED");
}

ShellAccountingServiceResult ShellAccountingServiceAdapter::submitManualCashMovement(
    const ShellAccountingServiceRequest& request)
{
    return makeUnavailableWriteResult(
        request,
        "accounting.manual_cash_movement.create",
        "SHELL_ACCOUNTING_MANUAL_CASH_MOVEMENT_ADAPTER_NOT_CONFIGURED");
}

ShellAccountingServiceResult ShellAccountingServiceAdapter::previewExcelVbaImportReadOnly(
    const ShellAccountingServiceRequest& request)
{
    return makeUnavailablePreviewResult(
        request,
        "accounting.excel_vba_import.readonly_preview",
        "SHELL_ACCOUNTING_EXCEL_VBA_IMPORT_READONLY_PREVIEW_ADAPTER_NOT_CONFIGURED");
}

}  // namespace etfdt::shell_services
