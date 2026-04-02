// single_program_sim.cpp
// Implements the standard MP2 single-program simulation flow.
// This preserves the original MP2 behavior and output format.

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "predictor_base.h"
#include "predictor_factory.h"
#include "single_program_sim.h"
#include "trace_reader.h"
#include "types.h"

int run_single_program_sim(
    const std::string& mode,
    const std::vector<std::string>& predictor_args,
    const std::string& tracefile
)
{
    // Create the predictor object from predictor_factory.cpp
    std::unique_ptr<PredictorBase> predictor =
        create_predictor(mode, predictor_args);

    // Read the trace file into memory.
    std::vector<BranchRecord> trace = read_trace(tracefile);

    // Statistics for this run.
    SimulationStats stats;

    // Process each branch in the trace.
    for (std::size_t i = 0; i < trace.size(); i++)
    {
        const BranchRecord& branch = trace[i];

        // Ask the predictor for its guess.
        bool predicted_taken = predictor->predict(branch.pc);

        // Count this branch as one prediction.
        stats.predictions++;

        // If the guess was wrong, count a misprediction.
        if (predicted_taken != branch.taken)
        {
            stats.mispredictions++;
        }

        // Update predictor state using the actual outcome.
        predictor->update(branch.pc, branch.taken);
    }

    // Compute misprediction percentage.
    double misprediction_rate = 0.0;
    if (stats.predictions > 0)
    {
        misprediction_rate =
            100.0 * static_cast<double>(stats.mispredictions) /
            static_cast<double>(stats.predictions);
    }

    // Print required MP2 output format.
    std::cout << "COMMAND" << std::endl;

    std::cout << "./sim " << mode;
    for (std::size_t i = 0; i < predictor_args.size(); i++)
    {
        std::cout << " " << predictor_args[i];
    }
    std::cout << " "
              << std::filesystem::path(tracefile).filename().string()
              << std::endl;

    std::cout << "OUTPUT" << std::endl;
    std::cout << "number of predictions:\t\t" << stats.predictions << std::endl;
    std::cout << "number of mispredictions:\t" << stats.mispredictions << std::endl;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "misprediction rate:\t\t" << misprediction_rate << "%" << std::endl;

    predictor->print_final_contents(std::cout);

    return 0;
}