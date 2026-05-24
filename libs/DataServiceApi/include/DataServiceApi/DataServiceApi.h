#pragma once

#include "DataServiceApi/DataServiceActionRegistrar.h"
#include "DataServiceApi/DataServiceActions.h"
#include "DataServiceApi/JsonBuilders.h"
#include "DataServiceApi/WriteActionPolicy.h"

#include <string_view>

namespace etfdt::data_service_api {

constexpr std::string_view kDataServiceApiModuleName = "ETFDataServiceApi";

std::string_view dataServiceApiModuleName() noexcept;

}  // namespace etfdt::data_service_api
