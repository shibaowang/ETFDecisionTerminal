#pragma once

#include "ShellCore/ShellNavigationModel.h"
#include "ShellCore/ShellNavigationTypes.h"

#include <QObject>
#include <QString>

namespace etfdt::shell {

class ShellNavigationController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentPageKey READ currentPageKey NOTIFY currentPageChanged)
    Q_PROPERTY(QString currentPageTitle READ currentPageTitle NOTIFY currentPageChanged)
    Q_PROPERTY(QString currentPageDescription READ currentPageDescription NOTIFY currentPageChanged)
    Q_PROPERTY(QString currentPageQmlComponent READ currentPageQmlComponent NOTIFY currentPageChanged)
    Q_PROPERTY(bool currentPagePlaceholder READ currentPagePlaceholder NOTIFY currentPageChanged)
    Q_PROPERTY(ShellNavigationModel* navigationModel READ navigationModel CONSTANT)

public:
    explicit ShellNavigationController(QObject* parent = nullptr);

    [[nodiscard]] QString currentPageKey() const;
    [[nodiscard]] QString currentPageTitle() const;
    [[nodiscard]] QString currentPageDescription() const;
    [[nodiscard]] QString currentPageQmlComponent() const;
    [[nodiscard]] bool currentPagePlaceholder() const;
    [[nodiscard]] ShellNavigationModel* navigationModel();

    Q_INVOKABLE [[nodiscard]] bool selectPage(const QString& key);
    Q_INVOKABLE [[nodiscard]] bool selectPageByIndex(int index);

signals:
    void currentPageChanged();

private:
    bool setCurrentMetadata(const ShellPageMetadata& metadata);

    ShellNavigationModel navigationModel_;
    ShellPageMetadata currentMetadata_;
};

}  // namespace etfdt::shell
