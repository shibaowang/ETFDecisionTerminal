#include "ShellAccountingQmlSmokeTestObjects.h"

#include <QCoreApplication>
#include <QObject>

using namespace etfdt::tests::shell_accounting_qml_smoke;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const QString qml = privacyDisplayQml();
    expectTrue(!qml.contains(QStringLiteral("rawText")), "privacy smoke QML does not bind rawText");
    expectTrue(!qml.contains(QStringLiteral("maskedText")), "privacy smoke QML does not calculate maskedText");

    const QVariantMap status =
        makeStatus(QStringLiteral("Unavailable"), QStringLiteral("POSITION_LIST_NOT_AVAILABLE"), false, false);
    const QVariantList issues = makeIssueList({
        makeIssue(QStringLiteral("POSITION_LIST_NOT_AVAILABLE"), QStringLiteral("ERROR"), QStringLiteral("Unavailable"), true),
        makeIssue(QStringLiteral("MARKET_PRICE_MISSING"), QStringLiteral("WARNING"), QStringLiteral("Warning remains visible"), false),
    });
    const QVariantList positions = makePositionRows(false);

    for (const bool privacyMode : {false, true}) {
        const QVariantMap amount =
            makeAmount(QStringLiteral("12345.67 CNY"), QStringLiteral("*** CNY"), privacyMode);
        const QVariantMap presenter = makePresenter(status, issues, positions, privacyMode);
        auto loaded = loadInlineQml(qml, status, issues, positions, amount, presenter);
        QObject* root = loaded.object.get();
        expectTrue(root != nullptr, "privacy display root exists");
        if (root == nullptr) {
            continue;
        }

        expectEqual(root->property("privacyMode").toBool(), privacyMode, "privacyMode is readable");
        expectEqual(
            root->property("visibleAmountText").toString(),
            privacyMode ? QStringLiteral("*** CNY") : QStringLiteral("12345.67 CNY"),
            "QML displays displayText from ShellServices");
        expectEqual(
            amount.value(QStringLiteral("rawText")).toString(),
            QStringLiteral("12345.67 CNY"),
            "rawText is unchanged outside QML");
        expectEqual(root->property("issueCode").toString(), QStringLiteral("POSITION_LIST_NOT_AVAILABLE"), "issue remains visible under privacy");
        expectEqual(root->property("warningVisible").toBool(), true, "warning remains visible under privacy");
    }

    const QVariantMap unavailableAmount =
        makeAmount(QStringLiteral("UNAVAILABLE"), QStringLiteral("UNAVAILABLE"), true);
    expectEqual(
        unavailableAmount.value(QStringLiteral("displayText")).toString(),
        QStringLiteral("UNAVAILABLE"),
        "Unavailable does not fake an amount");
    const QVariantMap emptyAmount = makeAmount(QString(), QString(), true);
    expectEqual(emptyAmount.value(QStringLiteral("displayText")).toString(), QString(), "Empty does not fake an amount");

    return failureCount() == 0 ? 0 : 1;
}
