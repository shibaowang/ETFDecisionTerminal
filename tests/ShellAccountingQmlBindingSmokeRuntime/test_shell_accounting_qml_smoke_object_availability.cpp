#include "ShellAccountingQmlSmokeTestObjects.h"

#include <QCoreApplication>
#include <QObject>

using namespace etfdt::tests::shell_accounting_qml_smoke;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const QVariantMap status = makeStatus(QStringLiteral("Loaded"), QStringLiteral("OK"), true, true);
    const QVariantList issues = makeIssueList({
        makeIssue(
            QStringLiteral("POSITION_LIST_NOT_AVAILABLE"),
            QStringLiteral("ERROR"),
            QStringLiteral("Position list guard payload is unavailable"),
            true),
    });
    const QVariantList positions = makePositionRows(true);
    const QVariantMap amount = makeAmount(QStringLiteral("123.45 CNY"), QStringLiteral("*** CNY"), false);
    const QVariantMap presenter = makePresenter(status, issues, positions, false);

    const QString qml = objectAvailabilityQml();
    for (const QString& token : directDependencyForbiddenTokens()) {
        expectTrue(!qml.contains(token), "object availability smoke QML has no direct dependency token");
    }

    auto loaded = loadInlineQml(qml, status, issues, positions, amount, presenter);
    QObject* root = loaded.object.get();
    expectTrue(root != nullptr, "object availability root exists");
    if (root != nullptr) {
        expectEqual(root->property("presenterReadOnly").toBool(), true, "QML reads presenter readOnly=true");
        expectEqual(
            root->property("presenterWriteEnabled").toBool(),
            false,
            "QML reads presenter writeEnabled=false");
        expectEqual(
            root->property("statusText").toString(),
            QStringLiteral("Loaded"),
            "QML reads status object");
        expectEqual(root->property("issueCount").toInt(), 1, "QML reads issue list");
        expectEqual(
            root->property("firstIssueCode").toString(),
            QStringLiteral("POSITION_LIST_NOT_AVAILABLE"),
            "QML reads issue code");
        expectEqual(root->property("positionCount").toInt(), 1, "QML reads position model/list");
        expectEqual(
            root->property("firstInstrumentCode").toString(),
            QStringLiteral("ETF-SMOKE"),
            "QML reads position role");
    }

    return failureCount() == 0 ? 0 : 1;
}
