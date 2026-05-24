#include "Watchdog/ServiceProcessManager.h"

#include "DataServiceClient/DataServiceClientApi.h"

#include <QDateTime>

#include <filesystem>
#include <utility>

namespace etfdt::watchdog {
namespace {

std::string currentUtcTimestamp()
{
    return QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString();
}

QStringList toQStringList(const std::vector<std::string>& values)
{
    QStringList result;
    for (const auto& value : values) {
        result.append(QString::fromStdString(value));
    }
    return result;
}

bool responseSucceeded(
    const etfdt::data_service_client::DataServiceClientResult<etfdt::protocol::ProtocolResponse>&
        response)
{
    return response.hasValue() && response.value().success;
}

}  // namespace

ServiceProcessManager::~ServiceProcessManager()
{
    stopAll();
}

WatchdogResult<bool> ServiceProcessManager::addService(ServiceProcessConfig config)
{
    if (config.serviceName.empty()) {
        return WatchdogResult<bool>::failure("serviceName is required");
    }
    if (services_.find(config.serviceName) != services_.end()) {
        return WatchdogResult<bool>::failure("Service already exists: " + config.serviceName);
    }

    ManagedService service;
    service.status.serviceName = config.serviceName;
    service.status.healthState = HealthState::Stopped;
    service.status.message = "Service configured";
    service.config = std::move(config);
    service.process = std::make_unique<QProcess>();
    services_.emplace(service.config.serviceName, std::move(service));
    return WatchdogResult<bool>::success(true);
}

WatchdogResult<bool> ServiceProcessManager::startService(const std::string& serviceName)
{
    auto* service = findService(serviceName);
    if (service == nullptr) {
        return WatchdogResult<bool>::failure("Unknown service: " + serviceName);
    }
    if (!service->config.enabled) {
        return WatchdogResult<bool>::failure("Service is disabled: " + serviceName);
    }
    if (service->config.executablePath.empty()) {
        return WatchdogResult<bool>::failure("executablePath is required for " + serviceName);
    }
    if (!std::filesystem::exists(service->config.executablePath)) {
        service->status.healthState = HealthState::Failed;
        service->status.lastErrorMessage =
            "Executable does not exist: " + service->config.executablePath;
        return WatchdogResult<bool>::failure(service->status.lastErrorMessage);
    }

    refreshRunningStatus(*service);
    if (service->status.running) {
        return WatchdogResult<bool>::success(true);
    }

    service->process = std::make_unique<QProcess>();
    service->process->setProgram(QString::fromStdString(service->config.executablePath));
    service->process->setArguments(toQStringList(service->config.arguments));
    if (!service->config.workingDirectory.empty()) {
        service->process->setWorkingDirectory(QString::fromStdString(service->config.workingDirectory));
    }
    service->process->setProcessChannelMode(QProcess::MergedChannels);

    service->status.healthState = HealthState::Starting;
    service->status.message = "Starting service";
    service->status.lastStartTimeUtc = currentUtcTimestamp();
    service->status.lastErrorMessage.clear();
    service->process->start();
    if (!service->process->waitForStarted(service->config.startupTimeoutMs)) {
        service->status.running = false;
        service->status.pid = 0;
        service->status.healthState = HealthState::Failed;
        service->status.lastErrorMessage =
            "Failed to start service: " + service->process->errorString().toStdString();
        return WatchdogResult<bool>::failure(service->status.lastErrorMessage);
    }

    refreshRunningStatus(*service);
    service->status.message = "Service process started";
    return WatchdogResult<bool>::success(true);
}

WatchdogResult<bool> ServiceProcessManager::stopService(const std::string& serviceName)
{
    auto* service = findService(serviceName);
    if (service == nullptr) {
        return WatchdogResult<bool>::failure("Unknown service: " + serviceName);
    }

    refreshRunningStatus(*service);
    if (!service->status.running) {
        service->status.healthState = HealthState::Stopped;
        service->status.message = "Service already stopped";
        return WatchdogResult<bool>::success(true);
    }

    service->process->terminate();
    if (!service->process->waitForFinished(service->config.shutdownTimeoutMs)) {
        service->process->kill();
        (void)service->process->waitForFinished(service->config.shutdownTimeoutMs);
    }

    service->status.lastStopTimeUtc = currentUtcTimestamp();
    if (service->process->state() == QProcess::NotRunning) {
        service->status.lastExitCode = service->process->exitCode();
    }
    refreshRunningStatus(*service);
    service->status.healthState = HealthState::Stopped;
    service->status.message = "Service stopped";
    return WatchdogResult<bool>::success(true);
}

void ServiceProcessManager::stopAll()
{
    for (auto& entry : services_) {
        (void)stopService(entry.first);
    }
}

WatchdogResult<bool> ServiceProcessManager::isRunning(const std::string& serviceName) const
{
    const auto* service = findService(serviceName);
    if (service == nullptr) {
        return WatchdogResult<bool>::failure("Unknown service: " + serviceName);
    }
    return WatchdogResult<bool>::success(
        service->process && service->process->state() != QProcess::NotRunning);
}

WatchdogResult<ServiceProcessStatus> ServiceProcessManager::status(
    const std::string& serviceName) const
{
    const auto* service = findService(serviceName);
    if (service == nullptr) {
        return WatchdogResult<ServiceProcessStatus>::failure("Unknown service: " + serviceName);
    }
    return WatchdogResult<ServiceProcessStatus>::success(service->status);
}

std::vector<ServiceProcessStatus> ServiceProcessManager::listStatuses() const
{
    std::vector<ServiceProcessStatus> statuses;
    statuses.reserve(services_.size());
    for (const auto& entry : services_) {
        statuses.push_back(entry.second.status);
    }
    return statuses;
}

WatchdogResult<ServiceProcessStatus> ServiceProcessManager::checkHealth(
    const std::string& serviceName)
{
    auto* service = findService(serviceName);
    if (service == nullptr) {
        return WatchdogResult<ServiceProcessStatus>::failure("Unknown service: " + serviceName);
    }

    refreshRunningStatus(*service);
    if (!service->status.running) {
        service->status.healthState = HealthState::Stopped;
        service->status.message = "Service process is not running";
        return WatchdogResult<ServiceProcessStatus>::success(service->status);
    }
    if (service->config.socketName.empty()) {
        service->status.healthState = HealthState::Unhealthy;
        service->status.message = "Service has no socketName configured";
        return WatchdogResult<ServiceProcessStatus>::success(service->status);
    }

    etfdt::data_service_client::DataServiceClient client(
        etfdt::protocol::ServiceName::ETFWatchdog,
        etfdt::protocol::ServiceName::ETFDataService);
    auto connected = client.connect(service->config.socketName, service->config.healthTimeoutMs);
    if (!connected) {
        service->status.healthState = HealthState::Unhealthy;
        service->status.message = connected.error().message;
        return WatchdogResult<ServiceProcessStatus>::success(service->status);
    }

    const bool pingOk = responseSucceeded(client.ping(service->config.healthTimeoutMs));
    const bool healthOk = responseSucceeded(client.health(service->config.healthTimeoutMs));
    client.disconnect();

    service->status.healthState = (pingOk && healthOk) ? HealthState::Healthy : HealthState::Unhealthy;
    service->status.message = (pingOk && healthOk) ? "DataService ping and data.health succeeded"
                                                   : "DataService health check failed";
    return WatchdogResult<ServiceProcessStatus>::success(service->status);
}

std::vector<ServiceProcessStatus> ServiceProcessManager::checkAllHealth()
{
    std::vector<ServiceProcessStatus> statuses;
    statuses.reserve(services_.size());
    for (auto& entry : services_) {
        auto checked = checkHealth(entry.first);
        if (checked) {
            statuses.push_back(checked.value());
        }
    }
    return statuses;
}

ServiceProcessManager::ManagedService* ServiceProcessManager::findService(
    const std::string& serviceName)
{
    auto it = services_.find(serviceName);
    return it == services_.end() ? nullptr : &it->second;
}

const ServiceProcessManager::ManagedService* ServiceProcessManager::findService(
    const std::string& serviceName) const
{
    auto it = services_.find(serviceName);
    return it == services_.end() ? nullptr : &it->second;
}

void ServiceProcessManager::refreshRunningStatus(ManagedService& service)
{
    if (!service.process || service.process->state() == QProcess::NotRunning) {
        service.status.running = false;
        service.status.pid = 0;
        return;
    }
    service.status.running = true;
    service.status.pid = static_cast<std::int64_t>(service.process->processId());
}

}  // namespace etfdt::watchdog
