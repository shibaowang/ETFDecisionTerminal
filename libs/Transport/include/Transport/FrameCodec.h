#pragma once

#include "Transport/TransportTypes.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace etfdt::transport {

class FrameCodec final {
public:
    static constexpr std::uint32_t kDefaultMaxFrameBytes = 1024U * 1024U;

    explicit FrameCodec(std::uint32_t maxFrameBytes = kDefaultMaxFrameBytes);

    [[nodiscard]] static TransportResult<std::vector<std::uint8_t>> encodeFrame(
        const std::string& json,
        std::uint32_t maxFrameBytes = kDefaultMaxFrameBytes);

    [[nodiscard]] TransportResult<bool> appendData(
        const std::uint8_t* data,
        std::size_t size);
    [[nodiscard]] TransportResult<bool> appendData(const std::vector<std::uint8_t>& data);

    [[nodiscard]] TransportResult<std::optional<std::string>> tryDecodeNextFrame();
    void reset();

private:
    std::uint32_t maxFrameBytes_;
    std::vector<std::uint8_t> buffer_;
};

}  // namespace etfdt::transport
