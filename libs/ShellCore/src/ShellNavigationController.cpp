#include "ShellCore/ShellNavigationController.h"

#include "ShellCore/ShellPageRegistry.h"

namespace etfdt::shell {

ShellNavigationController::ShellNavigationController(QObject* parent)
    : QObject(parent),
      navigationModel_(this),
      currentMetadata_(ShellPageRegistry::metadataForKey("dashboard").value_or(
          ShellPageRegistry::fallbackMetadata("dashboard")))
{
}

QString ShellNavigationController::currentPageKey() const
{
    return QString::fromStdString(currentMetadata_.key);
}

QString ShellNavigationController::currentPageTitle() const
{
    return QString::fromStdString(currentMetadata_.title);
}

QString ShellNavigationController::currentPageDescription() const
{
    return QString::fromStdString(currentMetadata_.description);
}

QString ShellNavigationController::currentPageQmlComponent() const
{
    return QString::fromStdString(currentMetadata_.qmlComponent);
}

bool ShellNavigationController::currentPagePlaceholder() const
{
    return currentMetadata_.placeholder;
}

ShellNavigationModel* ShellNavigationController::navigationModel()
{
    return &navigationModel_;
}

bool ShellNavigationController::selectPage(const QString& key)
{
    const auto metadata = ShellPageRegistry::metadataForKey(key.toStdString());
    if (!metadata.has_value()) {
        return false;
    }
    return setCurrentMetadata(*metadata);
}

bool ShellNavigationController::selectPageByIndex(int index)
{
    const auto* item = navigationModel_.itemAt(index);
    if (item == nullptr) {
        return false;
    }
    return selectPage(QString::fromStdString(item->key));
}

bool ShellNavigationController::setCurrentMetadata(const ShellPageMetadata& metadata)
{
    if (currentMetadata_.key == metadata.key) {
        return true;
    }

    currentMetadata_ = metadata;
    emit currentPageChanged();
    return true;
}

}  // namespace etfdt::shell
