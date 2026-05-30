#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    const auto docs = docs103Text(sourceRoot(argc, argv));
    bool ok = expect(docs.find("rollback") != std::string::npos, "docs/103 contains rollback");
    ok &= expect(docs.find("broker order / strategy execution") != std::string::npos,
                 "docs/103 defers broker and strategy surfaces");
    ok &= expect(docs.find("TASK-152") != std::string::npos,
                 "docs/103 records authorized production trading UI");
    return ok ? 0 : 1;
}
