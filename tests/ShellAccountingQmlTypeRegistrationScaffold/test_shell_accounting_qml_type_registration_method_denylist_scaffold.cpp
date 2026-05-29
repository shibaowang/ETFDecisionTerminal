#include "ShellAccountingQmlTypeRegistrationExpectedType.h"

#include <algorithm>
#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_type_registration;

namespace {

bool contains(const std::vector<std::string>& values, const std::string& value)
{
    return std::find(values.begin(), values.end(), value) != values.end();
}

}  // namespace

int main()
{
    const auto methods = forbiddenMethodTokens();
    const auto properties = forbiddenPropertyTokens();

    for (const auto& required : {
             "buy",
             "sell",
             "createTradeDraft",
             "brokerOrder",
             "submitOrder",
             "strategyExecute",
             "executeStrategy",
             "data.audit.append",
             "writeAction",
             "snapshotWrite",
             "tradeLogWrite",
             "positionSnapshotWrite",
             "cashSnapshotWrite",
             "portfolioSummaryWrite",
             "setWriteEnabled",
             "enableTrading",
             "submitTrade",
             "placeOrder",
             "generateSuggestion",
         }) {
        if (!contains(methods, required)) {
            std::cerr << "Missing forbidden method token: " << required << '\n';
            return 1;
        }
    }

    if (!contains(properties, "writeEnabled: true")) {
        std::cerr << "Missing forbidden writeEnabled true property token.\n";
        return 1;
    }

    for (const auto& expectedType : firstStageExpectedTypes()) {
        if (expectedType.forbiddenMethods.size() != methods.size() ||
            expectedType.forbiddenProperties.size() != properties.size()) {
            std::cerr << "Allowed type does not carry the full method/property denylist: "
                      << expectedType.typeName << '\n';
            return 1;
        }
    }
    return 0;
}
