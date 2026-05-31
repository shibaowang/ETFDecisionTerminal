#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    const auto actions = dataServiceActionsText(sourceRoot(argc, argv));
    return expect(actions.find("ShellAccountingBrokerOrderPortModeSelector.h") != std::string::npos,
                  "DataServiceActions must include the disabled-default mode selector") &&
            expect(actions.find("defaultShellAccountingBrokerOrderPortMode()") != std::string::npos,
                   "DataServiceActions must use the disabled default mode") &&
            expect(actions.find("shellAccountingBrokerOrderPortForMode(brokerPortMode)") != std::string::npos,
                   "DataServiceActions must select the port from the disabled default mode") &&
            expect(actions.find("shellAccountingBrokerOrderPortForMode(\"sandbox\")") == std::string::npos,
                   "DataServiceActions must not select sandbox mode at runtime") &&
            expect(actions.find("shellAccountingBrokerOrderPortForMode(\"paper\")") == std::string::npos,
                   "DataServiceActions must not select paper mode at runtime") &&
            expect(actions.find("shellAccountingBrokerOrderPortForMode(\"real\")") == std::string::npos,
                   "DataServiceActions must not select real mode at runtime") &&
            expect(actions.find("brokerRuntimeMode") == std::string::npos,
                   "DataServiceActions must not read a broker runtime mode") &&
            expect(actions.find("sandboxRuntimeMode") == std::string::npos,
                   "DataServiceActions must not read a sandbox runtime mode") &&
            expect(actions.find("runtimeBrokerMode") == std::string::npos,
                   "DataServiceActions must not read an external runtime broker mode") &&
            expect(actions.find("extractJsonStringField(context.request.payloadJson, \"broker") == std::string::npos,
                   "DataServiceActions must not read broker mode from payload") &&
            expect(actions.find("getenv(") == std::string::npos,
                   "DataServiceActions must not read broker mode from environment") &&
            expect(actions.find("qgetenv(") == std::string::npos,
                   "DataServiceActions must not read broker mode from Qt environment") &&
            expect(actions.find("QSettings") == std::string::npos,
                   "DataServiceActions must not read broker mode from settings") &&
            expect(actions.find("commandLineMode") == std::string::npos,
                   "DataServiceActions must not read broker mode from CLI") &&
            expect(actions.find("credentialStore") == std::string::npos,
                   "DataServiceActions must not read broker mode from credential store") &&
            expect(actions.find("secretManager") == std::string::npos,
                   "DataServiceActions must not read broker mode from secret manager")
        ? 0
        : 1;
}
