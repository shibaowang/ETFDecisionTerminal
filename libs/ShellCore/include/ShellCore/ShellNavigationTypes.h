#pragma once

#include <string>

namespace etfdt::shell {

struct ShellNavigationItem final {
    std::string key;
    std::string title;
    std::string iconName;
    std::string category;
    bool enabled = true;
    bool placeholder = true;
    int sortOrder = 0;
};

struct ShellPageMetadata final {
    std::string key;
    std::string title;
    std::string qmlComponent;
    std::string description;
    bool placeholder = true;
    bool available = true;
    std::string category;
};

}  // namespace etfdt::shell
