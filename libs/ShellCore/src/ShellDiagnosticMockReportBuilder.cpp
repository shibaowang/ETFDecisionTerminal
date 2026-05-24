#include "ShellCore/ShellDiagnosticMockReportBuilder.h"

namespace etfdt::shell {

std::string ShellDiagnosticMockReportBuilder::healthyReportJson()
{
    return R"json({
  "configPath": "mock://healthy",
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
      "executablePath": "mock/ETFDataService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceMock",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": true,
      "issues": []
    }
  ]
})json";
}

std::string ShellDiagnosticMockReportBuilder::warningReportJson()
{
    return R"json({
  "configPath": "mock://warning",
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
      "executablePath": "mock/ETFDataService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceMock",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": true,
      "issues": [
        {"level": "WARNING", "code": "MOCK_WARNING", "message": "mock warning for UI prototype"}
      ]
    }
  ]
})json";
}

std::string ShellDiagnosticMockReportBuilder::errorReportJson()
{
    return R"json({
  "configPath": "mock://error",
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
      "executablePath": "mock/missing.exe",
      "executableExists": false,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceMock",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": false,
      "issues": [
        {"level": "ERROR", "code": "MOCK_ERROR", "message": "mock blocked service for UI prototype"}
      ]
    }
  ]
})json";
}

std::string ShellDiagnosticMockReportBuilder::mixedReportJson()
{
    return R"json({
  "configPath": "mock://mixed",
  "version": "1.0",
  "totalServices": 4,
  "enabledServices": 3,
  "disabledServices": 1,
  "errorCount": 1,
  "warningCount": 1,
  "services": [
    {
      "serviceName": "ETFDataService",
      "enabled": true,
      "supported": true,
      "executablePath": "mock/ETFDataService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceMock",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": true,
      "issues": []
    },
    {
      "serviceName": "ETFMarketService",
      "enabled": true,
      "supported": true,
      "executablePath": "mock/ETFMarketService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFMarketServiceMock",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": true,
      "issues": [
        {"level": "WARNING", "code": "MOCK_MARKET_WARNING", "message": "mock delayed market data"}
      ]
    },
    {
      "serviceName": "ETFStrategyService",
      "enabled": true,
      "supported": true,
      "executablePath": "mock/missing.exe",
      "executableExists": false,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFStrategyServiceMock",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": false,
      "issues": [
        {"level": "ERROR", "code": "MOCK_STRATEGY_ERROR", "message": "mock strategy service blocked"}
      ]
    },
    {
      "serviceName": "ETFAlertService",
      "enabled": false,
      "supported": true,
      "executablePath": "mock/ETFAlertService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFAlertServiceMock",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": false,
      "issues": []
    }
  ]
})json";
}

}  // namespace etfdt::shell
