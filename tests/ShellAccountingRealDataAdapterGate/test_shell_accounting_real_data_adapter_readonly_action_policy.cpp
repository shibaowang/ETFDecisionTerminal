#include "ShellAccountingRealDataAdapterGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = readTextFile(docs78Path(root)) + "\n" + readTextFile(docs79Path(root));
    for (const auto& token : {
             "position.list",
             "cash.summary",
             "portfolio.pnl.summary",
             "base_position.summary",
             "sniper_pool.summary",
             "trade_log write",
             "trade_execution_group write",
             "trade_draft write",
             "cash_snapshot write",
             "position_snapshot write",
             "portfolio_summary write",
             "audit_log write",
             "data.audit.append",
             "confirmTrade",
             "manualEntry",
             "cashAdjustment",
             "createTradeDraft",
             "brokerOrder",
             "strategyExecute",
         }) {
        if (docs.find(token) == std::string::npos) {
            std::cerr << "read-only action policy missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
