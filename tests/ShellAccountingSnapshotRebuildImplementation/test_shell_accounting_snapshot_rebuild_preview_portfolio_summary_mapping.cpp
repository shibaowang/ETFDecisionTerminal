#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <QJsonArray>

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "portfolio.pnl.summary");
    const auto preview = payload.value("snapshotRebuildPreview").toObject();
    const auto summaries = preview.value("portfolioSummaries").toArray();

    bool ok = expectReplayPayload(payload, "portfolio.pnl.summary", "portfolio preview");
    ok &= expect(!summaries.isEmpty(), "portfolio preview row exists");
    ok &= expect(preview.value("status").toString().toStdString() == "READY"
                     || preview.value("status").toString().toStdString() == "ISSUE",
                 "portfolio preview status mapped");
    ok &= expect(hasIssue(payload, "MARKET_PRICE_MISSING"), "portfolio preview preserves missing price issue");
    return ok ? 0 : 1;
}
