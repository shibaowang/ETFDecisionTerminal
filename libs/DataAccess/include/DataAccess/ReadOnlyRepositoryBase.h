#pragma once

#include "DataAccess/RepositoryTypes.h"
#include "DataAccess/SQLiteConnection.h"

#include <string>
#include <vector>

namespace etfdt::data_access {

class ReadOnlyRepositoryBase {
public:
    explicit ReadOnlyRepositoryBase(SQLiteConnection& connection) noexcept;
    virtual ~ReadOnlyRepositoryBase() = default;

    ReadOnlyRepositoryBase(const ReadOnlyRepositoryBase&) = delete;
    ReadOnlyRepositoryBase& operator=(const ReadOnlyRepositoryBase&) = delete;
    ReadOnlyRepositoryBase(ReadOnlyRepositoryBase&&) = delete;
    ReadOnlyRepositoryBase& operator=(ReadOnlyRepositoryBase&&) = delete;

protected:
    [[nodiscard]] RepositoryResult<std::vector<SqlQueryRow>> queryRows(
        const std::string& sql,
        const std::vector<std::string>& parameters = {}) const;

    [[nodiscard]] RepositoryResult<SqlQueryRow> querySingleRow(
        const std::string& sql,
        const std::vector<std::string>& parameters,
        const std::string& notFoundMessage) const;

private:
    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access
