#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    bool ok = expect(dataServiceActionsText(root).find("accounting.tradedraft.confirm") != std::string::npos,
                     "DataService exposes confirmation action");
    ok &= expect(dataServiceActionsText(root).find("TASK-150_TRADEDRAFT_CONFIRM") != std::string::npos,
                 "authorization token present");
    ok &= expect(docs103Text(root).find("TASK-150") != std::string::npos, "docs/103 records TASK-150");
    return ok ? 0 : 1;
}
