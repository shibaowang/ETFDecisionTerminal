#include "ShellAccountingQmlSmokeTestObjects.h"

#include <QCoreApplication>
#include <QObject>

using namespace etfdt::tests::shell_accounting_qml_smoke;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const QVariantList issues = makeIssueList({
        makeIssue(QStringLiteral("SMOKE_STATE"), QStringLiteral("INFO"), QStringLiteral("state visible"), false),
    });
    const QVariantList positions = makePositionRows(true);
    const QVariantMap amount = makeAmount(QStringLiteral("100.00 CNY"), QStringLiteral("*** CNY"), false);

    for (const QVariantMap& status : stateDisplayScenarios()) {
        const QVariantMap presenter =
            makePresenter(status, issues, positions, status.value(QStringLiteral("state")).toString() == "Loaded");
        auto loaded = loadInlineQml(stateDisplayQml(), status, issues, positions, amount, presenter);
        QObject* root = loaded.object.get();
        expectTrue(root != nullptr, "state display root exists");
        if (root == nullptr) {
            continue;
        }

        const QString state = status.value(QStringLiteral("state")).toString();
        expectEqual(root->property("stateText").toString(), state, "QML reads state text");
        expectEqual(root->property("readOnly").toBool(), true, "state display keeps readOnly=true");
        expectEqual(root->property("writeEnabled").toBool(), false, "state display keeps writeEnabled=false");

        if (state == QStringLiteral("Empty")) {
            expectEqual(root->property("empty").toBool(), true, "Empty is visible");
            expectEqual(root->property("unavailable").toBool(), false, "Empty is not Unavailable");
        }
        if (state == QStringLiteral("Unavailable")) {
            expectEqual(root->property("unavailable").toBool(), true, "Unavailable is visible");
            expectEqual(root->property("empty").toBool(), false, "Unavailable is not Empty");
            expectEqual(root->property("implemented").toBool(), false, "implemented=false remains visible");
        }
        if (state == QStringLiteral("Warning")) {
            expectEqual(root->property("warning").toBool(), true, "Warning is not hidden");
        }
        if (state == QStringLiteral("Error")) {
            expectEqual(root->property("error").toBool(), true, "Error is not hidden");
        }
        if (state == QStringLiteral("Stale")) {
            expectEqual(root->property("stale").toBool(), true, "Stale is not treated as OK");
        }
        if (state == QStringLiteral("Loading")) {
            expectEqual(root->property("loading").toBool(), true, "Loading is visible");
        }
        if (state == QStringLiteral("Loaded")) {
            expectEqual(root->property("loaded").toBool(), true, "Loaded is visible");
        }
    }

    return failureCount() == 0 ? 0 : 1;
}
