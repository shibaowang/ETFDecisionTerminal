#include "ShellCore/ShellMockMetricsProvider.h"

#include <utility>

namespace etfdt::shell {

namespace {

ShellPageMetric metric(
    std::string key,
    std::string label,
    std::string valueText,
    std::string unitText,
    std::string trendText,
    std::string severity,
    std::string description)
{
    ShellPageMetric result;
    result.key = std::move(key);
    result.label = std::move(label);
    result.valueText = std::move(valueText);
    result.unitText = std::move(unitText);
    result.trendText = std::move(trendText);
    result.severity = std::move(severity);
    result.mock = true;
    result.description = std::move(description);
    return result;
}

ShellPageActionHint hint(std::string key, std::string title, std::string description, std::string severity)
{
    ShellPageActionHint result;
    result.key = std::move(key);
    result.title = std::move(title);
    result.description = std::move(description);
    result.severity = std::move(severity);
    result.enabled = false;
    result.mock = true;
    result.actionText = "Mock only";
    return result;
}

ShellPageMetricList placeholderMetrics(const QString& pageKey)
{
    return {
        metric(
            "placeholder_status",
            "Placeholder",
            "Mock",
            "",
            "No real data",
            "INFO",
            "Placeholder metric for " + pageKey.toStdString()),
    };
}

}  // namespace

ShellPageMetricList ShellMockMetricsProvider::metricsForPage(const QString& pageKey)
{
    if (pageKey == QStringLiteral("dashboard")) {
        return {
            metric("total_assets", "Account total assets", "1,000,000", "CNY", "+0.00%", "INFO", "Mock total assets."),
            metric("principal_base", "Principal baseline", "800,000", "CNY", "Mock baseline", "INFO", "Mock principal baseline."),
            metric("cash_left", "Remaining cash", "200,000", "CNY", "20%", "OK", "Mock available cash."),
            metric("base_completion", "Base position completion", "72", "%", "+2%", "OK", "Mock base position completion."),
            metric("active_drafts", "Active TradeDraft", "0", "items", "No real drafts", "DISABLED", "Mock TradeDraft count."),
            metric("base_damage", "Base damage count", "0", "items", "Mock safe", "OK", "Mock base damage count."),
        };
    }

    if (pageKey == QStringLiteral("diagnostics")) {
        return {
            metric("service_total", "Service total", "6", "services", "Mock manifest", "INFO", "Mock service total."),
            metric("service_startable", "Startable services", "1", "service", "Mock readiness", "OK", "Mock startable services."),
            metric("warning_count", "Warning count", "1", "issue", "Mock warning", "WARNING", "Mock warning count."),
            metric("error_count", "Error count", "0", "issue", "Mock healthy", "OK", "Mock error count."),
        };
    }

    return placeholderMetrics(pageKey);
}

ShellPageActionHintList ShellMockMetricsProvider::actionHintsForPage(const QString& pageKey)
{
    if (pageKey == QStringLiteral("dashboard")) {
        return {
            hint("review_cash", "Review cash mock", "Placeholder hint only; no action is executed.", "INFO"),
            hint("review_drafts", "Review TradeDraft mock", "No real TradeDraft lifecycle is connected.", "DISABLED"),
        };
    }

    if (pageKey == QStringLiteral("diagnostics")) {
        return {
            hint("open_diagnostics", "Review diagnostics mock", "Diagnostics data comes from ShellCore mock models.", "INFO"),
            hint("export_report", "Export report mock", "This page does not read or write a real report.", "DISABLED"),
        };
    }

    return {
        hint(
            "placeholder_only",
            "Placeholder module",
            "This page has no real service data; this action hint is not executable.",
            "DISABLED"),
    };
}

}  // namespace etfdt::shell
