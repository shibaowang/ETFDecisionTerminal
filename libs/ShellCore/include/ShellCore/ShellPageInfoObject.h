#pragma once

#include "ShellCore/ShellPageStatusTypes.h"

#include <QObject>
#include <QString>

namespace etfdt::shell {

class ShellPageInfoObject final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString pageKey READ pageKey NOTIFY pageInfoChanged)
    Q_PROPERTY(QString title READ title NOTIFY pageInfoChanged)
    Q_PROPERTY(QString moduleStatus READ moduleStatus NOTIFY pageInfoChanged)
    Q_PROPERTY(QString dataMode READ dataMode NOTIFY pageInfoChanged)
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY pageInfoChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY pageInfoChanged)
    Q_PROPERTY(QString detailText READ detailText NOTIFY pageInfoChanged)
    Q_PROPERTY(int warningCount READ warningCount NOTIFY pageInfoChanged)
    Q_PROPERTY(int errorCount READ errorCount NOTIFY pageInfoChanged)
    Q_PROPERTY(bool placeholder READ placeholder NOTIFY pageInfoChanged)

public:
    explicit ShellPageInfoObject(QObject* parent = nullptr);

    [[nodiscard]] QString pageKey() const;
    [[nodiscard]] QString title() const;
    [[nodiscard]] QString moduleStatus() const;
    [[nodiscard]] QString dataMode() const;
    [[nodiscard]] QString connectionStatus() const;
    [[nodiscard]] QString statusText() const;
    [[nodiscard]] QString detailText() const;
    [[nodiscard]] int warningCount() const;
    [[nodiscard]] int errorCount() const;
    [[nodiscard]] bool placeholder() const;

    void setPageStatus(const ShellPageStatus& status);

signals:
    void pageInfoChanged();

private:
    ShellPageStatus status_;
};

}  // namespace etfdt::shell
