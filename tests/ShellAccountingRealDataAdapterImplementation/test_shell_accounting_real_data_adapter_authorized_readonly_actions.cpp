#include "ShellAccountingRealDataAdapterImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs80 = readTextFile(docs80Path(root));
    const auto boundaryFiles = adapterBoundaryFiles(root);

    for (const auto& action : allowedReadOnlyActions()) {
        if (docs80.find(action) == std::string::npos ||
            countTokenInFiles(boundaryFiles, action) == 0) {
            std::cerr << "authorized read-only action missing from docs or adapter path: " << action << "\n";
            return 1;
        }
    }
    for (const auto& token : {
             "confirmTrade",
             "manualEntry",
             "cashAdjustment",
             "createTradeDraft",
             "brokerOrder(",
             "strategyExecute(",
             "data.audit.append",
             "trade_log write",
             "trade_draft write",
         }) {
        if (containsTokenInFiles(boundaryFiles, token)) {
            std::cerr << "adapter boundary contains forbidden write/trade token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
