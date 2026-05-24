#include "DataServiceApi/DataServiceActionRegistrar.h"

#include "DataServiceApi/DataServiceActions.h"

namespace etfdt::data_service_api {

void registerDataServiceReadOnlyActions(
    etfdt::service_runtime::ActionDispatcher& dispatcher,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)dispatcher.registerAction(kActionDataHealth, [&connection](const auto& context) {
        return handleDataHealth(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataSummary, [&connection](const auto& context) {
        return handleDataSummary(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataAccountsList, [&connection](const auto& context) {
        return handleDataAccountsList(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataPortfoliosList, [&connection](const auto& context) {
        return handleDataPortfoliosList(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataInstrumentsList, [&connection](const auto& context) {
        return handleDataInstrumentsList(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataStrategiesList, [&connection](const auto& context) {
        return handleDataStrategiesList(context, connection);
    });
    (void)dispatcher.registerAction(kActionDataOtcList, [&connection](const auto& context) {
        return handleDataOtcList(context, connection);
    });
}

}  // namespace etfdt::data_service_api
