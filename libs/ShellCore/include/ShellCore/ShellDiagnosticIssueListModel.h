#pragma once

#include "ShellCore/ShellDiagnosticRows.h"

#include <QAbstractListModel>

#include <vector>

namespace etfdt::shell {

class ShellDiagnosticIssueListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        LevelRole = Qt::UserRole + 1,
        CodeRole,
        MessageRole,
        DisplayTextRole,
        SeverityRankRole,
    };

    explicit ShellDiagnosticIssueListModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void setIssues(std::vector<ShellDiagnosticIssueRow> issues);
    void clearIssues();
    [[nodiscard]] const std::vector<ShellDiagnosticIssueRow>& issues() const;

private:
    std::vector<ShellDiagnosticIssueRow> issues_;
};

}  // namespace etfdt::shell
