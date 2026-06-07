#include "ShellServices/ShellAccountingDataServiceClientPort.h"

namespace etfdt::shell_services {

namespace {

ShellAccountingDataServiceClientResponse makeUnavailablePortResponse(
    const ShellAccountingDataServiceClientRequest& request,
    const char* status)
{
    ShellAccountingDataServiceClientResponse response;
    response.actionName = request.actionName;
    response.protocolSuccess = false;
    response.implemented = false;
    response.readOnly = false;
    response.writeEnabled = false;
    response.payloadStatus = status;
    response.dataQualityStatus = "UNAVAILABLE";
    response.protocolError = true;
    response.errorMessage = "ShellAccounting draft DataService port is not configured.";
    response.issues.push_back(
        {status, "ERROR", response.errorMessage, true, "ShellAccountingDataServiceClientPort"});
    return response;
}

}  // namespace

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPort::callTradeDraftCreate(
    const ShellAccountingDataServiceClientRequest& request)
{
    return makeUnavailablePortResponse(
        request,
        "SHELL_ACCOUNTING_CREATE_DRAFT_PORT_NOT_CONFIGURED");
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPort::callTradeDraftConfirm(
    const ShellAccountingDataServiceClientRequest& request)
{
    return makeUnavailablePortResponse(
        request,
        "SHELL_ACCOUNTING_CONFIRM_DRAFT_PORT_NOT_CONFIGURED");
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPort::callManualTransactionCreate(
    const ShellAccountingDataServiceClientRequest& request)
{
    return makeUnavailablePortResponse(
        request,
        "SHELL_ACCOUNTING_MANUAL_TRANSACTION_PORT_NOT_CONFIGURED");
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPort::callManualCashMovementCreate(
    const ShellAccountingDataServiceClientRequest& request)
{
    return makeUnavailablePortResponse(
        request,
        "SHELL_ACCOUNTING_MANUAL_CASH_MOVEMENT_PORT_NOT_CONFIGURED");
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPort::callExcelVbaImportReadOnlyPreview(
    const ShellAccountingDataServiceClientRequest& request)
{
    ShellAccountingDataServiceClientResponse response;
    response.actionName = request.actionName;
    response.protocolSuccess = false;
    response.implemented = false;
    response.readOnly = true;
    response.writeEnabled = false;
    response.payloadStatus =
        "SHELL_ACCOUNTING_EXCEL_VBA_IMPORT_READONLY_PREVIEW_PORT_NOT_CONFIGURED";
    response.dataQualityStatus = "UNAVAILABLE";
    response.protocolError = true;
    response.errorMessage =
        "ShellAccounting Excel/VBA import preview DataService port is not configured.";
    response.importPreviewRejected = true;
    response.issues.push_back(
        {response.payloadStatus, "ERROR", response.errorMessage, true,
         "ShellAccountingDataServiceClientPort"});
    return response;
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPort::callExcelVbaImportPersistManualEntry(
    const ShellAccountingDataServiceClientRequest& request)
{
    return makeUnavailablePortResponse(
        request,
        "SHELL_ACCOUNTING_EXCEL_VBA_IMPORT_PERSIST_MANUAL_ENTRY_PORT_NOT_CONFIGURED");
}

}  // namespace etfdt::shell_services
