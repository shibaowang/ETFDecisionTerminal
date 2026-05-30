#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto source = dataServiceActionsText(root);
    const auto docs = docs86Text(root);
    int failures = 0;
    failures += expect(docs.find("TASK-139") != std::string::npos, "docs/86 records TASK-139") ? 0 : 1;
    failures += expect(docs.find("read-only replay") != std::string::npos, "docs/86 documents read-only replay") ? 0 : 1;
    failures += expect(docs.find("snapshot rebuild still not implemented") != std::string::npos, "docs/86 forbids snapshot rebuild") ? 0 : 1;
    failures += expect(source.find("AccountingReplayEngine engine") != std::string::npos, "DataService creates replay engine internally") ? 0 : 1;
    failures += expect(source.find("calculationMode") != std::string::npos, "replay mode is explicit and static-scannable") ? 0 : 1;
    return failures == 0 ? 0 : 1;
}
