#include "ShellServices/ShellDataConnectionObject.h"

namespace etfdt::shell_services {

ShellDataConnectionObject::ShellDataConnectionObject(QObject* parent)
    : QObject(parent)
{
}

bool ShellDataConnectionObject::connected() const noexcept
{
    return connected_;
}

bool ShellDataConnectionObject::healthy() const noexcept
{
    return healthy_;
}

QString ShellDataConnectionObject::socketName() const
{
    return socketName_;
}

QString ShellDataConnectionObject::stateText() const
{
    return stateText_;
}

QString ShellDataConnectionObject::lastError() const
{
    return lastError_;
}

void ShellDataConnectionObject::updateFromState(const ShellDataConnectionState& state)
{
    connected_ = state.connected;
    socketName_ = QString::fromStdString(state.socketName);
    lastError_ = QString::fromStdString(state.lastError);
    if (state.connecting) {
        stateText_ = QStringLiteral("CONNECTING");
    } else if (state.connected && state.unhealthy) {
        stateText_ = QStringLiteral("UNHEALTHY");
    } else if (state.connected) {
        stateText_ = QStringLiteral("CONNECTED");
    } else {
        stateText_ = QStringLiteral("DISCONNECTED");
        healthy_ = false;
    }
    emit connectionChanged();
}

void ShellDataConnectionObject::setHealthy(bool healthy)
{
    healthy_ = healthy;
    if (connected_) {
        stateText_ = healthy ? QStringLiteral("HEALTHY") : QStringLiteral("UNHEALTHY");
    }
    emit connectionChanged();
}

void ShellDataConnectionObject::setLastError(const std::string& message)
{
    lastError_ = QString::fromStdString(message);
    if (!message.empty() && connected_) {
        stateText_ = QStringLiteral("UNHEALTHY");
    }
    emit connectionChanged();
}

void ShellDataConnectionObject::reset()
{
    connected_ = false;
    healthy_ = false;
    socketName_.clear();
    stateText_ = QStringLiteral("DISCONNECTED");
    lastError_.clear();
    emit connectionChanged();
}

}  // namespace etfdt::shell_services
