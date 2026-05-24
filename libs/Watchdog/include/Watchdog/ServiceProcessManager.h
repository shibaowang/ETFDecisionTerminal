#pragma once

#include "Watchdog/ServiceProcessConfig.h"
#include "Watchdog/ServiceProcessStatus.h"

#include <QProcess>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace etfdt::watchdog {

struct WatchdogError final {
    std::string message;
};

template <typename T>
class WatchdogResult final {
public:
    static WatchdogResult success(T value)
    {
        return WatchdogResult(std::move(value));
    }

    static WatchdogResult failure(std::string message)
    {
        return WatchdogResult(WatchdogError{std::move(message)});
    }

    [[nodiscard]] bool hasValue() const noexcept
    {
        return hasValue_;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return hasValue();
    }

    [[nodiscard]] const T& value() const
    {
        return value_;
    }

    [[nodiscard]] T& value()
    {
        return value_;
    }

    [[nodiscard]] const WatchdogError& error() const noexcept
    {
        return error_;
    }

private:
    explicit WatchdogResult(T value)
        : hasValue_(true)
        , value_(std::move(value))
    {
    }

    explicit WatchdogResult(WatchdogError error)
        : hasValue_(false)
        , error_(std::move(error))
    {
    }

    bool hasValue_ = false;
    T value_{};
    WatchdogError error_;
};

class ServiceProcessManager final {
public:
    ServiceProcessManager() = default;
    ~ServiceProcessManager();

    ServiceProcessManager(const ServiceProcessManager&) = delete;
    ServiceProcessManager& operator=(const ServiceProcessManager&) = delete;

    [[nodiscard]] WatchdogResult<bool> addService(ServiceProcessConfig config);
    [[nodiscard]] WatchdogResult<bool> startService(const std::string& serviceName);
    [[nodiscard]] WatchdogResult<bool> stopService(const std::string& serviceName);
    void stopAll();

    [[nodiscard]] WatchdogResult<bool> isRunning(const std::string& serviceName) const;
    [[nodiscard]] WatchdogResult<ServiceProcessStatus> status(
        const std::string& serviceName) const;
    [[nodiscard]] std::vector<ServiceProcessStatus> listStatuses() const;
    [[nodiscard]] WatchdogResult<ServiceProcessStatus> checkHealth(
        const std::string& serviceName);
    [[nodiscard]] std::vector<ServiceProcessStatus> checkAllHealth();

private:
    struct ManagedService final {
        ServiceProcessConfig config;
        ServiceProcessStatus status;
        std::unique_ptr<QProcess> process;
    };

    [[nodiscard]] ManagedService* findService(const std::string& serviceName);
    [[nodiscard]] const ManagedService* findService(const std::string& serviceName) const;
    static void refreshRunningStatus(ManagedService& service);

    std::map<std::string, ManagedService> services_;
};

}  // namespace etfdt::watchdog
