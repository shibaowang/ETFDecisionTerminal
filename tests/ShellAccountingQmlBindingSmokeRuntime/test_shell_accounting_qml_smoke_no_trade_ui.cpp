#include "ShellAccountingQmlSmokeTestObjects.h"

#include <QCoreApplication>
#include <QObject>

using namespace etfdt::tests::shell_accounting_qml_smoke;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const QString qml = noTradeSmokeQml();
    for (const QString& token : noTradeForbiddenTokens()) {
        expectTrue(!qml.contains(token), "test-only smoke QML has no no-trade forbidden token");
    }
    for (const QString& token : directDependencyForbiddenTokens()) {
        expectTrue(!qml.contains(token), "test-only smoke QML has no direct dependency token");
    }

    const QVariantMap status =
        makeStatus(QStringLiteral("Unavailable"), QStringLiteral("BASE_POSITION_SUMMARY_NOT_AVAILABLE"), false, false);
    const QVariantList issues = makeIssueList({
        makeIssue(QStringLiteral("BASE_POSITION_SUMMARY_NOT_AVAILABLE"), QStringLiteral("ERROR"), QStringLiteral("Base unavailable"), true),
    });
    const QVariantList positions = makePositionRows(false);
    const QVariantMap amount = makeAmount(QStringLiteral("UNAVAILABLE"), QStringLiteral("UNAVAILABLE"), true);
    const QVariantMap presenter = makePresenter(status, issues, positions, true);

    auto loaded = loadInlineQml(qml, status, issues, positions, amount, presenter);
    QObject* root = loaded.object.get();
    expectTrue(root != nullptr, "no-trade UI root exists");
    if (root != nullptr) {
        expectEqual(root->property("readOnly").toBool(), true, "no-trade QML reads readOnly=true");
        expectEqual(root->property("writeEnabled").toBool(), false, "no-trade QML reads writeEnabled=false");
        expectEqual(root->property("visibleState").toString(), QStringLiteral("Unavailable"), "no-trade QML reads state");
        expectEqual(root->property("amountText").toString(), QStringLiteral("UNAVAILABLE"), "no-trade QML does not fake amount");
        expectEqual(root->property("noTradingButtons").toBool(), true, "no trading buttons rule is fixed");
        expectEqual(root->property("noWriteMenu").toBool(), true, "no write menu rule is fixed");
    }

    return failureCount() == 0 ? 0 : 1;
}
