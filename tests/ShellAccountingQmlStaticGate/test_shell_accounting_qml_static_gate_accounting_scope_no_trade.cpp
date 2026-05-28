#include "QmlStaticGateScanner.h"

#include <iostream>
#include <initializer_list>
#include <vector>

using namespace etfdt::tests::shell_accounting_qml_static_gate;

namespace {

std::string bytes(std::initializer_list<unsigned char> values)
{
    std::string result;
    result.reserve(values.size());
    for (const auto value : values) {
        result.push_back(static_cast<char>(value));
    }
    return result;
}

}  // namespace

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto files = loadQmlFiles(root);
    const std::vector<std::string> tokens{
        "buyButton",
        "sellButton",
        "BuyButton",
        "SellButton",
        "createTradeDraft",
        "TradeDraft",
        "brokerOrder",
        "submitOrder",
        "strategyExecute",
        "executeStrategy",
        "data.audit.append",
        "writeEnabled: true",
        bytes({0xE4, 0xB8, 0x8B, 0xE5, 0x8D, 0x95}),
        bytes({0xE4, 0xB9, 0xB0, 0xE5, 0x85, 0xA5, 0xE6, 0x8C, 0x89,
               0xE9, 0x92, 0xAE}),
        bytes({0xE5, 0x8D, 0x96, 0xE5, 0x87, 0xBA, 0xE6, 0x8C, 0x89,
               0xE9, 0x92, 0xAE}),
        bytes({0xE7, 0x94, 0x9F, 0xE6, 0x88, 0x90, 0xE8, 0x8D, 0x89,
               0xE7, 0xA8, 0xBF}),
        bytes({0xE6, 0x89, 0xA7, 0xE8, 0xA1, 0x8C, 0xE7, 0xAD, 0x96,
               0xE7, 0x95, 0xA5}),
        bytes({0xE5, 0x88, 0xB8, 0xE5, 0x95, 0x86, 0xE6, 0x8F, 0x90,
               0xE4, 0xBA, 0xA4}),
    };

    const auto hits = scanTokensInAccountingScope(files, tokens);
    if (!hits.empty()) {
        std::cerr << "Accounting-scoped QML must not expose trading, draft, strategy, broker, or write actions.\n";
        printHits(hits);
        return 1;
    }
    return 0;
}
