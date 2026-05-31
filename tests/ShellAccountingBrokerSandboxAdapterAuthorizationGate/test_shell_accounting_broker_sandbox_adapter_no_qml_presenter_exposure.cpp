#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(task160DocsText(root), {"No QML / Presenter / Shell Direct Adapter Policy"})
        && !containsAnyToken(qmlText(root), qmlPresenterExposureTokens())
        && !containsAnyToken(presenterText(root), qmlPresenterExposureTokens())
        ? 0
        : 1;
}
