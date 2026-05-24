#pragma once

#include "CoreDomain/CoreDomain.h"
#include "DataAccess/DatabaseTypes.h"

#include <cstdint>
#include <string>

namespace etfdt::data_access {

using RepositoryError = DatabaseError;

template <typename T>
using RepositoryResult = DatabaseResult<T>;

struct AccountRecord final {
    std::int64_t id = 0;
    etfdt::core::Uuid uid;
    std::string name;
    etfdt::core::AccountType accountType = etfdt::core::AccountType::Real;
    std::string brokerName;
    std::string baseCurrency;
    bool isActive = false;
    etfdt::core::MoneyCents initialCashCents;
};

struct PortfolioRecord final {
    std::int64_t id = 0;
    etfdt::core::Uuid uid;
    std::string name;
    etfdt::core::RatioPpm basePositionRatioPpm;
    bool isActive = false;
};

struct InstrumentRecord final {
    std::int64_t id = 0;
    etfdt::core::Uuid uid;
    etfdt::core::InstrumentCode code;
    std::string name;
    etfdt::core::InstrumentType instrumentType = etfdt::core::InstrumentType::Etf;
    etfdt::core::MarketCode marketCode = etfdt::core::MarketCode::CnA;
    std::string currency;
    bool enabled = false;
};

struct StrategyRecord final {
    std::int64_t id = 0;
    etfdt::core::Uuid uid;
    etfdt::core::StrategyCode strategyCode;
    std::int64_t instrumentId = 0;
    std::string name;
    bool enabled = false;
    etfdt::core::RatioPpm externalPremiumThresholdPpm;
    etfdt::core::RatioPpm takeProfitPremiumThresholdPpm;
    etfdt::core::RatioPpm returnTakeProfitRatioPpm;
    etfdt::core::RatioPpm addPremiumLimitPpm;
    std::int64_t adjustmentFactor1e6 = 0;
};

struct OtcChannelRecord final {
    std::int64_t id = 0;
    etfdt::core::Uuid uid;
    etfdt::core::StrategyCode strategyCode;
    etfdt::core::InstrumentCode actualCode;
    std::string fundClass;
    bool enabled = false;
    etfdt::core::MoneyCents dailyLimitCents;
    int priority = 0;
    etfdt::core::MoneyCents minBuyAmountCents;
};

struct SchemaVersionRecord final {
    std::int64_t id = 0;
    etfdt::core::Uuid uid;
    int version = 0;
    std::string migrationName;
    std::string checksum;
    std::string appliedAtUtc;
};

}  // namespace etfdt::data_access
