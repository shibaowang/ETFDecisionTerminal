#pragma once

#include "ShellCore/ShellPageStatusTypes.h"

#include <QAbstractListModel>
#include <QString>

#include <vector>

namespace etfdt::shell {

class ShellMockLogModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        TimeTextRole = Qt::UserRole + 1,
        LevelRole,
        SourceRole,
        MessageRole,
    };

    explicit ShellMockLogModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void appendMockLog(QString level, QString source, QString message);
    [[nodiscard]] const std::vector<ShellMockLogEntry>& entries() const;

private:
    std::vector<ShellMockLogEntry> entries_;
};

}  // namespace etfdt::shell
