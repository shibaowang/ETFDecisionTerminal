#pragma once

#include "ShellServices/ShellDataTypes.h"

#include <QObject>
#include <QString>

namespace etfdt::shell_services {

class ShellDataConnectionObject final : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectionChanged)
    Q_PROPERTY(bool healthy READ healthy NOTIFY connectionChanged)
    Q_PROPERTY(QString socketName READ socketName NOTIFY connectionChanged)
    Q_PROPERTY(QString stateText READ stateText NOTIFY connectionChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY connectionChanged)

public:
    explicit ShellDataConnectionObject(QObject* parent = nullptr);

    [[nodiscard]] bool connected() const noexcept;
    [[nodiscard]] bool healthy() const noexcept;
    [[nodiscard]] QString socketName() const;
    [[nodiscard]] QString stateText() const;
    [[nodiscard]] QString lastError() const;

    void updateFromState(const ShellDataConnectionState& state);
    void setHealthy(bool healthy);
    void setLastError(const std::string& message);
    void reset();

signals:
    void connectionChanged();

private:
    bool connected_ = false;
    bool healthy_ = false;
    QString socketName_;
    QString stateText_ = QStringLiteral("DISCONNECTED");
    QString lastError_;
};

}  // namespace etfdt::shell_services
