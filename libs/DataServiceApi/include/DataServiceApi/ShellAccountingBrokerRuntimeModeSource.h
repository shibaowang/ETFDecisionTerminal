#pragma once

#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"

namespace etfdt::dataservice {

class ShellAccountingBrokerRuntimeModeSource {
public:
    virtual ~ShellAccountingBrokerRuntimeModeSource() = default;

    [[nodiscard]] virtual ShellAccountingBrokerOrderPortMode brokerOrderPortMode() const noexcept = 0;
};

class DisabledShellAccountingBrokerRuntimeModeSource final : public ShellAccountingBrokerRuntimeModeSource {
public:
    [[nodiscard]] ShellAccountingBrokerOrderPortMode brokerOrderPortMode() const noexcept override;
};

class SandboxShellAccountingBrokerRuntimeModeSourceScaffold final : public ShellAccountingBrokerRuntimeModeSource {
public:
    [[nodiscard]] ShellAccountingBrokerOrderPortMode brokerOrderPortMode() const noexcept override;
};

[[nodiscard]] ShellAccountingBrokerRuntimeModeSource& defaultShellAccountingBrokerRuntimeModeSource() noexcept;

}  // namespace etfdt::dataservice
