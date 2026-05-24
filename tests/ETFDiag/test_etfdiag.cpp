#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QProcess>
#include <QStringList>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

int gFailures = 0;

struct ProcessResult final {
    int exitCode = -1;
    QString output;
};

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

std::filesystem::path optionValue(int argc, char* argv[], const std::string& option)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == option) {
            return argv[i + 1];
        }
    }
    return {};
}

std::filesystem::path createTempDirectory()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    auto path = std::filesystem::temp_directory_path() / ("etfdt_etfdiag_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
}

std::filesystem::path writeFile(
    const std::filesystem::path& directory,
    const std::string& fileName,
    const std::string& content)
{
    const auto path = directory / fileName;
    std::ofstream out(path);
    out << content;
    return path;
}

QString toQString(const std::filesystem::path& path)
{
    return QString::fromStdWString(path.wstring());
}

ProcessResult runDiag(const std::filesystem::path& diagExe, const QStringList& arguments)
{
    QProcess process;
    process.setProgram(toQString(diagExe));
    process.setArguments(arguments);
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start();
    if (!process.waitForStarted(5000)) {
        return {-1, process.errorString()};
    }
    if (!process.waitForFinished(10000)) {
        process.kill();
        process.waitForFinished(3000);
        return {-1, "ETFDiag timed out"};
    }
    return {process.exitStatus() == QProcess::NormalExit ? process.exitCode() : -1,
            QString::fromUtf8(process.readAll())};
}

std::string validReportJson()
{
    return R"json({
  "configPath": "config/services.local.example.json",
  "version": "1.0",
  "totalServices": 1,
  "enabledServices": 1,
  "disabledServices": 0,
  "errorCount": 0,
  "warningCount": 0,
  "services": [
    {
      "serviceName": "ETFDataService",
      "enabled": true,
      "supported": true,
      "executablePath": "build/apps/ETFDataService/ETFDataService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceReadonly",
      "socketNamePresent": true,
      "autoRestart": false,
      "autoRestartEnabledButIgnored": false,
      "healthCheckSupported": true,
      "canStart": true,
      "issues": []
    }
  ]
})json";
}

std::string errorReportJson()
{
    return R"json({
  "configPath": "config/services.local.example.json",
  "version": "1.0",
  "totalServices": 1,
  "enabledServices": 1,
  "disabledServices": 0,
  "errorCount": 1,
  "warningCount": 0,
  "services": [
    {
      "serviceName": "ETFDataService",
      "enabled": true,
      "supported": true,
      "executablePath": "missing.exe",
      "executableExists": false,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceReadonly",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": false,
      "issues": [
        {"level": "ERROR", "code": "EXECUTABLE_NOT_FOUND", "message": "missing executable"}
      ]
    }
  ]
})json";
}

std::string cannotStartWithoutErrorJson()
{
    return R"json({
  "configPath": "config/services.local.example.json",
  "version": "1.0",
  "totalServices": 1,
  "enabledServices": 1,
  "disabledServices": 0,
  "errorCount": 0,
  "warningCount": 1,
  "services": [
    {
      "serviceName": "ETFDataService",
      "enabled": true,
      "supported": false,
      "executablePath": "ETFDataService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceReadonly",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": false,
      "issues": [
        {"level": "WARNING", "code": "UNSUPPORTED_SERVICE", "message": "cannot start"}
      ]
    }
  ]
})json";
}

std::string warningOnlyJson()
{
    return R"json({
  "configPath": "config/services.local.example.json",
  "version": "1.0",
  "totalServices": 1,
  "enabledServices": 1,
  "disabledServices": 0,
  "errorCount": 0,
  "warningCount": 1,
  "services": [
    {
      "serviceName": "ETFDataService",
      "enabled": true,
      "supported": true,
      "executablePath": "ETFDataService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceReadonly",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": true,
      "issues": [
        {"level": "WARNING", "code": "AUTO_RESTART_IGNORED", "message": "autoRestart ignored"}
      ]
    }
  ]
})json";
}

void testCli(const std::filesystem::path& diagExe, const std::filesystem::path& tempDir)
{
    const auto validPath = writeFile(tempDir, "valid.json", validReportJson());
    const auto errorPath = writeFile(tempDir, "error.json", errorReportJson());
    const auto cannotStartPath = writeFile(tempDir, "cannot_start.json", cannotStartWithoutErrorJson());
    const auto warningPath = writeFile(tempDir, "warning.json", warningOnlyJson());
    const auto invalidJsonPath = writeFile(tempDir, "invalid.json", "{ invalid json");
    const auto missingServicesPath = writeFile(
        tempDir,
        "missing_services.json",
        R"json({"version":"1.0","totalServices":0,"enabledServices":0,"disabledServices":0,"errorCount":0,"warningCount":0})json");
    const auto servicesNotArrayPath = writeFile(
        tempDir,
        "services_not_array.json",
        R"json({"version":"1.0","totalServices":0,"enabledServices":0,"disabledServices":0,"errorCount":0,"warningCount":0,"services":{}})json");
    const auto boolWrongPath = writeFile(
        tempDir,
        "bool_wrong.json",
        R"json({"version":"1.0","totalServices":1,"enabledServices":1,"disabledServices":0,"errorCount":0,"warningCount":0,"services":[{"serviceName":"ETFDataService","enabled":"true","supported":true,"executablePath":"x","executableExists":true,"workingDirectory":".","workingDirectoryExists":true,"socketName":"s","socketNamePresent":true,"healthCheckSupported":true,"canStart":true,"issues":[]}]})json");
    const auto numberWrongPath = writeFile(
        tempDir,
        "number_wrong.json",
        R"json({"version":"1.0","totalServices":"1","enabledServices":1,"disabledServices":0,"errorCount":0,"warningCount":0,"services":[]})json");
    const auto issuesWrongPath = writeFile(
        tempDir,
        "issues_wrong.json",
        R"json({"version":"1.0","totalServices":1,"enabledServices":1,"disabledServices":0,"errorCount":0,"warningCount":0,"services":[{"serviceName":"ETFDataService","enabled":true,"supported":true,"executablePath":"x","executableExists":true,"workingDirectory":".","workingDirectoryExists":true,"socketName":"s","socketNamePresent":true,"healthCheckSupported":true,"canStart":true,"issues":{}}]})json");

    const auto help = runDiag(diagExe, {"--help"});
    expectTrue(help.exitCode == 0, "--help returns 0");

    const auto valid = runDiag(diagExe, {"--watchdog-report", toQString(validPath)});
    expectTrue(valid.exitCode == 0, "valid report returns 0");
    expectTrue(
        valid.output.contains("Watchdog Manifest Report"),
        "valid report output contains title");

    const auto error = runDiag(diagExe, {"--watchdog-report", toQString(errorPath)});
    expectTrue(error.exitCode != 0, "errorCount > 0 returns non-zero");

    const auto cannotStart = runDiag(diagExe, {"--watchdog-report", toQString(cannotStartPath)});
    expectTrue(
        cannotStart.exitCode != 0,
        "enabled service with canStart=false returns non-zero");

    const auto warning = runDiag(diagExe, {"--watchdog-report", toQString(warningPath)});
    expectTrue(warning.exitCode == 0, "warning-only report returns 0");
    expectTrue(warning.output.contains("AUTO_RESTART_IGNORED"), "warning output includes issue");

    const auto missingFile = runDiag(diagExe, {"--watchdog-report", toQString(tempDir / "missing.json")});
    expectTrue(missingFile.exitCode != 0, "missing JSON file returns non-zero");

    const auto invalidJson = runDiag(diagExe, {"--watchdog-report", toQString(invalidJsonPath)});
    expectTrue(invalidJson.exitCode != 0, "invalid JSON returns non-zero");

    const auto missingServices = runDiag(diagExe, {"--watchdog-report", toQString(missingServicesPath)});
    expectTrue(missingServices.exitCode != 0, "missing services returns non-zero");

    const auto servicesNotArray = runDiag(diagExe, {"--watchdog-report", toQString(servicesNotArrayPath)});
    expectTrue(servicesNotArray.exitCode != 0, "services not array returns non-zero");

    const auto boolWrong = runDiag(diagExe, {"--watchdog-report", toQString(boolWrongPath)});
    expectTrue(boolWrong.exitCode != 0, "bool field with wrong type returns non-zero");

    const auto numberWrong = runDiag(diagExe, {"--watchdog-report", toQString(numberWrongPath)});
    expectTrue(numberWrong.exitCode != 0, "number field with wrong type returns non-zero");

    const auto issuesWrong = runDiag(diagExe, {"--watchdog-report", toQString(issuesWrongPath)});
    expectTrue(issuesWrong.exitCode != 0, "issues with wrong type returns non-zero");
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const auto diagExe = optionValue(argc, argv, "--diag-exe");
    if (diagExe.empty()) {
        std::cerr << "--diag-exe is required\n";
        return 1;
    }

    const auto tempDir = createTempDirectory();
    try {
        testCli(diagExe, tempDir);
    } catch (const std::exception& ex) {
        ++gFailures;
        std::cerr << "FAILED: unexpected exception: " << ex.what() << '\n';
    }
    std::filesystem::remove_all(tempDir);

    if (gFailures != 0) {
        std::cerr << gFailures << " ETFDiag test failure(s)\n";
        return 1;
    }
    return 0;
}
