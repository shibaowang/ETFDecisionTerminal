#pragma once

#include "DataAccess/ReadOnlyRepositoryBase.h"

#include <cstdint>
#include <string>
#include <vector>

namespace etfdt::data_access {

class AccountRepository final : public ReadOnlyRepositoryBase {
public:
    using ReadOnlyRepositoryBase::ReadOnlyRepositoryBase;

    [[nodiscard]] RepositoryResult<std::vector<AccountRecord>> listAccounts() const;
    [[nodiscard]] RepositoryResult<AccountRecord> getActiveAccount() const;
    [[nodiscard]] RepositoryResult<AccountRecord> getById(std::int64_t id) const;
};

class PortfolioRepository final : public ReadOnlyRepositoryBase {
public:
    using ReadOnlyRepositoryBase::ReadOnlyRepositoryBase;

    [[nodiscard]] RepositoryResult<std::vector<PortfolioRecord>> listPortfolios() const;
    [[nodiscard]] RepositoryResult<PortfolioRecord> getActivePortfolio() const;
    [[nodiscard]] RepositoryResult<PortfolioRecord> getById(std::int64_t id) const;
};

class InstrumentRepository final : public ReadOnlyRepositoryBase {
public:
    using ReadOnlyRepositoryBase::ReadOnlyRepositoryBase;

    [[nodiscard]] RepositoryResult<std::vector<InstrumentRecord>> listEnabledInstruments() const;
    [[nodiscard]] RepositoryResult<InstrumentRecord> getByCode(const std::string& code) const;
    [[nodiscard]] RepositoryResult<InstrumentRecord> getById(std::int64_t id) const;
};

class StrategyRepository final : public ReadOnlyRepositoryBase {
public:
    using ReadOnlyRepositoryBase::ReadOnlyRepositoryBase;

    [[nodiscard]] RepositoryResult<std::vector<StrategyRecord>> listEnabledStrategies() const;
    [[nodiscard]] RepositoryResult<StrategyRecord> getByStrategyCode(
        const std::string& strategyCode) const;
    [[nodiscard]] RepositoryResult<StrategyRecord> getById(std::int64_t id) const;
};

class OtcChannelRepository final : public ReadOnlyRepositoryBase {
public:
    using ReadOnlyRepositoryBase::ReadOnlyRepositoryBase;

    [[nodiscard]] RepositoryResult<std::vector<OtcChannelRecord>> listEnabledByStrategyCode(
        const std::string& strategyCode) const;
    [[nodiscard]] RepositoryResult<std::vector<OtcChannelRecord>> listAllByStrategyCode(
        const std::string& strategyCode) const;
    [[nodiscard]] RepositoryResult<int> countAll() const;
};

class SchemaVersionRepository final : public ReadOnlyRepositoryBase {
public:
    using ReadOnlyRepositoryBase::ReadOnlyRepositoryBase;

    [[nodiscard]] RepositoryResult<std::vector<SchemaVersionRecord>> listVersions() const;
    [[nodiscard]] RepositoryResult<bool> hasMigration(const std::string& versionOrMigrationName)
        const;
};

}  // namespace etfdt::data_access
