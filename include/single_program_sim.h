#ifndef SINGLE_PROGRAM_SIM_H
#define SINGLE_PROGRAM_SIM_H

#include <string>
#include <vector>

// Run the standard MP2 single-program simulation.
// Returns 0 on success, 1 on error.
int run_single_program_sim(
    const std::string& mode,
    const std::vector<std::string>& predictor_args,
    const std::string& tracefile
);

#endif