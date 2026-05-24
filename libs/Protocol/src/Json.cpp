#include "Protocol/Json.h"

#include <cctype>
#include <iomanip>
#include <sstream>

namespace etfdt::protocol {
namespace {

std::string trimCopy(std::string_view value)
{
    auto first = value.begin();
    auto last = value.end();

    while (first != last
           && std::isspace(static_cast<unsigned char>(*first)) != 0) {
        ++first;
    }

    while (first != last
           && std::isspace(static_cast<unsigned char>(*(last - 1))) != 0) {
        --last;
    }

    return std::string(first, last);
}

}  // namespace

std::string escapeJsonString(std::string_view value)
{
    std::string escaped;
    escaped.reserve(value.size());

    for (const char ch : value) {
        switch (ch) {
        case '"':
            escaped += "\\\"";
            break;
        case '\\':
            escaped += "\\\\";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            escaped += "\\r";
            break;
        case '\t':
            escaped += "\\t";
            break;
        default:
            if (static_cast<unsigned char>(ch) < 0x20) {
                std::ostringstream stream;
                stream << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                       << static_cast<int>(static_cast<unsigned char>(ch));
                escaped += stream.str();
            }
            else {
                escaped.push_back(ch);
            }
            break;
        }
    }

    return escaped;
}

bool isLikelyJsonObjectOrArray(std::string_view value)
{
    const std::string trimmed = trimCopy(value);
    if (trimmed.size() < 2) {
        return false;
    }

    const char first = trimmed.front();
    const char last = trimmed.back();
    return (first == '{' && last == '}') || (first == '[' && last == ']');
}

}  // namespace etfdt::protocol
