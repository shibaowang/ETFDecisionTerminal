#include "ShellAccountingRealDataAdapterImplementation.h"

#include "ShellServices/ShellAccountingPresenter.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"
#include "ShellServices/ShellAccountingState.h"

#include <iostream>
#include <memory>

using namespace etfdt::shell_services;
using namespace etfdt::tests::shell_accounting_real_data_adapter_implementation;

int main()
{
    auto controller = std::make_shared<ShellAccountingReadOnlyController>();
    controller->setServiceAdapter(
        std::make_shared<ScriptedAccountingServiceAdapter>(makeUnavailable("position.list")));

    ShellAccountingPresenter presenter;
    presenter.setController(controller);
    presenter.refreshPositionList();

    if (presenter.statusObject().state() != ShellAccountingViewState::Unavailable ||
        presenter.statusObject().issueCount() == 0 ||
        presenter.statusObject().writeEnabled()) {
        std::cerr << "unavailable service must map to unavailable issue state without writes\n";
        return 1;
    }
    return 0;
}
