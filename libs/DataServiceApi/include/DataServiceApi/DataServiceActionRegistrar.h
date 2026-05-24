#pragma once

#include "DataAccess/DataAccess.h"
#include "ServiceRuntime/ServiceRuntime.h"

namespace etfdt::data_service_api {

void registerDataServiceReadOnlyActions(
    etfdt::service_runtime::ActionDispatcher& dispatcher,
    etfdt::data_access::SQLiteConnection& connection);

void registerDataServiceWriteActions(
    etfdt::service_runtime::ActionDispatcher& dispatcher,
    etfdt::data_access::SQLiteConnection& connection);

}  // namespace etfdt::data_service_api
