#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    const auto docs = docs109Text(sourceRoot(argc, argv));
    bool ok = expect(docs.find("rollback / disable strategy") != std::string::npos, "rollback section");
    ok &= expect(docs.find("disable dry-run") != std::string::npos, "disable dry-run policy");
    ok &= expect(docs.find("TradeDraft create / confirm remain unaffected") != std::string::npos,
                 "fallback keeps draft features");
    return ok ? 0 : 1;
}
