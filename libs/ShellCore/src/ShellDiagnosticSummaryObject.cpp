#include "ShellCore/ShellDiagnosticSummaryObject.h"

namespace etfdt::shell {

ShellDiagnosticSummaryObject::ShellDiagnosticSummaryObject(QObject* parent)
    : QObject(parent)
{
}

bool ShellDiagnosticSummaryObject::ok() const
{
    return viewModel_.ok;
}

QString ShellDiagnosticSummaryObject::title() const
{
    return QString::fromStdString(viewModel_.title);
}

QString ShellDiagnosticSummaryObject::summaryText() const
{
    return QString::fromStdString(viewModel_.summaryText);
}

int ShellDiagnosticSummaryObject::totalServices() const
{
    return viewModel_.totalServices;
}

int ShellDiagnosticSummaryObject::enabledServices() const
{
    return viewModel_.enabledServices;
}

int ShellDiagnosticSummaryObject::disabledServices() const
{
    return viewModel_.disabledServices;
}

int ShellDiagnosticSummaryObject::errorCount() const
{
    return viewModel_.errorCount;
}

int ShellDiagnosticSummaryObject::warningCount() const
{
    return viewModel_.warningCount;
}

int ShellDiagnosticSummaryObject::blockedServiceCount() const
{
    return viewModel_.blockedServiceCount;
}

void ShellDiagnosticSummaryObject::setViewModel(const ShellDiagnosticViewModel& viewModel)
{
    viewModel_ = viewModel;
    emit summaryChanged();
}

void ShellDiagnosticSummaryObject::clear()
{
    viewModel_ = {};
    emit summaryChanged();
}

}  // namespace etfdt::shell
