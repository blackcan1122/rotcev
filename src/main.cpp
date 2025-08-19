#include "rotcev.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <map>
#include "functionality.hpp"
#include "logging_profiling.hpp"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <parameter>" << std::endl;
        return 1;
    }

    std::string param = argv[1];
    if (param == "-test")
    {
        StartBenchmark();
    }

    if (param == "-func")
    {
        Func::Insertions();
    }

    return 0;
}

