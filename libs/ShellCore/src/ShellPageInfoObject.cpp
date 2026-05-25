#include "ShellCore/ShellPageInfoObject.h"

namespace etfdt::shell {

ShellPageInfoObject::ShellPageInfoObject(QObject* parent)
    : QObject(parent)
{
}

QString ShellPageInfoObject::pageKey() const
{
    return QString::fromStdString(status_.pageKey);
}

QString ShellPageInfoObject::title() const
{
    return QString::fromStdString(status_.title);
}

QString ShellPageInfoObject::moduleStatus() const
{
    return QString::fromStdString(status_.moduleStatus);
}

QString ShellPageInfoObject::dataMode() const
{
    return QString::fromStdString(status_.dataMode);
}

QString ShellPageInfoObject::connectionStatus() const
{
    return QString::fromStdString(status_.connectionStatus);
}

QString ShellPageInfoObject::statusText() const
{
    return QString::fromStdString(status_.statusText);
}

QString ShellPageInfoObject::detailText() const
{
    return QString::fromStdString(status_.detailText);
}

int ShellPageInfoObject::warningCount() const
{
    return status_.warningCount;
}

int ShellPageInfoObject::errorCount() const
{
    return status_.errorCount;
}

bool ShellPageInfoObject::placeholder() const
{
    return status_.placeholder;
}

void ShellPageInfoObject::setPageStatus(const ShellPageStatus& status)
{
    status_ = status;
    emit pageInfoChanged();
}

}  // namespace etfdt::shell
