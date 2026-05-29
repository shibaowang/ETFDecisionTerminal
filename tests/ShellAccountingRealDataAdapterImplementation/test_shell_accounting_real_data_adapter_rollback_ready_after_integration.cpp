#include "ShellAccountingRealDataAdapterImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs78 = readTextFile(root / "docs" / "78_shell_accounting_real_data_adapter_gate.md");
    const auto docs79 = readTextFile(root / "docs" / "79_shell_accounting_real_data_adapter_test_plan.md");
    const auto docs80 = readTextFile(docs80Path(root));

    for (const auto& text : {docs78, docs79, docs80}) {
        if (text.find("rollback") == std::string::npos ||
            text.find("must not fake data") == std::string::npos ||
            text.find("must not write DB") == std::string::npos ||
            text.find("must not generate TradeDraft") == std::string::npos ||
            text.find("must not bypass DataService with SQLite direct reads") == std::string::npos) {
            std::cerr << "rollback docs must keep unavailable fallback and no direct SQLite bypass policy\n";
            return 1;
        }
    }
    return 0;
}
