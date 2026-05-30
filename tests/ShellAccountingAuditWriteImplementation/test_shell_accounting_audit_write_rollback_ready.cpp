#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = docs97Text(root);
    bool ok = expect(docs.find("rollback") != std::string::npos, "docs/97 includes rollback");
    ok &= expect(docs.find("disable") != std::string::npos, "docs/97 includes disable");
    ok &= expect(docs.find("must not fake audit success") != std::string::npos,
                 "docs/97 forbids fake audit success");
    ok &= expect(docs.find("must not write trade_log") != std::string::npos,
                 "docs/97 forbids trade_log fallback");
    return ok ? 0 : 1;
}
