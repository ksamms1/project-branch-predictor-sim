// multiprogram_sim.cpp
// Scaffold for future multi-program shared-predictor simulation mode.

#include <iostream>
#include <string>
#include <vector>

#include "multiprogram_sim.h"

int run_multi_program_sim(
    const std::string& predictor_mode,
    const std::vector<std::string>& predictor_args,
    const std::vector<std::string>& tracefiles
)
{
    std::cout << "MULTI-PROGRAM MODE" << std::endl;
    std::cout << "This mode is not implemented yet." << std::endl;
    std::cout << "Predictor mode: " << predictor_mode << std::endl;

    std::cout << "Predictor arguments:";
    for (std::size_t i = 0; i < predictor_args.size(); i++)
    {
        std::cout << " " << predictor_args[i];
    }
    std::cout << std::endl;

    std::cout << "Trace files:" << std::endl;
    for (std::size_t i = 0; i < tracefiles.size(); i++)
    {
        std::cout << "  " << tracefiles[i] << std::endl;
    }

    return 0;
}