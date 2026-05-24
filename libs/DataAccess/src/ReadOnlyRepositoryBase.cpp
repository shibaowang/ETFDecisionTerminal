#include "DataAccess/ReadOnlyRepositoryBase.h"

namespace etfdt::data_access {

ReadOnlyRepositoryBase::ReadOnlyRepositoryBase(SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

RepositoryResult<std::vector<SqlQueryRow>> ReadOnlyRepositoryBase::queryRows(
    const std::string& sql,
    const std::vector<std::string>& parameters) const
{
    return connection_.queryRows(sql, parameters);
}

RepositoryResult<SqlQueryRow> ReadOnlyRepositoryBase::querySingleRow(
    const std::string& sql,
    const std::vector<std::string>& parameters,
    const std::string& notFoundMessage) const
{
    auto rows = queryRows(sql, parameters);
    if (!rows) {
        return RepositoryResult<SqlQueryRow>::failure(
            rows.error().errorCode,
            rows.error().message);
    }

    if (rows.value().empty()) {
        return RepositoryResult<SqlQueryRow>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            notFoundMessage);
    }

    return RepositoryResult<SqlQueryRow>::success(rows.value().front());
}

}  // namespace etfdt::data_access
