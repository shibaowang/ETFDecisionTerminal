#include "ShellAccountingQmlSmokeTestObjects.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QCoreApplication>
#include <QObject>

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_qml_smoke {
namespace {

int gFailures = 0;

QString boolStateName(const QString& state)
{
    if (state == QStringLiteral("Warning")) {
        return QStringLiteral("warning");
    }
    if (state == QStringLiteral("Error")) {
        return QStringLiteral("error");
    }
    if (state == QStringLiteral("Stale")) {
        return QStringLiteral("stale");
    }
    if (state == QStringLiteral("Empty")) {
        return QStringLiteral("empty");
    }
    if (state == QStringLiteral("Unavailable")) {
        return QStringLiteral("unavailable");
    }
    if (state == QStringLiteral("Loading")) {
        return QStringLiteral("loading");
    }
    return QStringLiteral("loaded");
}

bool stateFlag(const QString& state, const QString& flagName)
{
    return boolStateName(state) == flagName;
}

}  // namespace

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

void expectEqual(QString actual, QString expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected.toStdString()
                  << " actual " << actual.toStdString() << '\n';
    }
}

void expectEqual(bool actual, bool expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

void expectEqual(int actual, int expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

int failureCount()
{
    return gFailures;
}

std::filesystem::path sourceRootFromArgs(int argc, char* argv[])
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string_view(argv[i]) == "--source-root") {
            return std::filesystem::path(argv[i + 1]);
        }
    }
    return std::filesystem::current_path();
}

QString readTextFile(const std::filesystem::path& path)
{
    std::ifstream input(path, std::ios::binary);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return QString::fromStdString(buffer.str());
}

QVariantMap makeStatus(
    const QString& state,
    const QString& payloadStatus,
    bool implemented,
    bool hasRows)
{
    QVariantMap status;
    status.insert(QStringLiteral("state"), state);
    status.insert(QStringLiteral("stateText"), state);
    status.insert(QStringLiteral("implemented"), implemented);
    status.insert(QStringLiteral("readOnly"), true);
    status.insert(QStringLiteral("writeEnabled"), false);
    status.insert(QStringLiteral("dataQualityStatus"), QStringLiteral("GUARD"));
    status.insert(QStringLiteral("payloadStatus"), payloadStatus);
    status.insert(QStringLiteral("hasRows"), hasRows);
    status.insert(QStringLiteral("loading"), stateFlag(state, QStringLiteral("loading")));
    status.insert(QStringLiteral("unavailable"), !implemented || stateFlag(state, QStringLiteral("unavailable")));
    status.insert(QStringLiteral("warning"), stateFlag(state, QStringLiteral("warning")));
    status.insert(QStringLiteral("error"), stateFlag(state, QStringLiteral("error")));
    status.insert(QStringLiteral("stale"), stateFlag(state, QStringLiteral("stale")));
    status.insert(QStringLiteral("empty"), stateFlag(state, QStringLiteral("empty")) && implemented);
    status.insert(QStringLiteral("loaded"), state == QStringLiteral("Loaded"));
    return status;
}

QVariantMap makeIssue(
    const QString& code,
    const QString& level,
    const QString& message,
    bool blocking,
    const QString& sourceId)
{
    QVariantMap issue;
    issue.insert(QStringLiteral("code"), code);
    issue.insert(QStringLiteral("level"), level);
    issue.insert(QStringLiteral("message"), message);
    issue.insert(QStringLiteral("blocking"), blocking);
    issue.insert(QStringLiteral("sourceId"), sourceId);
    return issue;
}

QVariantList makeIssueList(std::initializer_list<QVariantMap> issues)
{
    QVariantList result;
    for (const QVariantMap& issue : issues) {
        result.append(issue);
    }
    return result;
}

QVariantMap makeAmount(const QString& rawText, const QString& maskedText, bool privacyMode)
{
    QVariantMap amount;
    amount.insert(QStringLiteral("rawText"), rawText);
    amount.insert(QStringLiteral("maskedText"), maskedText);
    amount.insert(QStringLiteral("displayText"), privacyMode ? maskedText : rawText);
    amount.insert(QStringLiteral("privacyMode"), privacyMode);
    return amount;
}

QVariantList makePositionRows(bool hasRows)
{
    QVariantList rows;
    if (hasRows) {
        QVariantMap row;
        row.insert(QStringLiteral("accountId"), QStringLiteral("ACC-SMOKE"));
        row.insert(QStringLiteral("portfolioId"), QStringLiteral("PF-SMOKE"));
        row.insert(QStringLiteral("instrumentCode"), QStringLiteral("ETF-SMOKE"));
        row.insert(QStringLiteral("quantityText"), QStringLiteral("10.00"));
        row.insert(QStringLiteral("marketValueDisplayText"), QStringLiteral("UNAVAILABLE"));
        rows.append(row);
    }
    return rows;
}

QVariantMap makePresenter(
    const QVariantMap& status,
    const QVariantList& issues,
    const QVariantList& positions,
    bool privacyMode)
{
    QVariantMap presenter;
    presenter.insert(QStringLiteral("status"), status);
    presenter.insert(QStringLiteral("issues"), issues);
    presenter.insert(QStringLiteral("positions"), positions);
    presenter.insert(QStringLiteral("privacyMode"), privacyMode);
    presenter.insert(QStringLiteral("readOnly"), true);
    presenter.insert(QStringLiteral("writeEnabled"), false);
    return presenter;
}

std::vector<QVariantMap> stateDisplayScenarios()
{
    return {
        makeStatus(QStringLiteral("Idle"), QStringLiteral("IDLE"), true, false),
        makeStatus(QStringLiteral("Loading"), QStringLiteral("LOADING"), true, false),
        makeStatus(QStringLiteral("Loaded"), QStringLiteral("OK"), true, true),
        makeStatus(QStringLiteral("Empty"), QStringLiteral("OK"), true, false),
        makeStatus(QStringLiteral("Unavailable"), QStringLiteral("POSITION_LIST_NOT_AVAILABLE"), false, false),
        makeStatus(QStringLiteral("Warning"), QStringLiteral("MARKET_PRICE_MISSING"), true, true),
        makeStatus(QStringLiteral("Error"), QStringLiteral("MULTI_CURRENCY_UNSUPPORTED"), true, false),
        makeStatus(QStringLiteral("Stale"), QStringLiteral("STALE"), true, true),
    };
}

std::vector<QVariantMap> guardUnavailableScenarios()
{
    return {
        makeStatus(QStringLiteral("Unavailable"), QStringLiteral("POSITION_LIST_NOT_AVAILABLE"), false, false),
        makeStatus(QStringLiteral("Unavailable"), QStringLiteral("CASH_SUMMARY_NOT_AVAILABLE"), false, false),
        makeStatus(QStringLiteral("Unavailable"), QStringLiteral("PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE"), false, false),
        makeStatus(QStringLiteral("Unavailable"), QStringLiteral("BASE_POSITION_SUMMARY_NOT_AVAILABLE"), false, false),
        makeStatus(QStringLiteral("Unavailable"), QStringLiteral("SNIPER_POOL_SUMMARY_NOT_AVAILABLE"), false, false),
    };
}

QString objectAvailabilityQml()
{
    return QStringLiteral(R"QML(
import QtQml
QtObject {
    property bool presenterReadOnly: accountingPresenter.readOnly
    property bool presenterWriteEnabled: accountingPresenter.writeEnabled
    property string statusText: accountingPresenter.status.stateText
    property int issueCount: accountingPresenter.issues.length
    property string firstIssueCode: accountingPresenter.issues[0].code
    property int positionCount: accountingPresenter.positions.length
    property string firstInstrumentCode: accountingPresenter.positions[0].instrumentCode
}
)QML");
}

QString stateDisplayQml()
{
    return QStringLiteral(R"QML(
import QtQml
QtObject {
    property string stateText: accountingStatus.stateText
    property bool unavailable: accountingStatus.unavailable
    property bool empty: accountingStatus.empty
    property bool warning: accountingStatus.warning
    property bool error: accountingStatus.error
    property bool stale: accountingStatus.stale
    property bool loading: accountingStatus.loading
    property bool loaded: accountingStatus.loaded
    property bool readOnly: accountingStatus.readOnly
    property bool writeEnabled: accountingStatus.writeEnabled
    property bool implemented: accountingStatus.implemented
}
)QML");
}

QString guardUnavailableQml()
{
    return QStringLiteral(R"QML(
import QtQml
QtObject {
    property string visibleState: accountingStatus.stateText
    property string payloadStatus: accountingStatus.payloadStatus
    property bool unavailable: accountingStatus.unavailable
    property bool empty: accountingStatus.empty
    property bool hasRows: accountingStatus.hasRows
    property bool readOnly: accountingStatus.readOnly
    property bool writeEnabled: accountingStatus.writeEnabled
    property int issueCount: issueModel.length
    property string issueCode: issueModel[0].code
}
)QML");
}

QString issueVisibilityQml()
{
    return QStringLiteral(R"QML(
import QtQml
QtObject {
    property string code: issueModel[0].code
    property string level: issueModel[0].level
    property string message: issueModel[0].message
    property bool blocking: issueModel[0].blocking
    property string sourceId: issueModel[0].sourceId
    property bool warningVisible: issueModel[0].level === "WARNING"
    property string blockingIssueCode: issueModel[1].code
    property bool blockingIssueVisible: issueModel[1].blocking
    property string unknownCode: issueModel[2].code
}
)QML");
}

QString privacyDisplayQml()
{
    return QStringLiteral(R"QML(
import QtQml
QtObject {
    property string visibleAmountText: amount.displayText
    property bool privacyMode: amount.privacyMode
    property string issueCode: issueModel[0].code
    property bool warningVisible: issueModel[1].level === "WARNING"
}
)QML");
}

QString noTradeSmokeQml()
{
    return QStringLiteral(R"QML(
import QtQml
QtObject {
    property bool readOnly: accountingPresenter.readOnly
    property bool writeEnabled: accountingPresenter.writeEnabled
    property string visibleState: accountingPresenter.status.stateText
    property string amountText: amount.displayText
    property bool noTradingButtons: true
    property bool noWriteMenu: true
}
)QML");
}

QStringList noTradeForbiddenTokens()
{
    return {
        QStringLiteral("buyButton"),
        QStringLiteral("sellButton"),
        QStringLiteral("BuyButton"),
        QStringLiteral("SellButton"),
        QStringLiteral("createTradeDraft"),
        QStringLiteral("TradeDraft"),
        QStringLiteral("brokerOrder"),
        QStringLiteral("submitOrder"),
        QStringLiteral("strategyExecute"),
        QStringLiteral("executeStrategy"),
        QStringLiteral("data.audit.append"),
        QStringLiteral("writeEnabled: true"),
        QString::fromUtf8("下单"),
        QString::fromUtf8("买入按钮"),
        QString::fromUtf8("卖出按钮"),
        QString::fromUtf8("生成草稿"),
        QString::fromUtf8("执行策略"),
        QString::fromUtf8("券商提交"),
    };
}

QStringList directDependencyForbiddenTokens()
{
    return {
        QStringLiteral("DataServiceClient"),
        QStringLiteral("DataServiceApi"),
        QStringLiteral("AccountingEngine"),
        QStringLiteral("DataAccess"),
        QStringLiteral("SQLite"),
        QStringLiteral("ShellAccountingDataServiceAdapter"),
    };
}

QStringList productionAccountingBindingTokens()
{
    return {
        QStringLiteral("ShellAccountingPresenter"),
        QStringLiteral("accountingPresenter"),
        QStringLiteral("ShellAccountingStatusObject"),
        QStringLiteral("ShellAccountingIssueListModel"),
        QStringLiteral("ShellPositionListModel"),
    };
}

LoadedQmlObject loadInlineQml(
    const QString& qmlSource,
    const QVariantMap& status,
    const QVariantList& issues,
    const QVariantList& positions,
    const QVariantMap& amount,
    const QVariantMap& presenter)
{
    auto engine = std::make_unique<QQmlEngine>();
    engine->rootContext()->setContextProperty(QStringLiteral("accountingStatus"), status);
    engine->rootContext()->setContextProperty(QStringLiteral("issueModel"), issues);
    engine->rootContext()->setContextProperty(QStringLiteral("positionModel"), positions);
    engine->rootContext()->setContextProperty(QStringLiteral("amount"), amount);
    engine->rootContext()->setContextProperty(QStringLiteral("accountingPresenter"), presenter);

    QQmlComponent component(engine.get());
    component.setData(qmlSource.toUtf8(), QUrl());
    for (int i = 0; component.status() == QQmlComponent::Loading && i < 20; ++i) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    }
    expectEqual(static_cast<int>(component.status()), static_cast<int>(QQmlComponent::Ready), "inline QML component is ready");
    if (component.isError()) {
        for (const QQmlError& error : component.errors()) {
            std::cerr << error.toString().toStdString() << '\n';
        }
    }
    expectTrue(!component.isError(), "inline QML compiles");
    std::unique_ptr<QObject> object(component.create(engine->rootContext()));
    if (object == nullptr && component.isError()) {
        for (const QQmlError& error : component.errors()) {
            std::cerr << error.toString().toStdString() << '\n';
        }
    }
    expectTrue(object != nullptr, "inline QML object is created");
    return LoadedQmlObject{std::move(engine), std::move(object)};
}

}  // namespace etfdt::tests::shell_accounting_qml_smoke
