#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include <iostream>
#include <string>
#include <vector>

namespace {

struct DiagResult final {
    bool ok = false;
    std::string message;
};

void printHelp()
{
    std::cout
        << "ETFDiag diagnostic utility\n"
        << "\n"
        << "Usage:\n"
        << "  ETFDiag --watchdog-report <path>\n"
        << "  ETFDiag --help\n"
        << "\n"
        << "Consumes a Watchdog manifest status JSON report and prints a diagnostic summary.\n";
}

bool hasOption(const std::vector<std::string>& args, const std::string& option)
{
    for (const auto& arg : args) {
        if (arg == option) {
            return true;
        }
    }
    return false;
}

std::string optionValue(const std::vector<std::string>& args, const std::string& option)
{
    for (std::size_t i = 0; i + 1 < args.size(); ++i) {
        if (args[i] == option) {
            return args[i + 1];
        }
    }
    return {};
}

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

DiagResult requireString(const QJsonObject& object, const QString& field, const std::string& prefix)
{
    const auto value = object.value(field);
    if (!object.contains(field)) {
        return {false, prefix + ": missing " + field.toStdString()};
    }
    if (!value.isString()) {
        return {false, prefix + ": " + field.toStdString() + " must be string, got " + typeName(value)};
    }
    return {true, {}};
}

DiagResult requireNumber(const QJsonObject& object, const QString& field, const std::string& prefix)
{
    const auto value = object.value(field);
    if (!object.contains(field)) {
        return {false, prefix + ": missing " + field.toStdString()};
    }
    if (!value.isDouble()) {
        return {false, prefix + ": " + field.toStdString() + " must be number, got " + typeName(value)};
    }
    return {true, {}};
}

DiagResult requireBool(const QJsonObject& object, const QString& field, const std::string& prefix)
{
    const auto value = object.value(field);
    if (!object.contains(field)) {
        return {false, prefix + ": missing " + field.toStdString()};
    }
    if (!value.isBool()) {
        return {false, prefix + ": " + field.toStdString() + " must be bool, got " + typeName(value)};
    }
    return {true, {}};
}

DiagResult requireArray(const QJsonObject& object, const QString& field, const std::string& prefix)
{
    const auto value = object.value(field);
    if (!object.contains(field)) {
        return {false, prefix + ": missing " + field.toStdString()};
    }
    if (!value.isArray()) {
        return {false, prefix + ": " + field.toStdString() + " must be array, got " + typeName(value)};
    }
    return {true, {}};
}

DiagResult validateService(const QJsonObject& service, int index)
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

    auto issuesResult = requireArray(service, "issues", prefix);
    if (!issuesResult.ok) {
        return issuesResult;
    }
    return {true, {}};
}

DiagResult validateReport(const QJsonObject& root)
{
    const std::vector<QString> stringFields = {"version"};
    for (const auto& field : stringFields) {
        auto result = requireString(root, field, "report");
        if (!result.ok) {
            return result;
        }
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
        auto result = validateService(services.at(i).toObject(), i);
        if (!result.ok) {
            return result;
        }
    }
    return {true, {}};
}

int consumeWatchdogReport(const std::string& reportPath)
{
    if (reportPath.empty()) {
        std::cerr << "--watchdog-report is required\n";
        return 1;
    }

    QFile file(QString::fromStdString(reportPath));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Failed to open report: " << reportPath << '\n';
        return 1;
    }

    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        std::cerr << "Invalid JSON: " << parseError.errorString().toStdString() << '\n';
        return 1;
    }
    if (!document.isObject()) {
        std::cerr << "Invalid report: root must be object\n";
        return 1;
    }

    const auto root = document.object();
    auto validation = validateReport(root);
    if (!validation.ok) {
        std::cerr << "Invalid report: " << validation.message << '\n';
        return 1;
    }

    const auto services = root.value("services").toArray();
    const int errorCount = root.value("errorCount").toInt();
    const int warningCount = root.value("warningCount").toInt();
    bool enabledServiceCannotStart = false;

    std::cout << "Watchdog Manifest Report\n";
    std::cout << "Config: " << root.value("configPath").toString().toStdString() << '\n';
    std::cout << "Version: " << root.value("version").toString().toStdString() << '\n';
    std::cout << "Services: total=" << root.value("totalServices").toInt()
              << " enabled=" << root.value("enabledServices").toInt()
              << " disabled=" << root.value("disabledServices").toInt() << '\n';
    std::cout << "Issues: errors=" << errorCount << " warnings=" << warningCount << '\n';

    for (const auto& value : services) {
        const auto service = value.toObject();
        const bool enabled = service.value("enabled").toBool();
        const bool canStart = service.value("canStart").toBool();
        if (enabled && !canStart) {
            enabledServiceCannotStart = true;
        }

        std::cout << '\n' << '[' << service.value("serviceName").toString().toStdString() << "]\n";
        std::cout << "enabled: " << (enabled ? "true" : "false") << '\n';
        std::cout << "supported: " << (service.value("supported").toBool() ? "true" : "false") << '\n';
        std::cout << "canStart: " << (canStart ? "true" : "false") << '\n';
        std::cout << "executableExists: "
                  << (service.value("executableExists").toBool() ? "true" : "false") << '\n';
        std::cout << "workingDirectoryExists: "
                  << (service.value("workingDirectoryExists").toBool() ? "true" : "false") << '\n';
        std::cout << "socketNamePresent: "
                  << (service.value("socketNamePresent").toBool() ? "true" : "false") << '\n';
        std::cout << "healthCheckSupported: "
                  << (service.value("healthCheckSupported").toBool() ? "true" : "false") << '\n';

        const auto issues = service.value("issues").toArray();
        for (const auto& issueValue : issues) {
            if (!issueValue.isObject()) {
                continue;
            }
            const auto issue = issueValue.toObject();
            std::cout << "issue: " << issue.value("level").toString().toStdString() << ' '
                      << issue.value("code").toString().toStdString() << " - "
                      << issue.value("message").toString().toStdString() << '\n';
        }
    }

    return errorCount > 0 || enabledServiceCannotStart ? 1 : 0;
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    if (args.empty() || hasOption(args, "--help")) {
        printHelp();
        return 0;
    }

    if (hasOption(args, "--watchdog-report")) {
        return consumeWatchdogReport(optionValue(args, "--watchdog-report"));
    }

    printHelp();
    return 1;
}
