#include "MarketEngine/FixtureMarketDataProvider.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <utility>

namespace etfdt::market_engine {
namespace {

std::string stringField(const QJsonObject& object, const char* key, std::string fallback = {})
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isString() ? value.toString().toStdString() : std::move(fallback);
}

MarketInstrumentKey keyFromObject(const QJsonObject& object)
{
    MarketInstrumentKey key;
    key.instrumentCode = stringField(object, "instrumentCode");
    key.instrumentType = stringField(object, "instrumentType");
    key.exchange = stringField(object, "exchange");
    key.providerSymbol = stringField(object, "providerSymbol");
    key.provider = stringField(object, "provider", "fixture");
    return key;
}

}  // namespace

FixtureMarketDataProvider::FixtureMarketDataProvider(std::string fixturePayloadJson)
    : fixturePayloadJson_(std::move(fixturePayloadJson))
{
}

MarketProviderSnapshot FixtureMarketDataProvider::fetchReadOnly(
    const MarketRefreshInput& input)
{
    MarketProviderSnapshot snapshot;
    snapshot.source = "fixture";

    QJsonParseError parseError {};
    const auto document =
        QJsonDocument::fromJson(QByteArray::fromStdString(fixturePayloadJson_), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        snapshot.issues.push_back({
            "MARKET_DATA_FIXTURE_INVALID_JSON",
            "fixture",
            "Market data fixture payload could not be parsed.",
            true,
        });
        return snapshot;
    }

    const auto root = document.object();
    const auto fixture = root.value(QStringLiteral("fixture")).isObject()
        ? root.value(QStringLiteral("fixture")).toObject()
        : root;
    const auto quotes = fixture.value(QStringLiteral("quotes")).toArray();
    for (const auto& value : quotes) {
        if (!value.isObject()) {
            continue;
        }
        const auto object = value.toObject();
        MarketQuote quote;
        quote.key = keyFromObject(object);
        quote.quoteTimeUtc = stringField(object, "quoteTimeUtc");
        quote.priceText = stringField(object, "priceText");
        quote.previousCloseText = stringField(object, "previousCloseText");
        quote.openText = stringField(object, "openText");
        quote.highText = stringField(object, "highText");
        quote.lowText = stringField(object, "lowText");
        quote.volumeText = stringField(object, "volumeText");
        quote.currency = stringField(object, "currency", "CNY");
        quote.source = stringField(object, "source", "fixture");
        quote.dataQualityStatus = stringField(object, "dataQualityStatus", "OK");
        snapshot.quotes.push_back(std::move(quote));
    }

    const auto history = fixture.value(QStringLiteral("history")).toObject();
    for (const auto& key : input.instruments) {
        const auto bars = history.value(QString::fromStdString(key.instrumentCode)).toArray();
        for (const auto& value : bars) {
            if (!value.isObject()) {
                continue;
            }
            const auto object = value.toObject();
            MarketHistoryBar bar;
            bar.instrumentCode = key.instrumentCode;
            bar.date = stringField(object, "date");
            bar.highText = stringField(object, "highText");
            bar.closeText = stringField(object, "closeText");
            bar.source = stringField(object, "source", "fixture");
            snapshot.historyBars.push_back(std::move(bar));
        }
    }

    return snapshot;
}

}  // namespace etfdt::market_engine
