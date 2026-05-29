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
        std::make_shared<ScriptedAccountingServiceAdapter>(makeReadOnlySuccess("position.list")));

    ShellAccountingPresenter presenter;
    presenter.setController(controller);
    presenter.refreshPositionList();

    if (presenter.statusObject().state() != ShellAccountingViewState::Loaded ||
        !presenter.statusObject().implemented() ||
        !presenter.statusObject().readOnly() ||
        presenter.statusObject().writeEnabled()) {
        std::cerr << "fake read-only success must map to loaded, read-only presenter state\n";
        return 1;
    }
    return 0;
}
