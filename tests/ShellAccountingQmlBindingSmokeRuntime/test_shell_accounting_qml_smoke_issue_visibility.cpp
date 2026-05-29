#include "ShellAccountingQmlSmokeTestObjects.h"

#include <QCoreApplication>
#include <QObject>

using namespace etfdt::tests::shell_accounting_qml_smoke;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const QVariantMap status =
        makeStatus(QStringLiteral("Warning"), QStringLiteral("MARKET_PRICE_MISSING"), true, true);
    const QVariantList issues = makeIssueList({
        makeIssue(
            QStringLiteral("MARKET_PRICE_MISSING"),
            QStringLiteral("WARNING"),
            QStringLiteral("Market price is missing"),
            false,
            QStringLiteral("market-price")),
        makeIssue(
            QStringLiteral("POSITION_LIST_NOT_AVAILABLE"),
            QStringLiteral("ERROR"),
            QStringLiteral("Position list guard is unavailable"),
            true,
            QStringLiteral("position-list")),
        makeIssue(
            QStringLiteral("UNKNOWN_ACCOUNTING_ISSUE"),
            QStringLiteral("INFO"),
            QStringLiteral("Unknown issue is still visible"),
            false,
            QStringLiteral("unknown")),
    });
    const QVariantList positions = makePositionRows(false);
    const QVariantMap amount = makeAmount(QStringLiteral("UNAVAILABLE"), QStringLiteral("UNAVAILABLE"), false);
    const QVariantMap presenter = makePresenter(status, issues, positions, false);

    auto loaded = loadInlineQml(issueVisibilityQml(), status, issues, positions, amount, presenter);
    QObject* root = loaded.object.get();
    expectTrue(root != nullptr, "issue visibility root exists");
    if (root != nullptr) {
        expectEqual(root->property("code").toString(), QStringLiteral("MARKET_PRICE_MISSING"), "issue code is readable");
        expectEqual(root->property("level").toString(), QStringLiteral("WARNING"), "issue level is readable");
        expectEqual(root->property("message").toString(), QStringLiteral("Market price is missing"), "issue message is readable");
        expectEqual(root->property("blocking").toBool(), false, "issue blocking flag is readable");
        expectEqual(root->property("sourceId").toString(), QStringLiteral("market-price"), "issue sourceId is readable");
        expectEqual(root->property("warningVisible").toBool(), true, "warning issue is visible");
        expectEqual(root->property("blockingIssueCode").toString(), QStringLiteral("POSITION_LIST_NOT_AVAILABLE"), "blocking issue is readable");
        expectEqual(root->property("blockingIssueVisible").toBool(), true, "blocking issue flag is visible");
        expectEqual(root->property("unknownCode").toString(), QStringLiteral("UNKNOWN_ACCOUNTING_ISSUE"), "unknown issue is not swallowed");
    }

    return failureCount() == 0 ? 0 : 1;
}
