#include "ShellServices/ShellAccountingQmlRegistration.h"

#include "ShellServices/ShellAccountingIssueListModel.h"
#include "ShellServices/ShellAccountingPresenter.h"
#include "ShellServices/ShellAccountingStatusObject.h"
#include "ShellServices/ShellPositionListModel.h"

#include <QtQml/qqml.h>

namespace etfdt::shell_services {
namespace {

constexpr const char* kReadOnlyReason =
    "ShellAccounting objects are provided by ShellServices and are read-only.";

template <typename T>
bool registerReadOnlyType(
    const ShellAccountingQmlRegistrationOptions& options,
    const char* qmlName)
{
    return qmlRegisterUncreatableType<T>(
               options.uri,
               options.majorVersion,
               options.minorVersion,
               qmlName,
               kReadOnlyReason) >= 0;
}

}  // namespace

bool registerShellAccountingQmlTypes(
    const ShellAccountingQmlRegistrationOptions& options)
{
    return registerReadOnlyType<ShellAccountingPresenter>(options, "ShellAccountingPresenter") &&
           registerReadOnlyType<ShellAccountingStatusObject>(options, "ShellAccountingStatusObject") &&
           registerReadOnlyType<ShellAccountingIssueListModel>(options, "ShellAccountingIssueListModel") &&
           registerReadOnlyType<ShellPositionListModel>(options, "ShellPositionListModel");
}

}  // namespace etfdt::shell_services
