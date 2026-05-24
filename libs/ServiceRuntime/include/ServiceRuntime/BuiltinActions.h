#pragma once

#include "ServiceRuntime/ActionDispatcher.h"

namespace etfdt::service_runtime {

constexpr const char* kActionSystemPing = "system.ping";
constexpr const char* kActionSystemHealth = "system.health";

[[nodiscard]] etfdt::protocol::ProtocolResponse handleSystemPing(
    const ActionContext& context);

[[nodiscard]] etfdt::protocol::ProtocolResponse handleSystemHealth(
    const ActionContext& context);

void registerBuiltinActions(ActionDispatcher& dispatcher);

}  // namespace etfdt::service_runtime
