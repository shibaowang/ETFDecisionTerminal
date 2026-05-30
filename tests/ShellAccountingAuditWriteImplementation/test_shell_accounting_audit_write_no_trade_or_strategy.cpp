#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto text = auditWriteRepositoryText(root);
    return containsAnyToken(text, {
        "brokerOrder(",
        "placeOrder",
        "\"strategyExecute\"",
        "strategyExecute(",
        "confirmTrade",
        "order confirmation",
        "trading action",
    }) ? 1 : 0;
}
