#pragma once

#include "DataAccess/SQLiteConnection.h"

namespace etfdt::data_access {

class Transaction final {
public:
    explicit Transaction(SQLiteConnection& connection) noexcept;
    ~Transaction();

    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;
    Transaction(Transaction&&) = delete;
    Transaction& operator=(Transaction&&) = delete;

    [[nodiscard]] DatabaseResult<bool> begin();
    [[nodiscard]] DatabaseResult<bool> commit();
    [[nodiscard]] DatabaseResult<bool> rollback();
    [[nodiscard]] bool isActive() const noexcept;

private:
    SQLiteConnection& connection_;
    bool active_ = false;
};

}  // namespace etfdt::data_access
