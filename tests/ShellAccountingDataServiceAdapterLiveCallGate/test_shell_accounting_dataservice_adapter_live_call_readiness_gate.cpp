#include "ShellAccountingDataServiceAdapterLiveCallGate.h"

#include <iostream>
#include <string_view>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_dataservice_adapter_live_call_gate;

    const auto gate = liveCallReadinessGate();

    expectTrue(gate.requiresExplicitAuthorization, "live-call requires explicit authorization");
    expectTrue(gate.qmlRequiresSeparateAuthorization, "QML requires separate authorization");
    expectTrue(gate.sqliteForbidden, "SQLite access is forbidden");
    expectTrue(gate.accountingEngineForbidden, "AccountingEngine access is forbidden");
    expectTrue(gate.rollbackToNotConnectedRequired, "rollback to not-connected is required");
    expectTrue(gate.transportRepeatRequired, "transport 50 repeat is required");

    return gFailures == 0 ? 0 : 1;
}
