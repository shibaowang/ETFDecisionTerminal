#include "StrategyEngine/StrategyEngine.h"

#include <iostream>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    std::cout << "ETFStrategyService skeleton using "
              << etfdt::strategy_engine::strategyEngineModuleName() << '\n';
    return 0;
}
