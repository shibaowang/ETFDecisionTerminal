#pragma once

#include "ShellCore/ShellDiagnosticRows.h"
#include "ShellCore/ShellDiagnosticViewModel.h"

#include <QAbstractListModel>

#include <vector>

namespace etfdt::shell {

class ShellDiagnosticServiceListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        ServiceNameRole = Qt::UserRole + 1,
        EnabledRole,
        SupportedRole,
        CanStartRole,
        HealthCheckSupportedRole,
        ExecutableExistsRole,
        WorkingDirectoryExistsRole,
        SocketNamePresentRole,
        StatusTextRole,
        StatusSeverityRole,
        StatusSeverityRankRole,
        IssueCountRole,
    };

    explicit ShellDiagnosticServiceListModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void setViewModel(const ShellDiagnosticViewModel& viewModel);
    void setRows(std::vector<ShellDiagnosticServiceRow> rows);
    [[nodiscard]] const std::vector<ShellDiagnosticServiceRow>& rows() const;
    [[nodiscard]] const ShellDiagnosticServiceRow* rowAt(int row) const;

private:
    std::vector<ShellDiagnosticServiceRow> rows_;
};

}  // namespace etfdt::shell
