#include "Watchdog/ServiceManifestStatus.h"

#include <algorithm>
#include <utility>

namespace etfdt::watchdog {
namespace {

bool isSupportedServiceName(const std::string& serviceName)
{
    return serviceName == "ETFDataService";
}

std::filesystem::path resolvePath(
    const std::filesystem::path& baseDirectory,
    const std::string& path)
{
    if (path.empty()) {
        return {};
    }
    std::filesystem::path candidate(path);
    if (candidate.is_absolute()) {
        return candidate;
    }
    return baseDirectory / candidate;
}

void addIssue(
    ServiceConfigStatus& status,
    ManifestStatusReport& report,
    ServiceConfigIssueLevel level,
    std::string code,
    std::string message)
{
    status.issues.push_back(ServiceConfigIssue{level, std::move(code), std::move(message)});
    if (level == ServiceConfigIssueLevel::Error) {
        ++report.errorCount;
    } else if (level == ServiceConfigIssueLevel::Warning) {
        ++report.warningCount;
    }
}

bool hasError(const ServiceConfigStatus& status)
{
    return std::any_of(
        status.issues.begin(),
        status.issues.end(),
        [](const ServiceConfigIssue& issue) {
            return issue.level == ServiceConfigIssueLevel::Error;
        });
}

}  // namespace

std::string_view toString(ServiceConfigIssueLevel level) noexcept
{
    switch (level) {
    case ServiceConfigIssueLevel::Info:
        return "INFO";
    case ServiceConfigIssueLevel::Warning:
        return "WARNING";
    case ServiceConfigIssueLevel::Error:
        return "ERROR";
    }
    return "UNKNOWN";
}

ManifestStatusReport buildManifestStatusReport(
    const ServiceManifest& manifest,
    const ManifestStatusOptions& options)
{
    ManifestStatusReport report;
    report.configPath = options.configPath;
    report.version = manifest.version;
    report.totalServices = manifest.services.size();
    report.serviceStatuses.reserve(manifest.services.size());

    for (const auto& service : manifest.services) {
        ServiceConfigStatus status;
        status.serviceName = service.serviceName;
        status.enabled = service.enabled;
        status.supported = isSupportedServiceName(service.serviceName);
        status.executablePath = service.executablePath;
        status.workingDirectory = service.workingDirectory;
        status.socketName = service.socketName;
        status.socketNamePresent = !service.socketName.empty();
        status.autoRestart = service.autoRestart;
        status.autoRestartEnabledButIgnored = service.autoRestart;
        status.healthCheckSupported = status.supported && status.socketNamePresent;

        if (service.enabled) {
            ++report.enabledServices;
        } else {
            ++report.disabledServices;
            addIssue(
                status,
                report,
                ServiceConfigIssueLevel::Info,
                "SERVICE_DISABLED",
                "Service is disabled and will not be started");
        }

        if (service.serviceName.empty()) {
            addIssue(
                status,
                report,
                ServiceConfigIssueLevel::Error,
                "SERVICE_NAME_EMPTY",
                "serviceName is required");
        }

        if (!status.supported) {
            addIssue(
                status,
                report,
                ServiceConfigIssueLevel::Warning,
                "SERVICE_UNSUPPORTED",
                "Only ETFDataService is supported by this Watchdog skeleton");
        }

        if (service.executablePath.empty()) {
            addIssue(
                status,
                report,
                ServiceConfigIssueLevel::Error,
                "EXECUTABLE_PATH_EMPTY",
                "executablePath is required");
        } else {
            status.executableExists =
                std::filesystem::exists(resolvePath(options.baseDirectory, service.executablePath));
            if (!status.executableExists) {
                addIssue(
                    status,
                    report,
                    ServiceConfigIssueLevel::Error,
                    "EXECUTABLE_NOT_FOUND",
                    "executablePath does not exist");
            }
        }

        if (service.workingDirectory.empty()) {
            status.workingDirectoryExists = true;
            addIssue(
                status,
                report,
                ServiceConfigIssueLevel::Warning,
                "WORKING_DIRECTORY_EMPTY",
                "workingDirectory is empty; current directory will be used");
        } else {
            status.workingDirectoryExists =
                std::filesystem::is_directory(resolvePath(options.baseDirectory, service.workingDirectory));
            if (!status.workingDirectoryExists) {
                addIssue(
                    status,
                    report,
                    ServiceConfigIssueLevel::Error,
                    "WORKING_DIRECTORY_NOT_FOUND",
                    "workingDirectory does not exist");
            }
        }

        if (service.autoRestart) {
            addIssue(
                status,
                report,
                ServiceConfigIssueLevel::Warning,
                "AUTO_RESTART_IGNORED",
                "autoRestart is configured but ignored in this version");
        }

        if (status.supported && service.serviceName == "ETFDataService") {
            if (service.socketName.empty()) {
                addIssue(
                    status,
                    report,
                    ServiceConfigIssueLevel::Error,
                    "SOCKET_NAME_MISSING",
                    "ETFDataService requires socketName for health check");
            }
        }

        status.canStart = service.enabled && status.supported && !hasError(status);
        report.serviceStatuses.push_back(std::move(status));
    }

    return report;
}

}  // namespace etfdt::watchdog
