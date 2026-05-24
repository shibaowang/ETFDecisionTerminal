#include "Diagnostics/WatchdogReportParser.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include <sstream>
#include <vector>

namespace etfdt::diagnostics {
namespace {

std::string typeName(const QJsonValue& value)
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

DiagnosticValidationResult requireString(
    const QJsonObject& object,
    const QString& field,
    const std::string& prefix)
{
    const auto value = object.value(field);
    if (!object.contains(field)) {
        return {false, prefix + ": missing " + field.toStdString()};
    }
    if (!value.isString()) {
        return {false,
                prefix + ": " + field.toStdString() + " must be string, got " + typeName(value)};
    }
    return {true, {}};
}

DiagnosticValidationResult requireNumber(
    const QJsonObject& object,
    const QString& field,
    const std::string& prefix)
{
    const auto value = object.value(field);
    if (!object.contains(field)) {
        return {false, prefix + ": missing " + field.toStdString()};
    }
    if (!value.isDouble()) {
        return {false,
                prefix + ": " + field.toStdString() + " must be number, got " + typeName(value)};
    }
    return {true, {}};
}

DiagnosticValidationResult requireBool(
    const QJsonObject& object,
    const QString& field,
    const std::string& prefix)
{
    const auto value = object.value(field);
    if (!object.contains(field)) {
        return {false, prefix + ": missing " + field.toStdString()};
    }
    if (!value.isBool()) {
        return {false,
                prefix + ": " + field.toStdString() + " must be bool, got " + typeName(value)};
    }
    return {true, {}};
}

DiagnosticValidationResult requireArray(
    const QJsonObject& object,
    const QString& field,
    const std::string& prefix)
{
    const auto value = object.value(field);
    if (!object.contains(field)) {
        return {false, prefix + ": missing " + field.toStdString()};
    }
    if (!value.isArray()) {
        return {false,
                prefix + ": " + field.toStdString() + " must be array, got " + typeName(value)};
    }
    return {true, {}};
}

DiagnosticValidationResult validateServiceObject(const QJsonObject& service, int index)
{
    const std::string prefix = "services[" + std::to_string(index) + "]";
    const std::vector<QString> stringFields = {
        "serviceName",
        "executablePath",
        "workingDirectory",
        "socketName",
    };
    for (const auto& field : stringFields) {
        auto result = requireString(service, field, prefix);
        if (!result.ok) {
            return result;
        }
    }

    const std::vector<QString> boolFields = {
        "enabled",
        "supported",
        "executableExists",
        "workingDirectoryExists",
        "socketNamePresent",
        "healthCheckSupported",
        "canStart",
    };
    for (const auto& field : boolFields) {
        auto result = requireBool(service, field, prefix);
        if (!result.ok) {
            return result;
        }
    }

    return requireArray(service, "issues", prefix);
}

DiagnosticValidationResult validateRootObject(const QJsonObject& root)
{
    auto version = requireString(root, "version", "report");
    if (!version.ok) {
        return version;
    }

    const std::vector<QString> numberFields = {
        "totalServices",
        "enabledServices",
        "disabledServices",
        "errorCount",
        "warningCount",
    };
    for (const auto& field : numberFields) {
        auto result = requireNumber(root, field, "report");
        if (!result.ok) {
            return result;
        }
    }

    auto servicesResult = requireArray(root, "services", "report");
    if (!servicesResult.ok) {
        return servicesResult;
    }

    const auto services = root.value("services").toArray();
    for (int i = 0; i < services.size(); ++i) {
        if (!services.at(i).isObject()) {
            return {false, "services[" + std::to_string(i) + "] must be object"};
        }
        auto result = validateServiceObject(services.at(i).toObject(), i);
        if (!result.ok) {
            return result;
        }
    }

    return {true, {}};
}

DiagnosticIssue parseIssue(const QJsonObject& object)
{
    DiagnosticIssue issue;
    issue.level = object.value("level").toString().toStdString();
    issue.code = object.value("code").toString().toStdString();
    issue.message = object.value("message").toString().toStdString();
    return issue;
}

DiagnosticServiceStatus parseService(const QJsonObject& object)
{
    DiagnosticServiceStatus service;
    service.serviceName = object.value("serviceName").toString().toStdString();
    service.enabled = object.value("enabled").toBool();
    service.supported = object.value("supported").toBool();
    service.executablePath = object.value("executablePath").toString().toStdString();
    service.executableExists = object.value("executableExists").toBool();
    service.workingDirectory = object.value("workingDirectory").toString().toStdString();
    service.workingDirectoryExists = object.value("workingDirectoryExists").toBool();
    service.socketName = object.value("socketName").toString().toStdString();
    service.socketNamePresent = object.value("socketNamePresent").toBool();
    service.healthCheckSupported = object.value("healthCheckSupported").toBool();
    service.canStart = object.value("canStart").toBool();

    const auto issues = object.value("issues").toArray();
    for (const auto& value : issues) {
        if (value.isObject()) {
            service.issues.push_back(parseIssue(value.toObject()));
        }
    }
    return service;
}

DiagnosticReport parseReportObject(const QJsonObject& root)
{
    DiagnosticReport report;
    report.configPath = root.value("configPath").toString().toStdString();
    report.version = root.value("version").toString().toStdString();
    report.totalServices = root.value("totalServices").toInt();
    report.enabledServices = root.value("enabledServices").toInt();
    report.disabledServices = root.value("disabledServices").toInt();
    report.errorCount = root.value("errorCount").toInt();
    report.warningCount = root.value("warningCount").toInt();

    const auto services = root.value("services").toArray();
    report.services.reserve(static_cast<std::size_t>(services.size()));
    for (const auto& value : services) {
        report.services.push_back(parseService(value.toObject()));
    }
    return report;
}

}  // namespace

WatchdogReportParseResult WatchdogReportParser::parseFromFile(const std::string& path)
{
    if (path.empty()) {
        return {false, {}, "report path is required"};
    }

    QFile file(QString::fromStdString(path));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {false, {}, "Failed to open report: " + path};
    }
    return parseFromJsonString(QString::fromUtf8(file.readAll()).toStdString());
}

WatchdogReportParseResult WatchdogReportParser::parseFromJsonString(const std::string& json)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return {false, {}, "Invalid JSON: " + parseError.errorString().toStdString()};
    }
    if (!document.isObject()) {
        return {false, {}, "Invalid report: root must be object"};
    }

    const auto root = document.object();
    auto validation = validateRootObject(root);
    if (!validation.ok) {
        return {false, {}, "Invalid report: " + validation.errorMessage};
    }

    auto report = parseReportObject(root);
    validation = validateReport(report);
    if (!validation.ok) {
        return {false, {}, "Invalid report: " + validation.errorMessage};
    }
    return {true, report, {}};
}

DiagnosticValidationResult WatchdogReportParser::validateReport(const DiagnosticReport& report)
{
    if (report.version.empty()) {
        return {false, "report: missing version"};
    }
    if (report.totalServices < 0 || report.enabledServices < 0 || report.disabledServices < 0
        || report.errorCount < 0 || report.warningCount < 0) {
        return {false, "report: counts must be non-negative"};
    }
    if (report.totalServices != static_cast<int>(report.services.size())) {
        return {false, "report: totalServices does not match services size"};
    }
    if (report.services.empty()) {
        return {false, "report: services must not be empty"};
    }
    for (std::size_t i = 0; i < report.services.size(); ++i) {
        if (report.services[i].serviceName.empty()) {
            return {false, "services[" + std::to_string(i) + "]: missing serviceName"};
        }
    }
    return {true, {}};
}

DiagnosticSummary WatchdogReportParser::summarize(const DiagnosticReport& report)
{
    return buildDiagnosticSummary(report);
}

}  // namespace etfdt::diagnostics
