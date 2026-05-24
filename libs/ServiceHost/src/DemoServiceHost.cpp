#include "ServiceHost/DemoServiceHost.h"

namespace etfdt::service_host {

DemoServiceHost::DemoServiceHost(etfdt::protocol::ServiceName serviceName)
    : dispatcher_(serviceName)
    , host_(dispatcher_)
{
    etfdt::service_runtime::registerBuiltinActions(dispatcher_);
}

DemoServiceHost::~DemoServiceHost()
{
    close();
}

etfdt::transport::TransportResult<bool> DemoServiceHost::listen(const std::string& socketName)
{
    return host_.listen(socketName);
}

void DemoServiceHost::close()
{
    host_.close();
}

bool DemoServiceHost::isListening() const noexcept
{
    return host_.isListening();
}

std::size_t DemoServiceHost::clientCount() const noexcept
{
    return host_.clientCount();
}

}  // namespace etfdt::service_host
