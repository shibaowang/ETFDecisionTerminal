#pragma once

#include "ShellCore/ShellNavigationTypes.h"

#include <optional>
#include <string>
#include <vector>

namespace etfdt::shell {

class ShellPageRegistry final {
public:
    [[nodiscard]] static std::vector<ShellPageMetadata> pages();
    [[nodiscard]] static std::vector<ShellNavigationItem> listNavigationItems();
    [[nodiscard]] static std::optional<ShellPageMetadata> metadataForKey(const std::string& key);
    [[nodiscard]] static ShellPageMetadata fallbackMetadata(const std::string& key);
};

}  // namespace etfdt::shell
