#include "ShellCore/ShellPageRegistry.h"

#include <algorithm>
#include <utility>

namespace etfdt::shell {

namespace {

ShellPageMetadata makePage(
    std::string key,
    std::string title,
    std::string qmlComponent,
    std::string description,
    bool placeholder,
    std::string category)
{
    ShellPageMetadata metadata;
    metadata.key = std::move(key);
    metadata.title = std::move(title);
    metadata.qmlComponent = std::move(qmlComponent);
    metadata.description = std::move(description);
    metadata.placeholder = placeholder;
    metadata.available = true;
    metadata.category = std::move(category);
    return metadata;
}

}  // namespace

std::vector<ShellPageMetadata> ShellPageRegistry::pages()
{
    return {
        makePage("dashboard", "首页驾驶舱", "DashboardPlaceholderPage", "当前页面为占位 Mock，业务功能尚未接入。", true, "overview"),
        makePage("readonly_data", "只读数据预览", "ReadOnlyDataPage", "开发期只读数据页面，仅通过 ShellServices Controller 调用 DataService 只读接口。", false, "overview"),
        makePage("market", "行情监控", "MarketPlaceholderPage", "当前页面为占位 Mock，行情接口尚未接入。", true, "market"),
        makePage("account_portfolio", "账户与组合", "AccountPortfolioReadOnlyPage", "账户与组合只读预览，不支持编辑、入账或交易。", false, "portfolio"),
        makePage("shell_accounting", "ShellAccounting", "ShellAccountingReadOnlyPage", "Read-only accounting shell. Data binding is unavailable.", false, "portfolio"),
        makePage("positions", "持仓明细", "PositionPlaceholderPage", "当前页面为占位 Mock，持仓数据尚未接入。", true, "portfolio"),
        makePage("strategy", "标的与策略", "InstrumentStrategyReadOnlyPage", "标的、策略与 OTC 通道只读预览，不支持编辑、策略执行或交易。", false, "strategy"),
        makePage("trade_draft", "TradeDraft 建议", "TradeDraftPlaceholderPage", "当前页面为占位 Mock，TradeDraft 生命周期尚未接入。", true, "trade"),
        makePage("execution", "成交确认", "TradeConfirmPlaceholderPage", "当前页面为占位 Mock，成交确认尚未接入。", true, "trade"),
        makePage("trade_log", "TradeLog 账本", "TradeLogPlaceholderPage", "当前页面为占位 Mock，账本查询尚未接入。", true, "trade"),
        makePage("cash_plan", "资金分配计划", "CashPlanPlaceholderPage", "当前页面为占位 Mock，资金计划尚未接入。", true, "cash"),
        makePage("otc_channel", "OTC A/C 通道", "OtcChannelPlaceholderPage", "当前页面为占位 Mock，OTC 通道业务尚未接入。", true, "otc"),
        makePage("alerts", "预警中心", "AlertPlaceholderPage", "当前页面为占位 Mock，预警服务尚未接入。", true, "alert"),
        makePage("diagnostics", "诊断中心", "DiagnosticsMockPage", "当前为 Mock 数据，未连接真实服务。", false, "system"),
        makePage("settings", "系统设置", "SettingsPlaceholderPage", "当前页面为占位 Mock，系统设置尚未接入。", true, "system"),
    };
}

std::vector<ShellNavigationItem> ShellPageRegistry::listNavigationItems()
{
    std::vector<ShellNavigationItem> items;
    const auto metadata = pages();
    items.reserve(metadata.size());

    int sortOrder = 0;
    for (const auto& page : metadata) {
        ShellNavigationItem item;
        item.key = page.key;
        item.title = page.title;
        item.iconName = page.key;
        item.category = page.category;
        item.enabled = page.available;
        item.placeholder = page.placeholder;
        item.sortOrder = sortOrder++;
        items.push_back(std::move(item));
    }

    std::sort(items.begin(), items.end(), [](const auto& left, const auto& right) {
        return left.sortOrder < right.sortOrder;
    });
    return items;
}

std::optional<ShellPageMetadata> ShellPageRegistry::metadataForKey(const std::string& key)
{
    const auto metadata = pages();
    const auto found = std::find_if(metadata.begin(), metadata.end(), [&key](const auto& page) {
        return page.key == key;
    });
    if (found == metadata.end()) {
        return std::nullopt;
    }
    return *found;
}

ShellPageMetadata ShellPageRegistry::fallbackMetadata(const std::string& key)
{
    ShellPageMetadata metadata;
    metadata.key = key;
    metadata.title = "未知页面";
    metadata.qmlComponent = "PlaceholderPage";
    metadata.description = "当前页面未注册，已回退到占位 Mock。";
    metadata.placeholder = true;
    metadata.available = false;
    metadata.category = "unknown";
    return metadata;
}

}  // namespace etfdt::shell
