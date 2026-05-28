#include "ShellServices/ShellAccountingIssueListModel.h"

#include <QCoreApplication>

#include <cstdlib>
#include <iostream>

namespace {

void expectTrue(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

std::string asString(const QVariant& value)
{
    return value.toString().toStdString();
}

}  // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    etfdt::shell_services::ShellAccountingIssueListModel model;
    model.setIssues({
        {"MARKET_PRICE_MISSING", "WARNING", "Market price is missing.", false, "position.list"},
        {"NEGATIVE_CASH", "ERROR", "Cash balance is negative.", true, "cash.summary"},
        {"UNKNOWN_DOMAIN_ISSUE", "WARNING", "Unknown issue remains visible.", false, "unknown"},
    });

    expectTrue(model.rowCount() == 3, "issue model rowCount is correct");
    expectTrue(model.blockingIssueCount() == 1, "blocking issue count is correct");
    expectTrue(model.roleNames().contains(etfdt::shell_services::ShellAccountingIssueListModel::CodeRole), "code role exists");
    expectTrue(model.roleNames().contains(etfdt::shell_services::ShellAccountingIssueListModel::LevelRole), "level role exists");
    expectTrue(model.roleNames().contains(etfdt::shell_services::ShellAccountingIssueListModel::MessageRole), "message role exists");
    expectTrue(model.roleNames().contains(etfdt::shell_services::ShellAccountingIssueListModel::BlockingRole), "blocking role exists");
    expectTrue(model.roleNames().contains(etfdt::shell_services::ShellAccountingIssueListModel::SourceIdRole), "sourceId role exists");

    const QModelIndex warning = model.index(0, 0);
    expectTrue(asString(model.data(warning, etfdt::shell_services::ShellAccountingIssueListModel::CodeRole)) == "MARKET_PRICE_MISSING", "warning issue code is preserved");
    expectTrue(asString(model.data(warning, etfdt::shell_services::ShellAccountingIssueListModel::LevelRole)) == "WARNING", "warning issue level is preserved");
    expectTrue(asString(model.data(warning, etfdt::shell_services::ShellAccountingIssueListModel::MessageRole)) == "Market price is missing.", "warning issue message is preserved");
    expectTrue(!model.data(warning, etfdt::shell_services::ShellAccountingIssueListModel::BlockingRole).toBool(), "warning issue blocking flag is preserved");
    expectTrue(asString(model.data(warning, etfdt::shell_services::ShellAccountingIssueListModel::SourceIdRole)) == "position.list", "warning issue sourceId is preserved");

    const QModelIndex blocking = model.index(1, 0);
    expectTrue(asString(model.data(blocking, etfdt::shell_services::ShellAccountingIssueListModel::CodeRole)) == "NEGATIVE_CASH", "blocking issue code is preserved");
    expectTrue(model.data(blocking, etfdt::shell_services::ShellAccountingIssueListModel::BlockingRole).toBool(), "blocking issue flag is preserved");

    const QModelIndex unknown = model.index(2, 0);
    expectTrue(asString(model.data(unknown, etfdt::shell_services::ShellAccountingIssueListModel::CodeRole)) == "UNKNOWN_DOMAIN_ISSUE", "unknown issue is not swallowed");

    model.clearIssues();
    expectTrue(model.rowCount() == 0, "clearIssues empties model");

    return 0;
}
