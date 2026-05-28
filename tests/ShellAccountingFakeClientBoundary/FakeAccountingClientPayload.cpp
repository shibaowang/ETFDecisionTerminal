#include "FakeAccountingClientPayload.h"

namespace etfdt::tests::shell_accounting_fake {

etfdt::shell_services::ShellAccountingStateSnapshot toStateSnapshot(
    const FakeAccountingClientPayload& payload)
{
    etfdt::shell_services::ShellAccountingStateSnapshot snapshot;
    snapshot.actionName = payload.actionName;
    snapshot.implemented = payload.implemented;
    snapshot.readOnly = payload.readOnly;
    snapshot.writeEnabled = payload.writeEnabled;
    snapshot.dataQualityStatus = payload.dataQualityStatus;
    snapshot.payloadStatus = payload.payloadStatus;
    snapshot.hasRows = payload.hasRows;
    snapshot.stale = payload.stale;
    snapshot.issues = payload.issues;
    return snapshot;
}

etfdt::shell_services::ShellAccountingDisplayText toDisplayText(
    const FakeAccountingClientPayload& payload)
{
    return {payload.amountRawText, payload.amountMaskedText};
}

}  // namespace etfdt::tests::shell_accounting_fake
