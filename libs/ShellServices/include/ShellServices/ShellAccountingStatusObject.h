#pragma once

#include "ShellServices/ShellAccountingIssue.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"
#include "ShellServices/ShellAccountingState.h"

#include <QObject>
#include <QString>

#include <cstddef>
#include <string>
#include <vector>

namespace etfdt::shell_services {

class ShellAccountingStatusObject final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString actionNameText READ actionNameText)
    Q_PROPERTY(QString stateTextQml READ stateTextQml)
    Q_PROPERTY(bool implemented READ implemented)
    Q_PROPERTY(bool readOnly READ readOnly)
    Q_PROPERTY(bool writeEnabled READ writeEnabled)

public:
    explicit ShellAccountingStatusObject(QObject* parent = nullptr);

    void reset();
    void applyStateSnapshot(ShellAccountingStateSnapshot snapshot);

    [[nodiscard]] const std::string& actionName() const noexcept;
    [[nodiscard]] QString actionNameText() const;
    [[nodiscard]] ShellAccountingViewState state() const noexcept;
    [[nodiscard]] const char* stateText() const noexcept;
    [[nodiscard]] QString stateTextQml() const;
    [[nodiscard]] bool implemented() const noexcept;
    [[nodiscard]] bool readOnly() const noexcept;
    [[nodiscard]] bool writeEnabled() const noexcept;
    [[nodiscard]] bool loading() const noexcept;
    [[nodiscard]] bool unavailable() const noexcept;
    [[nodiscard]] bool warning() const noexcept;
    [[nodiscard]] bool error() const noexcept;
    [[nodiscard]] bool stale() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] const std::string& dataQualityStatus() const noexcept;
    [[nodiscard]] const std::string& payloadStatus() const noexcept;
    [[nodiscard]] std::size_t issueCount() const noexcept;
    [[nodiscard]] std::size_t blockingIssueCount() const noexcept;

private:
    std::string actionName_;
    ShellAccountingViewState state_ = ShellAccountingViewState::Idle;
    bool implemented_ = false;
    bool readOnly_ = true;
    bool writeEnabled_ = false;
    std::string dataQualityStatus_ = "IDLE";
    std::string payloadStatus_;
    std::vector<ShellAccountingIssue> issues_;
};

}  // namespace etfdt::shell_services
