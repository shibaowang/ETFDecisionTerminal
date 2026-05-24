#include "Watchdog/ServiceManifestStatusJson.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace etfdt::watchdog {
namespace {

QJsonObject issueToJsonObject(const ServiceConfigIssue& issue)
{
    QJsonObject object;
    object.insert("level", QString::fromStdString(std::string(toString(issue.level))));
    object.insert("code", QString::fromStdString(issue.code));
    object.insert("message", QString::fromStdString(issue.message));
    return object;
}

QJsonObject serviceStatusToJsonObject(const ServiceConfigStatus& status)
{
    QJsonArray issues;
    for (const auto& issue : status.issues) {
        issues.append(issueToJsonObject(issue));
    }

    QJsonObject object;
    object.insert("serviceName", QString::fromStdString(status.serviceName));
    object.insert("enabled", status.enabled);
    object.insert("supported", status.supported);
    object.insert("executablePath", QString::fromStdString(status.executablePath));
    object.insert("executableExists", status.executableExists);
    object.insert("workingDirectory", QString::fromStdString(status.workingDirectory));
    object.insert("workingDirectoryExists", status.workingDirectoryExists);
    object.insert("socketName", QString::fromStdString(status.socketName));
    object.insert("socketNamePresent", status.socketNamePresent);
    object.insert("autoRestart", status.autoRestart);
    object.insert("autoRestartEnabledButIgnored", status.autoRestartEnabledButIgnored);
    object.insert("healthCheckSupported", status.healthCheckSupported);
    object.insert("canStart", status.canStart);
    object.insert("issues", issues);
    return object;
}

}  // namespace

QJsonObject toJsonObject(const ManifestStatusReport& report)
{
    QJsonArray services;
    for (const auto& service : report.serviceStatuses) {
        services.append(serviceStatusToJsonObject(service));
    }

    QJsonObject object;
    object.insert("configPath", QString::fromStdString(report.configPath));
    object.insert("version", QString::fromStdString(report.version));
    object.insert("totalServices", static_cast<qint64>(report.totalServices));
    object.insert("enabledServices", static_cast<qint64>(report.enabledServices));
    object.insert("disabledServices", static_cast<qint64>(report.disabledServices));
    object.insert("errorCount", static_cast<qint64>(report.errorCount));
    object.insert("warningCount", static_cast<qint64>(report.warningCount));
    object.insert("services", services);
    return object;
}

std::string toJsonString(const ManifestStatusReport& report, bool pretty)
{
    const QJsonDocument document(toJsonObject(report));
    const auto format = pretty ? QJsonDocument::Indented : QJsonDocument::Compact;
    return document.toJson(format).toStdString();
}

WatchdogResult<bool> writeManifestStatusReportJson(
    const ManifestStatusReport& report,
    const std::filesystem::path& outputPath,
    bool pretty)
{
    if (outputPath.empty()) {
        return WatchdogResult<bool>::failure("output path is required");
    }

    QFile file(QString::fromStdWString(outputPath.wstring()));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return WatchdogResult<bool>::failure(
            "Failed to open output file: " + outputPath.string());
    }

    const auto json = QByteArray::fromStdString(toJsonString(report, pretty));
    if (file.write(json) != json.size()) {
        return WatchdogResult<bool>::failure(
            "Failed to write output file: " + outputPath.string());
    }
    return WatchdogResult<bool>::success(true);
}

}  // namespace etfdt::watchdog
