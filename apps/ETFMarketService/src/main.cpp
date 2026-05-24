#include "MarketEngine/MarketEngine.h"

#include <iostream>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    std::cout << "ETFMarketService skeleton using "
              << etfdt::market_engine::marketEngineModuleName() << '\n';
    return 0;
}
