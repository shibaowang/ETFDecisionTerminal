#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    const auto docs = docs86Text(sourceRoot(argc, argv));
    int failures = 0;
    for (const auto& token : {
             "rollback",
             "fallback to TASK-137 read-only facts query",
             "no fake data",
             "must not write database",
             "must not generate TradeDraft",
         }) {
        failures += expect(docs.find(token) != std::string::npos, std::string("docs/86 rollback token ") + token) ? 0 : 1;
    }
    return failures == 0 ? 0 : 1;
}
