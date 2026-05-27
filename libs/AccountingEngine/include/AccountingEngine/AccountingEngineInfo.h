#pragma once

#include <string>

namespace etfdt::accounting {

struct AccountingEngineInfo {
    std::string moduleName;
    std::string moduleVersion;
    std::string contractVersion;
    bool productionReady = false;
    bool replayImplemented = false;
    bool snapshotImplemented = false;
    bool writeEnabled = false;
    std::string description;
};

[[nodiscard]] AccountingEngineInfo accountingEngineInfo();

} // namespace etfdt::accounting
