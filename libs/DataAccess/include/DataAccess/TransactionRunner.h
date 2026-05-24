#pragma once

#include "DataAccess/Transaction.h"

#include <functional>

namespace etfdt::data_access {

class TransactionRunner final {
public:
    explicit TransactionRunner(SQLiteConnection& connection) noexcept;

    TransactionRunner(const TransactionRunner&) = delete;
    TransactionRunner& operator=(const TransactionRunner&) = delete;

    [[nodiscard]] DatabaseResult<bool> runInTransaction(
        const std::function<DatabaseResult<bool>()>& callback);

private:
    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access
