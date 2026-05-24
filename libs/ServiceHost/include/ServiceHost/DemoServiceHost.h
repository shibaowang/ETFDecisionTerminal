#pragma once

#include "ServiceHost/ActionServiceHost.h"

#include "Protocol/Protocol.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <cstddef>
#include <string>

namespace etfdt::service_host {

class DemoServiceHost final {
public:
    explicit DemoServiceHost(
        etfdt::protocol::ServiceName serviceName =
            etfdt::protocol::ServiceName::ETFDataService);
    ~DemoServiceHost();

    DemoServiceHost(const DemoServiceHost&) = delete;
    DemoServiceHost& operator=(const DemoServiceHost&) = delete;
    DemoServiceHost(DemoServiceHost&&) = delete;
    DemoServiceHost& operator=(DemoServiceHost&&) = delete;

    [[nodiscard]] etfdt::transport::TransportResult<bool> listen(
        const std::string& socketName);
    void close();
    [[nodiscard]] bool isListening() const noexcept;
    [[nodiscard]] std::size_t clientCount() const noexcept;

private:
    etfdt::service_runtime::ActionDispatcher dispatcher_;
    ActionServiceHost host_;
};

}  // namespace etfdt::service_host
