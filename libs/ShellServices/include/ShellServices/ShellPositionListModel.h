#pragma once

#include "ShellServices/ShellAccountingDisplayText.h"
#include "ShellServices/ShellAccountingState.h"

#include <QAbstractListModel>

#include <vector>

namespace etfdt::shell_services {

struct ShellPositionListRow final {
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string instrumentName;
    std::string quantityText;
    ShellAccountingDisplayText costAmountText;
    ShellAccountingDisplayText costPriceText;
    std::string currency;
    ShellAccountingDisplayText marketValueText;
    ShellAccountingDisplayText unrealizedPnlText;
    std::string dataQualityStatus = "UNAVAILABLE";
    ShellAccountingViewState costAmountState = ShellAccountingViewState::Loaded;
    ShellAccountingViewState costPriceState = ShellAccountingViewState::Loaded;
    ShellAccountingViewState marketValueState = ShellAccountingViewState::Loaded;
    ShellAccountingViewState unrealizedPnlState = ShellAccountingViewState::Loaded;
};

class ShellPositionListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        AccountIdRole = Qt::UserRole + 1,
        PortfolioIdRole,
        InstrumentCodeRole,
        InstrumentNameRole,
        QuantityTextRole,
        CostAmountDisplayTextRole,
        CostPriceDisplayTextRole,
        CurrencyRole,
        MarketValueDisplayTextRole,
        UnrealizedPnlDisplayTextRole,
        DataQualityStatusRole,
    };

    explicit ShellPositionListModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void setRows(std::vector<ShellPositionListRow> rows);
    void clearRows();
    void setPrivacyMode(bool enabled);
    [[nodiscard]] bool privacyMode() const noexcept;
    [[nodiscard]] const std::vector<ShellPositionListRow>& rows() const noexcept;

private:
    std::vector<ShellPositionListRow> rows_;
    bool privacyMode_ = false;
};

}  // namespace etfdt::shell_services
