// multiprogram_sim.cpp
// First working version of multi-program shared-predictor simulation.
// This version uses simple round-robin scheduling and executes
// one branch at a time from each trace, sharing one predictor instance.

#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>

#include "multiprogram_sim.h"
#include "predictor_base.h"
#include "predictor_factory.h"
#include "trace_reader.h"
#include "types.h"

// Holds one program's trace and execution progress.
struct ProgramTrace
{
    std::string tracefile;
    std::vector<BranchRecord> branches;
    std::size_t next_index = 0;
    SimulationStats stats;
};

int run_multi_program_sim(
    const std::string& predictor_mode,
    const std::vector<std::string>& predictor_args,
    const std::vector<std::string>& tracefiles
)
{
    // Create one shared predictor instance for all programs.
    std::unique_ptr<PredictorBase> predictor =
        create_predictor(predictor_mode, predictor_args);

    // Load all traces into memory.
    std::vector<ProgramTrace> programs;
    for (std::size_t i = 0; i < tracefiles.size(); i++)
    {
        ProgramTrace program;
        program.tracefile = tracefiles[i];
        program.branches = read_trace(tracefiles[i]);
        programs.push_back(program);
    }

    // Global multi-program statistics.
    SimulationStats total_stats;

    // Track whether any program still has work left.
    bool work_remaining = true;

    // Round-robin scheduling:
    // during each pass, give each non-finished program one branch.
    while (work_remaining)
    {
        work_remaining = false;

        for (std::size_t i = 0; i < programs.size(); i++)
        {
            ProgramTrace& program = programs[i];

            // Skip finished traces.
            if (program.next_index >= program.branches.size())
            {
                continue;
            }

            work_remaining = true;

            // Fetch the next branch for this program.
            const BranchRecord& branch = program.branches[program.next_index];

            // Shared predictor makes a prediction.
            bool predicted_taken = predictor->predict(branch.pc);

            // Update per-program stats.
            program.stats.predictions++;

            // Update total stats.
            total_stats.predictions++;

            if (predicted_taken != branch.taken)
            {
                program.stats.mispredictions++;
                total_stats.mispredictions++;
            }

            // Update shared predictor using the actual outcome.
            predictor->update(branch.pc, branch.taken);

            // Advance this program to its next branch.
            program.next_index++;
        }
    }

    // Print summary output for this first working version.
    std::cout << "MULTI-PROGRAM MODE" << std::endl;
    std::cout << "scheduler:\tround-robin (1 branch per turn)" << std::endl;
    std::cout << "predictor mode:\t" << predictor_mode << std::endl;

    std::cout << "predictor args:";
    for (std::size_t i = 0; i < predictor_args.size(); i++)
    {
        std::cout << " " << predictor_args[i];
    }
    std::cout << std::endl;

    std::cout << "program count:\t" << programs.size() << std::endl;
    std::cout << "total predictions:\t" << total_stats.predictions << std::endl;
    std::cout << "total mispredictions:\t" << total_stats.mispredictions << std::endl;

    double total_misprediction_rate = 0.0;
    if (total_stats.predictions > 0)
    {
        total_misprediction_rate =
            100.0 * static_cast<double>(total_stats.mispredictions) /
            static_cast<double>(total_stats.predictions);
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "total misprediction rate:\t"
              << total_misprediction_rate << "%" << std::endl;

    std::cout << "per-program results:" << std::endl;
    for (std::size_t i = 0; i < programs.size(); i++)
    {
        double rate = 0.0;
        if (programs[i].stats.predictions > 0)
        {
            rate =
                100.0 * static_cast<double>(programs[i].stats.mispredictions) /
                static_cast<double>(programs[i].stats.predictions);
        }

        std::cout << "  program " << i
                  << ": " << programs[i].tracefile
                  << std::endl;
        std::cout << "    predictions:\t" << programs[i].stats.predictions
                  << std::endl;
        std::cout << "    mispredictions:\t" << programs[i].stats.mispredictions
                  << std::endl;
        std::cout << "    misprediction rate:\t" << rate << "%"
                  << std::endl;
    }

    return 0;
}