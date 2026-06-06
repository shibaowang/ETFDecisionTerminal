#pragma once

#include <QString>

#include <cstdint>

namespace etfdt::shell_services {

struct ShellAccountingExcelVbaImportReadOnlyFileLoadResult final {
    bool success = false;
    QString payloadJson;
    QString sanitizedFileName;
    QString issue;
    std::int64_t fileSizeBytes = 0;
};

class ShellAccountingExcelVbaImportReadOnlyFileLoader final {
public:
    static constexpr std::int64_t kMaxFileSizeBytes = 1024 * 1024;

    [[nodiscard]] static ShellAccountingExcelVbaImportReadOnlyFileLoadResult
    loadLocalExportJsonPayload(const QString& fileUrlOrPath);
};

}  // namespace etfdt::shell_services
