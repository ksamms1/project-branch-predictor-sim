#ifndef MULTIPROGRAM_SIM_H
#define MULTIPROGRAM_SIM_H

#include <cstddef>
#include <string>
#include <vector>

#include "types.h"

// Run the future multi-program simulation mode.
int run_multi_program_sim(
    std::size_t time_slice,
    const std::string& predictor_mode,
    const std::vector<std::string>& predictor_args,
    const std::vector<std::string>& tracefiles
);

#endif