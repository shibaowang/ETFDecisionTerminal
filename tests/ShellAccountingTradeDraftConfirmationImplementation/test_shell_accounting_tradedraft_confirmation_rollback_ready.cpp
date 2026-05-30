#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    const auto docs = docs103Text(sourceRoot(argc, argv));
    bool ok = expect(docs.find("rollback") != std::string::npos, "docs/103 contains rollback");
    ok &= expect(docs.find("broker order / strategy execution / trading UI") != std::string::npos,
                 "docs/103 defers trading surfaces");
    return ok ? 0 : 1;
}
