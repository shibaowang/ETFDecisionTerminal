#include "Watchdog/ServiceManifestLoader.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include <set>
#include <string>
#include <utility>

namespace etfdt::watchdog {
namespace {

constexpr int kDefaultStartupTimeoutMs = 5000;
constexpr int kDefaultShutdownTimeoutMs = 3000;
constexpr int kDefaultHealthTimeoutMs = 2000;

bool isSupportedServiceName(const std::string& serviceName)
{
    return serviceName == "ETFDataService";
}

std::string valueTypeName(const QJsonValue& value)
{
    if (value.isUndefined()) {
        return "undefined";
    }
    if (value.isNull()) {
        return "null";
    }
    if (value.isBool()) {
        return "bool";
    }
    if (value.isDouble()) {
        return "number";
    }
    if (value.isString()) {
        return "string";
    }
    if (value.isArray()) {
        return "array";
    }
    if (value.isObject()) {
        return "object";
    }
    return "unknown";
}

WatchdogResult<std::string> requiredString(
    const QJsonObject& object,
    const QString& fieldName,
    const std::string& prefix)
{
    if (!object.contains(fieldName)) {
        return WatchdogResult<std::string>::failure(prefix + ": missing " + fieldName.toStdString());
    }
    const auto value = object.value(fieldName);
    if (!value.isString()) {
        return WatchdogResult<std::string>::failure(
            prefix + ": " + fieldName.toStdString() + " must be string, got " + valueTypeName(value));
    }
    const auto text = value.toString().trimmed().toStdString();
    if (text.empty()) {
        return WatchdogResult<std::string>::failure(prefix + ": " + fieldName.toStdString() + " is empty");
    }
    return WatchdogResult<std::string>::success(text);
}

WatchdogResult<std::string> optionalString(
    const QJsonObject& object,
    const QString& fieldName,
    const std::string& prefix)
{
    if (!object.contains(fieldName)) {
        return WatchdogResult<std::string>::success({});
    }
    const auto value = object.value(fieldName);
    if (!value.isString()) {
        return WatchdogResult<std::string>::failure(
            prefix + ": " + fieldName.toStdString() + " must be string, got " + valueTypeName(value));
    }
    return WatchdogResult<std::string>::success(value.toString().trimmed().toStdString());
}

WatchdogResult<bool> optionalBool(
    const QJsonObject& object,
    const QString& fieldName,
    bool defaultValue,
    const std::string& prefix)
{
    if (!object.contains(fieldName)) {
        return WatchdogResult<bool>::success(defaultValue);
    }
    const auto value = object.value(fieldName);
    if (!value.isBool()) {
        return WatchdogResult<bool>::failure(
            prefix + ": " + fieldName.toStdString() + " must be bool, got " + valueTypeName(value));
    }
    return WatchdogResult<bool>::success(value.toBool());
}

WatchdogResult<int> optionalTimeout(
    const QJsonObject& object,
    const QString& fieldName,
    int defaultValue,
    const std::string& prefix)
{
    if (!object.contains(fieldName)) {
        return WatchdogResult<int>::success(defaultValue);
    }
    const auto value = object.value(fieldName);
    if (!value.isDouble()) {
        return WatchdogResult<int>::failure(
            prefix + ": " + fieldName.toStdString() + " must be number, got " + valueTypeName(value));
    }
    const int timeout = value.toInt(-1);
    if (timeout < 0) {
        return WatchdogResult<int>::failure(prefix + ": " + fieldName.toStdString() + " must be non-negative");
    }
    return WatchdogResult<int>::success(timeout);
}

WatchdogResult<std::vector<std::string>> optionalArguments(
    const QJsonObject& object,
    const std::string& prefix)
{
    std::vector<std::string> arguments;
    if (!object.contains("arguments")) {
        return WatchdogResult<std::vector<std::string>>::success(arguments);
    }
    const auto value = object.value("arguments");
    if (!value.isArray()) {
        return WatchdogResult<std::vector<std::string>>::failure(
            prefix + ": arguments must be array, got " + valueTypeName(value));
    }

    const auto array = value.toArray();
    arguments.reserve(static_cast<std::size_t>(array.size()));
    for (int i = 0; i < array.size(); ++i) {
        const auto item = array.at(i);
        if (!item.isString()) {
            return WatchdogResult<std::vector<std::string>>::failure(
                prefix + ": arguments[" + std::to_string(i) + "] must be string");
        }
        arguments.push_back(item.toString().toStdString());
    }
    return WatchdogResult<std::vector<std::string>>::success(std::move(arguments));
}

void addValidationError(ServiceManifestValidationResult& result, std::string message)
{
    result.valid = false;
    result.errors.push_back(ServiceManifestError{std::move(message)});
}

WatchdogResult<ServiceProcessConfig> parseServiceConfig(const QJsonValue& value, int index)
{
    const std::string prefix = "services[" + std::to_string(index) + "]";
    if (!value.isObject()) {
        return WatchdogResult<ServiceProcessConfig>::failure(prefix + " must be object");
    }

    const auto object = value.toObject();
    ServiceProcessConfig config;

    auto serviceName = requiredString(object, "serviceName", prefix);
    if (!serviceName) {
        return WatchdogResult<ServiceProcessConfig>::failure(serviceName.error().message);
    }
    config.serviceName = serviceName.value();

    auto executablePath = requiredString(object, "executablePath", prefix);
    if (!executablePath) {
        return WatchdogResult<ServiceProcessConfig>::failure(executablePath.error().message);
    }
    config.executablePath = executablePath.value();

    auto workingDirectory = optionalString(object, "workingDirectory", prefix);
    if (!workingDirectory) {
        return WatchdogResult<ServiceProcessConfig>::failure(workingDirectory.error().message);
    }
    config.workingDirectory = workingDirectory.value();

    auto socketName = optionalString(object, "socketName", prefix);
    if (!socketName) {
        return WatchdogResult<ServiceProcessConfig>::failure(socketName.error().message);
    }
    config.socketName = socketName.value();

    auto enabled = optionalBool(object, "enabled", true, prefix);
    if (!enabled) {
        return WatchdogResult<ServiceProcessConfig>::failure(enabled.error().message);
    }
    config.enabled = enabled.value();

    auto autoRestart = optionalBool(object, "autoRestart", false, prefix);
    if (!autoRestart) {
        return WatchdogResult<ServiceProcessConfig>::failure(autoRestart.error().message);
    }
    config.autoRestart = autoRestart.value();

    auto startupTimeout = optionalTimeout(object, "startupTimeoutMs", kDefaultStartupTimeoutMs, prefix);
    if (!startupTimeout) {
        return WatchdogResult<ServiceProcessConfig>::failure(startupTimeout.error().message);
    }
    config.startupTimeoutMs = startupTimeout.value();

    auto shutdownTimeout = optionalTimeout(object, "shutdownTimeoutMs", kDefaultShutdownTimeoutMs, prefix);
    if (!shutdownTimeout) {
        return WatchdogResult<ServiceProcessConfig>::failure(shutdownTimeout.error().message);
    }
    config.shutdownTimeoutMs = shutdownTimeout.value();

    auto healthTimeout = optionalTimeout(object, "healthTimeoutMs", kDefaultHealthTimeoutMs, prefix);
    if (!healthTimeout) {
        return WatchdogResult<ServiceProcessConfig>::failure(healthTimeout.error().message);
    }
    config.healthTimeoutMs = healthTimeout.value();

    auto arguments = optionalArguments(object, prefix);
    if (!arguments) {
        return WatchdogResult<ServiceProcessConfig>::failure(arguments.error().message);
    }
    config.arguments = arguments.value();

    return WatchdogResult<ServiceProcessConfig>::success(std::move(config));
}

}  // namespace

WatchdogResult<ServiceManifest> ServiceManifestLoader::loadFromFile(
    const std::filesystem::path& path)
{
    QFile file(QString::fromStdWString(path.wstring()));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return WatchdogResult<ServiceManifest>::failure(
            "Failed to open service manifest: " + path.string());
    }
    const auto bytes = file.readAll();
    return loadFromJsonString(bytes.toStdString());
}

WatchdogResult<ServiceManifest> ServiceManifestLoader::loadFromJsonString(
    const std::string& json)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return WatchdogResult<ServiceManifest>::failure(
            "Invalid service manifest JSON: " + parseError.errorString().toStdString());
    }
    if (!document.isObject()) {
        return WatchdogResult<ServiceManifest>::failure("Service manifest root must be an object");
    }

    const auto root = document.object();
    ServiceManifest manifest;

    auto version = requiredString(root, "version", "manifest");
    if (!version) {
        return WatchdogResult<ServiceManifest>::failure(version.error().message);
    }
    manifest.version = version.value();

    if (!root.contains("services")) {
        return WatchdogResult<ServiceManifest>::failure("manifest: missing services");
    }
    const auto servicesValue = root.value("services");
    if (!servicesValue.isArray()) {
        return WatchdogResult<ServiceManifest>::failure(
            "manifest: services must be array, got " + valueTypeName(servicesValue));
    }

    const auto services = servicesValue.toArray();
    manifest.services.reserve(static_cast<std::size_t>(services.size()));
    for (int i = 0; i < services.size(); ++i) {
        auto service = parseServiceConfig(services.at(i), i);
        if (!service) {
            return WatchdogResult<ServiceManifest>::failure(service.error().message);
        }
        manifest.services.push_back(service.value());
    }

    const auto validation = validate(manifest);
    if (!validation.valid) {
        return WatchdogResult<ServiceManifest>::failure(validation.errors.front().message);
    }
    return WatchdogResult<ServiceManifest>::success(std::move(manifest));
}

ServiceManifestValidationResult ServiceManifestLoader::validate(const ServiceManifest& manifest)
{
    ServiceManifestValidationResult result;
    if (manifest.version.empty()) {
        addValidationError(result, "version is required");
    }
    if (manifest.services.empty()) {
        addValidationError(result, "services must contain at least one service");
    }

    std::set<std::string> serviceNames;
    for (const auto& service : manifest.services) {
        if (service.serviceName.empty()) {
            addValidationError(result, "serviceName is required");
            continue;
        }
        if (!serviceNames.insert(service.serviceName).second) {
            addValidationError(result, "duplicate serviceName: " + service.serviceName);
        }
        if (!isSupportedServiceName(service.serviceName)) {
            if (service.enabled) {
                addValidationError(result, "unsupported enabled serviceName: " + service.serviceName);
            } else {
                result.warnings.push_back(
                    "unsupported disabled serviceName kept as future placeholder: " + service.serviceName);
            }
        }
        if (service.executablePath.empty()) {
            addValidationError(result, "executablePath is required for " + service.serviceName);
        }
        if (service.enabled && service.serviceName == "ETFDataService" && service.socketName.empty()) {
            addValidationError(result, "socketName is required for enabled ETFDataService");
        }
        if (service.startupTimeoutMs < 0) {
            addValidationError(result, "startupTimeoutMs must be non-negative for " + service.serviceName);
        }
        if (service.shutdownTimeoutMs < 0) {
            addValidationError(result, "shutdownTimeoutMs must be non-negative for " + service.serviceName);
        }
        if (service.healthTimeoutMs < 0) {
            addValidationError(result, "healthTimeoutMs must be non-negative for " + service.serviceName);
        }
        if (service.autoRestart) {
            result.warnings.push_back(
                "autoRestart is parsed but not active in this Watchdog skeleton: " + service.serviceName);
        }
    }
    return result;
}

}  // namespace etfdt::watchdog
