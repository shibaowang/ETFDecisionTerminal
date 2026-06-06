#include "ShellServices/ShellAccountingExcelVbaImportReadOnlyFileLoader.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QSet>
#include <QUrl>

namespace etfdt::shell_services {

namespace {

ShellAccountingExcelVbaImportReadOnlyFileLoadResult fail(QString issue)
{
    ShellAccountingExcelVbaImportReadOnlyFileLoadResult result;
    result.issue = std::move(issue);
    return result;
}

bool hasForbiddenKey(const QString& key)
{
    const auto lowered = key.toLower();
    return lowered == QStringLiteral("filepath")
        || lowered == QStringLiteral("path")
        || lowered == QStringLiteral("filename")
        || lowered.contains(QStringLiteral("credential"))
        || lowered.contains(QStringLiteral("endpoint"))
        || lowered.contains(QStringLiteral("password"))
        || lowered.contains(QStringLiteral("secret"));
}

bool hasForbiddenText(const QString& text)
{
    const auto lowered = text.toLower();
    const QStringList tokens{
        QStringLiteral("raw sql"),
        QStringLiteral("insert "),
        QStringLiteral("update "),
        QStringLiteral("delete "),
        QStringLiteral("drop "),
        QStringLiteral("select "),
        QStringLiteral("sqlite"),
        QStringLiteral("credential"),
        QStringLiteral("endpoint"),
        QStringLiteral("password"),
        QStringLiteral("secret"),
        QStringLiteral("http://"),
        QStringLiteral("https://"),
    };
    for (const auto& token : tokens) {
        if (lowered.contains(token)) {
            return true;
        }
    }
    return false;
}

bool containsForbiddenPayloadToken(const QJsonValue& value)
{
    if (value.isObject()) {
        const auto object = value.toObject();
        for (auto it = object.constBegin(); it != object.constEnd(); ++it) {
            if (hasForbiddenKey(it.key()) || containsForbiddenPayloadToken(it.value())) {
                return true;
            }
        }
        return false;
    }
    if (value.isArray()) {
        const auto array = value.toArray();
        for (const auto& item : array) {
            if (containsForbiddenPayloadToken(item)) {
                return true;
            }
        }
        return false;
    }
    return value.isString() && hasForbiddenText(value.toString());
}

QString localPathFromInput(const QString& input, bool& ok)
{
    ok = false;
    const auto trimmed = input.trimmed();
    if (trimmed.isEmpty()) {
        return {};
    }

    const QUrl url(trimmed);
    if (url.isValid() && !url.scheme().isEmpty()) {
        const auto scheme = url.scheme().toLower();
        if (scheme == QStringLiteral("file")) {
            const auto path = url.toLocalFile();
            ok = !path.isEmpty();
            return path;
        }
        if (scheme.size() == 1) {
            ok = true;
            return trimmed;
        }
        return {};
    }

    ok = true;
    return trimmed;
}

}  // namespace

ShellAccountingExcelVbaImportReadOnlyFileLoadResult
ShellAccountingExcelVbaImportReadOnlyFileLoader::loadLocalExportJsonPayload(
    const QString& fileUrlOrPath)
{
    bool localPathOk = false;
    const auto localPath = localPathFromInput(fileUrlOrPath, localPathOk);
    if (!localPathOk) {
        return fail(QStringLiteral("Only local JSON or text export files are supported."));
    }

    const QFileInfo fileInfo(localPath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        return fail(QStringLiteral("Selected export file is not available."));
    }

    const auto suffix = fileInfo.suffix().toLower();
    if (suffix != QStringLiteral("json") && suffix != QStringLiteral("txt")) {
        return fail(QStringLiteral("Selected export file must use .json or .txt."));
    }

    const auto fileSize = fileInfo.size();
    if (fileSize < 0 || fileSize > kMaxFileSizeBytes) {
        return fail(QStringLiteral("Selected export file exceeds the 1 MB read-only preview limit."));
    }

    QFile file(localPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return fail(QStringLiteral("Selected export file could not be read."));
    }
    const auto content = file.readAll();

    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(content, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return fail(QStringLiteral("Selected export file must contain a JSON object."));
    }

    const auto object = document.object();
    if (containsForbiddenPayloadToken(object)) {
        return fail(QStringLiteral("Selected export payload contains unsupported metadata or sensitive tokens."));
    }

    ShellAccountingExcelVbaImportReadOnlyFileLoadResult result;
    result.success = true;
    result.payloadJson = QString::fromUtf8(
        QJsonDocument(object).toJson(QJsonDocument::Compact));
    result.sanitizedFileName = fileInfo.fileName();
    result.fileSizeBytes = fileSize;
    return result;
}

}  // namespace etfdt::shell_services
