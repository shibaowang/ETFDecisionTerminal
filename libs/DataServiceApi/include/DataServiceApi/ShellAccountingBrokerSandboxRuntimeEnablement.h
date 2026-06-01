#pragma once

namespace etfdt::dataservice {

struct ShellAccountingBrokerSandboxRuntimeEnablementState {
    bool enabled{false};
    bool available{false};
    bool failClosed{true};
    const char* runtimeMode{"sandbox_runtime_disabled"};
    const char* status{"disabled"};
    const char* errorCode{"BROKER_SANDBOX_RUNTIME_DISABLED"};
};

[[nodiscard]] ShellAccountingBrokerSandboxRuntimeEnablementState
defaultShellAccountingBrokerSandboxRuntimeEnablementState() noexcept;

}  // namespace etfdt::dataservice
