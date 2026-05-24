#pragma once

#include "ShellCore/ShellDiagnosticFacade.h"
#include "ShellCore/ShellDiagnosticTools.h"
#include "ShellCore/ShellDiagnosticViewModel.h"

#include <string>

namespace etfdt::shell {

struct ShellDiagnosticPresenterState final {
    bool loaded = false;
    bool hasError = false;
    std::string lastError;
    std::string sourcePath;
    ShellDiagnosticViewModel viewModel;
    ShellDiagnosticAggregate aggregate;
    ShellDiagnosticFilter filter;
    ShellDiagnosticSort sort;
    ShellDiagnosticRefreshState refreshState;
};

class ShellDiagnosticPresenter final {
public:
    [[nodiscard]] bool loadFromFile(const std::string& path);
    [[nodiscard]] bool loadFromJson(const std::string& json);
    [[nodiscard]] bool loadMockHealthy();
    [[nodiscard]] bool loadMockWithWarnings();
    [[nodiscard]] bool loadMockWithErrors();
    [[nodiscard]] bool loadMockMixed();

    void setFilter(const ShellDiagnosticFilter& filter);
    void setSort(const ShellDiagnosticSort& sort);
    void clearFilter();

    [[nodiscard]] ShellDiagnosticPresenterState currentState() const;
    [[nodiscard]] ShellDiagnosticViewModel currentViewModel() const;
    [[nodiscard]] ShellDiagnosticAggregate currentAggregate() const;
    [[nodiscard]] ShellDiagnosticRefreshResult refreshIfChanged();
    [[nodiscard]] const std::string& lastError() const;

private:
    enum class SourceKind {
        None,
        File,
        Json,
        Mock,
    };

    bool loadFromJsonWithSource(const std::string& json, SourceKind sourceKind);
    void setError(const std::string& message);
    void setLoadedViewModel(const ShellDiagnosticViewModel& viewModel);
    void rebuildCurrentViewModel();

    ShellDiagnosticFacade facade_;
    ShellDiagnosticPresenterState state_;
    ShellDiagnosticViewModel baseViewModel_;
    SourceKind sourceKind_ = SourceKind::None;
};

}  // namespace etfdt::shell
