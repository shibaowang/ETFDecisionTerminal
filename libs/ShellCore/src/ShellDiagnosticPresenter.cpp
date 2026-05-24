#include "ShellCore/ShellDiagnosticPresenter.h"

#include "ShellCore/ShellDiagnosticMockReportBuilder.h"

namespace etfdt::shell {

bool ShellDiagnosticPresenter::loadFromFile(const std::string& path)
{
    const auto loaded = facade_.loadFromWatchdogReportFile(path);
    if (!loaded.ok) {
        state_.sourcePath = path;
        state_.refreshState = facade_.checkReportFile(path);
        setError(loaded.errorMessage);
        sourceKind_ = SourceKind::File;
        return false;
    }

    state_.sourcePath = path;
    state_.refreshState = facade_.checkReportFile(path);
    sourceKind_ = SourceKind::File;
    setLoadedViewModel(loaded.viewModel);
    return true;
}

bool ShellDiagnosticPresenter::loadFromJson(const std::string& json)
{
    return loadFromJsonWithSource(json, SourceKind::Json);
}

bool ShellDiagnosticPresenter::loadMockHealthy()
{
    return loadFromJsonWithSource(ShellDiagnosticMockReportBuilder::healthyReportJson(), SourceKind::Mock);
}

bool ShellDiagnosticPresenter::loadMockWithWarnings()
{
    return loadFromJsonWithSource(ShellDiagnosticMockReportBuilder::warningReportJson(), SourceKind::Mock);
}

bool ShellDiagnosticPresenter::loadMockWithErrors()
{
    return loadFromJsonWithSource(ShellDiagnosticMockReportBuilder::errorReportJson(), SourceKind::Mock);
}

bool ShellDiagnosticPresenter::loadMockMixed()
{
    return loadFromJsonWithSource(ShellDiagnosticMockReportBuilder::mixedReportJson(), SourceKind::Mock);
}

void ShellDiagnosticPresenter::setFilter(const ShellDiagnosticFilter& filter)
{
    state_.filter = filter;
    rebuildCurrentViewModel();
}

void ShellDiagnosticPresenter::setSort(const ShellDiagnosticSort& sort)
{
    state_.sort = sort;
    rebuildCurrentViewModel();
}

void ShellDiagnosticPresenter::clearFilter()
{
    state_.filter = {};
    rebuildCurrentViewModel();
}

ShellDiagnosticPresenterState ShellDiagnosticPresenter::currentState() const
{
    return state_;
}

ShellDiagnosticViewModel ShellDiagnosticPresenter::currentViewModel() const
{
    return state_.viewModel;
}

ShellDiagnosticAggregate ShellDiagnosticPresenter::currentAggregate() const
{
    return state_.aggregate;
}

ShellDiagnosticRefreshResult ShellDiagnosticPresenter::refreshIfChanged()
{
    if (sourceKind_ != SourceKind::File || state_.sourcePath.empty()) {
        ShellDiagnosticRefreshResult result;
        result.ok = false;
        result.reloaded = false;
        result.refreshState = state_.refreshState;
        result.refreshState.lastLoadSucceeded = state_.loaded && !state_.hasError;
        result.errorMessage = "refreshIfChanged requires a file source";
        result.refreshState.lastError = result.errorMessage;
        state_.refreshState = result.refreshState;
        state_.hasError = true;
        state_.lastError = result.errorMessage;
        return result;
    }

    auto refreshed = facade_.refreshIfChanged(state_.sourcePath);
    if (!refreshed.ok) {
        state_.refreshState = refreshed.refreshState;
        setError(refreshed.errorMessage);
        return refreshed;
    }

    state_.refreshState = refreshed.refreshState;
    if (refreshed.reloaded) {
        setLoadedViewModel(refreshed.viewModel);
        refreshed.viewModel = state_.viewModel;
    } else {
        refreshed.viewModel = state_.viewModel;
    }
    return refreshed;
}

const std::string& ShellDiagnosticPresenter::lastError() const
{
    return state_.lastError;
}

bool ShellDiagnosticPresenter::loadFromJsonWithSource(
    const std::string& json,
    SourceKind sourceKind)
{
    const auto loaded = facade_.loadFromWatchdogReportJson(json);
    sourceKind_ = sourceKind;
    state_.sourcePath.clear();
    state_.refreshState = {};
    if (!loaded.ok) {
        setError(loaded.errorMessage);
        return false;
    }

    setLoadedViewModel(loaded.viewModel);
    return true;
}

void ShellDiagnosticPresenter::setError(const std::string& message)
{
    state_.loaded = false;
    state_.hasError = true;
    state_.lastError = message.empty() ? "unknown diagnostics presenter error" : message;
    state_.viewModel = {};
    state_.aggregate = {};
    baseViewModel_ = {};
}

void ShellDiagnosticPresenter::setLoadedViewModel(const ShellDiagnosticViewModel& viewModel)
{
    baseViewModel_ = viewModel;
    state_.loaded = true;
    state_.hasError = false;
    state_.lastError.clear();
    rebuildCurrentViewModel();
}

void ShellDiagnosticPresenter::rebuildCurrentViewModel()
{
    if (!state_.loaded) {
        return;
    }
    state_.viewModel = ShellDiagnosticFacade::applySort(
        ShellDiagnosticFacade::applyFilter(baseViewModel_, state_.filter),
        state_.sort);
    state_.aggregate = ShellDiagnosticFacade::aggregate(state_.viewModel);
}

}  // namespace etfdt::shell
