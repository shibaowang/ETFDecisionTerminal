#include "Transport/FrameCodec.h"

#include "Protocol/Json.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>

namespace etfdt::transport {
namespace {

constexpr std::size_t kLengthPrefixBytes = 4;

std::uint32_t readBigEndianUint32(const std::vector<std::uint8_t>& bytes)
{
    return (static_cast<std::uint32_t>(bytes[0]) << 24U)
        | (static_cast<std::uint32_t>(bytes[1]) << 16U)
        | (static_cast<std::uint32_t>(bytes[2]) << 8U)
        | static_cast<std::uint32_t>(bytes[3]);
}

void appendBigEndianUint32(std::vector<std::uint8_t>& bytes, std::uint32_t value)
{
    bytes.push_back(static_cast<std::uint8_t>((value >> 24U) & 0xFFU));
    bytes.push_back(static_cast<std::uint8_t>((value >> 16U) & 0xFFU));
    bytes.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xFFU));
    bytes.push_back(static_cast<std::uint8_t>(value & 0xFFU));
}

TransportResult<bool> validatePayload(
    const std::string& json,
    std::uint32_t maxFrameBytes)
{
    if (json.empty()) {
        return TransportResult<bool>::failure("Frame payload must not be empty");
    }
    if (json.size() > maxFrameBytes) {
        return TransportResult<bool>::failure("Frame payload exceeds maximum frame size");
    }
    if (json.size() > static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max())) {
        return TransportResult<bool>::failure("Frame payload exceeds uint32 length prefix");
    }
    if (!etfdt::protocol::isLikelyJsonObjectOrArray(json)) {
        return TransportResult<bool>::failure(
            "Frame payload must look like a JSON object or array");
    }
    return TransportResult<bool>::success(true);
}

}  // namespace

FrameCodec::FrameCodec(std::uint32_t maxFrameBytes)
    : maxFrameBytes_(maxFrameBytes)
{
}

TransportResult<std::vector<std::uint8_t>> FrameCodec::encodeFrame(
    const std::string& json,
    std::uint32_t maxFrameBytes)
{
    auto validation = validatePayload(json, maxFrameBytes);
    if (!validation) {
        return TransportResult<std::vector<std::uint8_t>>::failure(validation.error().message);
    }

    std::vector<std::uint8_t> frame;
    frame.reserve(kLengthPrefixBytes + json.size());
    appendBigEndianUint32(frame, static_cast<std::uint32_t>(json.size()));
    frame.insert(frame.end(), json.begin(), json.end());
    return TransportResult<std::vector<std::uint8_t>>::success(std::move(frame));
}

TransportResult<bool> FrameCodec::appendData(const std::uint8_t* data, std::size_t size)
{
    if (data == nullptr && size != 0U) {
        return TransportResult<bool>::failure("appendData received null data pointer");
    }
    if (size == 0U) {
        return TransportResult<bool>::success(true);
    }
    buffer_.insert(buffer_.end(), data, data + size);
    return TransportResult<bool>::success(true);
}

TransportResult<bool> FrameCodec::appendData(const std::vector<std::uint8_t>& data)
{
    return appendData(data.data(), data.size());
}

TransportResult<std::optional<std::string>> FrameCodec::tryDecodeNextFrame()
{
    if (buffer_.size() < kLengthPrefixBytes) {
        return TransportResult<std::optional<std::string>>::success(std::nullopt);
    }

    const std::uint32_t payloadLength = readBigEndianUint32(buffer_);
    if (payloadLength == 0U) {
        reset();
        return TransportResult<std::optional<std::string>>::failure(
            "Frame payload length must not be zero");
    }
    if (payloadLength > maxFrameBytes_) {
        reset();
        return TransportResult<std::optional<std::string>>::failure(
            "Frame payload exceeds maximum frame size");
    }

    const std::size_t frameLength = kLengthPrefixBytes + static_cast<std::size_t>(payloadLength);
    if (buffer_.size() < frameLength) {
        return TransportResult<std::optional<std::string>>::success(std::nullopt);
    }

    std::string json(
        buffer_.begin() + static_cast<std::ptrdiff_t>(kLengthPrefixBytes),
        buffer_.begin() + static_cast<std::ptrdiff_t>(frameLength));
    auto validation = validatePayload(json, maxFrameBytes_);
    if (!validation) {
        reset();
        return TransportResult<std::optional<std::string>>::failure(validation.error().message);
    }

    buffer_.erase(buffer_.begin(), buffer_.begin() + static_cast<std::ptrdiff_t>(frameLength));
    return TransportResult<std::optional<std::string>>::success(std::move(json));
}

void FrameCodec::reset()
{
    buffer_.clear();
}

}  // namespace etfdt::transport
