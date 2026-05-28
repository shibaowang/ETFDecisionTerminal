#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingServiceAdapter.h"

#include <iostream>
#include <string_view>
#include <type_traits>

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
    using etfdt::shell_services::ShellAccountingDataServiceAdapter;
    using etfdt::shell_services::ShellAccountingServiceAdapter;

    static_assert(std::is_base_of_v<ShellAccountingServiceAdapter, ShellAccountingDataServiceAdapter>);
    static_assert(std::is_final_v<ShellAccountingDataServiceAdapter>);

    ShellAccountingDataServiceAdapter adapter;
    ShellAccountingServiceAdapter* serviceAdapter = &adapter;

    expectTrue(serviceAdapter != nullptr, "skeleton can be used through ShellAccountingServiceAdapter");
    expectTrue(!adapter.hasLiveClient(), "skeleton defaults to hasLiveClient=false");
    expectTrue(adapter.readOnly(), "skeleton readOnly=true");
    expectTrue(!adapter.writeEnabled(), "skeleton writeEnabled=false");

    return gFailures == 0 ? 0 : 1;
}
