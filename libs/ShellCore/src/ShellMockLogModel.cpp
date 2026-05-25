#include "ShellCore/ShellMockLogModel.h"

#include <QString>

#include <utility>

namespace etfdt::shell {

namespace {

ShellMockLogEntry makeEntry(std::string timeText, std::string level, std::string source, std::string message)
{
    ShellMockLogEntry entry;
    entry.timeText = std::move(timeText);
    entry.level = std::move(level);
    entry.source = std::move(source);
    entry.message = std::move(message);
    return entry;
}

}  // namespace

ShellMockLogModel::ShellMockLogModel(QObject* parent)
    : QAbstractListModel(parent),
      entries_({
          makeEntry("09:30:00", "INFO", "Shell", "Shell UI Mock started."),
          makeEntry("09:30:01", "INFO", "Shell", "No real services are connected."),
          makeEntry("09:30:02", "INFO", "Diagnostics", "Diagnostics Center is using mock data."),
          makeEntry("09:30:03", "WARNING", "Shell", "Business pages are placeholders."),
      })
{
}

int ShellMockLogModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(entries_.size());
}

QVariant ShellMockLogModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) {
        return {};
    }

    const auto& entry = entries_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case TimeTextRole:
        return QString::fromStdString(entry.timeText);
    case LevelRole:
        return QString::fromStdString(entry.level);
    case SourceRole:
        return QString::fromStdString(entry.source);
    case MessageRole:
        return QString::fromStdString(entry.message);
    default:
        return {};
    }
}

QHash<int, QByteArray> ShellMockLogModel::roleNames() const
{
    return {
        {TimeTextRole, "timeText"},
        {LevelRole, "level"},
        {SourceRole, "source"},
        {MessageRole, "message"},
    };
}

void ShellMockLogModel::appendMockLog(QString level, QString source, QString message)
{
    const int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    entries_.push_back(makeEntry("mock", level.toStdString(), source.toStdString(), message.toStdString()));
    endInsertRows();
}

const std::vector<ShellMockLogEntry>& ShellMockLogModel::entries() const
{
    return entries_;
}

}  // namespace etfdt::shell
