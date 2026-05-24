#include "DataAccess/TransactionRunner.h"

#include <exception>

namespace etfdt::data_access {

TransactionRunner::TransactionRunner(SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

DatabaseResult<bool> TransactionRunner::runInTransaction(
    const std::function<DatabaseResult<bool>()>& callback)
{
    if (!callback) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            "Transaction callback is required");
    }

    Transaction transaction(connection_);
    auto beginResult = transaction.begin();
    if (!beginResult) {
        return beginResult;
    }

    try {
        auto callbackResult = callback();
        if (!callbackResult) {
            auto rollbackResult = transaction.rollback();
            if (!rollbackResult) {
                return rollbackResult;
            }
            return callbackResult;
        }

        auto commitResult = transaction.commit();
        if (!commitResult) {
            return commitResult;
        }
        return DatabaseResult<bool>::success(true);
    }
    catch (const std::exception& ex) {
        auto rollbackResult = transaction.rollback();
        if (!rollbackResult) {
            return rollbackResult;
        }
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            std::string("Transaction callback threw exception: ") + ex.what());
    }
    catch (...) {
        auto rollbackResult = transaction.rollback();
        if (!rollbackResult) {
            return rollbackResult;
        }
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            "Transaction callback threw unknown exception");
    }
}

}  // namespace etfdt::data_access
