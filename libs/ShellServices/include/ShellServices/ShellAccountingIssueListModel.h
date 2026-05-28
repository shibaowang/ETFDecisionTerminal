#pragma once

#include "ShellServices/ShellAccountingIssue.h"

#include <QAbstractListModel>

#include <cstddef>
#include <vector>

namespace etfdt::shell_services {

class ShellAccountingIssueListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        CodeRole = Qt::UserRole + 1,
        LevelRole,
        MessageRole,
        BlockingRole,
        SourceIdRole,
    };

    explicit ShellAccountingIssueListModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void setIssues(std::vector<ShellAccountingIssue> issues);
    void clearIssues();
    [[nodiscard]] std::size_t blockingIssueCount() const noexcept;
    [[nodiscard]] const std::vector<ShellAccountingIssue>& issues() const noexcept;

private:
    std::vector<ShellAccountingIssue> issues_;
};

}  // namespace etfdt::shell_services
