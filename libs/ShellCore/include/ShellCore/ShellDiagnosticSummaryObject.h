#pragma once

#include "ShellCore/ShellDiagnosticViewModel.h"

#include <QObject>
#include <QString>

namespace etfdt::shell {

class ShellDiagnosticSummaryObject final : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool ok READ ok NOTIFY summaryChanged)
    Q_PROPERTY(QString title READ title NOTIFY summaryChanged)
    Q_PROPERTY(QString summaryText READ summaryText NOTIFY summaryChanged)
    Q_PROPERTY(int totalServices READ totalServices NOTIFY summaryChanged)
    Q_PROPERTY(int enabledServices READ enabledServices NOTIFY summaryChanged)
    Q_PROPERTY(int disabledServices READ disabledServices NOTIFY summaryChanged)
    Q_PROPERTY(int errorCount READ errorCount NOTIFY summaryChanged)
    Q_PROPERTY(int warningCount READ warningCount NOTIFY summaryChanged)
    Q_PROPERTY(int blockedServiceCount READ blockedServiceCount NOTIFY summaryChanged)

public:
    explicit ShellDiagnosticSummaryObject(QObject* parent = nullptr);

    [[nodiscard]] bool ok() const;
    [[nodiscard]] QString title() const;
    [[nodiscard]] QString summaryText() const;
    [[nodiscard]] int totalServices() const;
    [[nodiscard]] int enabledServices() const;
    [[nodiscard]] int disabledServices() const;
    [[nodiscard]] int errorCount() const;
    [[nodiscard]] int warningCount() const;
    [[nodiscard]] int blockedServiceCount() const;

    void setViewModel(const ShellDiagnosticViewModel& viewModel);
    void clear();

signals:
    void summaryChanged();

private:
    ShellDiagnosticViewModel viewModel_;
};

}  // namespace etfdt::shell
