#include "ShellAccountingQmlTypeRegistrationImplementationTestUtils.h"

#include <QCoreApplication>
#include <QQmlComponent>
#include <QQmlEngine>

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_qml_type_registration_implementation {
namespace {

bool hasExtension(const std::filesystem::path& path, const std::vector<std::string>& extensions)
{
    const auto extension = path.extension().string();
    for (const auto& allowed : extensions) {
        if (extension == allowed) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> splitLines(const std::string& text)
{
    std::vector<std::string> lines;
    std::istringstream input(text);
    std::string line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }
    return lines;
}

bool isAllowedNegativeQmlStatement(const std::string& line, const QString& token)
{
    const auto tokenText = token.toStdString();
    return (tokenText == "DataServiceClient" || tokenText == "SQLite" || tokenText == "AccountingEngine") &&
           line.find("QML ") != std::string::npos &&
           line.find(tokenText) != std::string::npos;
}

}  // namespace

std::filesystem::path sourceRoot(int argc, char** argv)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == "--source-root") {
            return argv[i + 1];
        }
    }
    return std::filesystem::current_path();
}

std::string readTextFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions)
{
    std::vector<std::filesystem::path> result;
    if (!std::filesystem::exists(root)) {
        return result;
    }
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file() && hasExtension(entry.path(), extensions)) {
            result.push_back(entry.path());
        }
    }
    return result;
}

QString importOnlyQml()
{
    return QStringLiteral(R"QML(
import QtQml
import ETFDecisionTerminal.ShellAccounting 1.0

QtObject {
    property bool imported: true
}
)QML");
}

QString instantiateTypeQml(const QString& typeName)
{
    return QStringLiteral("import QtQml\n"
                          "import ETFDecisionTerminal.ShellAccounting 1.0\n\n") +
           typeName + QStringLiteral(" {}\n");
}

QString componentErrors(const QString& qmlSource)
{
    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.setData(qmlSource.toUtf8(), QUrl());
    for (int i = 0; component.status() == QQmlComponent::Loading && i < 20; ++i) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    }

    QStringList errors;
    for (const QQmlError& error : component.errors()) {
        errors.append(error.toString());
    }
    return errors.join(QStringLiteral("\n"));
}

QStringList allowlistTypes()
{
    return {
        QStringLiteral("ShellAccountingPresenter"),
        QStringLiteral("ShellAccountingStatusObject"),
        QStringLiteral("ShellAccountingIssueListModel"),
        QStringLiteral("ShellPositionListModel"),
    };
}

QStringList forbiddenTypes()
{
    return {
        QStringLiteral("DataServiceClient"),
        QStringLiteral("DataServiceApi"),
        QStringLiteral("ShellAccountingDataServiceAdapter"),
        QStringLiteral("ShellAccountingDataServiceClientPortAdapter"),
        QStringLiteral("DataAccess"),
        QStringLiteral("SQLite"),
        QStringLiteral("SQLiteConnection"),
        QStringLiteral("AccountingEngine"),
        QStringLiteral("AccountingReplayEngine"),
        QStringLiteral("AccountingTradeFactReader"),
        QStringLiteral("StrategyEngine"),
        QStringLiteral("MarketEngine"),
        QStringLiteral("BrokerOrderApi"),
        QStringLiteral("WriteService"),
    };
}

QStringList forbiddenMethodTokens()
{
    return {
        QStringLiteral("buy("),
        QStringLiteral("sell("),
        QStringLiteral("confirmTradeDraft"),
        QStringLiteral("brokerOrder"),
        QStringLiteral("submitOrder"),
        QStringLiteral("strategyExecute"),
        QStringLiteral("executeStrategy"),
        QStringLiteral("data.audit.append"),
        QStringLiteral("writeAction"),
        QStringLiteral("snapshotWrite"),
        QStringLiteral("tradeLogWrite"),
        QStringLiteral("positionSnapshotWrite"),
        QStringLiteral("cashSnapshotWrite"),
        QStringLiteral("portfolioSummaryWrite"),
        QStringLiteral("setWriteEnabled"),
        QStringLiteral("enableTrading"),
        QStringLiteral("submitTrade"),
        QStringLiteral("placeOrder"),
        QStringLiteral("generateSuggestion"),
        QStringLiteral("WRITE true"),
    };
}

QStringList productionQmlBindingTokens()
{
    return {
        QStringLiteral("ShellAccountingStatusObject"),
        QStringLiteral("ShellAccountingIssueListModel"),
        QStringLiteral("ShellPositionListModel"),
        QStringLiteral("DataServiceClient"),
        QStringLiteral("AccountingEngine"),
        QStringLiteral("SQLite"),
    };
}

bool textContainsAny(const std::string& text, const QStringList& tokens)
{
    const auto lines = splitLines(text);
    for (std::size_t lineIndex = 0; lineIndex < lines.size(); ++lineIndex) {
        for (const QString& token : tokens) {
            if (lines[lineIndex].find(token.toStdString()) == std::string::npos ||
                isAllowedNegativeQmlStatement(lines[lineIndex], token)) {
                continue;
            }
            std::cerr << "forbidden token: " << token.toStdString()
                      << " at line " << (lineIndex + 1) << '\n';
            return true;
        }
    }
    return false;
}

bool productionQmlContainsAny(
    const std::filesystem::path& root,
    const QStringList& tokens)
{
    const auto files = filesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"});
    for (const auto& file : files) {
        const auto text = readTextFile(file);
        if (textContainsAny(text, tokens)) {
            std::cerr << file.generic_string() << '\n';
            return true;
        }
    }
    return false;
}

void expectTrue(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

}  // namespace etfdt::tests::shell_accounting_qml_type_registration_implementation
