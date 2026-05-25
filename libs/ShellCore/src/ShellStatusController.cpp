#include "ShellCore/ShellStatusController.h"

namespace etfdt::shell {

ShellStatusController::ShellStatusController(QObject* parent)
    : QObject(parent),
      statusModel_(this),
      pageInfo_(this),
      logModel_(this)
{
    (void)updateCurrentPageStatus(QStringLiteral("dashboard"));
}

ShellPageStatusModel* ShellStatusController::statusModel()
{
    return &statusModel_;
}

ShellPageInfoObject* ShellStatusController::pageInfo()
{
    return &pageInfo_;
}

ShellMockLogModel* ShellStatusController::logModel()
{
    return &logModel_;
}

bool ShellStatusController::updateCurrentPageStatus(const QString& pageKey)
{
    const auto* status = statusModel_.statusForKey(pageKey);
    if (status == nullptr) {
        return false;
    }

    pageInfo_.setPageStatus(*status);
    logModel_.appendMockLog(
        QStringLiteral("INFO"),
        QStringLiteral("Shell"),
        QStringLiteral("Selected mock page: %1").arg(QString::fromStdString(status->title)));
    emit currentPageStatusChanged();
    return true;
}

}  // namespace etfdt::shell
