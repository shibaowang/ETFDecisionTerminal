#include "DataAccess/DataAccess.h"

#include <iostream>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    std::cout << "ETFDataService skeleton; database write owner is "
              << etfdt::data_access::kDatabaseWriteOwner << '\n';
    return 0;
}
