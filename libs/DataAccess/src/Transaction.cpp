#include "DataAccess/Transaction.h"

namespace etfdt::data_access {

Transaction::Transaction(SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

Transaction::~Transaction()
{
    if (active_) {
        (void)connection_.rollbackTransaction();
        active_ = false;
    }
}

DatabaseResult<bool> Transaction::begin()
{
    if (active_) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            "Transaction is already active");
    }
    auto result = connection_.beginTransaction();
    if (!result) {
        return result;
    }
    active_ = true;
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> Transaction::commit()
{
    if (!active_) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            "Transaction is not active");
    }
    auto result = connection_.commitTransaction();
    if (!result) {
        return result;
    }
    active_ = false;
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> Transaction::rollback()
{
    if (!active_) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            "Transaction is not active");
    }
    auto result = connection_.rollbackTransaction();
    if (!result) {
        return result;
    }
    active_ = false;
    return DatabaseResult<bool>::success(true);
}

bool Transaction::isActive() const noexcept
{
    return active_;
}

}  // namespace etfdt::data_access
