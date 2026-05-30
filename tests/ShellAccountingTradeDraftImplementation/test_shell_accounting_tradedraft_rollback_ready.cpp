#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    const auto docs = docs100Text(sourceRoot(argc, argv));
    bool ok = expect(docs.find("rollback") != std::string::npos, "docs/100 contains rollback");
    ok &= expect(docs.find("disable") != std::string::npos, "docs/100 contains disable strategy");
    ok &= expect(docs.find("audit failure") != std::string::npos, "docs/100 documents audit failure rollback");
    ok &= expect(docs.find("TradeDraft confirmation") != std::string::npos,
                 "docs/100 keeps confirmation as future task");
    return ok ? 0 : 1;
}
