#pragma once

#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QObject>
#include <QQmlEngine>

#include <filesystem>
#include <memory>
#include <string_view>
#include <vector>

namespace etfdt::tests::shell_accounting_qml_smoke {

struct LoadedQmlObject {
    std::unique_ptr<QQmlEngine> engine;
    std::unique_ptr<QObject> object;
};

void expectTrue(bool condition, std::string_view message);
void expectEqual(QString actual, QString expected, std::string_view message);
void expectEqual(bool actual, bool expected, std::string_view message);
void expectEqual(int actual, int expected, std::string_view message);
int failureCount();

std::filesystem::path sourceRootFromArgs(int argc, char* argv[]);
QString readTextFile(const std::filesystem::path& path);

QVariantMap makeStatus(
    const QString& state,
    const QString& payloadStatus,
    bool implemented,
    bool hasRows);
QVariantMap makeIssue(
    const QString& code,
    const QString& level,
    const QString& message,
    bool blocking,
    const QString& sourceId = QStringLiteral("shell-accounting-smoke"));
QVariantList makeIssueList(std::initializer_list<QVariantMap> issues);
QVariantMap makeAmount(const QString& rawText, const QString& maskedText, bool privacyMode);
QVariantList makePositionRows(bool hasRows);
QVariantMap makePresenter(
    const QVariantMap& status,
    const QVariantList& issues,
    const QVariantList& positions,
    bool privacyMode);

std::vector<QVariantMap> stateDisplayScenarios();
std::vector<QVariantMap> guardUnavailableScenarios();

QString objectAvailabilityQml();
QString stateDisplayQml();
QString guardUnavailableQml();
QString issueVisibilityQml();
QString privacyDisplayQml();
QString noTradeSmokeQml();

QStringList noTradeForbiddenTokens();
QStringList directDependencyForbiddenTokens();
QStringList productionAccountingBindingTokens();

LoadedQmlObject loadInlineQml(
    const QString& qmlSource,
    const QVariantMap& status,
    const QVariantList& issues,
    const QVariantList& positions,
    const QVariantMap& amount,
    const QVariantMap& presenter);

}  // namespace etfdt::tests::shell_accounting_qml_smoke
