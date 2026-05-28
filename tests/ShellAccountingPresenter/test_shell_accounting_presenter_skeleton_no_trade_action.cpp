#include "ShellServices/ShellAccountingPresenter.h"

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

}  // namespace

int main()
{
    etfdt::shell_services::ShellAccountingPresenter presenter;

    expectTrue(!presenter.tradeDraftGenerationEnabled(), "presenter never generates TradeDraft");
    expectTrue(!presenter.tradeSuggestionEnabled(), "presenter never generates trade suggestions");
    expectTrue(!presenter.strategyExecutionEnabled(), "presenter never executes strategy");
    expectTrue(!presenter.brokerSubmissionEnabled(), "presenter never submits broker orders");

    presenter.refreshPositionList();
    expectTrue(!presenter.tradeDraftGenerationEnabled(), "refresh does not enable TradeDraft generation");
    expectTrue(!presenter.tradeSuggestionEnabled(), "refresh does not enable trade suggestions");
    expectTrue(!presenter.strategyExecutionEnabled(), "refresh does not enable strategy execution");
    expectTrue(!presenter.brokerSubmissionEnabled(), "refresh does not enable broker submission");

    return 0;
}
