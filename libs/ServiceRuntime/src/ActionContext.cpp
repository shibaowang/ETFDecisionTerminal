#include "ServiceRuntime/ActionContext.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <utility>

namespace etfdt::service_runtime {

std::string currentUtcTimestamp()
{
    const auto now = std::chrono::system_clock::now();
    const auto seconds = std::chrono::system_clock::to_time_t(now);

    std::tm utc {};
#if defined(_WIN32)
    gmtime_s(&utc, &seconds);
#else
    gmtime_r(&seconds, &utc);
#endif

    std::ostringstream stream;
    stream << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
    return stream.str();
}

ActionContext makeActionContext(
    const etfdt::protocol::MessageEnvelope& request,
    etfdt::protocol::ServiceName serviceName,
    std::optional<std::string> remotePeerId,
    std::map<std::string, std::string> metadata)
{
    ActionContext context;
    context.request = request;
    context.serviceName = serviceName;
    context.receivedAtUtc = currentUtcTimestamp();
    context.traceId = request.traceId;
    context.remotePeerId = std::move(remotePeerId);
    context.metadata = std::move(metadata);
    return context;
}

}  // namespace etfdt::service_runtime
