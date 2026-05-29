#include "ShellAccountingQmlSmokeTestObjects.h"

#include <QCoreApplication>
#include <QObject>

using namespace etfdt::tests::shell_accounting_qml_smoke;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const QVariantList positions = makePositionRows(false);
    const QVariantMap amount = makeAmount(QStringLiteral("UNAVAILABLE"), QStringLiteral("UNAVAILABLE"), false);

    for (const QVariantMap& status : guardUnavailableScenarios()) {
        const QString guardCode = status.value(QStringLiteral("payloadStatus")).toString();
        const QVariantList issues = makeIssueList({
            makeIssue(guardCode, QStringLiteral("ERROR"), QStringLiteral("Guard action unavailable"), true),
        });
        const QVariantMap presenter = makePresenter(status, issues, positions, false);
        auto loaded = loadInlineQml(guardUnavailableQml(), status, issues, positions, amount, presenter);
        QObject* root = loaded.object.get();
        expectTrue(root != nullptr, "guard unavailable root exists");
        if (root == nullptr) {
            continue;
        }

        expectEqual(root->property("visibleState").toString(), QStringLiteral("Unavailable"), "guard maps to Unavailable");
        expectEqual(root->property("payloadStatus").toString(), guardCode, "guard payload status is visible");
        expectEqual(root->property("unavailable").toBool(), true, "guard unavailable flag is visible");
        expectEqual(root->property("empty").toBool(), false, "guard is not Empty");
        expectEqual(root->property("hasRows").toBool(), false, "guard does not fake rows");
        expectEqual(root->property("readOnly").toBool(), true, "guard keeps readOnly=true");
        expectEqual(root->property("writeEnabled").toBool(), false, "guard keeps writeEnabled=false");
        expectEqual(root->property("issueCount").toInt(), 1, "guard issue is visible");
        expectEqual(root->property("issueCode").toString(), guardCode, "guard issue code is visible");
    }

    return failureCount() == 0 ? 0 : 1;
}
